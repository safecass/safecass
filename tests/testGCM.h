//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Apr 18, 2012
// Last revision: Apr 18, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _SFGCMTest_h
#define _SFGCMTest_h

#include "cpptest.h"

#include <iostream>
#include <sstream>

class SFGCMTest: public Test::Suite 
{
protected:
    virtual void setup(void);
    virtual void tear_down(void);

    void test(void);
    void test2(void);

public:
    SFGCMTest();
    ~SFGCMTest() {}
};

#endif // _SFGCMTest_h
