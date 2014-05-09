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
#include "config.h"
#include "publisher.h"
#include "subscriber.h"
#include "dict.h"
// Header from the Clipo project; See CMakeLists.txt for more details.
#include "command_line_interpreter.hpp"

#include <cisstMultiTask/mtsManagerLocal.h>

namespace po = boost::program_options;

typedef std::vector<std::string> StrVec;

class ConsoleSubscriberCallback : public SF::SFCallback {
public:
    ConsoleSubscriberCallback() {}
    ~ConsoleSubscriberCallback() {}

    void Callback(const std::string & json) {
        std::cout << "SUBSCRIBER: " << json << std::endl;
    }
};

class Communicator {
public:
    typedef struct {
        osaThread       Thread;
        osaThreadSignal ThreadEventBegin;
        osaThreadSignal ThreadEventEnd;
        bool            Running;
    } InternalThreadType;
    
    /*! Ice publisher and subscriber */
    SF::Publisher *  Publisher;
    SF::Subscriber * Subscriber;

    /*! Callback for subscriber */
    ConsoleSubscriberCallback * SubscriberCallback;

    InternalThreadType ThreadSubscriber;

    Communicator(void) {
        //Publisher = new SF::Publisher(SF::Dict::TopicNames::data);
        Publisher = new SF::Publisher(SF::Dict::TopicNames::control);
        if (!Publisher->Startup()) {
            std::stringstream ss;
            ss << "Communicator: Failed to initialize publisher for topic \""
                << SF::Dict::TopicNames::data << "\"";
            cmnThrow(ss.str());
        }

        SubscriberCallback = new ConsoleSubscriberCallback;
        Subscriber = new SF::Subscriber(SF::Dict::TopicNames::control, SubscriberCallback);
        ThreadSubscriber.Thread.Create<Communicator, unsigned int>(this, &Communicator::RunSubscriber, 0);
        ThreadSubscriber.ThreadEventBegin.Wait();
    }

    ~Communicator() {
        if (Subscriber && ThreadSubscriber.Running) {
            ThreadSubscriber.Running = false;
            // Terminating subscriber needs to call shutdown() on the Ice communicator
            Subscriber->Stop();
            ThreadSubscriber.ThreadEventEnd.Wait();

            delete Subscriber;
            Subscriber = 0;
            // SubscriberCallback is deleted by Subscriber's destructor.
        }
        delete Publisher;
    }

    void * RunSubscriber(unsigned int CMN_UNUSED(arg))
    {
        ThreadSubscriber.Running = true;

        ThreadSubscriber.ThreadEventBegin.Raise();

        try {
            Subscriber->Startup();
            while (ThreadSubscriber.Running) {
                Subscriber->Run();
            }
        } catch (const Ice::InitializationException & e) {
            CMN_LOG_RUN_ERROR << "Communicator::RunSubscriber: ice init failed: " << e.what() << std::endl;
        } catch (const Ice::AlreadyRegisteredException & e) {
            CMN_LOG_RUN_ERROR << "Communicator::RunSubscriber: ice init failed: " << e.what() << std::endl;
        } catch (const std::exception & e) {
            CMN_LOG_RUN_ERROR << "Communicator::RunSubscriber: exception: " << e.what() << std::endl;
        }

        ThreadSubscriber.ThreadEventEnd.Raise();

        return 0;
    }
};

// cisst Component Manager
mtsManagerLocal * componentManager = 0;
// Ice communicator instances
Communicator * comm = 0;

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
       << "    help   : "NL
       << "    filter : "NL
       << "    state  : "NL
       << "    exit   : exit console"NL
       << "    quit   : same as exit'"NL NL
       << "For more information:"NL
       << "    CASROS on Github: https://github.com/minyang/casros"NL NL;

    std::cout << ss.str();
}

//------------------------------------------------------------ 
//  filter
//------------------------------------------------------------ 
void handler_filter(const std::vector<std::string> &parameters)
{
    std::cout << "==== filter\n";
    std::vector<std::string>::const_iterator it = parameters.begin();
    for (; it != parameters.end(); ++it)
        std::cout << *it << std::endl;
}

//------------------------------------------------------------ 
//  state
//------------------------------------------------------------ 
void handler_state(const std::vector<std::string> &parameters)
{
    std::cout << "==== state\n";
    std::vector<std::string>::const_iterator it = parameters.begin();
    for (; it != parameters.end(); ++it)
        std::cout << *it << std::endl;
}

//------------------------------------------------------------ 
//  exit / quit
//------------------------------------------------------------ 
void handler_exit(const std::vector<std::string> &)
{
    delete comm;

    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
    componentManager->Cleanup();

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

    // cisst logger setup
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    //cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);
    //cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);

    // TODO: add option to enable/disable SC
    mtsManagerLocal::InstallSafetyCoordinator();

    // Get local component manager instance
    try {
        componentManager = mtsManagerLocal::GetInstance();
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // Create publisher and subscriber
    comm = new Communicator;

    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);

    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // Instantiate interpreter
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

    return 0;
}
