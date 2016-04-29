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

#include "common/common.h"

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
std::string to_lowercase(const std::string & s);

/*! to uppercase */
void to_uppercase(std::string & s);
std::string to_uppercase(const std::string & s);

//
// Time-related Utils
//
//! Returns current timestamp
TimestampType GetCurrentTimestamp(void);

//! Returns current timestamp string
/*!
    \param clockFormat Specifies time representation format.  If true, current
    timestamp is printed out in human-readable format (e.g., 23:33:12.043243).
    If false, current timestamp is printed out as a counter since epoch (Jan 1,
    1970).  True by default.
*/
std::string GetCurrentTimestampString(bool humanReadable = true);

//! Get current UTC time
std::string GetCurrentUTCTimeString(void);

//! Get given timestamp in UTC format
std::string GetUTCTimeString(TimestampType timestamp);

//! Print timestamp
void PrintTime(TimestampType t, std::ostream & os = std::cout);

};

#endif // _utils_h
