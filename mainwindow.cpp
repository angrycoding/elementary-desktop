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

QList<DesktopIcon*> allIcons;



MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {

	setAcceptDrops(true);

	dragPixmap = QPixmap(1, 1);


	rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    int iconWidth = 110;
    int iconHeight = 110;

    auto list = QDir("/Users/ruslanmatveev").entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    int row = 0;
    int col = 0;

	QFileIconProvider iconProvider;

    foreach (QFileInfo info, list) {

		DesktopIcon *button = new DesktopIcon(this);
		button->setPath(info.absoluteFilePath());
		button->setIcon(iconProvider.icon(info));

		allIcons.push_back(button);


        button->resize(iconWidth, iconHeight);
        button->move(col * (iconWidth + 10), row * (iconHeight + 10));

        col++;
        if (col > 6) {
            col = 0;
            row++;
        }
    }


}

MainWindow::~MainWindow() {
	delete this->rubberBand;
}

void MainWindow::unselectAll() {
	for (int c = 0; c < allIcons.length(); c++) {
		allIcons.at(c)->setSelected(false);
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
		unselectAll();
	}

	if (desktopIcon) {
		desktopIcon->setSelected(true);

		int minLeft = width();
		int minTop = height();
		int maxRight = 0;
		int maxBottom = 0;

		for (int c = 0; c < allIcons.length(); c++) {
			if (!allIcons.at(c)->isSelected()) continue;
			DesktopIcon* desktopIcon = allIcons.at(c);
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


		for (int c = 0; c < allIcons.length(); c++) {
			if (!allIcons.at(c)->isSelected()) continue;
			DesktopIcon* desktopIcon = allIcons.at(c);
			desktopIcon->render(&dragPixmap, QPoint(desktopIcon->x() - minLeft, desktopIcon->y() - minTop), QRegion(), 0);
		}


		pressPoint = QPoint(event->x() - minLeft, event->y() - minTop);

	}


	else {
		pressPoint = event->pos();
		rubberBand->setGeometry(QRect(origin, QSize()));
		rubberBand->show();
		for (int c = 0; c < allIcons.length(); c++) {
			DesktopIcon* desktopIcon = allIcons.at(c);
			desktopIcon->setProperty("selected", desktopIcon->isSelected());
		}
	}


}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {

	if (rubberBand->isVisible()) {
		QRect selectionArea = QRect(pressPoint, event->pos());
		rubberBand->setGeometry(selectionArea.normalized());
		for (int c = 0; c < allIcons.length(); c++) {
			DesktopIcon* desktopIcon = allIcons.at(c);
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

		for (int c = 0; c < allIcons.length(); c++) {
			if (!allIcons.at(c)->isSelected()) continue;
			DesktopIcon* di = allIcons.at(c);
			urls.push_back(di->getPath());

		}

		mimeData->setUrls(urls);
		dragger.setMimeData(mimeData);
		dragger.setPixmap(dragPixmap);
		dragger.setHotSpot(pressPoint);
		qDebug() << dragger.exec(Qt::CopyAction);

//		QKeySequence::SelectAll
	}

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {

	DesktopIcon* desktopIcon = dynamic_cast<DesktopIcon*>(this->childAt(event->pos()));


//	if (isShiftPressed(event)) {
//		if (desktopIcon)
//		return;
//	}

	if (rubberBand->isVisible()) {
		qDebug() << rubberBand->pos() << rubberBand->size();
		rubberBand->hide();
	}



}


bool MainWindow::event(QEvent *event) {
	if (event->type() == QEvent::WindowActivate || event->type() == QEvent::WindowDeactivate) {
		for (int c = 0; c < allIcons.length(); c++) {
			allIcons.at(c)->setActive(event->type() == QEvent::WindowActivate);
		}
	}
	return QWidget::event(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
	if (event->matches(QKeySequence::SelectAll)) {
		for (int c = 0; c < allIcons.length(); c++) {
			allIcons.at(c)->setSelected(true);
		}
	}
}

void MainWindow::dropEvent(QDropEvent *event) {
	qDebug() << event->dropAction();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
	qDebug() << event->possibleActions();
	qDebug() << "DragEnter";
	qDebug() << event->mimeData()->urls();
	qDebug() << event->dropAction();
	qDebug() << event->proposedAction();
//	event->setDropAction(Qt::CopyAction);
	event->setDropAction(Qt::MoveAction);
	event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event) {
	event->setDropAction(Qt::MoveAction);
//	event->set
}
