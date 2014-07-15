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
#include <string>

class QJsApiHandler: public QObject
{
    Q_OBJECT

signals:
    void Success(QString json);
    void Error(QString json);
    void Refresh(void);

public slots:
    void Request(QString json);

public slots:
    QString OnEvent(const QString & json);
    QString GetStateJSON(void);

public:
    static void SetInstance(QJsApiHandler * instance);
    static void UpdateJSONState(const std::string & json);
};
