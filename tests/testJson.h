//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 6, 2012
// Last revision: Mar 23, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _SFJSONTest_h
#define _SFJSONTest_h

#include "cpptest.h"

#include <iostream>
#include <sstream>

class SFJSONTest: public Test::Suite 
{
    std::stringstream SampleJSON;

    void TestJSONRead(void);
    void TestJSONReadFromFile(void);
    void TestJSONWrite(void);
    void TestJSONWriteToFile(void);

public:
    SFJSONTest();
    ~SFJSONTest() {}
};

#endif // _SFJSONTest_h
