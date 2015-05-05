//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 19, 2012
// Last revision: Apr 22, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _SCGCMHelper_h
#define _SCGCMHelper_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

namespace SC {

//-------------------------------------------------- 
// ForceSensor component
//
class ForceSensorComp: public mtsTaskPeriodic {
protected:
    // scalar type
    double ForceX;
    // vector type
    std::vector<double> ForceXYZ;

public:
    ForceSensorComp(const std::string & name, double period);
    ~ForceSensorComp() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void);
    void Cleanup(void) {}
};

//-------------------------------------------------- 
// Control component
//
class ControlComp: public mtsTaskPeriodic {
protected:
    // R1
    mtsFunctionRead ReadForceX;
    mtsFunctionRead ReadForceXYZ;

public:
    ControlComp(const std::string & name, double period);
    ~ControlComp() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void);
    void Cleanup(void) {}
};

//-------------------------------------------------- 
// Workflow (application) component
//
class WorkflowComp: public mtsTaskPeriodic {
protected:

public:
    WorkflowComp(const std::string & name, double period);
    ~WorkflowComp() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void);
    void Cleanup(void) {}
};

class GCMHelper {
public:
    static ForceSensorComp * ForceSensor;
    static ControlComp     * Control;
    static WorkflowComp    * Workflow;

    static mtsManagerLocal * ComponentManager;

    static void cisstInit(void);
    static void cisstCleanup(void);
};

} // namespace SC

#endif // _SCGCMHelper_h
