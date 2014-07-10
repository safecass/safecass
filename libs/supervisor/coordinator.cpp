//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : July 14, 2012
// Last revision: June 25, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "coordinator.h"

#define _VERBOSE 0

using namespace SF;

Coordinator::Coordinator(const std::string & name): Name(name), ComponentIdCounter(0)
{}

Coordinator::~Coordinator()
{
    // TODO: cleanup: FilterMapType, EventMapType
}

void Coordinator::ToStream(std::ostream & outputStream) const
{
    //TODO
    //PrintMonitoringTargets(outputStream);
    outputStream << "TODO" << std::endl;
}

bool Coordinator::AddMonitoringTarget(const std::string & targetUID,
                                      const std::string & targetJSON)
{
    if (FindMonitoringTarget(targetUID)) {
        SFLOG_ERROR << "Already registered target: " << targetUID << std::endl;
        return false;
    }

    MapMonitorTarget[targetUID] = targetJSON;

    return true;
}

bool Coordinator::FindMonitoringTarget(const std::string & uid) const
{
    return (MapMonitorTarget.find(uid) != MapMonitorTarget.end());
}

bool Coordinator::RemoveMonitoringTarget(const std::string & targetUID)
{
    if (!FindMonitoringTarget(targetUID))
        return true;

    MonitorTargetMapType::iterator it = MapMonitorTarget.find(targetUID);
    SFASSERT(it != MapMonitorTarget.end());
    MapMonitorTarget.erase(it);

    return true;
}

void Coordinator::PrintMonitoringTargets(std::ostream & outputStream) const
{
    int i = 0;
    MonitorTargetMapType::const_iterator it = MapMonitorTarget.begin();

    for (; it != MapMonitorTarget.end(); ++it)
        outputStream << "[" << ++i << "] " << it->first << std::endl;
}

unsigned int Coordinator::AddComponent(const std::string & componentName)
{
    if (MapComponentNameToId.find(componentName) != MapComponentNameToId.end()) {
        std::stringstream ss;
        ss << "Coordinator::AddComponent: component \"" << componentName 
           << "\" already registered [ ";
        ComponentNameToIdMapType::const_iterator it = MapComponentNameToId.begin();
        for (; it != MapComponentNameToId.end(); ++it)
            ss << it->first << " ";
        SFLOG_ERROR << ss.str() << std::endl;
        return 0;
    }

    unsigned int cid = ++ComponentIdCounter;
    MapComponentNameToId[componentName] = cid;
    MapComponentIdToName[cid] = componentName;

    GCM * gcm = new GCM(componentName);
    MapGCM[cid] = gcm;

    return cid;
}

unsigned int Coordinator::GetComponentId(const std::string & componentName) const
{
    ComponentNameToIdMapType::const_iterator it = MapComponentNameToId.find(componentName);
    if (it == MapComponentNameToId.end())
        return 0;

    return it->second;
}

const std::string Coordinator::GetComponentName(unsigned int componentId) const
{
    ComponentIdToNameMapType::const_iterator it = MapComponentIdToName.find(componentId);
    if (it == MapComponentIdToName.end())
        return "";

    return it->second;
}

bool Coordinator::AddInterface(const std::string & componentName, 
                               const std::string & interfaceName,
                               const GCM::InterfaceTypes type)
{
    unsigned int cid = GetComponentId(componentName);
    if (cid == 0) {
        SFLOG_ERROR << "Component \"" << componentName << "\" not found" << std::endl;
        return false;
    }

    // if component is not yet added, add it first
    GCMMapType::iterator it = MapGCM.find(cid);
    if (it == MapGCM.end()) {
        cid = AddComponent(componentName);
        it = MapGCM.find(cid);
        SFASSERT(it != MapGCM.end());
    }

    GCM * gcm = it->second;
    SFASSERT(gcm);

    return gcm->AddInterface(interfaceName, type);
}

bool Coordinator::RemoveInterface(const std::string & componentName, 
                                  const std::string & interfaceName,
                                  const GCM::InterfaceTypes type)
{
    unsigned int cid = GetComponentId(componentName);
    if (cid == 0) {
        SFLOG_ERROR << "Component \"" << componentName << "\" not found" << std::endl;
        return false;
    }

    GCMMapType::iterator it = MapGCM.find(cid);
    SFASSERT(it != MapGCM.end());

    GCM * gcm = it->second;
    SFASSERT(gcm);

    return gcm->RemoveInterface(interfaceName, type);
}

