//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
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
#include "testUtil.h"
#include "utils.h"

using namespace SC;

SCUtilTest::SCUtilTest(void)
{
    // test registration
    TEST_ADD(SCUtilTest::TestGetUTCTimeString);
}

SCUtilTest::~SCUtilTest(void)
{
}

void SCUtilTest::setup()
{
}

void SCUtilTest::tear_down()
{
}

void SCUtilTest::TestGetUTCTimeString(void)
{
    //GMT: March 23, 2015 11:33:29 GMT
    //EDT: March 23, 2015 7:33:29 AM EDT GMT-4:00 DST
    double tick = 1427110409.0;
    std::string expected = "2015-03-23T11:33:29.000Z";
    TEST_ASSERT(expected.compare(SC::GetUTCTimeString(tick)) == 0);

    tick = 1427110409.123;
    expected = "2015-03-23T11:33:29.123Z";
    TEST_ASSERT(expected.compare(SC::GetUTCTimeString(tick)) == 0);

    tick = 1427110409.678;
    expected = "2015-03-23T11:33:29.678Z";
    TEST_ASSERT(expected.compare(SC::GetUTCTimeString(tick)) == 0);
}
