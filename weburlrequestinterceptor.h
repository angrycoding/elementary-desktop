#ifndef WEBURLREQUESTINTERCEPTOR_H
#define WEBURLREQUESTINTERCEPTOR_H

#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineUrlRequestJob>

class WebUrlRequestInterceptor: public QWebEngineUrlSchemeHandler {
public:
    WebUrlRequestInterceptor(QObject *p = 0);
     void requestStarted(QWebEngineUrlRequestJob *request);
};

#endif // WEBURLREQUESTINTERCEPTOR_H
