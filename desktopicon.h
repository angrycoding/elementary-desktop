#ifndef DESKTOPICON_H
#define DESKTOPICON_H

#include <QWidget>
#include <QFileInfo>
#include <QIcon>s

class DesktopIcon : public QWidget
{
    Q_OBJECT

private:

    QFont* font;
    QFileInfo fileInfo;
    QIcon icon;
    bool active = false;
    bool selected = false;

public:
    explicit DesktopIcon(QWidget *parent = nullptr);
    ~DesktopIcon();

    void setFileInfo(QFileInfo fileInfo);
    void setSelected(bool selected);
    void setActive(bool active);
    bool isSelected();

public slots:

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // DESKTOPICON_H
