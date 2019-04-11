#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QRubberBand>
#include <QDrag>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    QPoint origin;
    QRubberBand* rubberBand;
    bool isDragging = false;


public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void unselectAll();



protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    // QWidget interface
protected:

    // QObject interface
public:
    bool event(QEvent *event);
};

#endif // MAINWINDOW_H
