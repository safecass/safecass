//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 10, 2014
// Last revision: Jul 28, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "accessor.h"
#include "QtJsBridge/jsApiHandler.h"
#include <cisstMultiTask/mtsManagerLocal.h>

#include <iomanip>

#define VERBOSE 0

using namespace SF;

// JSON object that has complete information of all states of the entire system (including 
// multiple processes)
// TODO: issue of shared resource with this
SF::JSON CasrosSystemStates;
SF::JSON::JSONVALUE & CasrosSystemStatesRoot = CasrosSystemStates.GetRoot();

// for safety coordinator color selection
static int SCColorCode = 0;

#define COLOR_RED1    "#ff0000"
#define COLOR_RED2    "#c91a26"
#define COLOR_YELLOW1 "#fffa00"
#define COLOR_YELLOW2 "#ffda00"
#define COLOR_WHITE   "#ffffff"
#define COLOR_GREY    "#e3e7ee"

const std::string GetColorCodeForState(unsigned int state, bool projectedState = false)
{
    if (projectedState) {
        switch (static_cast<State::StateType>(state)) {
        case State::NORMAL:  return COLOR_GREY;
        case State::WARNING: return COLOR_YELLOW2;
        case State::ERROR:   return COLOR_RED2;
        case State::FAILURE:
        case State::INVALID: return "black";
        }
    } else {
        switch (static_cast<State::StateType>(state)) {
        case State::NORMAL:  return COLOR_WHITE;
        case State::WARNING: return COLOR_YELLOW1;
        case State::ERROR:   return COLOR_RED1;
        case State::FAILURE:
        case State::INVALID: return "green";
        }
    }
}

const std::string GetColorCodeForSafetyCoordinator(void)
{
    const int N = 5;

    switch (SCColorCode++ % N) {
    case 0: return "#2e6093";
    case 1: return "#3a5988";
    case 2: return "#9d4e87";
    case 3: return "#92538c";
    case 4: return "#72659d";
    default:
            return "#ffffff";
    }
}

//
// Subscriber callback
//
void ViewerSubscriberCallback::CallbackControl(SF::Topic::Control::CategoryType category, const std::string & json)
{
    SFASSERT(false);
}

void ViewerSubscriberCallback::CallbackData(SF::Topic::Data::CategoryType category, const std::string & json)
{
    std::string categoryName;
    switch (category) {
    case SF::Topic::Data::MONITOR:
        categoryName = "MONITOR";
        break;
    case SF::Topic::Data::EVENT:
        categoryName = "EVENT";
        break;
    case SF::Topic::Data::READ_RES:
        categoryName = "READ_RES";
        break;
    default:
        categoryName = "INVALID";
        break;
    }

#if VERBOSE
    SFLOG_INFO << "message received - topic: " << TopicName << ", category: " << categoryName << " ]" << std::endl;
    if (category == SF::Topic::Data::READ_RES) {
        std::cout << "ViewerSubscriberCallback:" << __LINE__ << " Received json => " << json << std::endl;
    }
#endif

    // input JSON: state information from CASROS
    SF::JSON in;
    if (!in.Read(json.c_str())) {
        SFLOG_ERROR << "Failed to parse json: " << json << std::endl;
        return;
    }

    const SF::JSON::JSONVALUE & inroot = in.GetRoot();

    // Get target safety coordinator (assigned as process name in cisst)
    //const std::string targetProcessName = 
        //JSON::GetSafeValueString(inroot["target"], "safety_coordinator");
    //const std::string thisProcessName = mtsManagerLocal::GetInstance()->GetProcessName();
    //if (targetProcessName.compare("*") != 0 && (targetProcessName != thisProcessName))
        //return;

    // Check command type
    const std::string cmd = SF::JSON::GetSafeValueString(inroot, "cmd");
    if (cmd.compare("message") == 0) {
        std::cout << SF::JSON::GetSafeValueString(inroot, "msg") << std::endl;
        return;
    } else {
        if (cmd.compare("state_list") != 0) {
            SFLOG_INFO << "not state information: " << cmd << std::endl;
            return;
        }
    }

    // extract name of safety coordinator
    const std::string nameOfThisSC = SF::JSON::GetSafeValueString(inroot, "safety_coordinator");
    // update state cache (overwrite)
    CasrosSystemStatesRoot[nameOfThisSC] = inroot;

    // placeholder for D3
    SF::JSON D3States;
    SF::JSON::JSONVALUE D3StatesRoot = D3States.GetRoot();

    // iterate each state cache instance, convert it to D3 format, and stack it up.
    size_t cntSC = 0;
    JSON::JSONVALUE::iterator it = CasrosSystemStatesRoot.begin();
    const JSON::JSONVALUE::iterator itEnd = CasrosSystemStatesRoot.end();
    for (; it != itEnd; ++it) {
        SF::JSON outSC;
        SF::JSON::JSONVALUE & outSCroot = outSC.GetRoot();
        GenerateD3JSON(*it, outSCroot);
        D3StatesRoot[cntSC++] = outSCroot;
    }

    // get json-encoded string for state viewer
    const std::string outJson = SF::JSON::GetJSONString(D3StatesRoot);

    // Reset color code
    SCColorCode = 0;

    // refresh state chart
    QJsApiHandler::UpdateJSONState(outJson);
}

