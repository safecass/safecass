//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 7, 2014
// Last revision: Jul 23, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "config.h"
#include "accessor.h"
// Header from the Clipo project; See CMakeLists.txt for more details.
#include "command_line_interpreter.hpp"
#include "common.h"

namespace po = boost::program_options;

using namespace SF;

//
// command handlers
//
void handler_filter(const std::vector<std::string> & args);
void handler_state(const std::vector<std::string> & args);
void handler_event(const std::vector<std::string> & args);
void handler_connection(const std::vector<std::string> & args);
void handler_service(const std::vector<std::string> & args);

// instance of casros network accessor
AccessorConsole * casrosAccessor = 0;

//------------------------------------------------------------ 
//  help
//------------------------------------------------------------ 
void handler_help(const std::vector<std::string> &)
{
    std::stringstream ss;

#define NL << std::endl
    ss << "CASROS console utility" NL NL
       << "USAGE: console [command]"NL NL
       << "commands:"NL
       << "    state"NL
       << "    filter"NL
       << "    event"NL
       << "    connection"NL
       << "    service"NL
       << "    help      : show this help"NL
       << "    exit/quit : exit console"NL NL
       << "For more information:"NL
       << "    CASROS on Github: https://github.com/minyang/casros"NL NL;

    std::cout << ss.str();
}

//------------------------------------------------------------ 
//  exit / quit
//------------------------------------------------------------ 
void handler_exit(const std::vector<std::string> &)
{
    delete casrosAccessor;

    exit(0);
}

//------------------------------------------------------------ 
//  main
//------------------------------------------------------------ 
int main(int argc, char **argv)
{
    // TODO: if argc = 1, interactive mode; argc >= 2, one-time execution of input commands
    // TODO: connect to icebox

#if SF_USE_G2LOG
    // Logger setup
    g2LogWorker logger(argv[0], "./");
    g2::initializeLogging(&logger);
    std::cout << "Log file: \"" << logger.logFileName() << "\"\n" << std::endl;
#endif

    // Create publisher and subscriber
    casrosAccessor = new AccessorConsole;

    // Instantiate interpreter
    boost::cli::commands_description desc;
    desc.add_options()
        ("help",       po::value<StrVecType>()->zero_tokens()->notifier(boost::bind(&handler_help, _1)))
        ("filter",     po::value<StrVecType>()->notifier(&handler_filter)->multitoken())
        ("state",      po::value<StrVecType>()->notifier(&handler_state)->multitoken())
        ("event",      po::value<StrVecType>()->notifier(&handler_event)->multitoken())
        ("connection", po::value<StrVecType>()->notifier(&handler_connection)->multitoken())
        ("service",    po::value<StrVecType>()->notifier(&handler_service)->multitoken())
        ("exit",       po::value<StrVecType>()->zero_tokens()->notifier(boost::bind(&handler_exit, _1)))
        ("quit",       po::value<StrVecType>()->zero_tokens()->notifier(boost::bind(&handler_exit, _1)))
        ;

    boost::cli::command_line_interpreter cli(desc, ">");
    cli.interpret(std::cin);

    return 0;
}
