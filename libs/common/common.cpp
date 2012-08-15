/*

  Safety Framework for Component-based Robotics

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "common.h"
//#include <stdio.h>
#include <time.h>

namespace SF {

StrVecType GetMiddlewareInfo(void)
{
    StrVecType info;
#ifdef SF_HAS_CISST
    info.push_back(GetCISSTInfo());
#endif
    return info;
}

void GetMiddlewareInfo(StrVecType & info)
{
#ifdef SF_HAS_CISST
    info.push_back(GetCISSTInfo());
#endif
}

#ifdef SF_HAS_CISST
#include <cisstConfig.h>
#include <cisstRevision.h>
std::string GetCISSTInfo(void)
{
    std::stringstream ss;
    ss << "CISST: version: " << CISST_VERSION << ", svn revision: " << CISST_WC_REVISION;

    return ss.str();
}
#endif

/*! Returns current UTC time as formatted string: e.g. "2011-09-12T21:33:12Z"

    Time zone codes in the U.S. 

    AST	    ATLANTIC STANDARD TIME	UTC - 4
    EST	    EASTERN STANDARD TIME	UTC - 5
    EDT	    EASTERN DAYLIGHT TIME	UTC - 4
    CST	    CENTRAL STANDARD TIME	UTC - 6
    CDT	    CENTRAL DAYLIGHT TIME	UTC - 5
    MST	    MOUNTAIN STANDARD TIME	UTC - 7
    MDT	    MOUNTAIN DAYLIGHT TIME	UTC - 6
    PST	    PACIFIC STANDARD TIME	UTC - 8
    PDT	    PACIFIC DAYLIGHT TIME	UTC - 7
    AKST	ALASKA TIME	            UTC - 9
    AKDT	ALASKA DAYLIGHT TIME	UTC - 8
    HAST	HAWAII-ALEUTIAN STANDARD TIME	UTC - 10
    HADT	HAWAII-ALEUTIAN DAYLIGHT TIME	UTC - 9
    SST	    SAMOA STANDARD TIME	UTC - 11
    SDT	    SAMOA DAYLIGHT TIME	UTC - 10
    CHST	CHAMORRO STANDARD TIME	UTC +10

    References:

    [1] List of UTC time offsets (Wikipedia)
        : http://en.wikipedia.org/wiki/List_of_UTC_time_offsets
    [2] W3C Specification on the Date and Time Formats
        : http://www.w3.org/TR/NOTE-datetime
    [3] C++ Date & Time
        : http://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
*/
std::string GetCurrentUTCTimeString(void)
{
    // MJ TODO:  this const could be re-defined as enum to support different time zones
    static const int MST = -7;
    static const int UTC = 0;
    static const int CCT = +8;

    time_t rawtime;
    time(&rawtime);

    struct tm * ptm = gmtime(&rawtime);
    char buf[32];
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02dZ", 
                 ptm->tm_year + 1900,
                 ptm->tm_mon + 1,
                 ptm->tm_mday,
                 (ptm->tm_hour + UTC) % 24,
                 ptm->tm_min,
                 ptm->tm_sec);

    return std::string(buf);
}

/* MJ TODO: later
std::string GetCurrentLocalTimeString(void)
{}
*/

};
