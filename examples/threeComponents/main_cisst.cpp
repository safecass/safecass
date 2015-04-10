//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2015 Min Yang Jung, Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 7, 2015
// Last revision: Apr 7, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include <time.h> // for srand(time(NULL))

#include "config.h"
#include "common.h"

#include <cisstCommon/cmnGetChar.h>
#include <cisstOSAbstraction/osaSleep.h>

#include "components.h"

// thread period (in sec)
#define PERIOD 0.1

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
    SensorWrapper sensorWrapper("Sensor", PERIOD);
    Control control("Control", PERIOD);
    UI ui("UI", PERIOD);

    SFASSERT(ComponentManager->AddComponent(&sensorWrapper));
    SFASSERT(ComponentManager->AddComponent(&control));
    SFASSERT(ComponentManager->AddComponent(&ui));

    // Read json file
    std::string fileName(JSON_FOLDER"/Sensor.json");
    if (!GetSafetyCoordinator->ReadConfigFile(fileName)) {
        std::cerr << "Failed to read json file: " << fileName << std::endl;
        return 1;
    }

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
    collectorSensor.AddSignal("SensorValue2");

    // for periods
    //mtsCollectorState collectorControl(control.GetName(),
                                       //control.GetMonitoringStateTableName(),
                                       //mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
    //collectorControl.AddSignal("ExecTimeTotal");

    SFASSERT(ComponentManager->AddComponent(&collectorSensor));
    //SFASSERT(ComponentManager->AddComponent(&collectorControl));

    collectorSensor.Connect();
    //collectorControl.Connect();

    ComponentManager->CreateAll();
    ComponentManager->WaitForStateAll(mtsComponentState::READY);
    ComponentManager->StartAll();
    ComponentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // start data collection
    collectorSensor.StartCollection(0.0);
    //collectorControl.StartCollection(0.0);

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
    //collectorControl.StopCollection(0.0);

    std::cout << "Cleaning up... " << std::flush;

    ComponentManager->KillAll();
    ComponentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
    ComponentManager->Cleanup();

    std::cout << "Done\n";

    return 0;
}
