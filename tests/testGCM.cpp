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
    TEST_ADD(SFGCMTest::TestStateMachineBasics);
    TEST_ADD(SFGCMTest::TestStateProductOperator);
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
    std::cout << "tear_down\n" << std::flush;
}

void SFGCMTest::TestStateMachineBasics(void)
{
    StateMachine sm;

    // initialization: state entry/exit
    TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
    TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 1);
    TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_ENTRY)        == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_EXIT)         == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_EXIT)       == 0);
    // initialization: transitions
    //TEST_ASSERT_MSG(sm.GetCountTransition(State::FAULT_DETECTION)   == 0, sm.GetCounterStatus().c_str());
    TEST_ASSERT(sm.GetCountTransition(State::FAULT_DETECTION)   == 0);
    TEST_ASSERT(sm.GetCountTransition(State::FAULT_REMOVAL)     == 0);
    TEST_ASSERT(sm.GetCountTransition(State::FAULT_ACTIVATION)  == 0);
    TEST_ASSERT(sm.GetCountTransition(State::ERROR_DETECTION)   == 0);
    TEST_ASSERT(sm.GetCountTransition(State::ERROR_REMOVAL)     == 0);
    TEST_ASSERT(sm.GetCountTransition(State::ERROR_PROPAGATION) == 0);
    TEST_ASSERT(sm.GetCountTransition(State::FAILURE_DETECTION) == 0);
    TEST_ASSERT(sm.GetCountTransition(State::FAILURE_REMOVAL)   == 0);
    TEST_ASSERT(sm.GetCountTransition(State::FAILURE_STOP)      == 0);

    TEST_ASSERT(sm.GetCurrentState() == State::NORMAL);

    // to fault, back to normal
    sm.ProcessEvent(State::FAULT_DETECTION);
    TEST_ASSERT(sm.GetCurrentState() == State::FAULT);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_EXIT)         == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_EXIT)       == 0);

        TEST_ASSERT(sm.GetCountTransition(State::FAULT_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_REMOVAL)     == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_ACTIVATION)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_DETECTION)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_REMOVAL)     == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_PROPAGATION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_DETECTION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_REMOVAL)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_STOP)      == 0);
    }

    sm.ProcessEvent(State::FAULT_REMOVAL);
    TEST_ASSERT(sm.GetCurrentState() == State::NORMAL);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_EXIT)         == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_EXIT)       == 0);

        TEST_ASSERT(sm.GetCountTransition(State::FAULT_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_ACTIVATION)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_DETECTION)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_REMOVAL)     == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_PROPAGATION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_DETECTION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_REMOVAL)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_STOP)      == 0);
    }

    // to error, back to normal
    sm.ProcessEvent(State::ERROR_DETECTION);
    TEST_ASSERT(sm.GetCurrentState() == State::ERROR);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_EXIT)         == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_EXIT)       == 0);

        TEST_ASSERT(sm.GetCountTransition(State::FAULT_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_ACTIVATION)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_REMOVAL)     == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_PROPAGATION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_DETECTION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_REMOVAL)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_STOP)      == 0);
    }

    sm.ProcessEvent(State::ERROR_REMOVAL);
    TEST_ASSERT(sm.GetCurrentState() == State::NORMAL);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 3);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_EXIT)         == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_EXIT)       == 0);

        TEST_ASSERT(sm.GetCountTransition(State::FAULT_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_ACTIVATION)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_PROPAGATION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_DETECTION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_REMOVAL)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_STOP)      == 0);
    }

    // to failure, back to normal
    sm.ProcessEvent(State::FAILURE_DETECTION);
    TEST_ASSERT(sm.GetCurrentState() == State::FAILURE);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 3);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 3);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_EXIT)         == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_EXIT)       == 0);

        TEST_ASSERT(sm.GetCountTransition(State::FAULT_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_ACTIVATION)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_PROPAGATION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_DETECTION) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_REMOVAL)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_STOP)      == 0);
    }

    sm.ProcessEvent(State::FAILURE_REMOVAL);
    TEST_ASSERT(sm.GetCurrentState() == State::NORMAL);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 4);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 3);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_EXIT)         == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_EXIT)       == 1);

        TEST_ASSERT(sm.GetCountTransition(State::FAULT_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_ACTIVATION)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_PROPAGATION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_DETECTION) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_REMOVAL)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_STOP)      == 0);
    }

    // to fault, error, failure, and back to normal
    sm.ProcessEvent(State::FAULT_DETECTION);
    TEST_ASSERT(sm.GetCurrentState() == State::FAULT);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 4);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 4);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_ENTRY)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_EXIT)         == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_EXIT)       == 1);

        TEST_ASSERT(sm.GetCountTransition(State::FAULT_DETECTION)   == 2);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_ACTIVATION)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_PROPAGATION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_DETECTION) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_REMOVAL)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_STOP)      == 0);
    }

    sm.ProcessEvent(State::FAULT_ACTIVATION);
    TEST_ASSERT(sm.GetCurrentState() == State::ERROR);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 4);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 4);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_ENTRY)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_EXIT)         == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_EXIT)       == 1);

        TEST_ASSERT(sm.GetCountTransition(State::FAULT_DETECTION)   == 2);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_ACTIVATION)  == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_PROPAGATION) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_DETECTION) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_REMOVAL)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_STOP)      == 0);
    }

    sm.ProcessEvent(State::ERROR_PROPAGATION);
    TEST_ASSERT(sm.GetCurrentState() == State::FAILURE);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 4);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 4);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_ENTRY)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_EXIT)         == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_EXIT)       == 1);

        TEST_ASSERT(sm.GetCountTransition(State::FAULT_DETECTION)   == 2);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_ACTIVATION)  == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_PROPAGATION) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_DETECTION) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_REMOVAL)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_STOP)      == 0);
    }

    sm.ProcessEvent(State::FAILURE_REMOVAL);
    TEST_ASSERT(sm.GetCurrentState() == State::NORMAL);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 5);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 4);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_ENTRY)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAULT_ON_EXIT)         == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_ENTRY)      == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::FAILURE_ON_EXIT)       == 2);

        TEST_ASSERT(sm.GetCountTransition(State::FAULT_DETECTION)   == 2);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAULT_ACTIVATION)  == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_DETECTION)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_REMOVAL)     == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_PROPAGATION) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_DETECTION) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_REMOVAL)   == 2);
        TEST_ASSERT(sm.GetCountTransition(State::FAILURE_STOP)      == 0);
    }
}

