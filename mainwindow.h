#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QRubberBand>
#include <QDrag>
#include <QProcess>
#include <QFileIconProvider>
#include <QThread>
#include "desktopicon.h"

class MainWindow : public QMainWindow {

	Q_OBJECT

private:

	QPoint pressPoint;
	QPixmap dragPixmap;
	QRubberBand* rubberBand;
	QFileIconProvider iconProvider;
	QPoint clientToGrid(QPoint pos);
	QPoint gridToClient(QPoint pos);
	bool isShiftPressed(QMouseEvent *event);


public:

	void updateDesktop(QStringList files);
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	void setAllIconsSelection(bool selected);
	void selectIcon(DesktopIcon* icon);
	void recalcGrid();
	void realignIcons();

public:

	bool event(QEvent *event);

protected:

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void dropEvent(QDropEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);

};

#endif // MAINWINDOW_H
