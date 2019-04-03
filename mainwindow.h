#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGuiApplication>
#include <QtWebView/QtWebView>
#include <QWebEngineView>
#include <QJsonArray>
#include <QJsonObject>

class MainWindow : public QMainWindow
{
    Q_OBJECT

     QThread thread;

public:
    MainWindow(QWidget *parent = 0);
    void directoryChange();
    ~MainWindow();

private:
    QWebEngineView* view;


public slots:
     void myfunction();
    QJsonArray readDesktop();
    void showModified(const QString &path);
    void showContextMenu(int x, int y);
    void dragStart(int x, int y, int w, int h);


};

#endif // MAINWINDOW_H
