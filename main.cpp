#include "mainwindow.h"
#include <QApplication>
#include <QtDebug>
#include <QGuiApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.setStyleSheet("background-color: #017C7B" );


	w.setAttribute(Qt::WA_TranslucentBackground);
	w.setWindowFlag(Qt::WindowStaysOnBottomHint, true);
//	w.setWindowFlag(Qt::Desktop, true);
	w.setAttribute(Qt::WA_X11NetWmWindowTypeDesktop, true);

	w.setGeometry(QApplication::desktop()->geometry());
//	w.move(100, 100);
//	w.resize(600, 600);


	w.show();
//


	return a.exec();
}
