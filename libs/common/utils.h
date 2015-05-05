//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 16, 2012
// Last revision: Aug 5, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _utils_h
#define _utils_h

#include "common.h"

namespace SC {

//-------------------------------------------------- 
// String
//-------------------------------------------------- 
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
/*! Typedef for representation of timestamp */
// TODO: refactor time-related semantics within the framework for consistent use of time
// at higher precision (possibly between different processes).
typedef double TimestampType;

// TODO: integrate boost time/date library
TimestampType GetCurrentTimeTick(void);

/*! Get current UTC time */
std::string GetCurrentUTCTimeString(void);

/*! Get given timestamp in UTC format */
std::string GetUTCTimeString(TimestampType timestamp);

};

#endif // _utils_h
