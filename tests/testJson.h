/*

  Safety Framework for Component-based Robotics

  Created on: July 6, 2012

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _SFTests_h
#define _SFTests_h

#include "cpptest.h"

#include <iostream>
#include <sstream>

#define TEST_DEBUG( _cond, _message )\
    if (_cond) { std::cout << "FAILED (" << __LINE__ << ") : " << _message << std::endl; }

class SFTests: public Test::Suite 
{
    std::stringstream SampleJSON;

    void TestJSONRead(void);
    void TestJSONReadFromFile(void);
    void TestJSONWrite(void);
    void TestJSONWriteToFile(void);

public:
    SFTests();
    ~SFTests() {}
};

#endif // _SFTests_h
