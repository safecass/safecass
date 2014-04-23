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
#ifndef _SFGCMTest_h
#define _SFGCMTest_h

#include "cpptest.h"

class mtsManagerLocal;

class SFGCMTest: public Test::Suite 
{
protected:
    void setup(void);
    void tear_down(void);

    /*! Correctness tests against SF::StateMachine class for basic feature implementation */
    void TestStateMachineBasics(void);

    /*! State product operation */
    void TestStateProductOperator(void);

    /*! Tests of SafeComp14 scenario */
    void TestSafecompScenario(void);

public:
    SFGCMTest(void);
    ~SFGCMTest(void);
};

#endif // _SFGCMTest_h
