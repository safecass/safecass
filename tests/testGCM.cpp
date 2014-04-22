//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 18, 2012
// Last revision: Apr 19, 2014
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
    TEST_ADD(SFGCMTest::StateMachineBasics);

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
    std::cout << "tear_down\n" << std::flush;
}

void SFGCMTest::StateMachineBasics(void)
{
    StateMachine * sm = GCMHelper::ForceSensor->FaultState;
    /*
    std::cout << "###### " 
        << sm->GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) 
        << ", " << sm->GetCountTransition(State::ON_ENTRY)
        << std::endl;
    sm->print();

            STATEMACHINE_ON_ENTRY,
            ,
            // normal state
            ,
            ,
            // fault state
            ,
            ,
            // error state
            ,
            ,
            // failure state
            ,
            ,
            // total number of state entry/exit types
            NUMBER_OF_ENTRY_EXIT
        } StateEntryExitType;

        typedef enum {
            ,
            ,
            ,
            ,
            ,
            ,
            ,
            ,
            ,
            ,
            ,
            // total number of state transitions
            NUMBER_OF_TRANSITIONS
    */

    // initialization: state entry/exit
    TEST_ASSERT(sm->GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
    TEST_ASSERT(sm->GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
    TEST_ASSERT(sm->GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 1);
    TEST_ASSERT(sm->GetCountEntryExit(State::NORMAL_ON_EXIT)        == 0);
    TEST_ASSERT(sm->GetCountEntryExit(State::FAULT_ON_ENTRY)        == 0);
    TEST_ASSERT(sm->GetCountEntryExit(State::FAULT_ON_EXIT)         == 0);
    TEST_ASSERT(sm->GetCountEntryExit(State::ERROR_ON_ENTRY)        == 0);
    TEST_ASSERT(sm->GetCountEntryExit(State::ERROR_ON_EXIT)         == 0);
    TEST_ASSERT(sm->GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 0);
    TEST_ASSERT(sm->GetCountEntryExit(State::FAILURE_ON_EXIT)       == 0);
    // initialization: transitions
    TEST_ASSERT(sm->GetCountTransition(State::ON_ENTRY)          == 0);
    TEST_ASSERT(sm->GetCountTransition(State::ON_EXIT)           == 0);
    TEST_ASSERT(sm->GetCountTransition(State::FAULT_DETECTION)   == 0);
    TEST_ASSERT(sm->GetCountTransition(State::FAULT_REMOVAL)     == 0);
    TEST_ASSERT(sm->GetCountTransition(State::FAULT_ACTIVATION)  == 0);
    TEST_ASSERT(sm->GetCountTransition(State::ERROR_DETECTION)   == 0);
    TEST_ASSERT(sm->GetCountTransition(State::ERROR_REMOVAL)     == 0);
    TEST_ASSERT(sm->GetCountTransition(State::ERROR_PROPAGATION) == 0);
    TEST_ASSERT(sm->GetCountTransition(State::FAILURE_DETECTION) == 0);
    TEST_ASSERT(sm->GetCountTransition(State::FAILURE_REMOVAL)   == 0);
    TEST_ASSERT(sm->GetCountTransition(State::FAILURE_STOP)      == 0);

#if 0
    // to fault, back to normal
    ProcessEvent(State::FAULT_DETECTION); std::cout << GetString(GetState()) << std::endl;
    ProcessEvent(State::FAULT_REMOVAL); std::cout << GetString(GetState()) << std::endl;

    // to error, back to normal
    ProcessEvent(State::ERROR_DETECTION); std::cout << GetString(GetState()) << std::endl;
    ProcessEvent(State::ERROR_REMOVAL); std::cout << GetString(GetState()) << std::endl;

    // to failure, back to normal
    ProcessEvent(State::FAILURE_DETECTION); std::cout << GetString(GetState()) << std::endl;
    ProcessEvent(State::FAILURE_REMOVAL); std::cout << GetString(GetState()) << std::endl;

    // to fault, error, failure, and back to normal
    ProcessEvent(State::FAULT_DETECTION); std::cout << GetString(GetState()) << std::endl;
    ProcessEvent(State::FAULT_ACTIVATION); std::cout << GetString(GetState()) << std::endl;
    ProcessEvent(State::ERROR_PROPAGATION); std::cout << GetString(GetState()) << std::endl;
    ProcessEvent(State::FAILURE_REMOVAL); std::cout << GetString(GetState()) << std::endl;
#endif
}
