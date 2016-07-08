#include <QApplication>
#include <QtGui>
#include <QtWidgets>

#include "Window.h"
#include "NvidiaSaturationController.h"

int main(int argc, char* argv[]) {
	Q_INIT_RESOURCE(icons);

	QApplication app(argc, argv);

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, 
							  QObject::tr("Systray"),
		                      QObject::tr("I couldn't detect any system tray on this system."));
		return 1;
	}
	QApplication::setQuitOnLastWindowClosed(false);

	SaturationChanger::Window window;
	window.show();
	return app.exec();
}