const std::string Coordinator::GetStateSnapshot(const std::string & componentName) const
{
    std::stringstream ss;

    ss << "{ ";

    GCMMapType::const_iterator it;
    const GCMMapType::const_iterator itEnd = MapGCM.end();

    bool allComponents = (componentName.compare("*") == 0);
    if (allComponents) {
        // header
        ss << "\"safety_coordinator\": \"" << Name << "\", ";
        ss << "\"cmd\": \"state_list\",";

        // component list
        ss << "\"components\": [ ";
        it = MapGCM.begin();
        for (; it != itEnd; ++it) {
            if (it != MapGCM.begin())
                ss << ", ";
            ss << "\"" << it->second->GetComponentName() << "\"";
        }
        ss << " ], ";
    }

    // individual component
    bool first = true;
    it = MapGCM.begin();
    for (; it != itEnd; ++it) {
        GCM * gcm = it->second;
        if (!allComponents) {
            if (componentName.compare(it->second->GetComponentName()) != 0)
                continue;
        }
        if (!first)
            ss << ", ";
        // component name and component states
        if (allComponents)
            ss << "\n";

        // MJTODO: replace this manual build-up with SF::JSON
        ss << "\"" << it->second->GetComponentName() << "\": { "
              "\"s\": " << static_cast<int>(gcm->GetComponentState(GCM::SYSTEM_VIEW)) << ", "
              "\"s_F\": " << static_cast<int>(gcm->GetComponentState(GCM::FRAMEWORK_VIEW)) << ", "
              "\"s_A\": " << static_cast<int>(gcm->GetComponentState(GCM::APPLICATION_VIEW)) << ", ";
        StrVecType names;
        // provided interface states
#define GET_INTERFACE_STATE(_type, _key)\
        names.clear();\
        it->second->GetNamesOfInterfaces(_type, names);\
        ss << "\"" _key "\": [ ";\
        for (size_t i = 0; i < names.size(); ++i) {\
            if (i != 0)\
                ss << ", ";\
            ss << "{ \"" << names[i] << "\": ";\
            ss << static_cast<int>(gcm->GetInterfaceState(names[i], _type));\
            ss << " }";\
        }\
        ss << " ]";
        GET_INTERFACE_STATE(GCM::PROVIDED_INTERFACE, "s_P");
        ss << ", ";
        GET_INTERFACE_STATE(GCM::REQUIRED_INTERFACE, "s_R");
#undef GET_INTERFACE_STATE
        ss << " }";

        first = false;
    }

    ss << " }" << std::endl;

    return ss.str();
}

bool Coordinator::AddFilter(const std::string & componentName, FilterBase * filter)
{
    // TODO: "target" of filter
#if 0
    // state_machine: type
    State::StateMachineType smType;
    const std::string type = SF::JSON::GetSafeValueString(events[i]["state_machine"], "type");
    if (type.compare("s_F") == 0)
        smType = State::STATEMACHINE_FRAMEWORK;
    else if (type.compare("s_A") == 0)
        smType = State::STATEMACHINE_APP;
    else if (type.compare("s_P") == 0)
        smType = State::STATEMACHINE_PROVIDED;
    else if (type.compare("s_R") == 0)
        smType = State::STATEMACHINE_REQUIRED;
    else {
        SFLOG_ERROR << "AddEvents: Invalid state machine type: " << type << ", JSON: " << events[i] << std::endl;
        return false;
    }
    // state_machine: id 
    if (smType == State::STATEMACHINE_PROVIDED || smType == State::STATEMACHINE_REQUIRED) {
        smId = JSON::GetSafeValueString(events[i]["state_machine"], "id");
    }
#endif
        
    // check if component is added
    unsigned int cid = GetComponentId(componentName);
    if (cid == 0) {
        SFLOG_ERROR << "AddFilter: Component \"" << componentName << "\" not found" << std::endl;
        return false;
    }

    SFASSERT(filter);

    if (!filter->InitFilter()) {
        SFLOG_ERROR << "AddFilter: failed to initialize filter: " << *filter << std::endl;
        return false;
    }

    FilterBase::FilterIDType uid = filter->GetFilterUID();
    FilterMapType::iterator it = MapFilter.find(componentName);
    if (it == MapFilter.end()) {
        FiltersType * newList = new FiltersType;
        newList->insert(std::make_pair(uid, filter));
        MapFilter[componentName] = newList;
    } else {
        it->second->insert(std::make_pair(uid, filter));
    }

    SFLOG_INFO << "AddFilter: successfully added filter \"" << filter->GetFilterName() 
               << "\" to component \"" << componentName << "\"" << std::endl;

    return true;
}

