//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung
//
//------------------------------------------------------------------------
//
// Created on   : Jul 6, 2012
// Last revision: Apr 14, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "cpptest.h"
#include <boost/program_options.hpp>

// test suites
#include "testJson.h"

#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;
namespace po = boost::program_options;

// TODO: additional options for compiler output: Generic, BCC, GCC, MSVS 
typedef enum {TERSE, VERBOSE, COMPILER, HTML} OUTPUT;

int main(int argc, char ** argv)
{
    // Declare the supported options.
    po::options_description desc("unit-tests launcher");
    desc.add_options()
        ("help", "Print this help")
        ("output,o", po::value<string>(), "output format. arg=[terse|verbose|compiler|html]")
        ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch(const exception & e) {
        cout << e.what() << endl << endl;
        cout << desc << endl;
        return 1;
    }

    // help
    if (vm.count("help")) {
        cout << desc << endl;
        return 1;
    }

    // output
    OUTPUT outputFormat;
    if (vm.count("output")) {
        std::string output = vm["output"].as<string>();
        std::transform(output.begin(), output.end(), output.begin(), ::tolower);
        if (output.compare("terse") == 0)
            outputFormat = TERSE;
        else if (output.compare("verbose") == 0)
            outputFormat = VERBOSE;
        else if (output.compare("compiler") == 0)
            outputFormat = COMPILER;
        else if (output.compare("html") == 0)
            outputFormat = HTML;
        else {
            cout << "unknown output format: " << vm["output"].as<string>() << endl << endl;
            cout << desc << endl;
            return 1;
        }
    } else {
        outputFormat = TERSE;
    }

    SFTests tests;

    switch (outputFormat) {
    case TERSE:
        {
            Test::TextOutput output(Test::TextOutput::Terse);
            return (tests.run(output) ? 0 : 1);
        }
    case VERBOSE:
        {
            Test::TextOutput output(Test::TextOutput::Verbose);
            return (tests.run(output) ? 0 : 1);
        }
    case COMPILER:
        {
            Test::CompilerOutput output;
            return (tests.run(output) ? 0 : 1);
        }
    case HTML:
        {
            Test::HtmlOutput output;
            ofstream outputFile;
            outputFile.open("./result.html");

            cout << "Generating output file (\"result.html\") ... ";
            bool success = tests.run(output);
            if (success) {
                cout << "success" << endl;
            } else {
                cout << "failed" << endl;
            }

            output.generate(outputFile, true, "CASROS");
            outputFile.close();

            return 0;
        }
    }
}
