//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 14, 2014
// Last revision: Jul 14, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "jsApiProxy.h"

#include <QtWebKit/QWebFrame>

QJsApiProxy::QJsApiProxy(QWebFrame * webFrame): WebFrame(webFrame)
{
}

QString QJsApiProxy::OnEvent(const QString & para)
{
    if (WebFrame) {
        static QString functionSignature = "onEventStub(%1)";
        QString functionInvoke = functionSignature.arg(para);
        QVariant result = WebFrame->evaluateJavaScript(functionInvoke);
        return result.toString();
    }

    return "ERROR_DISCONNECTED";
}
