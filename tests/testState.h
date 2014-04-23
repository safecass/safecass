//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 22, 2012
// Last revision: Apr 22, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _SFStateTest_h
#define _SFStateTest_h

#include "cpptest.h"

class mtsManagerLocal;

class SFStateTest: public Test::Suite 
{
protected:
    void setup(void);
    void tear_down(void);

    /*! Correctness tests against SF::StateMachine class for basic feature implementation */
    void TestStateMachineBasics(void);

    /*! State product operation */
    void TestStateProductOperator(void);

public:
    SFStateTest(void);
    ~SFStateTest(void);
};

#endif // _SFStateTest_h
