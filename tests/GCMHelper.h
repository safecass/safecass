//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 19, 2012
// Last revision: Apr 19, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _SFGCMHelper_h
#define _SFGCMHelper_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

namespace SF {

//-------------------------------------------------- 
// ForceSensor component
//
class ForceSensor: public mtsTaskPeriodic {
protected:
    // scalar type
    double ForceX;
    // vector type
    std::vector<double> ForceXYZ;

public:
    ForceSensor(const std::string & name, double period);
    ~ForceSensor() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void);
    void Cleanup(void) {}
};

//-------------------------------------------------- 
// Control component
//
class Control: public mtsTaskPeriodic {
protected:

public:
    Control(const std::string & name, double period);
    ~Control() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void);
    void Cleanup(void) {}
};

//-------------------------------------------------- 
// Workflow (application) component
//
class Workflow: public mtsTaskPeriodic {
protected:

public:
    Workflow(const std::string & name, double period);
    ~Workflow() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void);
    void Cleanup(void) {}
};

class GCMHelper {
public:
    static ForceSensor   * ForceSensorComp;
    static Control  * ControlComp;
    static Workflow * WorkflowComp;

    static mtsManagerLocal * ComponentManager;

    static void cisstInit(void);
    static void cisstCleanup(void);
};

} // namespace SF

#endif // _SFGCMHelper_h
