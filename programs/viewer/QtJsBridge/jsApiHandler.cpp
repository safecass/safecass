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
#include "jsApiHandler.h"

void QJsApiHandler::Request(QString json)
{
    QString result = OnEvent(json);

    emit Success(result);
}

QString QJsApiHandler::OnEvent(const QString & json)
{
    return "OK from C++, " + json;
}
