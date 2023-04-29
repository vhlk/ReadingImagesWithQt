#include <string>

#include <ImageViewerDefinitions.hpp>

#include <QMainWindow>
#include <QFileDialog>
#include <QLabel>
#include <QScrollArea>
#include <QListView>

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
	void fitImageToWindow();
	void createBrightnessDialog();

private:
	void changeBrightness(int brightness);
	void createFileChooserDialog(QFileDialog& dialog);
	bool loadImage(const QString& filepath);
	void loadImageToScreen(const QImage& image);
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar* scrollBar, double factor);
	void createActionBar();
	void activateActions();
	void activateActionsOnFitToWindow();

private:
	QImage loadedImage;
	QImage currImage; // if the image has any modification to its attribute (brightness, contrast...) this will store the modified image
	QLabel* imageLabel; // displays the image
	QScrollArea* scrollArea;
	QListView* filesListView;

	double imgScale = 1;

	int currBrightness = 0;

	QAction* fitToWindowAction;
	QAction* zoomInAction;
	QAction* zoomOutAction;
	QAction* removeZoomAction;
	QAction* changeBrightnessAction;

	Definitions definitions;
};