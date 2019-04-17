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

void MainWindow::recalcGrid() {
	qreal width = this->width();
	qreal height = this->height();
	qreal _cols = (width - gridSpacing) / gridWidth;
	qreal _rows = (height - gridSpacing) / gridHeight;
	desktopIconWidth = _cols - gridSpacing;
	desktopIconHeight = _rows - gridSpacing;
	offsetX = ((width + gridSpacing) - _cols * gridWidth) / 2;
	offsetY = ((height + gridSpacing) - _rows * gridHeight) / 2;
	realignIcons();
}

void MainWindow::realignIcons() {
	foreach (DesktopIcon* icon, newList) {

		int col = icon->property("col").toInt();
		int row = icon->property("row").toInt();

		icon->move(offsetX + (col * gridSpacing + col * desktopIconWidth), offsetY + (row * gridSpacing + row * desktopIconHeight));
		icon->resize(desktopIconWidth, desktopIconHeight);
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
		if (row >= gridHeight) {
			row = 0;
			col++;
		}

	}

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
	recalcGrid();
	grabGesture(Qt::SwipeGesture);
}

MainWindow::~MainWindow() {
	delete this->rubberBand;
}

void MainWindow::setGrid(int width, int height, int spacing) {
	gridWidth = width;
	gridHeight = height;
	gridSpacing = spacing;
	recalcGrid();
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
	return QPoint(offsetX + (gridX * gridSpacing + gridX * desktopIconWidth), offsetY + (gridY * gridSpacing + gridY * desktopIconHeight));
}

QPoint MainWindow::clientToGrid(QPoint pos) {
	int gridX = (pos.x() - offsetX + gridSpacing / 2) / (desktopIconWidth + gridSpacing);
	int gridY = (pos.y() - offsetY + gridSpacing / 2) / (desktopIconHeight + gridSpacing);

	gridX = qMax(gridX, 0);
	gridX = qMin(gridX, gridWidth - 1);

	gridY = qMax(gridY, 0);
	gridY = qMin(gridY, gridHeight - 1);

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
	else  if (event->type() == QEvent::Gesture) {
		qDebug() << "OOOOK";
	}
	return QWidget::event(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
	if (event->matches(QKeySequence::SelectAll)) {
		setAllIconsSelection(true);
	}

	else if (event->matches(QKeySequence::MoveToNextChar)) {
	}

	else if (event->matches(QKeySequence::MoveToPreviousChar)) {

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
		desktopIcon->move(clientPos.x(), clientPos.y());

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

	event->acceptProposedAction();


	QPoint pos = event->pos();
	QPoint grid = clientToGrid(pos);

	foreach (DesktopIcon* icon, newList) {
		int col = icon->property("col").toInt();
		int row = icon->property("row").toInt();
		if (col == grid.x() && row == grid.y()) {

			if (icon->isDirectory()) {
				icon->setSelected(true);
			}

			else {
				event->setDropAction(Qt::IgnoreAction);
			}





			return;
		}
	}

	event->setDropAction(Qt::MoveAction);
}





void MainWindow::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.setPen(QPen(QColor("#FF0000"), 0.4, Qt::DotLine));


	painter.drawRect(offsetX, offsetY, this->width() - offsetX * 2, this->height() - offsetY * 2);


	for (int col = 1; col < gridWidth; col++) {
		painter.drawLine(offsetX + (col * gridSpacing + col * desktopIconWidth), offsetY, offsetX + (col * gridSpacing + col * desktopIconWidth), this->height() - offsetY);
		painter.drawLine(offsetX + (col * gridSpacing - gridSpacing + col * desktopIconWidth), offsetY, offsetX + (col * gridSpacing - gridSpacing + col * desktopIconWidth), this->height() - offsetY);
	}

	for (int row = 1; row < gridHeight; row++) {
		painter.drawLine(offsetX, offsetY + (row * gridSpacing + row * desktopIconHeight), this->width() - offsetX, offsetY + (row * gridSpacing + row * desktopIconHeight));
		painter.drawLine(offsetX, offsetY + (row * gridSpacing - gridSpacing + row * desktopIconHeight), this->width() - offsetX, offsetY + (row * gridSpacing - gridSpacing + row * desktopIconHeight));
	}


}




void MainWindow::resizeEvent(QResizeEvent *event) {
	recalcGrid();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
//	qDebug() << event;
//	if (event->delta() > 0) {
//		QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
//		animation->setDuration(250);
//		animation->setStartValue(QPoint(this->pos().x(), this->pos().y()));
//		animation->setEndValue(QPoint(-950, this->pos().y()));
//		animation->start();
//	} else {
//		QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
//		animation->setDuration(250);
//		animation->setStartValue(QPoint(this->pos().x(), this->pos().y()));
//		animation->setEndValue(QPoint(0, this->pos().y()));
//		animation->start();

//	}
}
