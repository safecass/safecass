//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 10, 2014
// Last revision: Jul 16, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include <cisstMultiTask/mtsTaskManager.h>
#include <QApplication>

#include "accessor.h"
#include "viewerQtComponent.h"

using namespace SF;

const std::string GetColorCodeForState(unsigned int state);

int main(int argc, char *argv[])
{
    // create a Qt user interface
    QApplication application(argc, argv);

    // Get local component manager instance
    mtsManagerLocal::InstallSafetyCoordinator();
    mtsManagerLocal * componentManager;
    try {
        componentManager = mtsManagerLocal::GetInstance();
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // Set name of safety coordinator
    mtsSafetyCoordinator * sc = componentManager->GetCoordinator();
    CMN_ASSERT(sc);
    sc->SetName("casros_state_viewer");

    // create the components with their respective UIs
    viewerQtComponent * viewer = new viewerQtComponent("viewer");
    componentManager->AddComponent(viewer);

    // Create publisher and subscriber
    AccessorViewer casrosAccessor;

    // create and start all components
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // run Qt user interface
    application.exec();

    // kill all components and perform cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
    componentManager->Cleanup();

    return 0;
}
