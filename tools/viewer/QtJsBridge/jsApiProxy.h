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

// The call from C++ code to Java script is via this class.
// This class will delegate the call the Java script function onEventStub.
// In the html page, the Java script function "function onEventStub(para) { return "NO_IMP";}" 
// should be implemented to response the call.
class QJsApiProxy: QObject
{
    Q_OBJECT

public:
    QJsApiProxy(QWebFrame * webFrame);

    QString OnEvent(const QString & para);

private:
    QWebFrame * WebFrame;
};
