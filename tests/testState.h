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
#ifndef _SCStateTest_h
#define _SCStateTest_h

#include "cpptest.h"

class mtsManagerLocal;

class SCStateTest: public Test::Suite 
{
protected:
    void setup(void);
    void tear_down(void);

    /*! Correctness tests against SC::StateMachine class for basic feature implementation */
    void TestStateMachineBasics(void);

    /*! State product operation */
    void TestStateProductOperator(void);

public:
    SCStateTest(void);
    ~SCStateTest(void);
};

#endif // _SCStateTest_h
