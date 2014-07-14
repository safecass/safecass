//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 10, 2014
// Last revision: Jul 10, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "accessor.h"

#include <iomanip>

using namespace SF;

const std::string GetColorCodeForState(unsigned int state)
{
    switch (static_cast<State::StateType>(state)) {
    case State::NORMAL: return "white"; break;
    case State::WARNING: return "yellow"; break;
    case State::ERROR: return "red"; break;
    case State::FAILURE: return "black"; break;
    case State::INVALID: return "blue"; break;
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

    SFLOG_INFO << "message received - topic: " << TopicName << ", category: " << categoryName << " ]" << std::endl;
    if (category == SF::Topic::Data::READ_RES)
        std::cout << json << std::endl;

    // input JSON: state information from CASROS
    SF::JSON in;
    if (!in.Read(json.c_str())) {
        SFLOG_ERROR << "Failed to parse json: " << json << std::endl;
        return;
    }

    const SF::JSON::JSONVALUE & inroot = in.GetRoot();

    const std::string cmd = SF::JSON::GetSafeValueString(in.GetRoot(), "cmd");
    if (cmd.compare("state_list") != 0) {
        SFLOG_INFO << "not state information: " << cmd << std::endl;
        return;
    }

    // output JSON: state information for D3
    SF::JSON out;
    SF::JSON::JSONVALUE & root = out.GetRoot();

    SF::JSON outSC;
    size_t cntSafetyCoordinator = 0;

    // MJTEMP: state information from casros may contain multiple instances
    // of safety coordinator, but assume only one instance for now.
    SF::JSON::JSONVALUE & outSCroot = outSC.GetRoot();

    // safety coordinator name
    outSCroot["name"] = SF::JSON::GetSafeValueString(inroot, "safety_coordinator");
    outSCroot["color"] = "#72659d";

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
            outComponentRoot["color"] = GetColorCodeForState(JSON::GetSafeValueUInt(inComponent, "s"));

            size_t j = 0;

            // for each state
            SF::JSON outStateFramework;
            SF::JSON::JSONVALUE & outStateFrameworkRoot = outStateFramework.GetRoot();
            {
                outStateFrameworkRoot["name"] = "s_F";
                outStateFrameworkRoot["color"] = GetColorCodeForState(JSON::GetSafeValueUInt(inComponent, "s_F"));
            }
            outComponentRoot["children"][j++] = outStateFrameworkRoot;

            SF::JSON outStateApp;
            SF::JSON::JSONVALUE & outStateAppRoot = outStateApp.GetRoot();
            {
                outStateAppRoot["name"] = "s_A";
                outStateAppRoot["color"] = GetColorCodeForState(JSON::GetSafeValueUInt(inComponent, "s_A"));
            }
            outComponentRoot["children"][j++] = outStateAppRoot;

            // outputs for provided interface
            SF::JSON outInterfaceProvided;
            SF::JSON::JSONVALUE & outInterfaceProvidedRoot = outInterfaceProvided.GetRoot();
            {
                outInterfaceProvidedRoot["name"] = "s_A Provided";
                // TODO: calculate state product to decide state color code ("s_P")
                outInterfaceProvidedRoot["color"] = "cyan"; // TODO

                // for each interface
                const JSON::JSONVALUE inPrvInterfaces = inComponent["s_P"];
                for (size_t k = 0; k < inPrvInterfaces.size(); ++k) {
                    // input for provided interface
                    const JSON::JSONVALUE inPrvInterface = inPrvInterfaces[k];
                    // output for provided interface
                    SF::JSON outInterfaceProvidedEach;
                    SF::JSON::JSONVALUE & outInterfaceProvidedEachRoot = outInterfaceProvidedEach.GetRoot();
                    {
                        outInterfaceProvidedEachRoot["name"] = inPrvInterface["name"];
                        outInterfaceProvidedEachRoot["color"] = 
                            GetColorCodeForState(JSON::GetSafeValueUInt(inPrvInterface, "state"));
                    }
                    outInterfaceProvidedRoot["children"][k] = outInterfaceProvidedEachRoot;
                }
            }
            outComponentRoot["children"][j++] = outInterfaceProvidedRoot;

            // outputs for required interface
            SF::JSON outInterfaceRequired;
            SF::JSON::JSONVALUE & outInterfaceRequiredRoot = outInterfaceRequired.GetRoot();
            {
                outInterfaceRequiredRoot["name"] = "s_A Required";
                // TODO: calculate state product to decide state color code ("s_R")
                outInterfaceRequiredRoot["color"] = "cyan"; // TODO

                // for each interface
                const JSON::JSONVALUE inReqInterfaces = inComponent["s_R"];
                for (size_t k = 0; k < inReqInterfaces.size(); ++k) {
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
            outComponentRoot["children"][j++] = outInterfaceRequiredRoot;
        }
        outSCroot["children"][i] = outComponentRoot;
    }
    root[cntSafetyCoordinator++] = outSCroot;

    // pass c++ object to javascript var: QtWebKit Bridge
    // http://qt-project.org/doc/qt-4.8/qtwebkit-bridge.html
    
    // d3 data read from javascript object: doable!

    // TODO: redraw
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
