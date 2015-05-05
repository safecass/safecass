//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 10, 2014
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "accessor.h"
#include "QtJsBridge/jsApiHandler.h"
#include <cisstMultiTask/mtsManagerLocal.h>

#include <iomanip>

#define VERBOSE 0

using namespace SC;

// JSON object that has complete information of all states of the entire system (including 
// multiple processes)
// TODO: issue of shared resource with this
JsonWrapper CasrosSystemStates;
JsonWrapper::JsonValue & CasrosSystemStatesRoot = CasrosSystemStates.GetRoot();

// for safety coordinator color selection
static int SCColorCode = 0;

#define COLOR_RED1    "#ff0000"
#define COLOR_RED2    "#c91a26"
#define COLOR_YELLOW1 "#fffa00"
#define COLOR_YELLOW2 "#ffda00"
#define COLOR_WHITE   "#ffffff"
#define COLOR_GREY    "#e3e7ee"

// new color scheme based on thesis writing
typedef enum {
    LAYER_PROCESS,   // process layer
    LAYER_COMPONENT, // component layer
    LAYER_META,      // meta layer
    LAYER_OUTERMOST  // outermost layer
} LayerType;

#define COLOR_PROCESS   "#666666"
#define COLOR_COMPONENT "#999999"
#define COLOR_META      "#cccccc"
#define COLOR_NORMAL    "#ffffff"
#define COLOR_WARNING   "#ffcc00"
#define COLOR_ERROR     "#ff0000"

//const std::string GetColorCodeForState(unsigned int state, bool projectedState = false)
const std::string GetColorCodeForState(unsigned int state, LayerType layer = LAYER_OUTERMOST)
{
    switch (static_cast<State::StateType>(state)) {
        case State::WARNING: return COLOR_WARNING;
        case State::ERROR:
        case State::FAILURE: return COLOR_ERROR;
        case State::INVALID: return "black"; // for debugging 
        case State::NORMAL: ; // fall through
    }

    SCASSERT(state == static_cast<State::StateType>(State::NORMAL));

    switch (layer) {
        case LAYER_PROCESS:   return COLOR_PROCESS;
        case LAYER_COMPONENT: return COLOR_COMPONENT;
        case LAYER_META:      return COLOR_META;
        case LAYER_OUTERMOST: return COLOR_NORMAL;
    }
    
#if 0
    if (projectedState) {
        switch (static_cast<State::StateType>(state)) {
        case State::NORMAL:  return COLOR_NORMAL;
        case State::WARNING: return COLOR_WARNING;
        case State::ERROR:
        case State::FAILURE: return COLOR_ERROR;
        case State::INVALID: return "black"; // for debugging 
        }
    } else {
        switch (static_cast<State::StateType>(state)) {
        case State::NORMAL:  return COLOR_NORMAL;
        case State::WARNING: return COLOR_YELLOW;
        case State::ERROR:
        case State::FAILURE: return COLOR_ERROR;
        case State::INVALID: return "green"; // for debugging
        }
    }
#endif
}

const std::string GetColorCodeForSafetyCoordinator(void)
{
    // ref: http://www.dtelepathy.com/blog/inspiration/beautiful-color-palettes-for-your-next-web-project
    const int N = 5;

    switch (SCColorCode++ % N) {
    case 0: return "#2e6093";
    case 1: return "#3a5988";
    case 2: return "#9d4e87";
    case 3: return "#92538c";
    case 4: return "#72659d";
    case 5: return "#293E6A";
    case 6: return "#3B5A99";
    case 7: return "#74ABF8";
    case 8: return "#6992AC";
    default:
            return "#ffffff";
    }
}

//
// Subscriber callback
//
void ViewerSubscriberCallback::CallbackControl(Topic::Control::CategoryType category, const std::string & json)
{
    SCASSERT(false);
}

