//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 22, 2012
// Last revision: Jun 25, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "testState.h"
#include "statemachine.h"

using namespace SC;

SCStateTest::SCStateTest(void)
{
    // test registration
    TEST_ADD(SCStateTest::TestStateMachineBasics);
    TEST_ADD(SCStateTest::TestStateProductOperator);
}

SCStateTest::~SCStateTest(void) {}

void SCStateTest::setup() {}
void SCStateTest::tear_down() {}

void SCStateTest::TestStateMachineBasics(void)
{
#if 0
    SC::StateMachine sm;

    // initialization: state entry/exit
    TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
    TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 1);
    TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_ENTRY)      == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_EXIT)       == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 0);
    TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 0);
    // initialization: transitions
    TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_WARNING) == 0);
    TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_NORMAL) == 0);
    TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_ERROR)  == 0);
    TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_ERROR)   == 0);
    TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_NORMAL)   == 0);
    TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_WARNING)  == 0);

    TEST_ASSERT(sm.GetCurrentState() == State::NORMAL);

    // to warning, back to normal
    sm.ProcessEvent(State::NORMAL_TO_WARNING);
    TEST_ASSERT(sm.GetCurrentState() == State::WARNING);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_ENTRY)      == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_EXIT)       == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 0);

        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_WARNING) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_NORMAL) == 0);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_ERROR)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_ERROR)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_NORMAL)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_WARNING)  == 0);
    }

    sm.ProcessEvent(State::WARNING_TO_NORMAL);
    TEST_ASSERT(sm.GetCurrentState() == State::NORMAL);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_ENTRY)      == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_EXIT)       == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 0);

        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_WARNING) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_NORMAL) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_ERROR)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_ERROR)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_NORMAL)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_WARNING)  == 0);
    }

    // to error, back to normal
    sm.ProcessEvent(State::NORMAL_TO_ERROR);
    TEST_ASSERT(sm.GetCurrentState() == State::ERROR);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_ENTRY)      == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_EXIT)       == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 0);

        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_WARNING) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_NORMAL) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_ERROR)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_ERROR)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_NORMAL)   == 0);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_WARNING)  == 0);
    }

    sm.ProcessEvent(State::ERROR_TO_NORMAL);
    TEST_ASSERT(sm.GetCurrentState() == State::NORMAL);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 3);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_ENTRY)      == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_EXIT)       == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 1);

        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_WARNING) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_NORMAL) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_ERROR)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_ERROR)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_NORMAL)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_WARNING)  == 0);
    }

    // to warning, error, and back to normal
    sm.ProcessEvent(State::NORMAL_TO_WARNING);
    TEST_ASSERT(sm.GetCurrentState() == State::WARNING);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 3);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 3);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_ENTRY)      == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_EXIT)       == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 1);

        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_WARNING) == 2);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_NORMAL) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_ERROR)  == 0);
        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_ERROR)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_NORMAL)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_WARNING)  == 0);
    }

    sm.ProcessEvent(State::WARNING_TO_ERROR);
    TEST_ASSERT(sm.GetCurrentState() == State::ERROR);
    {
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_ENTRY) == 1);
        TEST_ASSERT(sm.GetCountEntryExit(State::STATEMACHINE_ON_EXIT)  == 0);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_ENTRY)       == 3);
        TEST_ASSERT(sm.GetCountEntryExit(State::NORMAL_ON_EXIT)        == 3);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_ENTRY)      == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::WARNING_ON_EXIT)       == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_ENTRY)        == 2);
        TEST_ASSERT(sm.GetCountEntryExit(State::ERROR_ON_EXIT)         == 1);

        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_WARNING) == 2);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_NORMAL) == 1);
        TEST_ASSERT(sm.GetCountTransition(State::WARNING_TO_ERROR)  == 1);
        TEST_ASSERT(sm.GetCountTransition(State::NORMAL_TO_ERROR)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_NORMAL)   == 1);
        TEST_ASSERT(sm.GetCountTransition(State::ERROR_TO_WARNING)  == 0);
    }
#endif
}

void SCStateTest::TestStateProductOperator(void)
{
#if 0
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
#endif
}
