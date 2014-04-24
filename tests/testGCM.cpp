//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 18, 2012
// Last revision: Apr 22, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "testGCM.h"
#include "GCMHelper.h"

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>

using namespace SF;

SFGCMTest::SFGCMTest(void)
{
    // test registration
    TEST_ADD(SFGCMTest::TestInitialStates);

    GCMHelper::cisstInit();
}

SFGCMTest::~SFGCMTest(void)
{
    GCMHelper::cisstCleanup();
}

void SFGCMTest::setup()
{
}

void SFGCMTest::tear_down()
{
}

void SFGCMTest::TestInitialStates(void)
{
    // 1. Check initial states of all states (components, interfaces)
    mtsComponent * component;
    GCM * gcm;

    // Workflow component
    {
        component = GCMHelper::Workflow;
        SFASSERT(component);
        gcm = component->GCMInstance;
        SFASSERT(gcm);

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
        SFASSERT(component);
        gcm = component->GCMInstance;
        SFASSERT(gcm);

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
        SFASSERT(component);
        gcm = component->GCMInstance;
        SFASSERT(gcm);

        // component states
        TEST_ASSERT(gcm->GetComponentState(GCM::SYSTEM_VIEW)      == State::NORMAL);
        TEST_ASSERT(gcm->GetComponentState(GCM::FRAMEWORK_VIEW)   == State::NORMAL);
        TEST_ASSERT(gcm->GetComponentState(GCM::APPLICATION_VIEW) == State::NORMAL);
        // interfaces
        TEST_ASSERT(gcm->GetInterfaceState("P1", GCM::PROVIDED_INTERFACE) == State::NORMAL);
    }
}

// TODO: test if state product operator works (S=sF*sA)
// TODO: if interface state changes, does system view change?
// TODO: if ForceSensor:P1 fails, does state of Control also change?
