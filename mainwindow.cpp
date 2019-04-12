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




MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {

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
    auto list = this->findChildren<DesktopIcon*>();
    for (int c = 0; c < list.length(); c++) {
        list.at(c)->setSelected(false);
    }
}





void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
//    DesktopIcon* i = this->findChild<DesktopIcon*>();
//    i->setSelected(true);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {

    QWidget* widget = this->childAt(event->pos());
    Qt::KeyboardModifiers modifiers = event->modifiers();

    bool isShiftPressed = (
        modifiers.testFlag(Qt::ShiftModifier) ||
        modifiers.testFlag(Qt::ControlModifier)
    );

    isDragging = false;

    if (widget) {
        if (QString(widget->metaObject()->className()) == "DesktopIcon") {


            DesktopIcon* icon = (DesktopIcon*)widget;

            if (!icon->isSelected()) {
                if (!isShiftPressed) unselectAll();
                icon->setSelected(true);
            } else if (isShiftPressed) {
                icon->setSelected(false);
            }

            isDragging = true;



        }
    }

    else {
        unselectAll();
        origin = event->pos();
        rubberBand->setGeometry(QRect(origin, QSize()));
        rubberBand->show();
    }

}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {


    if (isDragging) {

        auto list = this->findChildren<DesktopIcon*>();
        for (int c = 0; c < list.length(); c++) {
            if (list.at(c)->isSelected()) {

                DesktopIcon* di = list.at(c);

                QPixmap pm(di->size());
                pm.fill(QColor("transparent"));
                di->render(&pm, QPoint(), QRegion(), 0);

                QDrag dragger(this);
                QMimeData *mimeData = new QMimeData;
                QString filename = "/Users/ruslanmatveev/Desktop/1.png";
                mimeData->setData("text/uri-list", filename.toUtf8());
                dragger.setMimeData(mimeData);
                dragger.setPixmap(pm);
                dragger.setHotSpot(QPoint(50, 50));
                qDebug() << dragger.exec(Qt::CopyAction);

                break;
            }
        }




    } else {
        rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
    }

}

bool MainWindow::event(QEvent *event) {
    if (event->type() == QEvent::WindowActivate ||
            event->type() == QEvent::WindowDeactivate) {
        auto list = this->findChildren<DesktopIcon*>();
        for (int c = 0; c < list.length(); c++) {
            list.at(c)->setActive(event->type() == QEvent::WindowActivate);
        }
    }

    return QWidget::event(event);
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (rubberBand->isVisible()) {
        qDebug() << rubberBand->pos() << rubberBand->size();
        rubberBand->hide();
    }
    isDragging = false;
}