Coordinator::FiltersType * Coordinator::GetFilters(const std::string & componentName) const
{
    unsigned int cid = GetComponentId(componentName);
    if (cid == 0) {
        SFLOG_ERROR << "GetFilters: Component \"" << componentName << "\" not found" << std::endl;
        return 0;
    }

    FilterMapType::const_iterator it = MapFilter.find(componentName);
    return (it == MapFilter.end() ? 0 : it->second);
}

const std::string Coordinator::GetFilterList(const std::string & componentName) const
{
    // TODO: json encoding
    std::stringstream ss;

    bool allComponents = (componentName.compare("*") == 0);

    FilterMapType::const_iterator it = MapFilter.begin();
    const FilterMapType::const_iterator itEnd = MapFilter.end();
    for (; it != itEnd; ++it) {
        FiltersType * filters = it->second;
        SFASSERT(filters);

        if (!allComponents)
            if (it->first.compare(componentName) != 0)
                continue;

        ss << "Component: \"" << it->first << "\"" << std::endl;

        FiltersType::const_iterator it2 = filters->begin();
        const FiltersType::const_iterator itEnd2 = filters->end();
        for (; it2 != itEnd2; ++it2)
            ss << "\t" << *it2->second << std::endl;
    }

    return ss.str();
}

bool Coordinator::InjectInputToFilter(FilterBase::FilterIDType fuid, const DoubleVecType & inputs)
{
    FilterMapType::const_iterator it = MapFilter.begin();
    const FilterMapType::const_iterator itEnd = MapFilter.end();
    for (; it != itEnd; ++it) {
        FiltersType * filters = it->second;
        SFASSERT(filters);

        FiltersType::const_iterator it2 = filters->find(fuid);
        if (it2 == filters->end())
            continue;

        FilterBase * filter = it2->second;
        SFASSERT(filter);

        filter->InjectInput(inputs);

        return true;
    }

    return false;
}

bool Coordinator::AddEvent(const std::string & componentName, Event * event)
{
    // check if component is added
    unsigned int cid = GetComponentId(componentName);
    if (cid == 0) {
        SFLOG_ERROR << "AddEvent: Component \"" << componentName << "\" not found" << std::endl;
        return false;
    }

    SFASSERT(event);

    const std::string eventName = event->GetName();
    EventMapType::iterator it = MapEvent.find(componentName);
    if (it == MapEvent.end()) {
        EventsType * newList = new EventsType;
        newList->insert(std::make_pair(eventName, event));
        MapEvent[componentName] = newList;
    } else {
        it->second->insert(std::make_pair(eventName, event));
    }

    SFLOG_INFO << "AddFilter: successfully added event \"" << eventName << "\" to component \"" << componentName << "\"" << std::endl;

    return true;
}

