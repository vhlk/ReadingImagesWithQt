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

ImageViewer::ImageViewer(QWidget* parent): QMainWindow(parent), imageLabel(new QLabel), scrollArea(new QScrollArea)
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

void ImageViewer::changeBrightness()
{
}

void ImageViewer::fitImageToWindow()
{
    bool fitToWindow = fitToWindowAction->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);

    if (!fitToWindow)
        setImageOriginalSize();

    activateActionsOnFitToWindow();
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
    activateActionsOnFitToWindow();
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
}

void ImageViewer::activateActionsOnFitToWindow()
{
    zoomInAction->setEnabled(!fitToWindowAction->isChecked() && !loadedImage.size().isNull());
    zoomOutAction->setEnabled(!fitToWindowAction->isChecked() && !loadedImage.size().isNull());
    removeZoomAction->setEnabled(!fitToWindowAction->isChecked() && !loadedImage.size().isNull());
}