void ViewerSubscriberCallback::CallbackData(Topic::Data::CategoryType category, const std::string & json)
{
    std::string categoryName;
    switch (category) {
    case Topic::Data::MONITOR:
        categoryName = "MONITOR";
        break;
    case Topic::Data::EVENT:
        categoryName = "EVENT";
        break;
    case Topic::Data::READ_RES:
        categoryName = "READ_RES";
        break;
    default:
        categoryName = "INVALID";
        break;
    }

    std::cout << json << std::endl;

#if VERBOSE
    SCLOG_INFO << "message received - topic: " << TopicName << ", category: " << categoryName << " ]" << std::endl;
    if (category == Topic::Data::READ_RES) {
        std::cout << "ViewerSubscriberCallback:" << __LINE__ << " Received json => " << json << std::endl;
    }
#endif

    // input JSON: state information from CASROS
    JsonWrapper in;
    if (!in.Read(json.c_str())) {
        SCLOG_ERROR << "Failed to parse json: " << json << std::endl;
        return;
    }

    const JsonWrapper::JsonValue & inroot = in.GetRoot();

    // Get target safety coordinator (assigned as process name in cisst)
    //const std::string targetProcessName = 
        //JsonWrapper::GetSafeValueString(inroot["target"], "safety_coordinator");
    //const std::string thisProcessName = mtsManagerLocal::GetInstance()->GetProcessName();
    //if (targetProcessName.compare("*") != 0 && (targetProcessName != thisProcessName))
        //return;

    // Check command type
    const std::string cmd = JsonWrapper::GetSafeValueString(inroot, "cmd");
    if (cmd.compare("message") == 0) {
        std::cout << JsonWrapper::GetSafeValueString(inroot, "msg") << std::endl;
        return;
    } else {
        if (cmd.compare("state_list") != 0) {
            SCLOG_INFO << "not state information: " << cmd << std::endl;
            return;
        }
    }

    // extract name of safety coordinator
    const std::string nameOfThisSC = JsonWrapper::GetSafeValueString(inroot, "safety_coordinator");
    // update state cache (overwrite)
    CasrosSystemStatesRoot[nameOfThisSC] = inroot;

    // placeholder for D3
    JsonWrapper D3States;
    JsonWrapper::JsonValue D3StatesRoot = D3States.GetRoot();

    // iterate each state cache instance, convert it to D3 format, and stack it up.
    Json::ArrayIndex cntSC = 0;
    JsonWrapper::JsonValue::iterator it = CasrosSystemStatesRoot.begin();
    const JsonWrapper::JsonValue::iterator itEnd = CasrosSystemStatesRoot.end();
    for (; it != itEnd; ++it) {
        JsonWrapper outSC;
        JsonWrapper::JsonValue & outSCroot = outSC.GetRoot();
        GenerateD3JSON(*it, outSCroot);
        D3StatesRoot[cntSC++] = outSCroot;
    }

    // get json-encoded string for state viewer
    const std::string outJson = JsonWrapper::GetJSONString(D3StatesRoot);

    // Reset color code
    SCColorCode = 0;

    // refresh state chart
    QJsApiHandler::UpdateJSONState(outJson);
}

