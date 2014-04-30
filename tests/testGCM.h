//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 18, 2012
// Last revision: Apr 28, 2014
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
    /*! Setup for this test suite */
    void setup(void);

    /*! Tear down for this test suite */
    void tear_down(void);

    /*! Check if initial states are correct */
    void TestInitialStates(void);

    /*! Check if internal filters are setup properly */
    void TestFilterInstalled(void);

    /*! Check if events are handled and propagated to other components correctly */
    void TestEventPropagation(void);

public:
    /*! Constructor */
    SFGCMTest(void);

    /*! Destructor */
    ~SFGCMTest(void);
};

#endif // _SFGCMTest_h
