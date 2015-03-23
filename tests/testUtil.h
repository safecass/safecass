//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2015 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Mar 23, 2015
// Last revision: Mar 23, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
// Unit tests for utilities.
//
#ifndef _SFUtilTest_h
#define _SFUtilTest_h

#include "cpptest.h"

class SFUtilTest: public Test::Suite 
{
protected:
    /*! Setup for this test suite */
    void setup(void);

    /*! Tear down for this test suite */
    void tear_down(void);

    /*! Test for GetUTCTimeString() */
    void TestGetUTCTimeString(void);

public:
    /*! Constructor */
    SFUtilTest(void);

    /*! Destructor */
    ~SFUtilTest(void);
};

#endif // _SFGCMTest_h
