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
	uint8_t borderRadius = 5;
	uint8_t paddingLeftRight = 5;
	uint8_t paddingTopBottom = 3;

	QIcon icon;
	QString path;
	QString name;
	bool active = false;
	bool selected = false;

	QColor bgColor = QColor("transparent");
	QColor activeSelectionBgColor = QColor("#308CC6");
	QColor inactiveSelectionBgColor = QColor("#D0D0D0");

	QColor textColor = QColor("#FFFFFF");
	QColor activeSelectionTextColor = QColor("#FFFFFF");
	QColor inactiveSelectionTextColor = QColor("#6C6C6C");

	QPainterPath buildRoundedRectPath(QRect rect, uint8_t border);
	void drawLine(QPainter *painter, QFontMetrics* fontMetrics, int y, QString text, QString text2);

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