void ViewerSubscriberCallback::GenerateD3JSON(const JsonWrapper::JsonValue & inroot, JsonWrapper::JsonValue & outSCroot)
{
    // safety coordinator name
    outSCroot["name"] = JsonWrapper::GetSafeValueString(inroot, "safety_coordinator");
    outSCroot["color"] = GetColorCodeForState(State::NORMAL, LAYER_PROCESS);

    // for each component
    const JsonWrapper::JsonValue inComponents = inroot["components"];
    for (Json::ArrayIndex i = 0; i < inComponents.size(); ++i) {
        // inputs for component
        const JsonWrapper::JsonValue inComponent = inComponents[i];
        // outputs for component
        JsonWrapper outComponent;
        JsonWrapper::JsonValue & outComponentRoot = outComponent.GetRoot();
        {
            // component name
            outComponentRoot["name"] = inComponent["name"];
            outComponentRoot["color"] = GetColorCodeForState(JsonWrapper::GetSafeValueUInt(inComponent, "s"), LAYER_COMPONENT);

            // outputs for component state (three views: system, framework, application)
            JsonWrapper outStateComponent;
            JsonWrapper::JsonValue & outStateComponentRoot = outStateComponent.GetRoot();
            // Get system view of component state
            State stateComponentSystemView, stateComponentFrameworkView, stateComponentAppView;
            stateComponentFrameworkView = static_cast<State::StateType>(JsonWrapper::GetSafeValueUInt(inComponent["s_F"], "state"));
            stateComponentAppView       = static_cast<State::StateType>(JsonWrapper::GetSafeValueUInt(inComponent["s_A"], "state"));
            stateComponentSystemView = stateComponentFrameworkView * stateComponentAppView;

            outStateComponentRoot["name"] = "Component";
            outStateComponentRoot["color"] = GetColorCodeForState(stateComponentSystemView.GetState(), LAYER_META);
            {
                Json::ArrayIndex cntComponentState = 0;
                JsonWrapper outStateFramework;
                JsonWrapper::JsonValue & outStateFrameworkRoot = outStateFramework.GetRoot();
                {
                    outStateFrameworkRoot["name"] = "Framework";
                    outStateFrameworkRoot["color"] = GetColorCodeForState(stateComponentFrameworkView.GetState());
                    if (inComponent["s_F"]["event"] != JsonWrapper::JsonValue::null)
                        outStateFrameworkRoot["pendingevent"] = JsonWrapper::GetJSONString(inComponent["s_F"]["event"]);
                }
                outStateComponentRoot["children"][cntComponentState++] = outStateFrameworkRoot;

                JsonWrapper outStateApp;
                JsonWrapper::JsonValue & outStateAppRoot = outStateApp.GetRoot();
                {
                    outStateAppRoot["name"] = "Application";
                    outStateAppRoot["color"] = GetColorCodeForState(stateComponentAppView.GetState());
                    if (inComponent["s_A"]["event"] != JsonWrapper::JsonValue::null)
                        outStateAppRoot["pendingevent"] = JsonWrapper::GetJSONString(inComponent["s_A"]["event"]);
                }
                outStateComponentRoot["children"][cntComponentState++] = outStateAppRoot;
            }
            Json::ArrayIndex j = 0;
            outComponentRoot["children"][j++] = outStateComponentRoot;

            // outputs for provided interface
            // for each interface
            const JsonWrapper::JsonValue inPrvInterfaces = inComponent["interfaces_provided"];
            const Json::ArrayIndex cntProvided = inPrvInterfaces.size();
            for (Json::ArrayIndex k = 0; k < cntProvided; ++k) {
                // input for provided interface
                const JsonWrapper::JsonValue inPrvInterface = inPrvInterfaces[k];
                // output for provided interface
                JsonWrapper outInterfaceProvidedEach;
                JsonWrapper::JsonValue & outInterfaceProvidedEachRoot = outInterfaceProvidedEach.GetRoot();
                {
                    outInterfaceProvidedEachRoot["name"] = "Service";
                    outInterfaceProvidedEachRoot["color"] = 
                        GetColorCodeForState(JsonWrapper::GetSafeValueUInt(inPrvInterface, "service_state"), LAYER_META);

                    Json::ArrayIndex cnt = 0;
                    outInterfaceProvidedEachRoot["children"][cnt]["name"] = inPrvInterface["name"];
                    outInterfaceProvidedEachRoot["children"][cnt]["color"] =
                        GetColorCodeForState(JsonWrapper::GetSafeValueUInt(inPrvInterface, "state"));
                    if (inPrvInterface["event"] != JsonWrapper::JsonValue::null)
                        outInterfaceProvidedEachRoot["pendingevent"] = JsonWrapper::GetJSONString(inPrvInterface["event"]);
                }
                //outInterfaceProvidedRoot["children"][k] = outInterfaceProvidedEachRoot;

                outComponentRoot["children"][j++] = outInterfaceProvidedEachRoot;
            }

            // outputs for required interface
            JsonWrapper outInterfaceRequired;
            JsonWrapper::JsonValue & outInterfaceRequiredRoot = outInterfaceRequired.GetRoot();
            Json::ArrayIndex cntRequired = 0;
            {
                outInterfaceRequiredRoot["name"] = "Required";
                outInterfaceRequiredRoot["color"] = GetColorCodeForState(JsonWrapper::GetSafeValueUInt(inComponent["s_R"], "state"), LAYER_META);
                if (inComponent["s_R"]["event"] != JsonWrapper::JsonValue::null)
                    outInterfaceRequiredRoot["pendingevent"] = JsonWrapper::GetJSONString(inComponent["s_R"]["event"]);

                // for each interface
                const JsonWrapper::JsonValue inReqInterfaces = inComponent["interfaces_required"];
                cntRequired = inReqInterfaces.size();
                for (Json::ArrayIndex k = 0; k < cntRequired; ++k) {
                    // input for provided interface
                    const JsonWrapper::JsonValue inReqInterface = inReqInterfaces[k];
                    // output for provided interface
                    JsonWrapper outInterfaceRequiredEach;
                    JsonWrapper::JsonValue & outInterfaceRequiredEachRoot = outInterfaceRequiredEach.GetRoot();
                    {
                        outInterfaceRequiredEachRoot["name"] = inReqInterface["name"];
                        outInterfaceRequiredEachRoot["color"] =
                            GetColorCodeForState(JsonWrapper::GetSafeValueUInt(inReqInterface, "state"));
                        if (inReqInterface["event"] != JsonWrapper::JsonValue::null)
                            outInterfaceRequiredEachRoot["pendingevent"] = JsonWrapper::GetJSONString(inReqInterface["event"]);
                    }
                    outInterfaceRequiredRoot["children"][k] = outInterfaceRequiredEachRoot;
                }
            }
            if (cntRequired)
                outComponentRoot["children"][j++] = outInterfaceRequiredRoot;
        }
        outSCroot["children"][i] = outComponentRoot;
    }
    //std::cout << "###### " << JsonWrapper::GetJSONString(outSCroot) << std::endl;
}

//
// CASROS accessor for console
//
AccessorViewer::AccessorViewer(void)
    : cisstAccessor(true,  // enablePublisherControl
                        false, // enablePublisherData
                        false,  // enableSubscriberControl
                        true,  // enableSubscriberData
                        0,//new ViewerSubscriberCallback(Dict::TopicNames::CONTROL),
                        new ViewerSubscriberCallback(Dict::TopicNames::DATA))
{
}

bool AccessorViewer::RequestStateList(const std::string & safetyCoordinatorName,
                                      const std::string & componentName) const
{
    std::stringstream ss;
    ss << "{ \"target\": { \"safety_coordinator\": \"" << safetyCoordinatorName << "\", "
          "\"component\": \"" << componentName << "\" }, "
          "\"request\": \"state_list\" }";
    if (!Publishers.Control->PublishControl(Topic::Control::READ_REQ, ss.str())) {
        std::cerr << "RequestStateList: Failed to publish message (Control, READ_REQ): " << ss.str() << std::endl;
        return false;
    }

    std::cout << "requested list of states" << std::endl;

    return true;
}
