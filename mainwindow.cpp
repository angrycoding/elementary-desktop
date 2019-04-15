#include "mainwindow.h"
#include "desktopicon.h"
#include <QRubberBand>
#include <QApplication>
#include <QStyle>
#include <QDebug>
#include <QDir>
#include <QFileIconProvider>
#include <QMouseEvent>
#include <QMimeData>
#include <QtMath>
#include <QApplication>
#include <QDesktopWidget>
#include <QCursor>
#include <QHash>
#include <QDateTime>
#include <QObject>
#include <QPropertyAnimation>


QHash<QString, DesktopIcon*> newList;

int desiredCols = 14;
int desiredRows = 8;
int desiredSpacing = 12;
int OFFSET_X = 0;
int OFFSET_Y = 0;
int W_WIDTH = 0;
int W_HEIGHT = 0;



void MainWindow::recalcGrid() {
	int desktopWidth = this->width();
	int desktopHeight = this->height();
	int _cols = (desktopWidth - desiredSpacing) / desiredCols;
	int _rows = (desktopHeight - desiredSpacing) / desiredRows;
	W_WIDTH = _cols - desiredSpacing;
	W_HEIGHT = _rows - desiredSpacing;
	OFFSET_X = ((desktopWidth + desiredSpacing) - _cols * desiredCols) / 2;
	OFFSET_Y = ((desktopHeight + desiredSpacing) - _rows * desiredRows) / 2;
}

void MainWindow::realignIcons() {
	foreach (DesktopIcon* icon, newList) {
		int col = icon->property("col").toInt();
		int row = icon->property("row").toInt();
		icon->move(OFFSET_X + (col * desiredSpacing + col * W_WIDTH), OFFSET_Y + (row * desiredSpacing + row * W_HEIGHT));

		icon->resize(W_WIDTH, W_HEIGHT);
	}
}



void MainWindow::updateDesktop(QStringList files) {


	foreach (QString path, newList.keys()) {
		DesktopIcon *desktopIcon = newList[path];
		if (!files.contains(path)) {
			newList.remove(path);
			delete desktopIcon;
		}
	}


	int col = 0;
	int row = 0;

	foreach (QString path, files) {
		if (newList.contains(path)) continue;

		DesktopIcon *button = new DesktopIcon(this);
		button->setFont(this->font());
		button->setPath(path);
		button->setIcon(iconProvider.icon(QFileInfo(path)));
		button->setParent(this);
		button->setProperty("col", col);
		button->setProperty("row", row);
		button->show();
		newList.insert(path, button);

		row++;
		if (row >= desiredRows) {
			row = 0;
			col++;
		}

	}

	recalcGrid();
	realignIcons();
}

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
	setAcceptDrops(true);
	QFont font("Lucida Grande");
	font.setPixelSize(12);
	font.setStyleStrategy(QFont::PreferAntialias);
	this->setFont(font);
	dragPixmap = QPixmap(1, 1);
	rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

MainWindow::~MainWindow() {
	delete this->rubberBand;
}

void MainWindow::setAllIconsSelection(bool selected) {
	foreach (DesktopIcon* desktopIcon, newList) {
		desktopIcon->setSelected(selected);
	}
}

bool MainWindow::isShiftPressed(QMouseEvent *event) {
	Qt::KeyboardModifiers modifiers = event->modifiers();
	return (
		modifiers.testFlag(Qt::ShiftModifier) ||
		modifiers.testFlag(Qt::ControlModifier)
				);
}

QPoint MainWindow::gridToClient(QPoint pos) {
	int gridX = pos.x();
	int gridY = pos.y();
	return QPoint(OFFSET_X + (gridX * desiredSpacing + gridX * W_WIDTH), OFFSET_Y + (gridY * desiredSpacing + gridY * W_HEIGHT));
}

