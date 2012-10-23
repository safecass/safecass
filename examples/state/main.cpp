/*

  Safety Framework for Component-based Robotics

  Created on: October 12, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include <iostream>
#include "statemachine.h"
/*
// boost msm
#include <boost/msm/back/state_machine.hpp> // back-end
#include <boost/msm/front/state_machine_def.hpp> // front-end
*/

namespace msm = boost::msm;
namespace mpl = boost::mpl;

//namespace {
    // Macros to define msm event
#define MSM_EVENT(_eventName) struct _eventName {};
    MSM_EVENT(fault_detection);
    MSM_EVENT(fault_removal);
    MSM_EVENT(fault_activation);
    MSM_EVENT(error_detection);
    MSM_EVENT(error_removal);
    MSM_EVENT(error_propagation);
    MSM_EVENT(failure_detection);
    MSM_EVENT(failure_removal);
    MSM_EVENT(failure_stop);
#if 0
    // A "complicated" event type that carries some data.
	enum DiskTypeEnum
    {
        DISK_CD=0,
        DISK_DVD=1
    };
    struct cd_detected
    {
        cd_detected(std::string name, DiskTypeEnum diskType)
            : name(name),
            disc_type(diskType)
        {}

        std::string name;
        DiskTypeEnum disc_type;
    };
#endif

    // Define msm front-end (the FSM structure)
    struct FaultState_: public msm::front::state_machine_def<FaultState_>
    {
        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) 
        {
            std::cout << ">>> entering" << std::endl;
        }
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) 
        {
            std::cout << ">>> leaving" << std::endl;
        }

        // List of FSM states
        struct Normal: public msm::front::state<> 
        {
            // every (optional) entry/exit methods get the event passed.
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                std::cout << ">>> SM: entering: Normal" << std::endl;
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                std::cout << ">>> SM: leaving: Normal" << std::endl;
            }
        };
        struct Fault: public msm::front::state<> 
        {
            // every (optional) entry/exit methods get the event passed.
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                std::cout << ">>> SM: entering: Fault" << std::endl;
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                std::cout << ">>> SM: leaving: Fault" << std::endl;
            }
        };
        struct Error: public msm::front::state<> 
        {
            // every (optional) entry/exit methods get the event passed.
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                std::cout << ">>> SM: entering: Error" << std::endl;
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                std::cout << ">>> SM: leaving: Error" << std::endl;
            }
        };
        struct Failure: public msm::front::state<> 
        {
            // every (optional) entry/exit methods get the event passed.
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                std::cout << ">>> SM: entering: Failure" << std::endl;
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                std::cout << ">>> SM: leaving: Failure" << std::endl;
            }
        };

        // Initial state (must be defined)
        typedef Normal initial_state;

        // Transition actions
        void fault_detected(fault_detection const&) { std::cout << ">>> SM: fault detected\n"; }
        void back_to_normal(fault_removal const&) { std::cout << ">>> SM: fault removed\n"; }
        void fault_activated(fault_activation const&) { std::cout << ">>> SM: fault activated\n"; }
        void error_detected(error_detection const&) { std::cout << ">>> SM: error detected\n"; } 
        void back_to_normal(error_removal const&) { std::cout << ">>> SM: error removed\n"; }
        void error_propagated(error_propagation const&) { std::cout << ">>> SM: error propagated\n"; } 
        void failure_detected(failure_detection const&) { std::cout << ">>> SM: failure detected\n"; }
        void back_to_normal(failure_removal const&) { std::cout << ">>> SM: failure removed\n"; }
        void terminated(failure_stop const&) { std::cout << ">>> SM: failure stop and terminated\n"; }
        void deactivated(failure_stop const&) { std::cout << ">>> SM: failure stop and deactivated\n"; }

        // Transition table for FaultState
        typedef FaultState_ fs; // to make transition table cleaner

        struct transition_table : mpl::vector<
            //    Start     Event               Next      Action				 Guard
            //  +---------+-------------------+---------+----------------------+---------------+
          a_row < Normal  , fault_detection   , Fault   , &fs::fault_detected                  >,
          a_row < Normal  , error_detection   , Error   , &fs::error_detected                  >,
          a_row < Normal  , failure_detection , Failure , &fs::failure_detected                >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Fault   , fault_activation  , Error   , &fs::fault_activated                 >,
          a_row < Fault   , fault_removal     , Normal  , &fs::back_to_normal                  >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Error   , error_propagation , Failure , &fs::error_propagated                >,
          a_row < Error   , error_removal     , Normal  , &fs::back_to_normal                  >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Failure , failure_removal   , Normal  , &fs::back_to_normal                  >
          // TODO: Next state needs to be fixed
          //a_row < Failure , failure_stop      , Failure , &fs::terminated                      >, 
          // TODO: Next state needs to be fixed
          //a_row < Failure , failure_stop      , Failure , &fs::deactivated                     >
            //  +---------+-------------+---------+---------------------+----------------------+
        > {};
