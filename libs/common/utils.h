//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 16, 2012
// Last revision: Apr 21, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _utils_h
#define _utils_h

#include "common.h"

namespace SF {

//-------------------------------------------------- 
// String
//
/*! trim from start */
std::string &ltrim(std::string &s);

/*! trim from end */
std::string &rtrim(std::string &s);

/*! trim from both ends */
std::string &trim(std::string &s);

/*! to lowercase */
void to_lowercase(std::string & s);

/*! to uppercase */
void to_uppercase(std::string & s);

//-------------------------------------------------- 
// Time
//
/*! Get current UTC time */
std::string GetCurrentUTCTimeString(void);

};

#endif // _utils_h
