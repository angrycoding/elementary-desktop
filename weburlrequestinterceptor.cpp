#include "weburlrequestinterceptor.h"

#include <QDebug>
#include <QBuffer>
#include <QFile>


WebUrlRequestInterceptor::WebUrlRequestInterceptor(QObject *p)
  :QWebEngineUrlSchemeHandler(p)
{

}


void WebUrlRequestInterceptor::requestStarted(QWebEngineUrlRequestJob *request) {
    const QUrl url = request->requestUrl();
    QFile *file = new QFile(":/resources" + url.path(), request);
    file->open(QIODevice::ReadOnly);
    request->reply("text/html", file);

}