#if 0
            //    Start     Event         Next      Action				 Guard
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Stopped , play        , Playing , &p::start_playback                         >,
          a_row < Stopped , open_close  , Open    , &p::open_drawer                            >,
           _row < Stopped , stop        , Stopped                                              >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Open    , open_close  , Empty   , &p::close_drawer                           >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Empty   , open_close  , Open    , &p::open_drawer                            >,
            row < Empty   , cd_detected , Stopped , &p::store_cd_info   ,&p::good_disk_format  >,
            row < Empty   , cd_detected , Playing , &p::store_cd_info   ,&p::auto_start        >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Playing , stop        , Stopped , &p::stop_playback                          >,
          a_row < Playing , pause       , Paused  , &p::pause_playback                         >,
          a_row < Playing , open_close  , Open    , &p::stop_and_open                          >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Paused  , end_pause   , Playing , &p::resume_playback                        >,
          a_row < Paused  , stop        , Stopped , &p::stop_playback                          >,
          a_row < Paused  , open_close  , Open    , &p::stop_and_open                          >
            //  +---------+-------------+---------+---------------------+----------------------+
        > {};
#endif
        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
            std::cout << ">>> SM: no transition from state " << state
                << " on event " << typeid(e).name() << std::endl;
        }
    };

    // Pick a back-end
    typedef msm::back::state_machine<FaultState_> FaultState;

    //
    // Testing utilities.
    //
    //static char const* const state_names[] = { "Normal", "Fault", "Error", "Failure" };
    void pstate(FaultState const& p)
    {
        std::cout << " -> " << state_names[p.current_state()[0]] << std::endl;
    }

    void test()
    {        
		FaultState p;
        // needed to start the highest-level SM. This will call on_entry and mark the start of the SM
        p.start(); 
        // to fault, back to normal
        std::cout <<"\nto fault, back to normal\n";
        p.process_event(fault_detection()); pstate(p);
        p.process_event(fault_removal()); pstate(p);
        // to error, back to Normal
        std::cout <<"\nto error, back to Normal\n";
        p.process_event(error_detection()); pstate(p);
        p.process_event(error_removal()); pstate(p);
        // to failure, back to Normal
        std::cout <<"\nto failure, back to Normal\n";
        p.process_event(failure_detection()); pstate(p);
        p.process_event(failure_removal()); pstate(p);
        // to fault and to error and to faillure and to normal
        std::cout <<"\nto fault and to error and to faillure and to normal\n";
        p.process_event(fault_detection()); pstate(p);
        p.process_event(fault_activation()); pstate(p);
        p.process_event(error_propagation()); pstate(p);
        p.process_event(failure_removal()); pstate(p);
        /* TODO
        // to failure and terminated
        p.process_event(failure_detection()); pstate(p);
        p.process_event(failure_stop()); pstate(p);
        // to failure and deactivated
        p.process_event(failure_detection()); pstate(p);
        */

        /*
        p.process_event(open_close()); pstate(p);
        // will be rejected, wrong disk type
        p.process_event(
            cd_detected("louie, louie",DISK_DVD)); pstate(p);
        p.process_event(
            cd_detected("louie, louie",DISK_CD)); pstate(p);
		p.process_event(play());

        // at this point, Play is active      
        p.process_event(pause()); pstate(p);
        // go back to Playing
        p.process_event(end_pause());  pstate(p);
        p.process_event(pause()); pstate(p);
        p.process_event(stop());  pstate(p);
        // event leading to the same state
        // no action method called as it is not present in the transition table
        p.process_event(stop());  pstate(p);
        */
        std::cout << "stop fsm" << std::endl;
        p.stop();
    }

    int main()
    {
        SF::StateMachine sm;

        //test();
        return 0;
    }