void ViewerSubscriberCallback::GenerateD3JSON(const JSON::JSONVALUE & inroot, JSON::JSONVALUE & outSCroot)
{
    // safety coordinator name
    outSCroot["name"] = SF::JSON::GetSafeValueString(inroot, "safety_coordinator");
    outSCroot["color"] = GetColorCodeForSafetyCoordinator();

    // for each component
    const JSON::JSONVALUE inComponents = inroot["components"];
    for (size_t i = 0; i < inComponents.size(); ++i) {
        // inputs for component
        const JSON::JSONVALUE inComponent = inComponents[i];
        // outputs for component
        SF::JSON outComponent;
        SF::JSON::JSONVALUE & outComponentRoot = outComponent.GetRoot();
        {
            // component name
            outComponentRoot["name"] = inComponent["name"];
            outComponentRoot["color"] = GetColorCodeForState(JSON::GetSafeValueUInt(inComponent, "s"), true);

            // outputs for component state (three views: system, framework, application)
            SF::JSON outStateComponent;
            SF::JSON::JSONVALUE & outStateComponentRoot = outStateComponent.GetRoot();
            // Get system view of component state
            SF::State stateComponentSystemView, stateComponentFrameworkView, stateComponentAppView;
            stateComponentFrameworkView = static_cast<State::StateType>(JSON::GetSafeValueUInt(inComponent, "s_F"));
            stateComponentAppView       = static_cast<State::StateType>(JSON::GetSafeValueUInt(inComponent, "s_A"));
            stateComponentSystemView = stateComponentFrameworkView * stateComponentAppView;

            outStateComponentRoot["name"] = "Component";
            outStateComponentRoot["color"] = GetColorCodeForState(stateComponentSystemView.GetState(), true);
            {
                size_t cntComponentState = 0;
                SF::JSON outStateFramework;
                SF::JSON::JSONVALUE & outStateFrameworkRoot = outStateFramework.GetRoot();
                {
                    outStateFrameworkRoot["name"] = "Framework";
                    outStateFrameworkRoot["color"] = GetColorCodeForState(stateComponentFrameworkView.GetState());
                }
                outStateComponentRoot["children"][cntComponentState++] = outStateFrameworkRoot;

                SF::JSON outStateApp;
                SF::JSON::JSONVALUE & outStateAppRoot = outStateApp.GetRoot();
                {
                    outStateAppRoot["name"] = "Application";
                    outStateAppRoot["color"] = GetColorCodeForState(stateComponentAppView.GetState());
                }
                outStateComponentRoot["children"][cntComponentState++] = outStateAppRoot;
            }
            size_t j = 0;
            outComponentRoot["children"][j++] = outStateComponentRoot;

            // outputs for provided interface
            // for each interface
            const JSON::JSONVALUE inPrvInterfaces = inComponent["interfaces_provided"];
            const size_t cntProvided = inPrvInterfaces.size();
            for (size_t k = 0; k < cntProvided; ++k) {
                // input for provided interface
                const JSON::JSONVALUE inPrvInterface = inPrvInterfaces[k];
                // output for provided interface
                SF::JSON outInterfaceProvidedEach;
                SF::JSON::JSONVALUE & outInterfaceProvidedEachRoot = outInterfaceProvidedEach.GetRoot();
                {
                    outInterfaceProvidedEachRoot["name"] = "Service";
                    outInterfaceProvidedEachRoot["color"] = 
                        GetColorCodeForState(JSON::GetSafeValueUInt(inPrvInterface, "service_state"), true);
                    size_t cnt = 0;
                    outInterfaceProvidedEachRoot["children"][cnt]["name"] = inPrvInterface["name"];
                    outInterfaceProvidedEachRoot["children"][cnt]["color"] =
                        GetColorCodeForState(JSON::GetSafeValueUInt(inPrvInterface, "state"));
                }
                //outInterfaceProvidedRoot["children"][k] = outInterfaceProvidedEachRoot;

                outComponentRoot["children"][j++] = outInterfaceProvidedEachRoot;
            }

            // outputs for required interface
            SF::JSON outInterfaceRequired;
            SF::JSON::JSONVALUE & outInterfaceRequiredRoot = outInterfaceRequired.GetRoot();
            size_t cntRequired = 0;
            {
                outInterfaceRequiredRoot["name"] = "Required";
                outInterfaceRequiredRoot["color"] = GetColorCodeForState(JSON::GetSafeValueUInt(inComponent, "s_R"), true);

                // for each interface
                const JSON::JSONVALUE inReqInterfaces = inComponent["interfaces_required"];
                cntRequired = inReqInterfaces.size();
                for (size_t k = 0; k < cntRequired; ++k) {
                    // input for provided interface
                    const JSON::JSONVALUE inReqInterface = inReqInterfaces[k];
                    // output for provided interface
                    SF::JSON outInterfaceRequiredEach;
                    SF::JSON::JSONVALUE & outInterfaceRequiredEachRoot = outInterfaceRequiredEach.GetRoot();
                    {
                        outInterfaceRequiredEachRoot["name"] = inReqInterface["name"];
                        outInterfaceRequiredEachRoot["color"] =
                            GetColorCodeForState(JSON::GetSafeValueUInt(inReqInterface, "state"));
                    }
                    outInterfaceRequiredRoot["children"][k] = outInterfaceRequiredEachRoot;
                }
            }
            if (cntRequired)
                outComponentRoot["children"][j++] = outInterfaceRequiredRoot;
        }
        outSCroot["children"][i] = outComponentRoot;
    }
    //std::cout << "###### " << JSON::GetJSONString(outSCroot) << std::endl;
}

//
// CASROS accessor for console
//
AccessorViewer::AccessorViewer(void)
    : SF::cisstAccessor(true,  // enablePublisherControl
                        false, // enablePublisherData
                        false,  // enableSubscriberControl
                        true,  // enableSubscriberData
                        0,//new ViewerSubscriberCallback(SF::Dict::TopicNames::CONTROL),
                        new ViewerSubscriberCallback(SF::Dict::TopicNames::DATA))
{
}

bool AccessorViewer::RequestStateList(const std::string & safetyCoordinatorName,
                                      const std::string & componentName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"component\": \"" << componentName << "\" }, "
          "\"request\": \"state_list\" }";
    if (!Publishers.Control->PublishControl(SF::Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestStateList: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested list of states" << std::endl;

    return true;
}
