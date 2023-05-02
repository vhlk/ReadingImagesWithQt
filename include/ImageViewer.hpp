#include <string>
#include <optional>

#include <ImageViewerDefinitions.hpp>

#include <QMainWindow>
#include <QFileDialog>
#include <QLabel>
#include <QScrollArea>
#include <QListWidget>
#include <QGridLayout>

/// <summary>
/// Class for displaying a Image Viewer window.
/// </summary>
class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = nullptr);

private slots:
	void openImage();
	void zoomIn();
	void zoomOut();
	void setImageOriginalSize();
	void fitImageToWindow();
	void createBrightnessDialog();
	void createContrastDialog();
	void rotateLeft();
	void rotateRight();
	void onFileListWidgetClicked();

private:
	void changeBrightnessContrast();
	constexpr std::optional<QColor> applyContrast(const QColor& pixel, const int contrast) const;
	constexpr std::optional<QColor> applyBrightness(const QColor& pixel, const int brightness) const;
	constexpr int truncateRGBInterval(const int value) const; // makes sure a value is in RGB accepted range
	void createFileChooserDialog(QFileDialog& dialog);
	bool loadImages(const QStringList& filepaths);
	bool loadImage(const QString& filepath);
	void loadImageToScreen(const QImage& image);
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar* scrollBar, double factor);
	void createActionBar();
	void activateActions(); // activate menu actions
	void activateActionsOnFitToWindow(); // activate menu actions based in fit to window been selected or not
	void applyRotation(int rotation);

private:
	QImage loadedImage;
	QImage currImage; // if the image has any modification to its attribute (brightness, contrast...) this will store the modified image
	QLabel* imageLabel; // displays the image
	QScrollArea* scrollArea;
	QListWidget* filesListWidget;
	QGridLayout* gridLayout;

	double imgScale = 1;

	int currBrightness = 0;
	int currContrast = 0;

	int currRotation = 0;

	QStringList chosenFiles;

	QAction* fitToWindowAction;
	QAction* zoomInAction;
	QAction* zoomOutAction;
	QAction* removeZoomAction;
	QAction* changeBrightnessAction;
	QAction* changeContrastAction;
	QAction* rotateLeftAction;
	QAction* rotateRightAction;

	Definitions definitions;
};