//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung, Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 9, 2015
// Last revision: Apr 9, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _COMPONENTS_HEADER
#define _COMPONENTS_HEADER

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsCollectorState.h>

//
// Component: Sensor wrapper
//
class SensorWrapper: public mtsTaskPeriodic {
protected:
    double StartTimestamp;
    double SensorValue, SensorValue2;
    int Step;
public:
    SensorWrapper(const std::string & name, double period);
    ~SensorWrapper() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void);
    void Run(void);
    void Cleanup(void) {}

    void TestFilter(void);
    void TestEvent(void);
};

//
// Component: Control
//
class Control: public mtsTaskPeriodic {
protected:
    mtsFunctionRead GetSensorValue;
    double ControlValue;
public:
    Control(const std::string & name, double period);
    ~Control() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void);
    void Cleanup(void) {}
};

//
// Component: User Interface
//
class UI: public mtsTaskPeriodic {
protected:
    mtsFunctionRead GetControlValue;
    double UIValue;
public:
    UI(const std::string & name, double period);
    ~UI() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void);
    void Cleanup(void) {}
};
#endif // _COMPONENTS_HEADER
