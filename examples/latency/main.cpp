/*

  Safety Framework for Component-based Robotics

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include <iostream>
#include <stdlib.h>

#include "common.h"

int main(int argc, char *argv[])
{
#if 0
    if (argc != 3) {
        std::cout << "usage: latency period duration\n";
        std::cout << "       period  : thread period in msec\n";
        std::cout << "       duration: test duration in seconds\n";
        return 1;
    }

    const double period = atof(argv[1]);
    const double duration = atof(argv[2]);

    std::cout << "period: " << period << ", duration: " << duration << std::endl;
#endif

    // Print information about middleware(s) available
    SF::StrVecType info;
    SF::GetMiddlewareInfo(info);
    for (size_t i = 0; i < info.size(); ++i) {
        std::cout << info[i] << std::endl;
    }

    // Create test thread

    // TODO: thread creation

    // TODO: FDD setup (or monitor), history buffer??

    // TODO: start experiment

    // TODO: collect data during experiment

    // TODO: (visualization setup?)

    // TODO: post-processing, statistics

    return 0;
}
