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
// This test demostrates which clocks in boost::chrono is available on the system.
//
#include "gtest/gtest.h"

#include <boost/chrono.hpp>

using namespace boost::chrono;

TEST(BoostChrono, Demo)
{
    std::cout << "system_clock::now(): " << system_clock::now() << std::endl;

    std::cout << "steady_clock::now(): ";
#ifdef BOOST_CHRONO_HAS_CLOCK_STEADY
    std::cout << steady_clock::now();
#else
    std::cout << "not supported on this platform";
#endif
    std::cout << std::endl;

    std::cout << "high_resolution_clock::now(): " << high_resolution_clock::now() << std::endl;

#ifdef BOOST_CHRONO_HAS_PROCESS_CLOCKS
    std::cout << "process_real_cpu_clock::now(): " << process_real_cpu_clock::now() << std::endl;
    std::cout << "process_user_cpu_clock::now(): " << process_user_cpu_clock::now() << std::endl;
    std::cout << "process_system_cpu_clock::now(): " << process_system_cpu_clock::now() << std::endl;
    std::cout << "process_cpu_clock::now(): " << process_cpu_clock::now() << std::endl;
#else
    std::cout << "process clocks: not supported on this platform" << std::endl;
#endif

    std::cout << "thread_clock::now(): ";
#ifdef BOOST_CHRONO_HAS_THREAD_CLOCK
    std::cout << thread_clock::now() << std::endl;
#else
    std::cout << "not supported on this platform";
#endif
    std::cout << std::endl;

    std::cout << "sizeof system_clock::time_point = " << sizeof(system_clock::time_point) << '\n';
    std::cout << "sizeof system_clock::duration = " << sizeof(system_clock::duration) << '\n';
    std::cout << "sizeof system_clock::rep = " << sizeof(system_clock::rep) << '\n';

#if 0 // example code for testing
    system_clock::duration delay(boost::chrono::milliseconds(5));
    system_clock::time_point start = system_clock::now();
    while (system_clock::now() - start <= delay) {}
    system_clock::time_point stop = system_clock::now();

    system_clock::duration elapsed = stop - start;
    std::cout << "paused " << boost::chrono::nanoseconds(elapsed).count() << " nanoseconds\n";

    std::time_t t(0);
    system_clock::duration elapsed2 = system_clock::now() - system_clock::from_time_t(t);
    std::cout << "--- epoch: " << boost::chrono::nanoseconds(elapsed2).count() << std::endl;
    std::cout << "--- now  : " << system_clock::now() << std::endl;

    long long count = boost::chrono::nanoseconds(elapsed2).count();
    std::cout << "count = " << count << std::endl;
#endif
}
