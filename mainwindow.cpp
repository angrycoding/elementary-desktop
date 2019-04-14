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


int iconWidth = 110;
int iconHeight = 110;
QHash<QString, DesktopIcon*> newList;


int rangedRand(unsigned int min, unsigned int max){
	return (qrand() % (max+1-min)) + min;
}

void MainWindow::updateDesktop(QStringList files) {

	foreach (QString path, newList.keys()) {
		DesktopIcon *desktopIcon = newList[path];
		if (!files.contains(path)) {
			newList.remove(path);
			delete desktopIcon;
		}
	}

	foreach (QString path, files) {
		if (newList.contains(path)) continue;

		int x = rangedRand(50, 1000);
		int y = rangedRand(50, 700);

		DesktopIcon *button = new DesktopIcon(this);
		button->setFont(this->font());
		button->setPath(path);
		button->setIcon(iconProvider.icon(QFileInfo(path)));

		qDebug() << path << QFileInfo(path).birthTime() << QFileInfo(path).size() << QFileInfo(path);

		button->setParent(this);

		button->resize(iconWidth, iconHeight);
		button->move(x, y);
		button->show();

		newList.insert(path, button);
	}


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
		rubberBand->setGeometry(selectionArea.normalized());
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
		qDebug() << "HERE???" << event->pos();
		desktopIcon->move(event->pos() - pressPoint);
//		urls.push_back(desktopIcon->getPath());

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
	qDebug() << event->pos();

//	event->setDropAction(Qt::IgnoreAction);
//	event->acceptProposedAction();
	//	event->set
}
