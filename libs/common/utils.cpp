/*

  Safety Framework for Component-based Robotics

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "common.h"
#include <time.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
//#include <locale>

namespace SF {

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
std::string GetCurrentUTCTimeString(void)
{
    // TODO:  this const could be re-defined as enum to support different time zones
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

/* TODO
std::string GetCurrentLocalTimeString(void)
{}
*/

std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), 
                                    s.end(), 
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), 
                         s.rend(), 
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}

// to lowercase
void to_lowercase(std::string & s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

// to uppercase 
void to_uppercase(std::string & s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}

};