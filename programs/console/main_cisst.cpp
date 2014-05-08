//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 7, 2014
// Last revision: May 7, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//

// Header from the Clipo project; See CMakeLists.txt for more details.
#include "command_line_interpreter.hpp"

namespace po = boost::program_options;

typedef std::vector<std::string> StrVec;

//------------------------------------------------------------ 
//  Help
//------------------------------------------------------------ 
void handler_help(const std::vector<std::string> &)
{
    std::cout << "==== help\n";
}

//------------------------------------------------------------ 
//  Filters
//------------------------------------------------------------ 
void handler_filter(const std::vector<std::string> &parameters)
{
    std::cout << "==== filter\n";
    std::vector<std::string>::const_iterator it = parameters.begin();
    for (; it != parameters.end(); ++it)
        std::cout << *it << std::endl;
}

//------------------------------------------------------------ 
//  States
//------------------------------------------------------------ 
void handler_state(const std::vector<std::string> &parameters)
{
    std::cout << "==== state\n";
    std::vector<std::string>::const_iterator it = parameters.begin();
    for (; it != parameters.end(); ++it)
        std::cout << *it << std::endl;
}

void handler_exit(const std::vector<std::string> &)
{
    exit(0);
}

int main(int argc, char **argv)
{
    // TODO: if argc = 1, interactive mode; argc >= 2, one-time execution of input commands
    // TODO: connect to icebox

    boost::cli::commands_description desc;
    desc.add_options()
        ("help",   po::value<StrVec>()->zero_tokens()->notifier(boost::bind(&handler_help, _1)))
        ("filter", po::value<StrVec>()->notifier(&handler_filter)->multitoken())
        ("state",  po::value<StrVec>()->notifier(&handler_state)->multitoken())
        ("exit",   po::value<StrVec>()->zero_tokens()->notifier(boost::bind(&handler_exit, _1)))
        ("quit",   po::value<StrVec>()->zero_tokens()->notifier(boost::bind(&handler_exit, _1)))
        ;

    boost::cli::command_line_interpreter cli(desc, ">");
    cli.interpret(std::cin);
}
