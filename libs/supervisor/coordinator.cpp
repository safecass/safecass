//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 14, 2012
// Last revision: Jul 17, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "coordinator.h"
#include "filterFactory.h"

#define VERBOSE 0

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
    //if (allComponents) {
        // header
        ss << "\"safety_coordinator\": \"" << Name << "\", ";
        ss << "\"cmd\": \"state_list\", ";
   // }

    // component list
    ss << "\"components\": [ ";

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
        //if (allComponents)
            //ss << "\n";

        // MJTODO: replace this manual build-up with SF::JSON
        ss << "{ \"name\": \"" << it->second->GetComponentName() << "\", "
           << "\"s\": " << static_cast<int>(gcm->GetComponentState(GCM::SYSTEM_VIEW)) << ", "
           << "\"s_F\": " << static_cast<int>(gcm->GetComponentState(GCM::FRAMEWORK_VIEW)) << ", "
           << "\"s_A\": " << static_cast<int>(gcm->GetComponentState(GCM::APPLICATION_VIEW)) << ", "
           << "\"s_P\": " << static_cast<int>(gcm->GetInterfaceState(GCM::PROVIDED_INTERFACE)) << ", "
           << "\"s_R\": " << static_cast<int>(gcm->GetInterfaceState(GCM::REQUIRED_INTERFACE)) << ", ";
        StrVecType names;
        // provided interface states
#define GET_EACH_INTERFACE_STATE(_type, _key)\
        names.clear();\
        it->second->GetNamesOfInterfaces(_type, names);\
        ss << "\"" _key "\": [ ";\
        for (size_t i = 0; i < names.size(); ++i) {\
            if (i != 0)\
                ss << ", ";\
            ss << "{ \"name\": \"" << names[i] << "\", ";\
            ss << "\"state\": " << static_cast<int>(gcm->GetInterfaceState(names[i], _type));\
            ss << " }";\
        }\
        ss << " ]";
        GET_EACH_INTERFACE_STATE(GCM::PROVIDED_INTERFACE, "interfaces_provided");
        ss << ", ";
        GET_EACH_INTERFACE_STATE(GCM::REQUIRED_INTERFACE, "interfaces_required");
#undef GET_EACH_INTERFACE_STATE
        ss << " }";

        first = false;
    }
    ss << " ] ";
    ss << " }" << std::endl;

    return ss.str();
}

bool Coordinator::ReadConfigFile(const std::string & jsonFileName)
{
    // NOTE: events should be processed first than filters because SC needs event information
    // to deploy filters.
    if (!AddEventFromJSONFile(jsonFileName)) {
        SFLOG_ERROR << "Failed to read config file (event): \"" << jsonFileName << "\"" << std::endl;
        return false;
    }

    if (!AddFilterFromJSONFile(jsonFileName)) {
        SFLOG_ERROR << "Failed to read config file (filter): \"" << jsonFileName << "\"" << std::endl;
        return false;
    }

    if (!AddServiceStateDependencyFromJSONFile(jsonFileName)) {
        SFLOG_ERROR << "Failed to read config file (service state): \"" << jsonFileName << "\"" << std::endl;
        return false;
    }

    SFLOG_DEBUG << "Successfully processed config file: \"" << jsonFileName << "\"" << std::endl;

    return true;
}

bool Coordinator::ReadConfigFileFramework(const std::string & jsonFileName, const std::string & componentName)
{
    // NOTE: events should be processed first than filters because SC needs event information
    // to deploy filters.
    if (!AddEventFromJSONFileToComponent(jsonFileName, componentName)) {
        SFLOG_ERROR << "Failed to read config file for framework (event): \"" << jsonFileName << "\"" << std::endl;
        return false;
    }

    if (!AddFilterFromJSONFileToComponent(jsonFileName, componentName)) {
        SFLOG_ERROR << "Failed to read config file for framework (filter): \"" << jsonFileName << "\"" << std::endl;
        return false;
    }

    SFLOG_DEBUG << "Successfully processed config file for framework: \"" << jsonFileName << "\"" << std::endl;

    return true;
}

