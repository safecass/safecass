//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2015 Min Yang Jung, Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : April 7, 2015
// Last revision: April 7, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include <time.h> // for srand(time(NULL))

#include "config.h"
#include "common.h"
//#include "json.h"

#include <cisstCommon/cmnGetChar.h>
#include <cisstOSAbstraction/osaSleep.h>
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
    double SensorValue;
public:
    SensorWrapper(const std::string & name, double period)
        : mtsTaskPeriodic(name, period, false, 10), SensorValue(0.0)
    {
        StateTable.AddData(SensorValue, "SensorValue");

        mtsInterfaceProvided * provided = AddInterfaceProvided("SensorValue");
        SFASSERT(provided);
        provided->AddCommandReadState(StateTable, SensorValue, "GetSensorValue");
    }
    ~SensorWrapper() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();
        
        //double delta = double(rand() % 10) * 0.1;
        //SensorValue += delta;

        // Generate sine wave
        SensorValue = 10.0 * // amplitude
                      sin(2 * cmnPI * static_cast<double>(this->GetTick()) * Period / 10.0);
        
        std::cout << "Sensor: " << SensorValue << std::endl;

    }
    void Cleanup(void) {}
};

//
// Component: Control
//
class Control: public mtsTaskPeriodic {
protected:
    mtsFunctionRead GetSensorValue;
    double ControlValue;
public:
    Control(const std::string & name, double period)
        : mtsTaskPeriodic(name, period, false, 10), ControlValue(0.0)
    {
        mtsInterfaceRequired * required = AddInterfaceRequired("ReadSensorValue", MTS_REQUIRED);
        SFASSERT(required);
        required->AddFunction("GetSensorValue", GetSensorValue);

        StateTable.AddData(ControlValue, "ControlValue");

        mtsInterfaceProvided * provided = AddInterfaceProvided("ControlValue");
        SFASSERT(provided);
        provided->AddCommandReadState(StateTable, ControlValue, "GetControlValue");
    }
    ~Control() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();

        GetSensorValue(ControlValue);
        ControlValue *= 2.0;
    }
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
    UI(const std::string & name, double period)
        : mtsTaskPeriodic(name, period, false, 10), UIValue(0.0)
    {
        mtsInterfaceRequired * required = AddInterfaceRequired("ReadControlValue", MTS_REQUIRED);
        SFASSERT(required);
        required->AddFunction("GetControlValue", GetControlValue);
    }
    ~UI() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();
        
        GetControlValue(UIValue);
    }
    void Cleanup(void) {}
};



int main(int argc, char *argv[])
{
    srand(time(NULL));

    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    
    // Enable safety framework
    mtsComponentManager::InstallSafetyCoordinator();

    mtsManagerLocal * ComponentManager = 0;
    try {
        ComponentManager = mtsComponentManager::GetInstance();
    } catch (...) {
        std::cerr << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // Create and setup components
    SensorWrapper sensorWrapper("Sensor", 0.5 * cmn_s);
    Control control("Control", 0.25 * cmn_s);
    UI ui("UI", 1.0 * cmn_s);

    SFASSERT(ComponentManager->AddComponent(&sensorWrapper));
    SFASSERT(ComponentManager->AddComponent(&control));
    SFASSERT(ComponentManager->AddComponent(&ui));

    CONNECT_LOCAL(sensorWrapper.GetName(), "SensorValue",
                  control.GetName(), "ReadSensorValue");
    CONNECT_LOCAL(control.GetName(), "ControlValue",
                  ui.GetName(), "ReadControlValue");

    // Install data collectors
    // for raw values
    mtsCollectorState collectorSensor(sensorWrapper.GetName(),
                                      sensorWrapper.GetDefaultStateTableName(),
                                      mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
    collectorSensor.AddSignal("SensorValue");

    // for periods
    mtsCollectorState collectorControl(control.GetName(),
                                       control.GetMonitoringStateTableName(),
                                       mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
    collectorControl.AddSignal("ExecTimeTotal");

    SFASSERT(ComponentManager->AddComponent(&collectorSensor));
    SFASSERT(ComponentManager->AddComponent(&collectorControl));

    collectorSensor.Connect();
    collectorControl.Connect();

    ComponentManager->CreateAll();
    ComponentManager->WaitForStateAll(mtsComponentState::READY);
    ComponentManager->StartAll();
    ComponentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // start data collection
    collectorSensor.StartCollection(0.0);
    collectorControl.StartCollection(0.0);

    std::cout << "Press 'q' to quit." << std::endl;

    // loop until 'q' is pressed
    int key = ' ';
    while (key != 'q') {
        key = cmnGetChar();
        osaSleep(100 * cmn_ms);
    }
    std::cout << std::endl;

    // stop data collection
    collectorSensor.StopCollection(0.0);
    collectorControl.StopCollection(0.0);

    std::cout << "Cleaning up... " << std::flush;

    ComponentManager->KillAll();
    ComponentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
    ComponentManager->Cleanup();

    std::cout << "Done\n";

    return 0;
}
