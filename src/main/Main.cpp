#include <iostream>
#include <ImageViewer.hpp>
#include <QApplication>

int main(int argc, char* argv[])
{
	using namespace std;

	QApplication app(argc, argv);
	QGuiApplication::setApplicationDisplayName(ImageViewer::tr("Image Viewer"));

	auto imgViewer = new ImageViewer();
	imgViewer->show();

	return app.exec();
}
