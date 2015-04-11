//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2015 Min Yang Jung, Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 9, 2015
// Last revision: Apr 10, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "components.h"

#include <cisstOSAbstraction/osaGetTime.h>

//
// SensorWrapper
//
SensorWrapper::SensorWrapper(const std::string & name, double period)
    : mtsTaskPeriodic(name, period, false, 1000),
        StartTimestamp(0.0),
        SensorValue(0.0),
        SensorValue2(0.0),
        Step(0)
{
    StateTable.AddData(SensorValue, "SensorValue");
    StateTable.AddData(SensorValue2, "SensorValue2");

    mtsInterfaceProvided * provided = AddInterfaceProvided("SensorValue");
    SFASSERT(provided);
    provided->AddCommandReadState(StateTable, SensorValue, "GetSensorValue");
}

void SensorWrapper::Startup(void)
{
    StartTimestamp = osaGetTime();
}

void SensorWrapper::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    ++Step;

    // Event generation by filters
    TestFilter();

    // Event generation by direct calls to API
    //TestEvent();
}

void SensorWrapper::TestFilter(void)
{
    double elapsed = osaGetTime() - StartTimestamp; // in second

    if (elapsed >= 21.0)
        return;

    if (elapsed < 5.0) {
#define VALUE1_NORMAL\
        SensorValue = double(rand() % 10) * 0.1 * 4.9999;
#define VALUE2_NORMAL\
        SensorValue2 = double(rand() % 10) * 0.1 * 9.9999;
        VALUE1_NORMAL;
        VALUE2_NORMAL;
        std::cout << "." << std::flush;
    } else if (5.0 <= elapsed && elapsed < 7.0) {
        // SensorValue exceeds threshold
#define VALUE1_EXCEED\
        SensorValue = double(rand() % 10) * 0.1 * 4.9999 + 5.1;
        VALUE1_EXCEED;
        VALUE2_NORMAL;
        std::cout << "W" << std::flush;
    } else if (7.0 <= elapsed && elapsed < 9.0) {
        VALUE1_NORMAL;
        VALUE2_NORMAL;
        std::cout << "." << std::flush;
    } else if (9.0 <= elapsed && elapsed < 11.0) {
#define VALUE2_EXCEED\
        SensorValue2 = double(rand() % 10) * 0.1 * 9.9999 + 10.1;
        VALUE1_NORMAL;
        VALUE2_EXCEED;
        std::cout << "E" << std::flush;
    } else if (11.0 <= elapsed && elapsed < 13.0) {
        VALUE1_NORMAL;
        VALUE2_NORMAL;
        std::cout << "." << std::flush;
    } else if (13.0 <= elapsed && elapsed < 15) {
        VALUE1_EXCEED;
        VALUE2_NORMAL;
        std::cout << "W" << std::flush;
    } else if (15.0 <= elapsed && elapsed < 17) {
        VALUE1_EXCEED;
        VALUE2_EXCEED;
        std::cout << "E" << std::flush;
    } else if (17.0 <= elapsed && elapsed < 19) {
        VALUE1_EXCEED;
        VALUE2_NORMAL;
        std::cout << "?" << std::flush;
    } else if (19.0 <= elapsed && elapsed < 21) {
        VALUE1_NORMAL;
        VALUE2_NORMAL;
        std::cout << "." << std::flush;
    }
    //std::cout << std::setprecision(12) << osaGetTime() << "\t" << SensorValue << "\t" << SensorValue2 << std::endl;
}

void SensorWrapper::TestEvent(void)
{
    if (Step >= 19)
        return;

    if (Step < 5) {
        // Initial state: NORMAL
        std::cout << "[ " << Step << " ]: Waiting..." << std::endl;
    } else if (Step == 5) {
        // Generate warning onset event
#define GENERATE_WARNING_ONSET\
        std::cout << "Generating WARNING event" << std::endl;\
        GetSafetyCoordinator->GenerateEvent("EVT_WARNING",\
                                            SF::State::STATEMACHINE_APP,\
                                            "WARNING event generated",\
                                            this->GetName());
        std::cout << "[ " << Step << " ]: ";
        GENERATE_WARNING_ONSET;
    } else if (Step == 7) {
        // Generate warning completion event
#define GENERATE_WARNING_COMPLETION\
        std::cout << "Generating /WARNING event" << std::endl;\
        GetSafetyCoordinator->GenerateEvent("/EVT_WARNING",\
                                            SF::State::STATEMACHINE_APP,\
                                            "/WARNING event generated",\
                                            this->GetName());
        std::cout << "[ " << Step << " ]: ";
        GENERATE_WARNING_COMPLETION;
    } else if (Step == 9) {
        // Generate error onset event
#define GENERATE_ERROR_ONSET\
        std::cout << "Generating ERROR event" << std::endl;\
        GetSafetyCoordinator->GenerateEvent("EVT_ERROR",\
                                            SF::State::STATEMACHINE_APP,\
                                            "ERROR event generated",\
                                            this->GetName());
        std::cout << "[ " << Step << " ]: ";
        GENERATE_ERROR_ONSET;
    } else if (Step == 11) {
        // Generate error completion event
#define GENERATE_ERROR_COMPLETION\
        std::cout << "Generating /ERROR event" << std::endl;\
        GetSafetyCoordinator->GenerateEvent("/EVT_ERROR",\
                                            SF::State::STATEMACHINE_APP,\
                                            "/ERROR event generated",\
                                            this->GetName());
        std::cout << "[ " << Step << " ]: ";
        GENERATE_ERROR_COMPLETION;
    } else if (Step == 13) {
        std::cout << "[ " << Step << " ]: ";
        GENERATE_WARNING_ONSET;
    } else if (Step == 15) {
        std::cout << "[ " << Step << " ]: ";
        GENERATE_ERROR_ONSET;
    } else if (Step == 17) {
        std::cout << "[ " << Step << " ]: ";
        GENERATE_ERROR_COMPLETION;
    } else if (Step == 19) {
        GENERATE_WARNING_COMPLETION;
    }
#if 0
    // Generate sine wave
    SensorValue = 10.0 * // amplitude
                    sin(2 * cmnPI * static_cast<double>(this->GetTick()) * Period / 10.0);
    
    std::cout << "Sensor: " << SensorValue << std::endl;
#endif
}

//
// Component: Control
//
Control::Control(const std::string & name, double period)
    : mtsTaskPeriodic(name, period, false, 1000), ControlValue(0.0)
{
    mtsInterfaceRequired * required = AddInterfaceRequired("ReadSensorValue", MTS_REQUIRED);
    SFASSERT(required);
    required->AddFunction("GetSensorValue", GetSensorValue);

    StateTable.AddData(ControlValue, "ControlValue");

    mtsInterfaceProvided * provided = AddInterfaceProvided("ControlValue");
    SFASSERT(provided);
    provided->AddCommandReadState(StateTable, ControlValue, "GetControlValue");
}

void Control::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    GetSensorValue(ControlValue);
    ControlValue *= 2.0;
}

//
// Component: User Interface
//
UI::UI(const std::string & name, double period)
    : mtsTaskPeriodic(name, period, false, 1000), UIValue(0.0)
{
    mtsInterfaceRequired * required = AddInterfaceRequired("ReadControlValue", MTS_REQUIRED);
    SFASSERT(required);
    required->AddFunction("GetControlValue", GetControlValue);
}

void UI::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();
    
    GetControlValue(UIValue);
}
