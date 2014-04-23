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
    TEST_ADD(SFGCMTest::TestSafecompScenario);

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

void SFGCMTest::TestSafecompScenario(void)
{
    // FIXME: Check initial states of every single structural elements (component, interface)
}
