#ifndef DESKTOPICON_H
#define DESKTOPICON_H

#include <QWidget>
#include <QIcon>

class DesktopIcon : public QWidget {

	Q_OBJECT

private:

	enum borderKind {
		TOP_INSET = 1,
		TOP_OUTSET = 2,
		BOTTOM_INSET = 4,
		BOTTOM_OUTSET = 8
	};

	uint8_t iconTextGap = 3;
	uint8_t borderRadius = 4;
	uint8_t paddingLeftRight = 5;
	uint8_t paddingTopBottom = 3;

	QIcon icon;
	QString path;
	QString name;
	bool active = false;
	bool selected = false;

	QPainterPath buildRoundedRectPath(QRect rect, uint8_t border);
	void drawLine(QPainter *painter, QFontMetrics* fontMetrics, int y, QString text, QString text2, QColor color);

public:

	explicit DesktopIcon(QWidget *parent = nullptr);
	~DesktopIcon();

	void setPath(QString path);
	QString getPath();
	void setIcon(const QIcon &icon);
	void setSelected(bool selected);
	void setActive(bool active);
	bool isSelected();

protected:

	void paintEvent(QPaintEvent *event);

};

#endif // DESKTOPICON_H