bool Coordinator::AddEvents(const std::string & componentName, const JSON::JSONVALUE & events)
{
    if (events.isNull() || events.size() == 0) {
        SFLOG_ERROR << "AddEvents: No event specification found in json: " << JSON::GetJSONString(events) << std::endl;
        return false;
    }

    // Figure out how many events are defined, and create and register event instances
    std::string  eventName, smId;
    unsigned int severity;
    SF::Event    *event = 0;
    for (size_t i = 0; i < events.size(); ++i) {
        // event name
        eventName = SF::JSON::GetSafeValueString(events[i], "name");
        if (eventName.size() == 0) {
            SFLOG_ERROR << "AddEvents: null event name. JSON: " << events[i] << std::endl;
            return false;
        }
        // severity (1: lowest priority, ..., 255: highest priority)
        severity = SF::JSON::GetSafeValueUInt(events[i], "severity");
        if ((severity == 0) || (severity > 255)) {
            SFLOG_ERROR << "AddEvents: Invalid severity: " << severity << ", JSON: " << events[i] << std::endl;
            return false;
        }
        // state transition
        std::string tname;
        State::TransitionType t;
        Event::TransitionsType ts;

        const JSON::JSONVALUE & ts_json = events[i]["state_transition"];
        for (size_t j = 0; j < ts_json.size(); ++j) {
            tname = ts_json[j].asString();
            if (tname.compare("N2E") == 0)
                t = State::NORMAL_TO_ERROR;
            else if (tname.compare("E2N") == 0)
                t = State::ERROR_TO_NORMAL;
            else if (tname.compare("N2W") == 0)
                t = State::NORMAL_TO_WARNING;
            else if (tname.compare("W2N") == 0)
                t = State::WARNING_TO_NORMAL;
            else if (tname.compare("W2E") == 0)
                t = State::WARNING_TO_ERROR;
            else if (tname.compare("E2W") == 0)
                t = State::ERROR_TO_WARNING;
            else {
                SFLOG_ERROR << "AddEvents: Invalid transition: " << tname << ", JSON: " << ts_json[i] << std::endl;
                return false;
            }
            ts.push_back(t);
        }

        // create event instance
        event = new Event(eventName, severity, ts);
        if (!AddEvent(componentName, event)) {
            SFLOG_ERROR << "AddEvents: Failed to add event \"" << eventName << "\"" << std::endl;
            delete event;
            return false;
        }
#if _VERBOSE
        SFLOG_INFO << "[" << (i + 1) << "/" << events.size() << "] "
                   << "Successfully installed event: \"" << event->GetName() << "\"" << std::endl;
#endif
   }

    return true;
}

bool Coordinator::AddEventFromJSON(const std::string & jsonString)
{
    // Construct JSON structure from JSON string
    SF::JSON json;
    if (!json.Read(jsonString.c_str())) {
        SFLOG_ERROR << "AddEventFromJSON: Failed to read json string: " << jsonString << std::endl;
        return false;
    }

    const SF::JSON::JSONVALUE events = json.GetRoot()["event"];
    const std::string componentName = JSON::GetSafeValueString(json.GetRoot(), "component");
    bool ret = AddEvents(componentName, events);
    if (!ret) {
        SFLOG_ERROR << "AddEventFromJSON: Failed to add events from JSON: " << jsonString << std::endl;
        return false;
    }

#if _VERBOSE
    SFLOG_INFO << "AddEventFromJSON: Successfully added events from JSON: " << jsonString << std::endl;
#endif

    return true;
}

