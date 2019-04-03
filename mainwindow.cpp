#include "mainwindow.h"
#include "weburlrequestinterceptor.h"

#include<QEvent>
#include <QDebug>
#include<QCoreApplication>

#include <QtCore/QUrl>
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QGuiApplication>
#include <QStyleHints>
#include <QScreen>
#include <QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtWebView/QtWebView>
#include <QWebChannel>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineUrlScheme>
#include <QStyle>
#include <QApplication>
#include <QMenu>
#include "custompage.h"

QString DESKTOP_DIR = "/home/ruslan/Qt";

void MainWindow::myfunction() {




    QList<QUrl> uriList;
    uriList << QUrl("file:///Users/ruslanmatveev/Desktop/circuit.png");


    QMimeData *mime_data = new QMimeData;
    mime_data->setUrls(uriList);



    QPixmap pixmap(100,100);
    pixmap.fill(QColor("red"));
    QIcon redIcon(pixmap);

    QDrag *drag = new QDrag(this);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(40, 40));
    drag->setMimeData(mime_data);
    drag->exec(Qt::CopyAction |  Qt::IgnoreAction);

    qDebug() << "FINISHED";
    qDebug() <<  drag->target();

    thread.terminate();
}



QString icon_for_filename(QFileInfo fileInfo) {
    QIcon icon;
    QMimeDatabase mime_database;
    QMimeType mimeType = mime_database.mimeTypeForFile(fileInfo, QMimeDatabase::MatchDefault);

    if (mimeType.isValid()) {
        icon = QIcon::fromTheme(mimeType.iconName());
    }

    if (icon.isNull()) {
        icon = QApplication::style()->standardIcon(fileInfo.isDir() ? QStyle::SP_DirIcon : QStyle::SP_FileIcon);
    }

    QImage image(icon.pixmap(64, 64).toImage());
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    image.save(&buffer, "PNG");
    return "data:image/png;base64," + QString::fromLatin1(byteArray.toBase64().data());
}

QJsonArray MainWindow::readDesktop() {
    QJsonArray result = {};
    QDir directory(DESKTOP_DIR);
    QFileInfoList files = directory.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries);
    for (int c = 0; c < files.length(); c++) {
        QFileInfo fileInfo = files.at(c);
        result.push_back(QJsonObject{
            {"name", fileInfo.fileName()},
            {"icon", icon_for_filename(fileInfo)},
            {"path", fileInfo.filePath()}
        });
    }
    return result;
}


void MainWindow::showContextMenu(int x, int y) {
    qDebug() << x << "," << y;

    QMenu contextMenu(tr("Context menu"), this);

      QAction action1("Remove Data Point", this);
      connect(&action1, SIGNAL(triggered()), this, SLOT(removeDataPoint()));
      contextMenu.addAction(&action1);
      contextMenu.addAction(new QAction("Acafadaasdfdfdstion 1", this));
      contextMenu.addAction(new QAction("Acafadadfdstion 1", this));
      contextMenu.addAction(new QAction("Action 1323421", this));
      contextMenu.addAction(new QAction("Actadsfasdion 1", this));
      contextMenu.addAction(new QAction("Action 1", this));


      contextMenu.exec(this->view->mapToGlobal(QPoint(x, y)));
}

void MainWindow::dragStart(int x, int y, int w, int h) {

    QPixmap pixmap(w, h);
    pixmap.fill(Qt::transparent); // add this!
    view->page()->view()->render(&pixmap, QPoint(0, 0),  QRegion(QRect(x, y, w, h)), QWidget::DrawChildren);


//    QList<QUrl> uriList;
//    uriList << QUrl("file:///Users/ruslanmatveev/Desktop/circuit.png");
//    QMimeData *mime_data = new QMimeData;
//    mime_data->setUrls(uriList);

//    QDrag *drag = new QDrag(this);
//    drag->setPixmap(pixmap);
//    drag->setHotSpot(QPoint(40, 40));
//    drag->setMimeData(mime_data);
//    drag->exec(Qt::CopyAction |  Qt::IgnoreAction);

//    moveToThread(&thread);
//    connect(&thread, SIGNAL(started()), this, SLOT(myfunction())); //cant have parameter sorry, when using connect
//    thread.start();
}




void MainWindow::showModified(const QString &path) {
    this->view->page()->runJavaScript("document.dispatchEvent(new Event('directoryChange'))");
}


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    this->view = new QWebEngineView();
    CustomPage *page = new CustomPage();
    this->view->setPage(page);
    this->view->page()->setBackgroundColor(Qt::transparent);



    QWebChannel *channel = new QWebChannel(page);
    channel->registerObject(QStringLiteral("core"), this);
    page->setWebChannel(channel);



    QWebEngineUrlScheme scheme("app");
    QWebEngineUrlScheme::registerScheme(scheme);
    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler("app", new WebUrlRequestInterceptor(this));


    setCentralWidget(this->view);
    this->view->load(QUrl("app://resources/index.html"));


    QFileSystemWatcher *watcher = new QFileSystemWatcher();
    watcher->addPath(DESKTOP_DIR);
    connect(watcher, SIGNAL(directoryChanged(const QString &)), this, SLOT(showModified(const QString &)));
    connect(watcher, SIGNAL(fileChanged(const QString &)), this, SLOT(showModified(const QString &)));

    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    this->setAttribute(Qt::WA_TranslucentBackground);


}

MainWindow::~MainWindow()
{

}


