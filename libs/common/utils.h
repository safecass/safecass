//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : May 16, 2012
// Last revision: Aug 21, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/casros
//
#ifndef _utils_h
#define _utils_h

#include "common.h"

namespace SC {

//
// String Manipulation Utils
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

//
// Time-related Utils
//
//! Returns current timestamp
TimestampType GetCurrentTimestamp(void);

//! Returns current timestamp string
std::string GetCurrentTimestampString(void);

//! Get current UTC time
std::string GetCurrentUTCTimeString(void);

//! Get given timestamp in UTC format
std::string GetUTCTimeString(TimestampType timestamp);

//! Print timestamp
void PrintTime(TimestampType t, std::ostream & os = std::cout);

};

#endif // _utils_h
