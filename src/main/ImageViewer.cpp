#include "ImageViewer.hpp"

#include<iostream>

#include <QGuiApplication>
#include <QScreen>
#include <QStandardPaths>
#include <QDir>
#include <QImageReader>
#include <QMessageBox>
#include <QColorSpace>
#include <QStatusBar>
#include <QScrollBar>
#include <QMenuBar>
#include <QApplication>
#include <QInputDialog>

ImageViewer::ImageViewer(QWidget* parent): QMainWindow(parent), imageLabel(new QLabel), scrollArea(new QScrollArea), filesListView(new QListView)
{
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);

    createActionBar();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

void ImageViewer::openImage()
{
    QFileDialog dialog(this, tr("Open File"));
    createFileChooserDialog(dialog);

    while (dialog.exec() == QDialog::Accepted && !loadImage(dialog.selectedFiles().constFirst())) {}
}

void ImageViewer::zoomIn()
{
    scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}

void ImageViewer::setImageOriginalSize()
{
    imageLabel->adjustSize();
    imgScale = 1.0;
}

void ImageViewer::moveRight()
{
}

void ImageViewer::moveLeft()
{
}

void ImageViewer::moveUp()
{
}

void ImageViewer::moveDown()
{
}

void ImageViewer::rotateLeft()
{
}

void ImageViewer::rotateRight()
{
}

void ImageViewer::changeBrightnessContrast()
{
    if (currBrightness > definitions.MAX_BRIGHTNESS || currBrightness < definitions.MIN_BRIGHTNESS) {
        std::cerr << "Incorrect brightness value: " << currBrightness << ". Ignoring the call for brightness and contrast change...\n";
        return;
    }

    if (currContrast > definitions.MAX_CONTRAST || currContrast < definitions.MIN_CONTRAST) {
        std::cerr << "Incorrect contrast value: " << currContrast << ". Ignoring the call for brightness and contrast change...\n";
        return;
    }

    currImage = loadedImage;

    for (int i = 0; i < loadedImage.width(); i++) {
        for (int j = 0; j < loadedImage.height(); j++) {
            const QColor pixel = loadedImage.pixelColor(i, j);

            // value is not been checked because we confirm it is in range before
            const QColor pixelBright = applyBrightness(pixel, currBrightness).value(); 
            const QColor pixelBrightContrast = applyContrast(pixelBright, currContrast).value();
            
            currImage.setPixelColor(i, j, pixelBrightContrast);
        }
    }

    imageLabel->setPixmap(QPixmap::fromImage(currImage));
}

/// <summary>
/// Applies contrast to a pixel using contrast adjustment provided by https://www.dfstudios.co.uk/articles/programming/image-programming-algorithms/image-processing-algorithms-part-5-contrast-adjustment/.
/// </summary>
/// <param name="pixel">The pixel to apply the contrast</param>
/// <param name="contrast">The contrast level (from -128 to 128)</param>
/// <returns>The pixel with the adjusted values if the contrast is in range.</returns>
constexpr std::optional<QColor> ImageViewer::applyContrast(const QColor& pixel, int contrast) const
{
    if (contrast < definitions.MIN_CONTRAST || contrast > definitions.MAX_CONTRAST)
        return std::nullopt;

    const double factor = (double) (259 * (contrast + 255)) / (255 * (259 - contrast));
    const int red = truncateRGBInterval(factor * (pixel.red() - 128) + 128);
    const int green = truncateRGBInterval(factor * (pixel.green() - 128) + 128);
    const int blue = truncateRGBInterval(factor * (pixel.blue() - 128) + 128);
    
    return QColor(red, green, blue);
}

constexpr std::optional<QColor> ImageViewer::applyBrightness(const QColor& pixel, const int brightness) const
{
    if (brightness < definitions.MIN_BRIGHTNESS || brightness > definitions.MAX_BRIGHTNESS)
        return std::nullopt;

    // check if we are increasing or decreasing the brightness
    if (currBrightness > 0) {
        const int tempBrightness = 100 + currBrightness;
        return pixel.lighter(tempBrightness);
    }
    else if (currBrightness < 0) {
        // selected brightness is multiplied by a factor of 2 to achieve better results.
        // the reason is due to the logarithm result of the values passed to the darker function
        const int tempBrightness = 100 + abs(currBrightness) * 2;
        return pixel.darker(tempBrightness);
    }
    else { 
        // if brightness is 0 we just use the original image
        return QColor(pixel);
    }
}

constexpr int ImageViewer::truncateRGBInterval(const int value) const
{
    const int rgbMax = 255;
    const int rgbMin = 0;

    return std::min(std::max(rgbMin, value), rgbMax);
}

