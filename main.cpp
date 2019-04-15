#include "mainwindow.h"
#include <signal.h>
#include <QApplication>
#include <QtDebug>
#include <QDir>
#include <QDesktopWidget>
#include <QFileSystemWatcher>
#include <QStandardPaths>

QProcess process;
QString DESKTOP_DIR_PATH = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

static bool isMac() {
	#ifdef Q_OS_MAC
		return true;
	#else
		return false;
	#endif
}

static void m_cleanup(int sig) {
	process.kill();
}

static void setupHandlers(QApplication *application) {
	signal(SIGKILL, m_cleanup);
	signal(SIGINT, m_cleanup);
	signal(SIGQUIT, m_cleanup);
	signal(SIGTERM, m_cleanup);
	signal(SIGHUP, m_cleanup);
	process.setEnvironment( QProcess::systemEnvironment() );
	process.setProcessChannelMode( QProcess::MergedChannels );
	process.start("gsettings",  QStringList{"monitor", "org.gnome.desktop.interface", "icon-theme"});
	process.waitForStarted();
	QObject::connect(&process, &QProcess::readyReadStandardOutput, []() {
		m_cleanup(0);
		QProcess::startDetached(QApplication::applicationFilePath());
		exit(12);
	});
	QObject::connect(application, &QApplication::aboutToQuit, [](){
		m_cleanup(0);
	});
}


static void updateDesktop(MainWindow *window) {
	QStringList files;
	auto list = QDir(DESKTOP_DIR_PATH).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
	foreach (QFileInfo info, list) files.push_front(info.absoluteFilePath());
	window->updateDesktop(files);
}

int main(int argc, char *argv[]) {
	QApplication application(argc, argv);
	MainWindow window;


//	setupHandlers(&application);

	QFileSystemWatcher watcher;
	watcher.addPath(DESKTOP_DIR_PATH);

	QObject::connect(&watcher, &QFileSystemWatcher::directoryChanged, [&window](){
		updateDesktop(&window);
	});

//	QObject::connect(&watcher, &QFileSystemWatcher::fileChanged, [&window](){
//		qDebug() << "CHANGED?";
//		updateDesktop(&window);
//	});



	updateDesktop(&window);
	window.setStyleSheet("background-color: #5979AE" );

	if (isMac()) {
		window.resize(1600, 900);
	}

	else {
		window.setAttribute(Qt::WA_TranslucentBackground);
		window.setWindowFlag(Qt::WindowStaysOnBottomHint, true);
		window.setAttribute(Qt::WA_X11NetWmWindowTypeDesktop, true);
		window.setGeometry(QApplication::desktop()->geometry());
	}

	window.show();

	return application.exec();
}
