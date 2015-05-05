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
#include "jsApiHandler.h"

std::string StateJSON;

QJsApiHandler * Instance = 0;

void QJsApiHandler::SetInstance(QJsApiHandler * instance)
{
    Instance = instance;
}

void QJsApiHandler::Request(QString json)
{
    QString result = OnEvent(json);

    emit Success(result);
}

QString QJsApiHandler::OnEvent(const QString & json)
{
    return "OK from C++, " + json;
}

void QJsApiHandler::UpdateJSONState(const std::string & json)
{
    if (Instance == 0)
        return;

    StateJSON = json;

    emit Instance->Refresh();
}

QString QJsApiHandler::GetStateJSON(void)
{
    //printf("%s\n", StateJSON.c_str());

    return QString::fromStdString(StateJSON);
}