//};




#if 0
#include <time.h> // for srand(time(NULL))

#include "config.h"
#include "common.h"
#include "json.h"
#include "monitor.h"
#include "cisstMonitor.h"
#include "threshold.h"

#include <cisstCommon/cmnGetChar.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif

using namespace SF;

// Example task that simulates sensor wrapper
class SensorReadingTask: public mtsTaskPeriodic {
public:
    static const double ThresholdForceX;
    static const std::string NameOfForceX;

protected:
    /*! Force reading on x-axis: Application-specific variable to monitor */
    double ForceX;
    /*! Force threshold of x-axis: Application-specific pre-defined threshold */
    double Threshold;
    /*! Time when test began */
    double Tic;

public:
    SensorReadingTask(const std::string & name, double period) : mtsTaskPeriodic(name, period, false, 5000)
    {
        StateTable.AddData(ForceX, SensorReadingTask::NameOfForceX);
        Threshold = SensorReadingTask::ThresholdForceX; // force threshold value (arbitrary for test)
        Tic = osaGetTime();
    }
    ~SensorReadingTask() {}

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();
        
        // Update sensor readings
        ForceX = Threshold;

        // Inject fault event at 10% probability
        if (rand() % 100 > 90) {
            ForceX += ForceX * 0.1 + (double)(rand() % 10) * 0.1;
        }

        // Check if force reading exceeds the pre-defined threshold
        if (ForceX - Threshold > 0.0) {
            std::cout << "[" << osaGetTime() - Tic << "] Sensor reading " << ForceX 
                << " exceeds threshold " << Threshold << " by " << ForceX - Threshold << std::endl;
        }
    }
    void Cleanup(void) {}
};

const double SensorReadingTask::ThresholdForceX = 10.0;
const std::string SensorReadingTask::NameOfForceX = "ForceX";

// Create periodic task
bool CreatePeriodicThread(const std::string & componentName, double period);
// Install monitors to monitor quantities in real-time
//bool InstallMonitor(const std::string & targetComponentName, unsigned int frequency);
// Install filters
bool InstallFilter(const std::string & targetComponentName);

// Local component manager
mtsManagerLocal * ComponentManager = 0;
// Test periodic task
SensorReadingTask * task = 0;

