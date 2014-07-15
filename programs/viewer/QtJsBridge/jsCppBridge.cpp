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
#include <QtWebKit/QWebFrame>
#include <QDebug>
#include <iostream>
#include "jsCppBridge.h"
#include "jsApiHandler.h"
#include "JsApiProxy.h"

QJsCppBridge::QJsCppBridge(QWebFrame * webFrame): WebFrame(webFrame)
{
    if (WebFrame) {
        // Listen to the javaScriptWindowObjectCleared signal.
        // This step is important because WebKit is going to clear all the JavaScript objects when loading a new page,
        // so you need to add back the bridge every time:
        QObject::connect(WebFrame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(bridge_javascriptWindowObjectCleared()));
        QObject::connect(WebFrame, SIGNAL(loadFinished(bool)), this, SLOT(bridge_loadFinished(bool)));
    }

    JsApiHandler = new QJsApiHandler;
    JsApiProxy   = new QJsApiProxy(WebFrame);

    QJsApiHandler::SetInstance(JsApiHandler);
}

QJsCppBridge::~QJsCppBridge()
{
    if (JsApiHandler) {
        delete JsApiHandler;
        JsApiHandler = 0;
    }
    if (JsApiProxy) {
        delete JsApiProxy;
        JsApiProxy = 0;
    }
}

// The jsHandler is exposed as a global object in the Java script.
// It can be used as var result = jsHandler.onEvent("parameter data");
void QJsCppBridge::bridge_javascriptWindowObjectCleared()
{
    if (WebFrame) {
        WebFrame->addToJavaScriptWindowObject("jsHandler", JsApiHandler);
    }
}

void QJsCppBridge::bridge_loadFinished(bool b)
{
    qDebug() << "QJsCppBridge::bridge_loadFinished: " << (int)b;
    if (JsApiProxy) {
        qDebug() << "QJsCppBridge::bridge_loadFinished - call";
        QString result = JsApiProxy->OnEvent("Initialization completes!");
        qDebug() << result;
    }
}

/*void QJsCppBridge::SetStateJSON(const std::string & json)*/
//{
    //StateJSON->
//}

////
////
////
//void QJsCppBridge::QObjectString::SetString(const std::string & str)
//{
/*}*/
