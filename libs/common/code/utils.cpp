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
// Boost.Chrono examples and tutorials:
//   http://www.boost.org/doc/libs/1_60_0/doc/html/chrono/users_guide.html
//
#include "common/utils.h"

#include <ctime>
#include <algorithm>
#include <functional>
#include <cctype>
#include <math.h> // modf
#include <iomanip> // setw

//#include "config.h"

#if SAFECASS_ON_LINUX
#include <stdio.h> // sprintf
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

std::string SC::to_lowercase(const std::string & s)
{
    std::string _s(s);
    to_lowercase(_s);
    return _s;
}

// to uppercase
void SC::to_uppercase(std::string & s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}

std::string SC::to_uppercase(const std::string & s)
{
    std::string _s(s);
    to_uppercase(_s);
    return _s;
}

void SC::PrintTime(TimestampType t, std::ostream & os)
{
    using namespace boost::chrono;

    nanoseconds ns(t);
    microseconds us = duration_cast<microseconds>(ns);
    seconds s = duration_cast<seconds>(ns);

    time_t epoch = s.count();

    std::tm * tmptr = localtime(&epoch);

    // Save current settings
    const std::ios::fmtflags f(os.flags());
    const char prevFiller = os.fill('0');

    os << std::setw(2) << tmptr->tm_hour << ':'
       << std::setw(2) << tmptr->tm_min << ':'
       << std::setw(2) << tmptr->tm_sec << '.'
       << (us - s).count();

    // Restore saved settings
    os.flags(f);
    os.fill(prevFiller);
}

/*
#ifdef __APPLE__
  // Note that on OS X platform boost::steady_clock is not truly monotonic, so we use
  // system_clock instead.  Refer to https://svn.boost.org/trac/boost/ticket/7719)
  typedef boost::chrono::system_clock base_steady_clock;
#else
  typedef boost::chrono::steady_clock base_steady_clock;
#endif
*/
SC::TimestampType SC::GetCurrentTimestamp(void)
{
    return boost::chrono::duration_cast<boost::chrono::nanoseconds>
        (boost::chrono::system_clock::now().time_since_epoch()).count();
}

std::string SC::GetCurrentTimestampString(bool humanReadable)
{
    std::stringstream ss;

    if (humanReadable)
        PrintTime(GetCurrentTimestamp(), ss);
    else
        ss << GetCurrentTimestamp();

    return ss.str();
}
