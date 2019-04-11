#include "desktopicon.h"
#include <QPainter>
#include <QIcon>
#include <QDebug>
#include <QFileIconProvider>
#include <QApplication>


#define BORDER_RADIUS 3
#define PADDING_LEFT_RIGHT 5
#define PADDING_TOP_BOTTOM 3
#define IMAGE_TEXT_GAP 4



#define TOP_LEFT 1
#define TOP_RIGHT 2
#define BOTTOM_LEFT 4
#define BOTTOM_RIGHT 8
#define TOP_LEFT_OUTSET 16
#define TOP_RIGHT_OUTSET 32

#define BORDER_TOP TOP_LEFT | TOP_RIGHT
#define BORDER_BOTTOM BOTTOM_LEFT | BOTTOM_RIGHT

#define BORDER_ALL TOP_LEFT | TOP_RIGHT | BOTTOM_LEFT | BOTTOM_RIGHT
#define BORDER_2 TOP_LEFT_OUTSET | TOP_RIGHT_OUTSET | BOTTOM_LEFT | BOTTOM_RIGHT


DesktopIcon::DesktopIcon(QWidget *parent) : QWidget(parent) {
    this->font = new QFont("Lucida Grande");
    font->setPixelSize(11);
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
    return {firstLine, secondLine};
}

QPainterPath drawRoundedRect(QRect rect, uint8_t border = 0) {

    QPainterPath path;
    int borderRadius = BORDER_RADIUS;
    int left = rect.left();
    int top = rect.top();
    int right = left + rect.width();
    int bottom = top + rect.height();

    path.moveTo(left, top + borderRadius);

    if (border & TOP_LEFT)
        path.cubicTo(left, top + borderRadius, left, top, left + borderRadius, top);

    else if (border & TOP_LEFT_OUTSET)
        path.cubicTo(left, top + borderRadius, left, top, left - borderRadius, top);

    else path.lineTo(left, top);

    path.lineTo(right - borderRadius, top);

    if (border & TOP_RIGHT)
        path.cubicTo(right - borderRadius, top, right, top, right, top + borderRadius);

    else if (border & TOP_RIGHT_OUTSET) {
        path.lineTo(right + borderRadius, top);
        path.cubicTo(right + borderRadius, top, right, top, right, top + borderRadius);
    }

    else path.lineTo(right, top);

    path.lineTo(right, bottom - borderRadius);

    if (border & BOTTOM_RIGHT)
        path.cubicTo(right, bottom - borderRadius, right, bottom, right - borderRadius, bottom);
    else path.lineTo(right, bottom);


    path.lineTo(left + borderRadius, bottom);

    if (border & BOTTOM_LEFT)
        path.cubicTo(left + borderRadius, bottom, left, bottom, left, bottom - borderRadius);
    else path.lineTo(left, bottom);


    return path;
}

void drawLine(QPainter *painter, int x, int y, QString text, QString text2, int someTestWidth, QColor color) {

    QFontMetrics fontMetrics = painter->fontMetrics();

    QRect t1 = fontMetrics.boundingRect(text);
    QRect t2 = fontMetrics.boundingRect(text2);

    int text1Width = t1.width();
    int text2Width = t2.width();

    int text1Height = t1.height() + PADDING_TOP_BOTTOM * 2;
    int text2Height = t2.height() + PADDING_TOP_BOTTOM * 2;

    bool myFlag = text2Width && text2Width > text1Width - BORDER_RADIUS * 2.5;



//    qDebug() << myFlag << text << text2;
//    qDebug() << text2Width << text1Width;

    text1Width += PADDING_LEFT_RIGHT * 2;
    text2Width += PADDING_LEFT_RIGHT * 2;


    QRect box1(x + (someTestWidth - text1Width) / 2, y, text1Width, text1Height);

    painter->fillPath(drawRoundedRect(box1, myFlag ? BORDER_TOP : BORDER_ALL), color);
    painter->drawText(box1, Qt::AlignCenter | Qt::AlignHCenter, text);


    if (text2.length() == 0) return;
    QRect box2(myFlag ? box1.left() : box1.left() + (box1.width() - text2Width) / 2, box1.bottom() + 1, myFlag ? text1Width : text2Width, text2Height - PADDING_TOP_BOTTOM);
    painter->fillPath(drawRoundedRect(box2, myFlag ? BORDER_BOTTOM : BORDER_2), color);
    box2.setY(box2.top() - PADDING_TOP_BOTTOM);
    painter->drawText(box2, Qt::AlignCenter | Qt::AlignHCenter, text2);

}




void DesktopIcon::setFileInfo(QFileInfo fileInfo)
{
    this->fileInfo = fileInfo;
    QFileIconProvider ip;
    this->icon = ip.icon(fileInfo);
}



void DesktopIcon::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setFont(*this->font);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing, true);
    QFontMetrics fontMetrics = painter.fontMetrics();


    int width = this->width();
    int height = this->height();

    int doubleHeight = fontMetrics.height() * 2 + PADDING_TOP_BOTTOM * 3;

    int iconSize = qMin(width, height - doubleHeight - IMAGE_TEXT_GAP);


    painter.setPen(QColor(selected && !active ? "#6C6C6C" : "#FFFFFF"));


    QColor cccColor = (
        selected && active ? QColor("#3067D2") :
        selected ? QColor("#D0D0D0") :
        QColor(Qt::transparent)
    );


    painter.drawPixmap((width - iconSize) / 2, 0, iconSize, iconSize, icon.pixmap(iconSize, iconSize));

    QStringList splitted = divideText(&fontMetrics, this->fileInfo.fileName(), width - PADDING_LEFT_RIGHT * 2);
    drawLine(&painter, 0, height - doubleHeight, splitted.at(0), splitted.at(1), width, cccColor);


//    painter.setPen(QColor("#FF0000"));
//    painter.drawRect(QRect(0, 0, width, height));
}
