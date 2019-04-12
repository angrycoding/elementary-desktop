#include "desktopicon.h"
#include <QPainter>
#include <QIcon>
#include <QDebug>
#include <QApplication>


QString splitText(QFontMetrics* fontMetrics, QString secondLine, int maxWidth) {
	QString result = "";
	for (int c = 0; c < secondLine.length(); c++) {
		QChar ch = secondLine.at(c);
		if (fontMetrics->width(result + ch) > maxWidth) break;
		result += ch;
	}
	return result;
}

QStringList divideText(QFontMetrics* fontMetrics, QString text, int maxWidth) {
	QString firstLine = splitText(fontMetrics, text, maxWidth);
	QString secondLine = text.mid(firstLine.length());
	if (secondLine.length()) {
		int firstLineWidth = fontMetrics->width(firstLine);
		int secondLineWidth = fontMetrics->width(secondLine);
		if (secondLineWidth > firstLineWidth) {
			QString xStr = "…" + secondLine.right(6);
			int xWidth = fontMetrics->width(xStr);
			secondLine = splitText(fontMetrics, secondLine.mid(0, -6), firstLineWidth - xWidth) + xStr;
		}
	}
	return {firstLine.trimmed(), secondLine.trimmed()};
}

DesktopIcon::DesktopIcon(QWidget *parent) : QWidget(parent) {
	this->font = new QFont("Lucida Grande");
	font->setPixelSize(12);
}

DesktopIcon::~DesktopIcon() {
	delete this->font;
}

void DesktopIcon::setSelected(bool selected) {
	this->selected = selected;
	this->update();
}

void DesktopIcon::setActive(bool active) {
	this->active = active;
	this->update();
}

bool DesktopIcon::isSelected() {
	return selected;
}


void DesktopIcon::setIcon(const QIcon &icon) {
	this->icon = icon;
	this->update();
}

void DesktopIcon::setPath(QString path) {
	this->path = path;
	this->name = QFileInfo(path).fileName();
	this->update();
}

QString DesktopIcon::getPath() {
	return this->path;
}

QPainterPath DesktopIcon::buildRoundedRectPath(QRect rect, uint8_t border) {

	QPainterPath path;
	int left = rect.left();
	int top = rect.top();
	int right = left + rect.width();
	int bottom = top + rect.height();

	path.moveTo(left, top + borderRadius);


	if (border & TOP_INSET) {
		path.cubicTo(left, top + borderRadius, left, top, left + borderRadius, top);
	} else if (border & TOP_OUTSET) {
		path.cubicTo(left, top + borderRadius, left, top, left - borderRadius, top);
	} else {
		path.lineTo(left, top);
	}

	path.lineTo(right - borderRadius, top);

	if (border & TOP_INSET) {
		path.cubicTo(right - borderRadius, top, right, top, right, top + borderRadius);
	} else if (border & TOP_OUTSET) {
		path.lineTo(right + borderRadius, top);
		path.cubicTo(right + borderRadius, top, right, top, right, top + borderRadius);
	} else {
		path.lineTo(right, top);
	}

	path.lineTo(right, bottom - borderRadius);

	if (border & BOTTOM_INSET) {
		path.cubicTo(right, bottom - borderRadius, right, bottom, right - borderRadius, bottom);
	} else {
		path.lineTo(right, bottom);
	}

	path.lineTo(left + borderRadius, bottom);

	if (border & BOTTOM_INSET) {
		path.cubicTo(left + borderRadius, bottom, left, bottom, left, bottom - borderRadius);
	} else {
		path.lineTo(left, bottom);
	}

	return path;
}

void DesktopIcon::drawLine(QPainter *painter, QFontMetrics *fontMetrics, int y, QString text, QString text2, QColor color) {

	QRect text1Rect = fontMetrics->boundingRect(text);
	QRect text2Rect = fontMetrics->boundingRect(text2);

	int text1Width = text1Rect.width();
	int text2Width = text2Rect.width();

	int text1Height = text1Rect.height() + paddingTopBottom * 2;
	int text2Height = text2Rect.height() + paddingTopBottom * 2;

	bool myFlag = text2Width && text2Width > text1Width - borderRadius * 2.5;

	text1Width += paddingLeftRight * 2;
	text2Width += paddingLeftRight * 2;


	QRect box1((this->width() - text1Width) / 2, y, text1Width, text1Height);

	painter->fillPath(buildRoundedRectPath(box1, myFlag ? TOP_INSET : TOP_INSET | BOTTOM_INSET), color);
	painter->drawText(box1, Qt::AlignCenter | Qt::AlignHCenter, text);

	if (text2.length() == 0) return;
	QRect box2(myFlag ? box1.left() : box1.left() + (box1.width() - text2Width) / 2, box1.bottom() + 1, myFlag ? text1Width : text2Width, text2Height - paddingTopBottom);
	painter->fillPath(buildRoundedRectPath(box2, myFlag ? BOTTOM_INSET : TOP_OUTSET | BOTTOM_INSET), color);
	box2.setY(box2.top() - paddingTopBottom);
	painter->drawText(box2, Qt::AlignCenter | Qt::AlignHCenter, text2);

}






void DesktopIcon::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.setFont(*this->font);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
	QFontMetrics fontMetrics = painter.fontMetrics();


    int width = this->width();
    int height = this->height();

	int doubleHeight = fontMetrics.height() * 2 + paddingTopBottom * 3;

	int iconSize = qMin(width, height - doubleHeight - iconTextGap);


    painter.setPen(QColor(selected && !active ? "#6C6C6C" : "#FFFFFF"));


    QColor cccColor = (
        selected && active ? QColor("#3067D2") :
        selected ? QColor("#D0D0D0") :
        QColor(Qt::transparent)
    );


	painter.drawPixmap((width - iconSize) / 2, 0, iconSize, iconSize, icon.pixmap(iconSize, iconSize));

	QStringList splitted = divideText(&fontMetrics, this->name, width - paddingLeftRight * 2);
	drawLine(&painter, &fontMetrics, height - doubleHeight, splitted.at(0), splitted.at(1), cccColor);


//    painter.setPen(QColor("#FF0000"));
//    painter.drawRect(QRect(0, 0, width, height));
}