int main(int argc, char *argv[])
{
    srand(time(NULL));

#if (CISST_OS == CISST_LINUX_XENOMAI)
    mlockall(MCL_CURRENT|MCL_FUTURE);
#endif

#if ENABLE_G2LOG
    // Logger setup
    g2LogWorker logger(argv[0], "./");
    g2::initializeLogging(&logger);
    std::cout << "Log file: \"" << logger.logFileName() << "\"\n" << std::endl;
#endif

    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    //cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    
    // Get instance of the cisst Component Manager
    mtsComponentManager::InstallSafetyCoordinator();
    ComponentManager = mtsComponentManager::GetInstance();

    // Print information about middleware(s) available
    StrVecType info;
    GetMiddlewareInfo(info);
    std::cout << "Middleware(s) detected: ";
    if (info.size() == 0) {
        std::cout << "none" << std::endl;
    } else {
        std::cout << std::endl;
        for (size_t i = 0; i < info.size(); ++i) {
            std::cout << "[" << (i+1) << "] " << info[i] << std::endl;
        }
    }
    std::cout << std::endl;

    // Create simulated sensor reading component
    std::string componentName("sensorWrapper");

    // Create periodic task
    if (!CreatePeriodicThread(componentName, 100 * cmn_ms)) {
        SFLOG_ERROR << "Failed to add periodic component \"" << componentName << "\"" << std::endl;
        return 1;
    }

    // Install filters
    if (!InstallFilter(componentName)) {
        SFLOG_ERROR << "Failed to install monitor for periodic component \"" << componentName << "\"" << std::endl;
        return 1;
    }
    
    // Activate all the monitors and filters installed
    if (ComponentManager->GetCoordinator()) {
        if (!ComponentManager->GetCoordinator()->DeployMonitorsAndFDDs()) {
            SFLOG_ERROR << "Failed to deploy monitors and FDDs" << std::endl;
            return 1;
        }
    } else {
        SFLOG_ERROR  << "Failed to get coordinator in this process";
        return 1;
    }

    // Create and run all components
    ComponentManager->CreateAll();
    ComponentManager->WaitForStateAll(mtsComponentState::READY);

    ComponentManager->StartAll();
    ComponentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    std::cout << "Press 'q' to quit." << std::endl;
    std::cout << "Running periodic tasks ";

    // loop until 'q' is pressed
    int key = ' ';
    while (key != 'q') {
        key = cmnGetChar();
        osaSleep(100 * cmn_ms);
    }
    std::cout << std::endl;

    // Clean up resources
    SFLOG_INFO << "Cleaning up..." << std::endl;

#if (CISST_OS != CISST_LINUX_XENOMAI)
    ComponentManager->KillAll();
    ComponentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
#endif   

    ComponentManager->Cleanup();

    return 0;
}

// Create periodic thread
bool CreatePeriodicThread(const std::string & componentName, double period)
{
    // Create periodic thread
    task = new SensorReadingTask(componentName, period);
    if (!ComponentManager->AddComponent(task)) {
        SFLOG_ERROR << "Failed to add component \"" << componentName << "\"" << std::endl;
        return false;
    }

    return true;
}

bool InstallFilter(const std::string & targetComponentName)
{
    mtsSafetyCoordinator * coordinator = ComponentManager->GetCoordinator();
    if (!coordinator) {
        SFLOG_ERROR  << "Failed to get coordinator in this process";
        return false;
    }

    // Create active types of thresholding filters
    SF::FilterThreshold * filterThresholdActive = 
        new FilterThreshold(// Common arguments
                            SF::FilterBase::FEATURE, // filter category
                            targetComponentName,     // name of target component
                            SF::FilterBase::ACTIVE,  // monitoring type
                            // Arguments specific to this filter
                            SensorReadingTask::NameOfForceX,  // name of input signal
                            SensorReadingTask::ThresholdForceX, // threshold
                            0.5,      // margin
                            0.0,      // output 0 (input is below threshold)
                            1.0);     // output 1 (input exceeds threshold)

    // Declare the filter as the last filter of FDD pipeline.
    filterThresholdActive->DeclareLastFilterOfPipeline();

    // Install filter to the target component [active monitoring]
    if (!coordinator->AddFilter(filterThresholdActive)) {
        SFLOG_ERROR << "Failed to add ACTIVE filter \"" << filterThresholdActive->GetFilterName() << "\""
            << " to target component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }
    SFLOG_INFO << "Successfully installed filter: \"" << filterThresholdActive->GetFilterName() << "\"" << std::endl;
    std::cout << *filterThresholdActive << std::endl;

    return true;
}
#endif
