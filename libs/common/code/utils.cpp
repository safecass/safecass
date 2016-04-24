//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : May 27, 2012
// Last revision: Apr 21, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include <time.h>
#include <algorithm>
#include <functional>
#include <cctype>
#include <math.h> // modf
#include <iomanip> // setw

#include "config.h"

#if SAFECASS_ON_LINUX
#include <stdio.h> // sprintf
#endif
#if SC_HAS_CISST
#include <cisstOSAbstraction/osaGetTime.h>
#endif
#include "common/utils.h"

TimestampType _dummy = SC::GetCurrentTimestamp();

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
// TODO: (at least) millisecond resolution?
std::string SC::GetCurrentUTCTimeString(void)
{
    // TODO:  this const could be re-defined as enum to support different time zones
    // static const int MST = -7;
    static const int UTC = 0;
    // static const int CCT = +8;

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

std::string SC::GetUTCTimeString(TimestampType timestamp)
{
    // Temporarily disabled -- FIXME later 
#if 0
    // static const int MST = -7;
    static const int UTC = 0;
    // static const int CCT = +8;

    double fractpart, intpart;
    fractpart = modf(timestamp, &intpart);

    time_t tick = (time_t)intpart;
    struct tm * ptm = gmtime(&tick);

    char buf[32];
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03.0fZ",
                 ptm->tm_year + 1900,
                 ptm->tm_mon + 1,
                 ptm->tm_mday,
                 (ptm->tm_hour + UTC) % 24,
                 ptm->tm_min,
                 ptm->tm_sec,
                 fractpart * 1000.0);

    return std::string(buf);
#endif
    return "";
}

std::string & SC::ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(),
                                    s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string & SC::rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(),
                         s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
std::string & SC::trim(std::string &s)
{
    return ltrim(rtrim(s));
}

// to lowercase
void SC::to_lowercase(std::string & s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

// to uppercase
void SC::to_uppercase(std::string & s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}

void SC::PrintTime(TimestampType t, std::ostream & os)
{
    using namespace boost::chrono;

    time_t c_time = system_clock::to_time_t(t);
    std::tm * tmptr = std::localtime(&c_time);
    system_clock::duration d = t.time_since_epoch();

    //const std::ios::fmtflags f(os.flags());

    os << tmptr->tm_hour << ':'
       << tmptr->tm_min << ':'
       << tmptr->tm_sec << '.'
       << (d - duration_cast<seconds>(d)).count();

    //os.flags(f);
}

#if 0
    boost::chrono::system_clock::time_point time_limit
        = boost::chrono::system_clock::now() +
          boost::chrono::seconds(4) +
          boost::chrono::milliseconds(500);

    PrintTime(GetCurrentTimestamp());
    std::cout << std::endl;

    PrintTime(time_limit);
    std::cout << std::endl;

    std::cout << GetCurrentTimestampString() << std::endl;
#endif

TimestampType SC::GetCurrentTimestamp(void)
{
    return boost::chrono::system_clock::now();
}

std::string SC::GetCurrentTimestampString(void)
{
    std::stringstream ss;
    PrintTime(boost::chrono::system_clock::now(), ss);

    return ss.str();
}
