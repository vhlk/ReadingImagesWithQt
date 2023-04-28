#include <string>

#include <ImageViewerDefinitions.hpp>

#include <QMainWindow>
#include <QFileDialog>
#include <QLabel>
#include <QScrollArea>

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
	void moveRight();
	void moveLeft();
	void moveUp();
	void moveDown();
	void rotateLeft();
	void rotateRight();
	void changeBrightness();
	void fitImageToWindow();

private:
	void createFileChooserDialog(QFileDialog& dialog);
	bool loadImage(const QString& filepath);
	void loadImageToScreen(const QImage& image);
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar* scrollBar, double factor);
	void createActionBar();
	void activateActionsOnFitToWindow();

private:
	QImage loadedImage;
	QLabel* imageLabel; // displays the image
	QScrollArea* scrollArea;

	double imgScale = 1;

	QAction* fitToWindowAction;
	QAction* zoomInAction;
	QAction* zoomOutAction;
	QAction* removeZoomAction;

	Definitions definitions;
};