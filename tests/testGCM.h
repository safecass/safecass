//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 18, 2012
// Last revision: Apr 23, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
// Unit tests for the three-component system that SafeComp14 paper describes.
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

    void TestInitialStates(void);

public:
    SFGCMTest(void);
    ~SFGCMTest(void);
};

#endif // _SFGCMTest_h
