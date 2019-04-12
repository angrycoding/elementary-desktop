#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QRubberBand>
#include <QDrag>
#include "desktopicon.h"

class MainWindow : public QMainWindow {

	Q_OBJECT

private:

	QRubberBand* rubberBand;
	bool isShiftPressed(QMouseEvent *event);
	QPoint pressPoint;
	QPixmap dragPixmap;
	QList<DesktopIcon*> allIcons;


public:

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


	// QWidget interface
protected:
	void keyPressEvent(QKeyEvent *event);

	// QWidget interface
protected:
	void dropEvent(QDropEvent *event);

	// QWidget interface
protected:
	void dragEnterEvent(QDragEnterEvent *event);

	// QWidget interface
protected:
	void dragMoveEvent(QDragMoveEvent *event);
};

#endif // MAINWINDOW_H