QPoint MainWindow::clientToGrid(QPoint pos) {
	int gridX = (pos.x() - OFFSET_X + desiredSpacing / 2) / (W_WIDTH + desiredSpacing);
	int gridY = (pos.y() - OFFSET_Y + desiredSpacing / 2) / (W_HEIGHT + desiredSpacing);

	gridX = qMax(gridX, 0);
	gridX = qMin(gridX, desiredCols - 1);

	gridY = qMax(gridY, 0);
	gridY = qMin(gridY, desiredRows - 1);

	return QPoint(gridX, gridY);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {

	DesktopIcon* desktopIcon = dynamic_cast<DesktopIcon*>(this->childAt(event->pos()));

	if (!isShiftPressed(event) && (!desktopIcon || !desktopIcon->isSelected())) {
		setAllIconsSelection(false);
	}

	if (desktopIcon) {
		desktopIcon->setSelected(true);

		int minLeft = width();
		int minTop = height();
		int maxRight = 0;
		int maxBottom = 0;


		foreach (DesktopIcon* desktopIcon, newList) {
			if (!desktopIcon->isSelected()) continue;
			QRect desktopIconRect = desktopIcon->geometry();
			minLeft = qMin(minLeft, desktopIconRect.left());
			minTop = qMin(minTop, desktopIconRect.top());
			maxRight = qMax(maxRight, desktopIconRect.right());
			maxBottom = qMax(maxBottom, desktopIconRect.bottom());
		}

		int pmWidth = maxRight - minLeft + 1;
		int pmHeight = maxBottom - minTop + 1;
		dragPixmap = dragPixmap.scaled(pmWidth, pmHeight);
		dragPixmap.fill(QColor("transparent"));


		foreach (DesktopIcon* desktopIcon, newList) {
			if (!desktopIcon->isSelected()) continue;
			desktopIcon->render(&dragPixmap, QPoint(desktopIcon->x() - minLeft, desktopIcon->y() - minTop), QRegion(), 0);
		}


		pressPoint = QPoint(event->x() - minLeft, event->y() - minTop);

	}


	else {
		pressPoint = event->pos();
		rubberBand->setGeometry(QRect(pressPoint, QSize()));
		rubberBand->show();
		foreach (DesktopIcon* desktopIcon, newList) {
			desktopIcon->setProperty("selected", desktopIcon->isSelected());
		}
	}


}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {

	if (rubberBand->isVisible()) {

		QRect selectionArea = QRect(pressPoint, event->pos());
		selectionArea = selectionArea.normalized();
		selectionArea.setLeft(qMax(selectionArea.left(), -1));
		selectionArea.setTop(qMax(selectionArea.top(), -1));
		selectionArea.setRight(qMin(selectionArea.right(), this->width() - 1));
		selectionArea.setBottom(qMin(selectionArea.bottom(), this->height() - 1));
		rubberBand->setGeometry(selectionArea);

		foreach (DesktopIcon* desktopIcon, newList) {
			bool savedState = desktopIcon->property("selected").toBool();
			if (selectionArea.intersects(desktopIcon->geometry())) {
				savedState = !savedState;
			}
			desktopIcon->setSelected(savedState);
		}


	}

	else {

		QDrag dragger(this);


		QList<QUrl> urls;
		QMimeData *mimeData = new QMimeData;


		foreach (DesktopIcon* desktopIcon, newList) {
			if (!desktopIcon->isSelected()) continue;
			urls.push_back(desktopIcon->getPath());

		}

		mimeData->setUrls(urls);
		dragger.setMimeData(mimeData);
		dragger.setPixmap(dragPixmap);
		dragger.setHotSpot(pressPoint);


//		QPixmap pm = QCursor(Qt::PointingHandCursor).pixmap();
//		qDebug() << pm;
//		QPixmap pm = new QPixmap();
//		dragger.setDragCursor(pm, Qt::MoveAction);

		qDebug() << dragger.exec(Qt::CopyAction | Qt::MoveAction, Qt::MoveAction) << 1;

//		QKeySequence::SelectAll
	}

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {

	if (rubberBand->isVisible()) {
		qDebug() << rubberBand->pos() << rubberBand->size();
		rubberBand->hide();
	}

//	else if (isShiftPressed(event)) {
//		DesktopIcon* desktopIcon = dynamic_cast<DesktopIcon*>(this->childAt(event->pos()));
//		if (desktopIcon && desktopIcon->isSelected()) {
//			desktopIcon->setSelected(false);
//		}
//	}



}

bool MainWindow::event(QEvent *event) {
	if (event->type() == QEvent::WindowActivate || event->type() == QEvent::WindowDeactivate) {
		foreach (DesktopIcon* desktopIcon, newList) {
			desktopIcon->setActive(event->type() == QEvent::WindowActivate);
		}
	}
	return QWidget::event(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
	if (event->matches(QKeySequence::SelectAll)) {
		setAllIconsSelection(true);
	}
}

void MainWindow::dropEvent(QDropEvent *event) {

	event->acceptProposedAction();

	foreach (DesktopIcon* desktopIcon, newList) {
		if (!desktopIcon->isSelected()) continue;

		QPoint gridPos = clientToGrid(event->pos());

		desktopIcon->setProperty("col", gridPos.x());
		desktopIcon->setProperty("row", gridPos.y());

		QPoint clientPos = gridToClient(gridPos);
		desktopIcon->move(clientPos);

	}

}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {


	qDebug() << event->possibleActions();
	qDebug() << "DragEnter";
	qDebug() << event->mimeData()->urls();
	qDebug() << event->dropAction();
	qDebug() << event->proposedAction();



//	event->setDropAction(Qt::CopyAction);
//	event->setDropAction(Qt::LinkAction);
	event->setDropAction(Qt::MoveAction);
//	event->acceptProposedAction();
	event->accept();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event) {
//	qDebug() << event->pos();

//	event->setDropAction(Qt::IgnoreAction);
//	event->acceptProposedAction();
	//	event->set
}





void MainWindow::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.setPen(QPen(QColor("#000000"), 0.4, Qt::DotLine));


	painter.drawRect(OFFSET_X, OFFSET_Y, this->width() - OFFSET_X * 2, this->height() - OFFSET_Y * 2);


	for (int col = 1; col < desiredCols; col++) {
		painter.drawLine(OFFSET_X + (col * desiredSpacing + col * W_WIDTH), OFFSET_Y, OFFSET_X + (col * desiredSpacing + col * W_WIDTH), this->height() - OFFSET_Y);
		painter.drawLine(OFFSET_X + (col * desiredSpacing - desiredSpacing + col * W_WIDTH), OFFSET_Y, OFFSET_X + (col * desiredSpacing - desiredSpacing + col * W_WIDTH), this->height() - OFFSET_Y);
	}

	for (int row = 1; row < desiredRows; row++) {
		painter.drawLine(OFFSET_X, OFFSET_Y + (row * desiredSpacing + row * W_HEIGHT), this->width() - OFFSET_X, OFFSET_Y + (row * desiredSpacing + row * W_HEIGHT));
		painter.drawLine(OFFSET_X, OFFSET_Y + (row * desiredSpacing - desiredSpacing + row * W_HEIGHT), this->width() - OFFSET_X, OFFSET_Y + (row * desiredSpacing - desiredSpacing + row * W_HEIGHT));
	}


}




void MainWindow::resizeEvent(QResizeEvent *event) {
	recalcGrid();
	realignIcons();
}
