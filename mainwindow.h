#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QRubberBand>
#include <QDrag>
#include <QProcess>
#include <QFileIconProvider>
#include "desktopicon.h"

class MainWindow : public QMainWindow {

	Q_OBJECT

private:

	QRubberBand* rubberBand;
	QFileIconProvider iconProvider;
	bool isShiftPressed(QMouseEvent *event);
	QPoint pressPoint;
	QPixmap dragPixmap;


public:

	void updateDesktop(QStringList files);
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	void setAllIconsSelection(bool selected);
	void selectIcon(DesktopIcon* icon);


protected:

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

public:

	bool event(QEvent *event);

protected:
	void keyPressEvent(QKeyEvent *event);
	void dropEvent(QDropEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);

};

#endif // MAINWINDOW_H