bool Coordinator::AddEventFromJSONFile(const std::string & jsonFileName)
{
    // Construct JSON structure from JSON file
    SF::JSON json;
    if (!json.ReadFromFile(jsonFileName)) {
        SFLOG_ERROR << "AddEventFromJSONFile: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }

    bool ret = AddEventFromJSON(SF::JSON::GetJSONString(json.GetRoot()));
    if (!ret) {
        SFLOG_ERROR << "AddEventFromJSONFile: Failed to add events from JSON file: " << jsonFileName << std::endl;
        return false;
    }

#if _VERBOSE
    SFLOG_INFO << "AddEventFromJSONFile: Successfully added events from JSON file: " << jsonFileName << std::endl;
#endif

    return true;
}

const std::string Coordinator::GetEventList(const std::string & componentName) const
{
    // TODO: json encoding
    std::stringstream ss;

    bool allComponents = (componentName.compare("*") == 0);

    EventMapType::const_iterator it = MapEvent.begin();
    const EventMapType::const_iterator itEnd = MapEvent.end();
    for (; it != itEnd; ++it) {
        EventsType * events = it->second;
        SFASSERT(events);

        if (!allComponents)
            if (it->first.compare(componentName) != 0)
                continue;

        ss << "Component: \"" << it->first << "\"" << std::endl;

        EventsType::const_iterator it2 = events->begin();
        const EventsType::const_iterator itEnd2 = events->end();
        for (; it2 != itEnd2; ++it2)
            ss << "\t" << *it2->second << std::endl;
    }

    return ss.str();
}

const Event * Coordinator::GetEvent(const std::string & componentName, const std::string & eventName) const
{
    EventMapType::const_iterator it = MapEvent.find(componentName);
    if (it == MapEvent.end())
        return 0;

    EventsType::const_iterator it2 = it->second->find(eventName);
    if (it2 == it->second->end())
        return 0;

    return it2->second;
}

const Event * Coordinator::GetEvent(const std::string & eventName) const
{
    EventMapType::const_iterator it = MapEvent.begin();
    const EventMapType::const_iterator itEnd = MapEvent.end();
    for (; it != itEnd; ++it) {
        EventsType::const_iterator it2 = it->second->find(eventName);
        if (it2 == it->second->end())
            continue;
        else
            return it2->second;
    }
    return 0;
}

bool Coordinator::FindEvent(const std::string & componentName, const std::string & eventName) const
{
    return (GetEvent(componentName, eventName) != 0);
}

bool Coordinator::FindEvent(const std::string & eventName) const
{
    return (GetEvent(eventName) != 0);
}

bool Coordinator::OnEvent(const std::string & event)
{
    // Construct JSON instance from JSON-encoded string
    JSON json;
    if (!json.Read(event.c_str())) {
        SFLOG_ERROR << "Coordinator::OnEvent: Failed to read json string: " << event << std::endl;
        return false;
    }

    const JSON::JSONVALUE & jsonEvent = json.GetRoot()["event"];

    const FilterBase::FilterIDType fuid = JSON::GetSafeValueUInt(jsonEvent, "fuid");
    const unsigned int severity         = JSON::GetSafeValueUInt(jsonEvent, "severity");
    const std::string eventName         = JSON::GetSafeValueString(jsonEvent, "name");
    const TimestampType timestamp       = JSON::GetSafeValueDouble(jsonEvent, "timestamp");

    const State::StateMachineType targetStateMachineType = 
        static_cast<State::StateMachineType>(JSON::GetSafeValueUInt(jsonEvent["target"], "type"));
    const std::string targetComponentName = JSON::GetSafeValueString(jsonEvent["target"], "component");
    const std::string targetInterfaceName = JSON::GetSafeValueString(jsonEvent["target"], "interface");

#if _VERBOSE
    SFLOG_ERROR << "fuid: " << fuid << std::endl
                << "severity: " << severity << std::endl
                << "name: " << eventName << std::endl
                << "timestamp: " << timestamp << std::endl
                << "targetStateMachineType: " << targetStateMachineType << std::endl
                << "targetComponentName: " << targetComponentName << std::endl
                << "targetInterfaceName: " << targetInterfaceName << std::endl;
#endif

    // check if event is registered
    const Event * e = GetEvent(eventName);
    if (!e) {
        SFLOG_ERROR << "OnEvent: event \"" << eventName << "\" is not registered" << std::endl;
        return false;
    }

    // Get state machine associated with the event, determine necessary transition 
    // based on the current state of the state machine. This state change may have
    // impact on other states.

    // Get state machine associated with the event
    GCM * gcm = GetGCMInstance(targetComponentName);
    StateMachine * sm = 0;
    GCM::InterfaceStateMachinesType::const_iterator it;
    switch (targetStateMachineType) {
    case State::STATEMACHINE_FRAMEWORK:
        sm = gcm->States.ComponentFramework;
        break;
    case State::STATEMACHINE_APP:
        sm = gcm->States.ComponentApplication;
        break;
    case State::STATEMACHINE_PROVIDED:
        it = gcm->States.ProvidedInterfaces.find(targetInterfaceName);
        if (it != gcm->States.ProvidedInterfaces.end())
            sm = it->second;
        break;
    case State::STATEMACHINE_REQUIRED:
        it = gcm->States.RequiredInterfaces.find(targetInterfaceName);
        if (it != gcm->States.RequiredInterfaces.end())
            sm = it->second;
        break;
    case State::STATEMACHINE_INVALID:
        SFLOG_ERROR << "OnEvent: event \"" << eventName << "\" has no valid state machine information." << std::endl;
        return false;
    }

    // Get current state
    const State::StateType currentState = sm->GetCurrentState();
    // Look for possible transitions from the current state
    const State::TransitionType transition = e->GetTransition(currentState);
    // Make transition
    sm->ProcessEvent(transition);

    // TODO: Propagate state changes/events to connected components

    // Call event hook for middleware
    return OnEventHandler(e);
}

GCM * Coordinator::GetGCMInstance(const std::string & componentName) const
{
    Coordinator::GCMMapType::const_iterator it = MapGCM.find(GetComponentId(componentName));
    if (it == MapGCM.end())
        return 0;
    else
        return it->second;
}