void SFGCMTest::TestStateProductOperator(void)
{
    State s1, s2;
    TEST_ASSERT(s1.GetState() == State::NORMAL);
    TEST_ASSERT(s2.GetState() == State::NORMAL);

    s1.SetState(State::NORMAL);
    TEST_ASSERT(s1.GetState() == State::NORMAL);
    s1.SetState(State::FAULT);
    TEST_ASSERT(s1.GetState() == State::FAULT);
    s1.SetState(State::ERROR);
    TEST_ASSERT(s1.GetState() == State::ERROR);
    s1.SetState(State::FAILURE);
    TEST_ASSERT(s1.GetState() == State::FAILURE);

    // operator==
    s1.SetState(State::NORMAL);
    s2.SetState(State::NORMAL);
    TEST_ASSERT(s1 == s2);
    s2.SetState(State::FAULT);
    TEST_ASSERT(!(s1 == s2));
    s2.SetState(State::ERROR);
    TEST_ASSERT(!(s1 == s2));
    s2.SetState(State::FAILURE);
    TEST_ASSERT(!(s1 == s2));

    // operator!=
    s1.SetState(State::NORMAL);
    s2.SetState(State::FAULT);
    TEST_ASSERT(s1 != s2);
    s2.SetState(State::ERROR);
    TEST_ASSERT(s1 != s2);
    s2.SetState(State::FAILURE);
    TEST_ASSERT(s1 != s2);

    // operator>
    s1.SetState(State::NORMAL); s2.SetState(State::NORMAL);
    TEST_ASSERT(!(s1 > s2));
    TEST_ASSERT(!(s2 > s1));

    s1.SetState(State::NORMAL); s2.SetState(State::FAULT);
    TEST_ASSERT(!(s1 > s2));
    TEST_ASSERT(s2 > s1);

    s1.SetState(State::NORMAL); s2.SetState(State::ERROR);
    TEST_ASSERT(!(s1 > s2));
    TEST_ASSERT(s2 > s1);

    s1.SetState(State::NORMAL); s2.SetState(State::FAILURE);
    TEST_ASSERT(!(s1 > s2));
    TEST_ASSERT(s2 > s1);

    s1.SetState(State::FAULT); s2.SetState(State::FAULT);
    TEST_ASSERT(!(s1 > s2));
    TEST_ASSERT(!(s2 > s1));

    s1.SetState(State::FAULT); s2.SetState(State::ERROR);
    TEST_ASSERT(!(s1 > s2));
    TEST_ASSERT(s2 > s1);

    s1.SetState(State::FAULT); s2.SetState(State::FAILURE);
    TEST_ASSERT(!(s1 > s2));
    TEST_ASSERT(s2 > s1);

    s1.SetState(State::ERROR); s2.SetState(State::ERROR);
    TEST_ASSERT(!(s1 > s2));
    TEST_ASSERT(!(s2 > s1));

    s1.SetState(State::ERROR); s2.SetState(State::FAILURE);
    TEST_ASSERT(!(s1 > s2));
    TEST_ASSERT(s2 > s1);

    s1.SetState(State::FAILURE); s2.SetState(State::FAILURE);
    TEST_ASSERT(!(s1 > s2));
    TEST_ASSERT(!(s2 > s1));

    // operator<
    s1.SetState(State::NORMAL); s2.SetState(State::NORMAL);
    TEST_ASSERT(!(s1 < s2));
    TEST_ASSERT(!(s2 < s1));

    s1.SetState(State::NORMAL); s2.SetState(State::FAULT);
    TEST_ASSERT(s1 < s2);
    TEST_ASSERT(!(s2 < s1));

    s1.SetState(State::NORMAL); s2.SetState(State::ERROR);
    TEST_ASSERT(s1 < s2);
    TEST_ASSERT(!(s2 < s1));

    s1.SetState(State::NORMAL); s2.SetState(State::FAILURE);
    TEST_ASSERT(s1 < s2);
    TEST_ASSERT(!(s2 < s1));

    s1.SetState(State::FAULT); s2.SetState(State::FAULT);
    TEST_ASSERT(!(s1 < s2));
    TEST_ASSERT(!(s2 < s1));

    s1.SetState(State::FAULT); s2.SetState(State::ERROR);
    TEST_ASSERT(s1 < s2);
    TEST_ASSERT(!(s2 < s1));

    s1.SetState(State::FAULT); s2.SetState(State::FAILURE);
    TEST_ASSERT(s1 < s2);
    TEST_ASSERT(!(s2 < s1));

    s1.SetState(State::ERROR); s2.SetState(State::ERROR);
    TEST_ASSERT(!(s1 < s2));
    TEST_ASSERT(!(s2 < s1));

    s1.SetState(State::ERROR); s2.SetState(State::FAILURE);
    TEST_ASSERT(s1 < s2);
    TEST_ASSERT(!(s2 < s1));

    s1.SetState(State::FAILURE); s2.SetState(State::FAILURE);
    TEST_ASSERT(!(s1 < s2));
    TEST_ASSERT(!(s2 < s1));

    // operator*
    s1.SetState(State::NORMAL); s2.SetState(State::NORMAL);   TEST_ASSERT((s1 * s2) == State(State::NORMAL));
    s1.SetState(State::NORMAL); s2.SetState(State::FAULT);    TEST_ASSERT((s1 * s2) == State(State::FAULT));
    s1.SetState(State::NORMAL); s2.SetState(State::ERROR);    TEST_ASSERT((s1 * s2) == State(State::ERROR));
    s1.SetState(State::NORMAL); s2.SetState(State::FAILURE);  TEST_ASSERT((s1 * s2) == State(State::FAILURE));
    s1.SetState(State::FAULT); s2.SetState(State::NORMAL);    TEST_ASSERT((s1 * s2) == State(State::FAULT));
    s1.SetState(State::FAULT); s2.SetState(State::FAULT);     TEST_ASSERT((s1 * s2) == State(State::FAULT));
    s1.SetState(State::FAULT); s2.SetState(State::ERROR);     TEST_ASSERT((s1 * s2) == State(State::ERROR));
    s1.SetState(State::FAULT); s2.SetState(State::FAILURE);   TEST_ASSERT((s1 * s2) == State(State::FAILURE));
    s1.SetState(State::ERROR); s2.SetState(State::NORMAL);    TEST_ASSERT((s1 * s2) == State(State::ERROR));
    s1.SetState(State::ERROR); s2.SetState(State::FAULT);     TEST_ASSERT((s1 * s2) == State(State::ERROR));
    s1.SetState(State::ERROR); s2.SetState(State::ERROR);     TEST_ASSERT((s1 * s2) == State(State::ERROR));
    s1.SetState(State::ERROR); s2.SetState(State::FAILURE);   TEST_ASSERT((s1 * s2) == State(State::FAILURE));
    s1.SetState(State::FAILURE); s2.SetState(State::NORMAL);  TEST_ASSERT((s1 * s2) == State(State::FAILURE));
    s1.SetState(State::FAILURE); s2.SetState(State::FAULT);   TEST_ASSERT((s1 * s2) == State(State::FAILURE));
    s1.SetState(State::FAILURE); s2.SetState(State::ERROR);   TEST_ASSERT((s1 * s2) == State(State::FAILURE));
    s1.SetState(State::FAILURE); s2.SetState(State::FAILURE); TEST_ASSERT((s1 * s2) == State(State::FAILURE));

    // assignment operator
    s1.SetState(State::NORMAL);
    s2.SetState(State::FAULT);
    s1 = s2;
    TEST_ASSERT(s1 == s2);
}

void SFGCMTest::TestSafecompScenario(void)
{
    //
}