bool Coordinator::AddFilterFromJSONFileToComponent(const std::string & jsonFileName,
                                                   const std::string & targetComponentName)
{
    // Construct JSON structure from JSON file
    JSON json;
    if (!json.ReadFromFile(jsonFileName)) {
        SFLOG_ERROR << "AddFilterFromJSONFileToComponent: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }
    SFLOG_DEBUG << "AddFilterFromJSONFileToComponent: Successfully read json file: " << jsonFileName << std::endl;

    // Replace placeholder for target component name with actual target component name
    std::string filterClassName;
    JSON::JSONVALUE & filters = json.GetRoot()["filter"];
    for (size_t i = 0; i < filters.size(); ++i) {
        JSON::JSONVALUE & filter = filters[i];
        filter["target"]["component"] = targetComponentName;
    }

    bool ret = AddFilters(filters);
    if (!ret) {
        SFLOG_ERROR << "AddFilterFromJSONFile: Failed to add filter(s) from JSON file: " << jsonFileName << std::endl;
        return false;
    }

    SFLOG_DEBUG << "AddFilterFromJSONFile: Successfully added filter(s) from JSON file: " << jsonFileName << std::endl;

    return true;
}

bool Coordinator::AddFilterFromJSONFile(const std::string & jsonFileName)
{
    // Construct JSON structure from JSON file
    JSON json;
    if (!json.ReadFromFile(jsonFileName)) {
        SFLOG_ERROR << "AddFilterFromJSONFile: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }

    bool ret = AddFilterFromJSON(JSON::GetJSONString(json.GetRoot()));
    if (!ret) {
        SFLOG_ERROR << "AddFilterFromJSONFile: Failed to add filter(s) from JSON file: " << jsonFileName << std::endl;
        return false;
    }

    SFLOG_DEBUG << "AddFilterFromJSONFile: Successfully added filter(s) from JSON file: " << jsonFileName << std::endl;

    return ret;
}

bool Coordinator::AddFilterFromJSON(const std::string & jsonString)
{
    // Construct JSON structure from JSON string
    JSON json;
    if (!json.Read(jsonString.c_str())) {
        SFLOG_ERROR << "AddFilterFromJSON: Failed to read json string: " << jsonString << std::endl;
        return false;
    }

    const JSON::JSONVALUE filters = json.GetRoot()["filter"];
    bool ret = AddFilters(filters);
    if (!ret) {
        SFLOG_DEBUG << "AddFilterFromJSON: Failed to add filter(s) using json string: " << jsonString << std::endl;
        return false;
    }

    SFLOG_DEBUG << "AddFilterFromJSON: Successfully added filter(s) using json string: " << jsonString << std::endl;

    return true;
}

bool Coordinator::AddFilters(const JSON::JSONVALUE & filters)
{
    if (filters.isNull() || filters.size() == 0) {
        SFLOG_INFO << "AddFilter: No filter specification found in json: " << filters << std::endl;
        return true;
    }

    // Create filter target instance

    // Figure out how many filters are defined, and 
    // create and install filter instances while iterating each filter specification
    std::string filterClassName;
    FilterBase * filter = 0; 
    bool enableLog = false;
    for (size_t i = 0; i < filters.size(); ++i) {
        filterClassName = JSON::GetSafeValueString(filters[i], "class_name");
        enableLog = JSON::GetSafeValueBool(filters[i], "debug");
        
        // Create filter instance based on filter class name using filter factory
        filter = FilterFactory::GetInstance()->CreateFilter(filterClassName, filters[i]);
        if (!filter) {
            SFLOG_ERROR << "AddFilter: Failed to create filter instance \"" << filterClassName << "\"\n";
            continue;
        }

        // Install filter to the target component
        if (!AddFilter(filter)) {
            SFLOG_ERROR << "AddFilter: Failed to add filter \"" << filter->GetFilterName() << "\"\n";
            delete filter;
            return false;
        }
        
        // configure filter (process filter-specific arguments)
        if (!filter->ConfigureFilter(filters[i])) {
            SFLOG_ERROR << "AddFilter: Failed to process filter-specfic parts for filter instance: \"" << filterClassName << "\"\n";
            delete filter;
            return false;
        }

        // enable debug log if specified
        if (enableLog)
            filter->EnableDebugLog();

        SFLOG_DEBUG << "[" << (i + 1) << "/" << filters.size() << "] "
            << "Successfully installed filter: \"" << filter->GetFilterName() << "\"" << std::endl;
   }

    return true;
}

bool Coordinator::AddFilter(const std::string & componentName, FilterBase * filter)
{
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
        SFLOG_INFO << "AddEvents: No event specification found in json: " << JSON::GetJSONString(events) << std::endl;
        return true;
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
#if VERBOSE
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

#if VERBOSE
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

#if VERBOSE
    SFLOG_INFO << "AddEventFromJSONFile: Successfully added events from JSON file: " << jsonFileName << std::endl;
#endif

    return true;
}

bool Coordinator::AddEventFromJSONFileToComponent(const std::string & jsonFileName,
                                                  const std::string & targetComponentName)
{
    // Construct JSON structure from JSON file
    SF::JSON json;
    if (!json.ReadFromFile(jsonFileName)) {
        SFLOG_ERROR << "AddEventFromJSONFile: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }

    // Insert target component name
    json.GetRoot()["component"] = targetComponentName;

    bool ret = AddEventFromJSON(SF::JSON::GetJSONString(json.GetRoot()));
    if (!ret) {
        SFLOG_ERROR << "AddEventFromJSONFile: Failed to add events from JSON file: " << jsonFileName << std::endl;
        return false;
    }

#if VERBOSE
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

#if VERBOSE
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
    SFLOG_DEBUG << "OnEvent: Received event: " << *e << std::endl;

    // Get state machine associated with the event, determine necessary transition 
    // based on the current state of the state machine. This state change may have
    // impact on other states.

    // Process state transition and get json string that includes the changes
    GCM * gcm = GetGCMInstance(targetComponentName);
    if (!gcm) {
        SFLOG_ERROR << "OnEvent: no GCM instance found for component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }

    const State::TransitionType transition = gcm->ProcessStateTransition(targetStateMachineType, e, targetInterfaceName);
    if (transition == State::INVALID_TRANSITION) {
        SFLOG_WARNING << "OnEvent: invalid transition for event " << *e << std::endl;
        return false;
    }

    // Refresh state information (for visualization)
    JSON _jsonRefresh;
    JSON::JSONVALUE & jsonRefresh = _jsonRefresh.GetRoot();
    jsonRefresh["target"]["safety_coordinator"] = "*";
    jsonRefresh["target"]["component"] = "*";
    jsonRefresh["request"] = "state_list";
    //std::stringstream ss;
    //ss << "{ \"target\": { \"safety_coordinator\": \"*\", "
            //"\"component\": \"*\" }, "
            //"\"request\": \"state_list\" }";
    PublishMessage(Topic::Control::READ_REQ, JSON::GetJSONString(jsonRefresh));

#if 0
    // If any service state change occurs
    if (targetStateMachineType != State::STATEMACHINE_REQUIRED) {
        // NORMAL_TO_WARNING and WARNING_TO_NORMAL don't need to get propagated to external components.
        if (transition == State::NORMAL_TO_ERROR || transition == State::WARNING_TO_ERROR ||
            transition == State::ERROR_TO_NORMAL || transition == State::ERROR_TO_WARNING)
        {
            JSON _jsonState;
            JSON::JSONVALUE & jsonState = _jsonState.GetRoot();
            jsonState["request"] = "state_update";
            // information about provided interface of which state just changed
            jsonState["source"]["safety_coordinator"] = Name;
            jsonState["source"]["component"] = targetComponentName;
            jsonState["source"]["interface"] = targetInterfaceName;
            // new state
            State::StateType nextState = State::GetNextState(transition);
            if (nextState == State::NORMAL || nextState == State::WARNING)
                jsonState["state"] = static_cast<unsigned int>(State::NORMAL);
            else
                jsonState["state"] = static_cast<unsigned int>(State::ERROR);
            // event information
            jsonState["event"] = e->GetName();

            PublishMessage(Topic::Control::STATE_UPDATE, JSON::GetJSONString(jsonState));
        }
    }
#endif

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

bool Coordinator::AddServiceStateDependencyFromJSON(const std::string & jsonString)
{
    // Construct JSON structure from JSON string
    JSON json;
    if (!json.Read(jsonString.c_str())) {
        SFLOG_ERROR << "AddServiceStateDependencyFromJSON: Failed to read json string: " << jsonString << std::endl;
        return false;
    }

    const JSON::JSONVALUE & services = json.GetRoot()["service"];
    if (services != JSON::JSONVALUE::null) {
        const std::string componentName = JSON::GetSafeValueString(json.GetRoot(), "component");
        GCM * gcm = GetGCMInstance(componentName);
        if (!gcm) {
            SFLOG_ERROR << "AddServiceStateDependencyFromJSON: no component found: \"" << componentName << "\"" << std::endl;
            return false;
        }

        gcm->AddServiceStateDependency(services);
    }

    SFLOG_DEBUG << "AddServiceStateDependencyFromJSON: Successfully added service state dependency using json string: " << jsonString << std::endl;

    return true;
}

bool Coordinator::AddServiceStateDependencyFromJSONFile(const std::string & jsonFileName)
{
    // Construct JSON structure from JSON file
    JSON json;
    if (!json.ReadFromFile(jsonFileName)) {
        SFLOG_ERROR << "AddServiceStateDependencyFromJSONFile: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }

    bool ret = AddServiceStateDependencyFromJSON(JSON::GetJSONString(json.GetRoot()));
    if (!ret) {
        SFLOG_ERROR << "AddServiceStateDependencyFromJSONFile: Failed to add service state dependency from JSON file: " << jsonFileName << std::endl;
        return false;
    }

    SFLOG_DEBUG << "AddServiceStateDependencyFromJSONFile: Successfully added service state dependency from JSON file: " << jsonFileName << std::endl;

    return ret;
}

