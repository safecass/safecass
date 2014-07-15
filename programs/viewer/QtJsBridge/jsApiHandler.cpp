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

void QJsApiHandler::UpdateJSONState(const QString & json)
{
    emit Update(json);
}

QString QJsApiHandler::OnEvent(const QString & json)
{
    return "OK from C++, " + json;
}

QString QJsApiHandler::GetStateJSON(void)
{
const std::string json="[ { \"children\" : [ { \"children\" : [ { \"color\" : \"green\", \"name\" : \"s_F\" }, { \"color\" : \"white\", \"name\" : \"s_A\" }, { \"children\" : [ { \"color\" : \"white\", \"name\" : \"prv.intfc.name.1\" }, { \"color\" : \"white\", \"name\" : \"prv.intfc.name.2\" } ], \"color\" : \"cyan\", \"name\" : \"s_A Provided\" }, { \"children\" : [ { \"color\" : \"white\", \"name\" : \"req.intfc.name.1\" }, { \"color\" : \"white\", \"name\" : \"req.intfc.name.2\" } ], \"color\" : \"cyan\", \"name\" : \"s_A Required\" } ], \"color\" : \"white\", \"name\" : \"componentA\" }, { \"children\" : [ { \"color\" : \"white\", \"name\" : \"s_F\" }, { \"color\" : \"white\", \"name\" : \"s_A\" }, { \"children\" : [ { \"color\" : \"white\", \"name\" : \"prv.intfc.name.1\" }, { \"color\" : \"white\", \"name\" : \"prv.intfc.name.2\" } ], \"color\" : \"cyan\", \"name\" : \"s_A Provided\" }, { \"children\" : [ { \"color\" : \"white\", \"name\" : \"req.intfc.name.1\" }, { \"color\" : \"white\", \"name\" : \"req.intfc.name.2\" } ], \"color\" : \"cyan\", \"name\" : \"s_A Required\" } ], \"color\" : \"white\", \"name\" : \"componentB\" } ], \"color\" : \"#ff0000\", \"name\" : \"LCM\" }, { \"children\" : [ { \"children\" : [ { \"color\" : \"white\", \"name\" : \"s_F\" }, { \"color\" : \"white\", \"name\" : \"s_A\" }, { \"children\" : [ { \"color\" : \"white\", \"name\" : \"prv.intfc.name.1\" }, { \"color\" : \"white\", \"name\" : \"prv.intfc.name.2\" } ], \"color\" : \"cyan\", \"name\" : \"s_A Provided\" }, { \"children\" : [ { \"color\" : \"white\", \"name\" : \"req.intfc.name.1\" }, { \"color\" : \"white\", \"name\" : \"req.intfc.name.2\" } ], \"color\" : \"cyan\", \"name\" : \"s_A Required\" } ], \"color\" : \"white\", \"name\" : \"componentA\" }, { \"children\" : [ { \"color\" : \"white\", \"name\" : \"s_F\" }, { \"color\" : \"white\", \"name\" : \"s_A\" }, { \"children\" : [ { \"color\" : \"white\", \"name\" : \"prv.intfc.name.1\" }, { \"color\" : \"white\", \"name\" : \"prv.intfc.name.2\" } ], \"color\" : \"cyan\", \"name\" : \"s_A Provided\" }, { \"children\" : [ { \"color\" : \"white\", \"name\" : \"req.intfc.name.1\" }, { \"color\" : \"white\", \"name\" : \"req.intfc.name.2\" } ], \"color\" : \"cyan\", \"name\" : \"s_A Required\" } ], \"color\" : \"white\", \"name\" : \"componentB\" } ], \"color\" : \"#ff0000\", \"name\" : \"Control\" } ]";
    
    return QString::fromStdString(json);
}