void ImageViewer::fitImageToWindow()
{
    bool fitToWindow = fitToWindowAction->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);

    if (!fitToWindow)
        setImageOriginalSize();

    activateActionsOnFitToWindow();
}

void ImageViewer::createBrightnessDialog()
{
    bool ok;
    const int i = QInputDialog::getInt(this, tr("Set image brightness"), tr("Brightness:"), currBrightness, definitions.MIN_BRIGHTNESS, definitions.MAX_BRIGHTNESS, 1, &ok);

    if (ok) {
        currBrightness = i;

        changeBrightnessContrast();
    }
}

void ImageViewer::createContrastDialog()
{
    bool ok;
    const int i = QInputDialog::getInt(this, tr("Set image contrast"), tr("Contrast:"), currContrast, definitions.MIN_CONTRAST, definitions.MAX_CONTRAST, 1, &ok);

    if (ok) {
        currContrast = i;

        changeBrightnessContrast();
    }
}

void ImageViewer::createFileChooserDialog(QFileDialog& dialog)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters({ "image/bmp", "image/png", "image/jpeg"});
    
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFiles); // enable to choose several files
}

bool ImageViewer::loadImage(const QString& filepath)
{
    QImageReader reader(filepath);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
            tr("Cannot load %1: %2").arg(QDir::toNativeSeparators(filepath), reader.errorString()));
        return false;
    }

    loadImageToScreen(newImage);

    setWindowFilePath(filepath);

    const QString description = newImage.colorSpace().isValid() ? newImage.colorSpace().description() : tr("unknown");
    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4 (%5)")
        .arg(QDir::toNativeSeparators(filepath)).arg(newImage.width()).arg(newImage.height())
        .arg(newImage.depth()).arg(description);

    statusBar()->showMessage(message);

    return true;
}

void ImageViewer::loadImageToScreen(const QImage& image) {
    loadedImage = image;

    if (loadedImage.colorSpace().isValid())
        loadedImage.convertToColorSpace(QColorSpace::SRgb);

    imageLabel->setPixmap(QPixmap::fromImage(loadedImage));

    // if it's not using fit to screen we set the size to be the original image size
    if (!fitToWindowAction->isChecked())
        setImageOriginalSize();

    imgScale = 1;

    scrollArea->setVisible(true);
    activateActions();
}

void ImageViewer::scaleImage(double factor)
{
    imgScale *= factor;
    imageLabel->resize(imgScale * imageLabel->pixmap().size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);


    zoomInAction->setEnabled(imgScale < definitions.MAX_ZOOM);
    zoomOutAction->setEnabled(imgScale > definitions.MIN_ZOOM);
}

void ImageViewer::adjustScrollBar(QScrollBar* scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
}

void ImageViewer::createActionBar()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    QAction* openAct = fileMenu->addAction(tr("Open..."), this, &ImageViewer::openImage);
    openAct->setShortcut(QKeySequence::Open);

    QAction* exitAct = fileMenu->addAction(tr("Exit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+W"));

    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));

    fitToWindowAction = viewMenu->addAction(tr("Fit to Window"), this, &ImageViewer::fitImageToWindow);
    fitToWindowAction->setCheckable(true);
    fitToWindowAction->setShortcut(tr("Ctrl+F"));

    viewMenu->addSeparator();

    zoomInAction = viewMenu->addAction(tr("Zoom In"), this, &ImageViewer::zoomIn);
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    zoomInAction->setEnabled(false);

    zoomOutAction = viewMenu->addAction(tr("Zoom Out"), this, &ImageViewer::zoomOut);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    zoomOutAction->setEnabled(false);

    removeZoomAction = viewMenu->addAction(tr("Normal Size"), this, &ImageViewer::setImageOriginalSize);
    removeZoomAction->setShortcut(tr("Ctrl+0"));
    removeZoomAction->setEnabled(false);

    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));

    changeBrightnessAction = editMenu->addAction(tr("Brightness..."), this, &ImageViewer::createBrightnessDialog);
    changeBrightnessAction->setEnabled(false);

    changeContrastAction = editMenu->addAction(tr("Contrast..."), this, &ImageViewer::createContrastDialog);
    changeContrastAction->setEnabled(false);
}

void ImageViewer::activateActions()
{
    changeBrightnessAction->setEnabled(true);
    changeContrastAction->setEnabled(true);

    activateActionsOnFitToWindow();
}

void ImageViewer::activateActionsOnFitToWindow()
{
    zoomInAction->setEnabled(!fitToWindowAction->isChecked() && !loadedImage.size().isNull());
    zoomOutAction->setEnabled(!fitToWindowAction->isChecked() && !loadedImage.size().isNull());
    removeZoomAction->setEnabled(!fitToWindowAction->isChecked() && !loadedImage.size().isNull());
}
