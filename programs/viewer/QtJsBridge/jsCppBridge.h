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
#pragma once

#include <QObject>

class QWebFrame;
class QJsApiHandler;
class QJsApiProxy;
class QJsCppBridge: public QObject
{
    Q_OBJECT

public:
    QJsCppBridge(QWebFrame * pWebFrame);
    ~QJsCppBridge();

public slots:
    void bridge_javascriptWindowObjectCleared();
    void bridge_loadFinished(bool b);

private:
    QWebFrame     * WebFrame;
    QJsApiHandler * JsApiHandler;
    QJsApiProxy   * JsApiProxy;
};
