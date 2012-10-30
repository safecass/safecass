/*

  Safety Framework for Component-based Robotics

  Created on: October 26, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

/*
#include <time.h> // for srand(time(NULL))

#include "config.h"
#include "common.h"
#include "json.h"
#include "monitor.h"
#include "cisstMonitor.h"
#include "threshold.h"
#include "statemachine.h"
*/

#include <cisstMultiTask/mtsTaskPeriodic.h>
/*
#include <cisstCommon/cmnGetChar.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif
*/

//-------------------------------------------------- 
// Simulated Force Sensor Component (could be generalized as generic sensor wrapper component)
//-------------------------------------------------- 
class ForceSensorComponent: public mtsTaskPeriodic {
public:
    typedef enum {
        // OK
        ERROR_OK,
        // ERROR
        ERROR_DSP_DEAD,
        ERROR_DATA_RECEPTION_ERROR,
        ERROR_SATURATION,
        ERROR_MEMORY_ERROR,
        ERROR_BAD_CRC,
        ERROR_WATCHDOG
    } ErrorCodes;

protected:
    double ForceScalar;
    mtsDoubleVec ForceVector;
    
    int    CheckSensorStatus(void);
    double ReadForceScalar(void);

public:
    ForceSensorComponent(const std::string & name, double period);
    ~ForceSensorComponent(void);

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void);
    void Cleanup(void) {}

    // predefined names and values
    static const std::string NameOfProvidedInterface;
    static const std::string NameOfScalarSignal;
    static const std::string NameOfVectorSignal;
};

//-------------------------------------------------- 
//  Simulated Control Component
//-------------------------------------------------- 
class ControlComponent: public mtsTaskPeriodic {
protected:
    mtsFunctionRead GetForceScalar;
    mtsFunctionRead GetForceVector;

public:
    ControlComponent(const std::string & name, double period);
    ~ControlComponent(void);

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void);
    void Cleanup(void) {}

    // public interface name
    static const std::string NameOfRequiredInterface;
};

// MJ TODO: video component, application component, or robot control wrapper
// (e.g., AMC driver communicator) component could be added.
