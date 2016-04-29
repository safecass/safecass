//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Apr 28, 2016
// Last revision: Apr 28, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
// This code is from tutorial examples of boost.chrono, which is available at
// http://www.boost.org/doc/libs/1_60_0/doc/html/chrono/users_guide.html#chrono.users_guide.examples.clocks.xtime_clock
// http://www.boost.org/doc/libs/1_60_0/libs/chrono/example/timeval_demo.cpp
//
#include <boost/chrono.hpp>
#include <boost/type_traits.hpp>

#include <iostream>

#if defined(BOOST_CHRONO_MAC_API)
#include <sys/time.h> //for gettimeofday and timeval
#endif

#if defined(BOOST_CHRONO_WINDOWS_API)
#include <windows.h>
#endif

#if defined(BOOST_CHRONO_WINDOWS_API)

namespace
{
#if defined UNDER_CE || BOOST_PLAT_WINDOWS_RUNTIME
    // Windows CE and Windows store does not define timeval
    struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
    };
#endif

    int gettimeofday(struct timeval * tp, void *)
    {
        FILETIME ft;
#if defined(UNDER_CE)
        // Windows CE does not define GetSystemTimeAsFileTime so we do it in two steps.
        SYSTEMTIME st;
        ::GetSystemTime(&st);
        ::SystemTimeToFileTime(&st, &ft);
#else
        ::GetSystemTimeAsFileTime(&ft);  // never fails
#endif
        long long t = (static_cast<long long>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
# if !defined( BOOST_MSVC ) || BOOST_MSVC > 1300 // > VC++ 7.0
        t -= 116444736000000000LL;
# else
        t -= 116444736000000000;
# endif
        t /= 10;  // microseconds
        tp->tv_sec = static_cast<long>(t / 1000000UL);
        tp->tv_usec = static_cast<long>(t % 1000000UL);
        return 0;
    }
}  // unnamed namespace

#endif

// Define timeeval-like struct to be used as the representation type for both
// boost::chrono::duration and boost::chrono::time_point.
class xtime {
private:
    long tv_sec;
    long tv_usec;

    void fixup() {
        if (tv_usec < 0) {
            tv_usec += 1000000;
            --tv_sec;
        }
    }

public:
    explicit xtime(long sec, long usec) {
        tv_sec = sec;
        tv_usec = usec;
        if (tv_usec < 0 || tv_usec >= 1000000) {
            tv_sec += tv_usec / 1000000;
            tv_usec %= 1000000;
            fixup();
        }
    }

    explicit xtime(long long usec) {
        tv_usec = static_cast<long>(usec % 1000000);
        tv_sec  = static_cast<long>(usec / 1000000);
        fixup();
    }

    // explicit
    operator long long() const {return static_cast<long long>(tv_sec) * 1000000 + tv_usec;}

    xtime& operator += (xtime rhs) {
        tv_sec += rhs.tv_sec;
        tv_usec += rhs.tv_usec;
        if (tv_usec >= 1000000) {
            tv_usec -= 1000000;
            ++tv_sec;
        }
        return *this;
    }

    xtime& operator -= (xtime rhs) {
        tv_sec -= rhs.tv_sec;
        tv_usec -= rhs.tv_usec;
        fixup();
        return *this;
    }

    xtime& operator %= (xtime rhs) {
        long long t = tv_sec * 1000000 + tv_usec;
        long long r = rhs.tv_sec * 1000000 + rhs.tv_usec;
        t %= r;
        tv_sec = static_cast<long>(t / 1000000);
        tv_usec = static_cast<long>(t % 1000000);
        fixup();
        return *this;
    }

    friend xtime operator+(xtime x, xtime y) {return x += y;}
    friend xtime operator-(xtime x, xtime y) {return x -= y;}
    friend xtime operator%(xtime x, xtime y) {return x %= y;}

    friend bool operator==(xtime x, xtime y)
        { return (x.tv_sec == y.tv_sec && x.tv_usec == y.tv_usec); }

    friend bool operator<(xtime x, xtime y) {
        if (x.tv_sec == y.tv_sec)
            return (x.tv_usec < y.tv_usec);
        return (x.tv_sec < y.tv_sec);
    }

    friend bool operator!=(xtime x, xtime y) { return !(x == y); }
    friend bool operator> (xtime x, xtime y) { return y < x; }
    friend bool operator<=(xtime x, xtime y) { return !(y < x); }
    friend bool operator>=(xtime x, xtime y) { return !(x < y); }

    friend std::ostream& operator<<(std::ostream& os, xtime x)
        {return os << '{' << x.tv_sec << ',' << x.tv_usec << '}';}
};

// Clock based on timeval-like struct
class xtime_clock
{
public:
    typedef xtime                                  rep;
    typedef boost::micro                           period;
    typedef boost::chrono::duration<rep, period>   duration;
    typedef boost::chrono::time_point<xtime_clock> time_point;

    static time_point now()
    {
    #if defined(BOOST_CHRONO_WINDOWS_API)
        time_point t(duration(xtime(0)));
        gettimeofday((timeval*)&t, 0);
        return t;

    #elif defined(BOOST_CHRONO_MAC_API)

        time_point t(duration(xtime(0)));
        gettimeofday((timeval*)&t, 0);
        return t;

    #elif defined(BOOST_CHRONO_POSIX_API)
        //time_point t(0,0);

        timespec ts;
        ::clock_gettime( CLOCK_REALTIME, &ts );

        xtime xt( ts.tv_sec, ts.tv_nsec/1000);
        return time_point(duration(xt));

    #endif  // POSIX
    }
};
