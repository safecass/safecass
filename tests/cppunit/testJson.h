//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 6, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// URL          : https://github.com/minyang/safecass
//
#ifndef _SCJSONTest_h
#define _SCJSONTest_h

#include "cpptest.h"

#include <iostream>
#include <sstream>

class SCJsonTest: public Test::Suite
{
    std::stringstream SampleJSON;

    void TestJSONRead(void);
    void TestJSONReadFromFile(void);
    void TestJSONWrite(void);
    void TestJSONWriteToFile(void);

public:
    SCJsonTest();
    ~SCJsonTest() {}
};

#endif // _SCJSONTest_h
