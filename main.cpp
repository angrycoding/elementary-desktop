#include "mainwindow.h"
#include <QDesktopWidget>
#include <QLayout>
#include <QLabel>
#include <QPainter>
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication application(argc, argv);
    MainWindow window;
    window.resize(1000, 800);
    window.show();



    return application.exec();
}
