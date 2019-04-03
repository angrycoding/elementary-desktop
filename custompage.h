#ifndef CUSTOMPAGE_H
#define CUSTOMPAGE_H

#include <QWebEnginePage>

class CustomPage : public QWebEnginePage {
    Q_OBJECT

public:
    CustomPage();

    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override;

private:
    QWebChannel* mWebChannel = nullptr;
};
#endif // CUSTOMPAGE_H
