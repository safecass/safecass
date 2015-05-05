//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 18, 2012
// Last revision: May 5, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "testGCM.h"
#include "GCMHelper.h"

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>

using namespace SC;

SCGCMTest::SCGCMTest(void)
{
    // test registration
    TEST_ADD(SCGCMTest::TestInitialStates);
    TEST_ADD(SCGCMTest::TestFilterInstalled);
    TEST_ADD(SCGCMTest::TestEventPropagation);

    GCMHelper::cisstInit();
}

SCGCMTest::~SCGCMTest(void)
{
    GCMHelper::cisstCleanup();
}

void SCGCMTest::setup()
{
}

void SCGCMTest::tear_down()
{
}

void SCGCMTest::TestInitialStates(void)
{
    // 1. Check initial states of all states (components, interfaces)
    mtsComponent * component;
    GCM * gcm;

    // Workflow component
    {
        component = GCMHelper::Workflow;
        SCASSERT(component);
        gcm = component->GCMInstance;
        SCASSERT(gcm);

        // component states
        TEST_ASSERT(gcm->GetComponentState(GCM::SYSTEM_VIEW)      == State::NORMAL);
        TEST_ASSERT(gcm->GetComponentState(GCM::FRAMEWORK_VIEW)   == State::NORMAL);
        TEST_ASSERT(gcm->GetComponentState(GCM::APPLICATION_VIEW) == State::NORMAL);
        // R1 interface
        TEST_ASSERT(gcm->GetInterfaceState("R1", GCM::REQUIRED_INTERFACE) == State::NORMAL);
    }

    // Control component
    {
        component = GCMHelper::Control;
        SCASSERT(component);
        gcm = component->GCMInstance;
        SCASSERT(gcm);

        // component states
        TEST_ASSERT(gcm->GetComponentState(GCM::SYSTEM_VIEW)      == State::NORMAL);
        TEST_ASSERT(gcm->GetComponentState(GCM::FRAMEWORK_VIEW)   == State::NORMAL);
        TEST_ASSERT(gcm->GetComponentState(GCM::APPLICATION_VIEW) == State::NORMAL);
        // interfaces
        TEST_ASSERT(gcm->GetInterfaceState("R1", GCM::REQUIRED_INTERFACE) == State::NORMAL);
        TEST_ASSERT(gcm->GetInterfaceState("P1", GCM::PROVIDED_INTERFACE) == State::NORMAL);
    }

    // ForceSensor component
    {
        component = GCMHelper::ForceSensor;
        SCASSERT(component);
        gcm = component->GCMInstance;
        SCASSERT(gcm);

        // component states
        TEST_ASSERT(gcm->GetComponentState(GCM::SYSTEM_VIEW)      == State::NORMAL);
        TEST_ASSERT(gcm->GetComponentState(GCM::FRAMEWORK_VIEW)   == State::NORMAL);
        TEST_ASSERT(gcm->GetComponentState(GCM::APPLICATION_VIEW) == State::NORMAL);
        // interfaces
        TEST_ASSERT(gcm->GetInterfaceState("P1", GCM::PROVIDED_INTERFACE) == State::NORMAL);
    }
}

void SCGCMTest::TestFilterInstalled(void)
{
    // Check if framework filters are installed properly
    // 1. f_{F}^{1} : exception filter
    // 2. f_{F}^{2} : thread overrun filter

    // Check if application filters are installed properly
    // TODO: install app-specific filters
}

void SCGCMTest::TestEventPropagation(void)
{
    // TODO: change each state one by one and see if relevant state changes occur.

    // TODO: if interface state changes, does system view change?
    // TODO: if ForceSensor:P1 fails, does state of Control also change?
}
