/*

  Safety Framework for Component-based Robotics

  Created on: July 6, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "cpptest.h"
#include "testJson.h"

//#define HTML_OUTPUT

#ifdef HTML_OUTPUT
#include <cpptest-htmloutput.h>
#include <iostream>
#include <fstream>
#endif

int main(int argc, char ** argv)
{
    SFTests tests;

#ifndef HTML_OUTPUT
    Test::TextOutput output(Test::TextOutput::Verbose);
    //Test::TextOutput output(Test::TextOutput::Terse);

    return (tests.run(output) ? 0 : 1);
#else
    // html output test
    Test::HtmlOutput output;
    std::ofstream outputFile;
    outputFile.open("./result.html");

    bool success = tests.run(output);
    if (success) {
        std::cout << "success" << std::endl;
    } else {
        std::cout << "failed" << std::endl;
    }
    output.generate(outputFile, true, "Safety Framework");
    outputFile.close();
#endif

    return 0;
}
