//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
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

//protected:
    //class QObjectString: public QObject {
        //std::string StdString;
    //public:
        //void SetString(const std::string & str);
    //};

    QWebFrame     * WebFrame;
    QJsApiHandler * JsApiHandler;
    QJsApiProxy   * JsApiProxy;
    //QObjectString * StateJSON;

public:
    QJsCppBridge(QWebFrame * pWebFrame);
    ~QJsCppBridge();

    //void SetStateJSON(const std::string & json);

public slots:
    void bridge_javascriptWindowObjectCleared();
    void bridge_loadFinished(bool b);

};
