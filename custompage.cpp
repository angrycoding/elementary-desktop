#include "custompage.h"

#include <QWebChannel>
#include <QWebEngineSettings>
#include <QWebEnginePage>
#include <QDebug>

void CustomPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID)
{
    qDebug() << QString("Javascript Console: Line: %1, Source: %2, %3").arg(lineNumber).arg(sourceID).arg(message);
}

CustomPage::CustomPage()
{
//    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
//    settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);

//    mWebChannel=new QWebChannel;
//    mWebChannel->registerObject(QString("qtObject"), this);
//    setWebChannel(mWebChannel);
}
