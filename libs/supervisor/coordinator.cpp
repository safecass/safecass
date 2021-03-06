//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 14, 2012
// Last revision: MAy 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#include "coordinator.h"
#include "filterFactory.h"
#include <boost/thread.hpp>

#define VERBOSE 0

using namespace SC;

// MJ TEMP: cisst-specific
#ifdef SC_HAS_CISST
bool IsInternalComponent(const std::string & name)
{
    return (name.compare("MCS") == 0 ||
            name.compare("LCM_MCC") == 0 ||
            name.compare("SafetyMonitor") == 0 ||
            name.substr(0, 17).compare("StateCollectorFor") == 0 ||
            name.substr(0, 17).compare("EventCollectorFor") == 0);
}

bool IsInternalInterface(const std::string & name)
{
    return (name.compare("ExecOut") == 0 ||
            name.compare("ExecIn") == 0 ||
            name.compare("Default") == 0 ||
            name.compare("Monitor") == 0 ||
            name.substr(0, 20).compare("ComponentRequiredFor") == 0 ||
            name.substr(0, 16).compare("InternalProvided") == 0 ||
            name.substr(0, 25).compare("InterfaceInternalProvided") == 0 ||
            name.substr(0, 14).compare("GCMRequiredFor") == 0 ||
            name.substr(0, 11).compare("LCMRequired") == 0);
}
#endif

Coordinator::Coordinator(const std::string & name): Name(name), ComponentIdCounter(0)
{
}

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
        SCLOG_ERROR << "Already registered target: " << targetUID << std::endl;
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
    SCASSERT(it != MapMonitorTarget.end());
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
    if (FindComponent(componentName)) {
        std::stringstream ss;
        ss << "Coordinator::AddComponent: component \"" << componentName 
           << "\" already registered [ ";
        ComponentNameToIdMapType::const_iterator it = MapComponentNameToId.begin();
        for (; it != MapComponentNameToId.end(); ++it)
            ss << it->first << " ";
        SCLOG_ERROR << ss.str() << std::endl;
        return 0;
    }

    unsigned int cid = ++ComponentIdCounter;
    MapComponentNameToId[componentName] = cid;
    MapComponentIdToName[cid] = componentName;

    GCM * gcm = new GCM(Name, componentName);
    MapGCM[cid] = gcm;

    return cid;
}

bool Coordinator::FindComponent(const std::string & componentName) const
{
    return (MapComponentNameToId.find(componentName) != MapComponentNameToId.end());
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
                               const GCM::InterfaceType type)
{
    unsigned int cid = GetComponentId(componentName);
    if (cid == 0) {
        SCLOG_ERROR << "Component \"" << componentName << "\" not found" << std::endl;
        return false;
    }

    // if component is not yet added, add it first
    GCMMapType::iterator it = MapGCM.find(cid);
    if (it == MapGCM.end()) {
        cid = AddComponent(componentName);
        it = MapGCM.find(cid);
        SCASSERT(it != MapGCM.end());
    }

    GCM * gcm = it->second;
    SCASSERT(gcm);

    return gcm->AddInterface(interfaceName, type);
}

bool Coordinator::RemoveInterface(const std::string & componentName, 
                                  const std::string & interfaceName,
                                  const GCM::InterfaceType type)
{
    unsigned int cid = GetComponentId(componentName);
    if (cid == 0) {
        SCLOG_ERROR << "Component \"" << componentName << "\" not found" << std::endl;
        return false;
    }

    GCMMapType::iterator it = MapGCM.find(cid);
    SCASSERT(it != MapGCM.end());

    GCM * gcm = it->second;
    SCASSERT(gcm);

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
#ifdef SC_HAS_CISST
        // TEMP: hide internal components and interfaces in case of cisst
        std::string name(it->second->GetComponentName());
        if (IsInternalComponent(name))
            continue;
#endif

        if (!first)
            ss << ", ";
        // component name and component states
        //if (allComponents)
            //ss << "\n";

        // MJTODO: replace this manual build-up with JsonWrapper
        ss << "{ \"name\": \"" << it->second->GetComponentName() << "\", "
           << "\"s\": " << static_cast<int>(gcm->GetComponentState(GCM::SYSTEM_VIEW)) << ", ";

        // component state - framework view
        const Event * e = 0;
        State::StateType stateComponentFramework = gcm->GetComponentState(GCM::FRAMEWORK_VIEW, e);
        ss << "\"s_F\": { \"state\": " << static_cast<int>(stateComponentFramework) << ", "
           << "\"event\": " << (e ? e->SerializeJSON() : JsonWrapper::JsonValue::null) << " }, ";
        // component state - application view
        e = 0;
        State::StateType stateComponentApp = gcm->GetComponentState(GCM::APPLICATION_VIEW, e);
        ss << "\"s_A\": { \"state\": " << static_cast<int>(stateComponentApp) << ", "
           << "\"event\": " << (e ? e->SerializeJSON() : JsonWrapper::JsonValue::null) << " }, ";
        // required interface state
        e = 0;
        State::StateType stateReq = gcm->GetInterfaceState(GCM::REQUIRED_INTERFACE, e);
        ss << "\"s_R\": { \"state\": " << static_cast<int>(stateReq) << ", "
           << "\"event\": " << (e ? e->SerializeJSON() : JsonWrapper::JsonValue::null) << " }, ";

        StrVecType names;
        std::string eventInfo;

        it->second->GetNamesOfInterfaces(GCM::PROVIDED_INTERFACE, names);
        ss << "\"interfaces_provided\": [ ";
        int count = 0;
        for (size_t i = 0; i < names.size(); ++i) {
#ifdef SC_HAS_CISST
            // TEMP: hide internal components and interfaces in case of cisst
            if (IsInternalInterface(names[i]))
                continue;
#endif
            if (count != 0)
                ss << ", ";

            State::StateType serviceState = gcm->GetServiceState(names[i], e, false);
            ss << "{ \"name\": \"" << names[i] << "\", "
               << "\"state\": " << static_cast<int>(gcm->GetInterfaceState(names[i], GCM::PROVIDED_INTERFACE)) << ", "
               << "\"service_state\": " << static_cast<int>(serviceState) << ", "
               << "\"event\": " << (e ? e->SerializeJSON() : JsonWrapper::JsonValue::null)
               << " }";
            ++count;
        }
        ss << " ], ";

        names.clear();
        it->second->GetNamesOfInterfaces(GCM::REQUIRED_INTERFACE, names);
        ss << "\"interfaces_required\": [ ";
        count = 0;
        for (size_t i = 0; i < names.size(); ++i) {
            if (count != 0)
                ss << ", ";
#ifdef SC_HAS_CISST
            // TEMP: hide internal components and interfaces in case of cisst
            if (IsInternalInterface(names[i]))
                continue;
#endif
            ss << "{ \"name\": \"" << names[i] << "\", ";
            ss << "\"state\": " << static_cast<int>(gcm->GetInterfaceState(names[i], GCM::REQUIRED_INTERFACE, e)) << ", "
               << "\"event\": " << (e ? e->SerializeJSON() : JsonWrapper::JsonValue::null)
               << " }";
            ++count;
        }
        ss << " ] }";

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
        SCLOG_ERROR << "Failed to read config file (event): \"" << jsonFileName << "\"" << std::endl;
        return false;
    }

    if (!AddFilterFromJSONFile(jsonFileName)) {
        SCLOG_ERROR << "Failed to read config file (filter): \"" << jsonFileName << "\"" << std::endl;
        return false;
    }

    if (!AddServiceStateDependencyFromJSONFile(jsonFileName)) {
        SCLOG_ERROR << "Failed to read config file (service state): \"" << jsonFileName << "\"" << std::endl;
        return false;
    }

    SCLOG_DEBUG << "Successfully processed config file: \"" << jsonFileName << "\"" << std::endl;

    return true;
}

bool Coordinator::ReadConfigFileFramework(const std::string & jsonFileName, const std::string & componentName)
{
    // NOTE: events should be processed first than filters because SC needs event information
    // to deploy filters.
    if (!AddEventFromJSONFileToComponent(jsonFileName, componentName)) {
        SCLOG_ERROR << "Failed to read config file for framework (event): \"" << jsonFileName << "\"" << std::endl;
        return false;
    }

    if (!AddFilterFromJSONFileToComponent(jsonFileName, componentName)) {
        SCLOG_ERROR << "Failed to read config file for framework (filter): \"" << jsonFileName << "\"" << std::endl;
        return false;
    }

    SCLOG_DEBUG << "Successfully processed config file for framework: \"" << jsonFileName << "\"" << std::endl;

    return true;
}

bool Coordinator::AddFilterFromJSONFileToComponent(const std::string & jsonFileName,
                                                   const std::string & targetComponentName)
{
    // Construct JSON structure from JSON file
    JsonWrapper json;
    if (!json.ReadFromFile(jsonFileName)) {
        SCLOG_ERROR << "AddFilterFromJSONFileToComponent: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }
    SCLOG_DEBUG << "AddFilterFromJSONFileToComponent: Successfully read json file: " << jsonFileName << std::endl;

    // Replace placeholder for target component name with actual target component name
    JsonWrapper::JsonValue & filters = json.GetRoot()["filter"];
    for (Json::ArrayIndex i = 0; i < filters.size(); ++i) {
        JsonWrapper::JsonValue & filter = filters[i];
        filter["target"]["component"] = targetComponentName;
    }

    bool ret = AddFilters(filters);
    if (!ret) {
        SCLOG_ERROR << "AddFilterFromJSONFile: Failed to add filter(s) from JSON file: " << jsonFileName << std::endl;
        return false;
    }

    SCLOG_DEBUG << "AddFilterFromJSONFile: Successfully added filter(s) from JSON file: " << jsonFileName << std::endl;

    return true;
}

bool Coordinator::AddFilterFromJSONFile(const std::string & jsonFileName)
{
    // Construct JSON structure from JSON file
    JsonWrapper json;
    if (!json.ReadFromFile(jsonFileName)) {
        SCLOG_ERROR << "AddFilterFromJSONFile: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }

    bool ret = AddFilterFromJSON(JsonWrapper::GetJSONString(json.GetRoot()));
    if (!ret) {
        SCLOG_ERROR << "AddFilterFromJSONFile: Failed to add filter(s) from JSON file: " << jsonFileName << std::endl;
        return false;
    }

    SCLOG_DEBUG << "AddFilterFromJSONFile: Successfully added filter(s) from JSON file: " << jsonFileName << std::endl;

    return ret;
}

bool Coordinator::AddFilterFromJSON(const std::string & jsonString)
{
    // Construct JSON structure from JSON string
    JsonWrapper json;
    if (!json.Read(jsonString.c_str())) {
        SCLOG_ERROR << "AddFilterFromJSON: Failed to read json string: " << jsonString << std::endl;
        return false;
    }

    const JsonWrapper::JsonValue filters = json.GetRoot()["filter"];
    bool ret = AddFilters(filters);
    if (!ret) {
        SCLOG_DEBUG << "AddFilterFromJSON: Failed to add filter(s) using json string: " << jsonString << std::endl;
        return false;
    }

    SCLOG_DEBUG << "AddFilterFromJSON: Successfully added filter(s) using json string: " << jsonString << std::endl;

    return true;
}

bool Coordinator::AddFilters(const JsonWrapper::JsonValue & filters)
{
    if (filters.isNull() || filters.size() == 0) {
        SCLOG_DEBUG << "AddFilter: No filter specification found in json: " << filters << std::endl;
        return true;
    }

    // Create filter target instance

    // Figure out how many filters are defined, and 
    // create and install filter instances while iterating each filter specification
    std::string filterClassName;
    FilterBase * filter = 0;
    bool enableLog = false;
    for (Json::ArrayIndex i = 0; i < filters.size(); ++i) {
        filterClassName = JsonWrapper::GetSafeValueString(filters[i], "class_name");
        enableLog = JsonWrapper::GetSafeValueBool(filters[i], "debug");

        // Create filter instance based on filter class name using filter factory
        filter = FilterFactory::GetInstance()->CreateFilter(filterClassName, filters[i]);
        if (!filter) {
            SCLOG_ERROR << "AddFilter: Failed to create filter instance \"" << filterClassName << "\"\n";
            continue;
        }

        // Install filter to the target component
        if (!AddFilter(filter)) {
            SCLOG_ERROR << "AddFilter: Failed to add filter \"" << filter->GetFilterName() << "\"\n";
            delete filter;
            return false;
        }

        // configure filter (process filter-specific arguments)
        if (!filter->ConfigureFilter(filters[i])) {
            SCLOG_ERROR << "AddFilter: Failed to process filter-specfic parts for filter instance: \"" << filterClassName << "\"\n";
            delete filter;
            return false;
        }

        // enable debug log if specified
        if (enableLog)
            filter->EnableDebugLog();

        SCLOG_DEBUG << "[" << (i + 1) << "/" << filters.size() << "] "
            << "Successfully installed filter: \"" << filter->GetFilterName() << "\"" << std::endl;
   }

    return true;
}

bool Coordinator::AddFilter(const std::string & componentName, FilterBase * filter)
{
    // check if component is added
    unsigned int cid = GetComponentId(componentName);
    if (cid == 0) {
        SCLOG_ERROR << "AddFilter: Component \"" << componentName << "\" not found" << std::endl;
        return false;
    }

    SCASSERT(filter);

    if (!filter->InitFilter()) {
        SCLOG_ERROR << "AddFilter: failed to initialize filter: " << *filter << std::endl;
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

    SCLOG_INFO << "AddFilter: successfully added filter \"" << filter->GetFilterName() 
               << "\" to component \"" << componentName << "\"" << std::endl;

    return true;
}

Coordinator::FiltersType * Coordinator::GetFilters(const std::string & componentName) const
{
    unsigned int cid = GetComponentId(componentName);
    if (cid == 0) {
        SCLOG_ERROR << "GetFilters: Component \"" << componentName << "\" not found" << std::endl;
        return 0;
    }

    FilterMapType::const_iterator it = MapFilter.find(componentName);
    return (it == MapFilter.end() ? 0 : it->second);
}

const std::string Coordinator::GetFilterList(const std::string & componentName, bool verbose) const
{
    std::stringstream ss;

    bool allComponents = (componentName.compare("*") == 0);

    FilterMapType::const_iterator it = MapFilter.begin();
    const FilterMapType::const_iterator itEnd = MapFilter.end();
    for (; it != itEnd; ++it) {
        FiltersType * filters = it->second;
        SCASSERT(filters);

        if (!allComponents)
            if (it->first.compare(componentName) != 0)
                continue;

        ss << "Component: \"" << it->first << "\"" << std::endl;

        FiltersType::const_iterator it2 = filters->begin();
        const FiltersType::const_iterator itEnd2 = filters->end();
        for (; it2 != itEnd2; ++it2)
            ss << "\t" << it2->second->ToString(verbose) << std::endl;
    }

    return ss.str();
}

bool Coordinator::InjectInputToFilter(FilterBase::FilterIDType fuid, const DoubleVecType & inputs, bool deepInjection)
{
    FilterMapType::const_iterator it = MapFilter.begin();
    const FilterMapType::const_iterator itEnd = MapFilter.end();
    for (; it != itEnd; ++it) {
        FiltersType * filters = it->second;
        SCASSERT(filters);

        FiltersType::const_iterator it2 = filters->find(fuid);
        if (it2 == filters->end())
            continue;

        FilterBase * filter = it2->second;
        SCASSERT(filter);

        filter->InjectInputScalar(inputs, deepInjection);

        return true;
    }

    return false;
}

bool Coordinator::InjectInputToFilter(FilterBase::FilterIDType fuid, const std::vector<DoubleVecType> & inputs, bool deepInjection)
{
    FilterMapType::const_iterator it = MapFilter.begin();
    const FilterMapType::const_iterator itEnd = MapFilter.end();
    for (; it != itEnd; ++it) {
        FiltersType * filters = it->second;
        SCASSERT(filters);

        FiltersType::const_iterator it2 = filters->find(fuid);
        if (it2 == filters->end())
            continue;

        FilterBase * filter = it2->second;
        SCASSERT(filter);

        for (size_t i = 0; i < inputs.size(); ++i)
            filter->InjectInputVector(inputs[i], deepInjection);

        return true;
    }

    return false;
}

bool Coordinator::AddEvent(const std::string & componentName, Event * event)
{
    // check if component is added
    unsigned int cid = GetComponentId(componentName);
    if (cid == 0) {
        SCLOG_ERROR << "AddEvent: Component \"" << componentName << "\" not found" << std::endl;
        return false;
    }

    SCASSERT(event);

    const std::string eventName = event->GetName();
    EventMapType::iterator it = MapEvent.find(componentName);
    if (it == MapEvent.end()) {
        EventsType * newList = new EventsType;
        newList->insert(std::make_pair(eventName, event));
        MapEvent[componentName] = newList;
    } else {
        it->second->insert(std::make_pair(eventName, event));
    }

    SCLOG_INFO << "AddFilter: successfully added event \"" << eventName << "\" to component \"" << componentName << "\"" << std::endl;

    return true;
}

bool Coordinator::AddEvents(const std::string & componentName, const JsonWrapper::JsonValue & events)
{
    if (events.isNull() || events.size() == 0) {
        SCLOG_INFO << "AddEvents: No event specification found in json: " << JsonWrapper::GetJSONString(events) << std::endl;
        return true;
    }

    // Figure out how many events are defined, and create and register event instances
    std::string  eventName, smId;
    unsigned int severity;
    SC::Event    *event = 0;
    for (Json::ArrayIndex i = 0; i < events.size(); ++i) {
        // event name
        eventName = JsonWrapper::GetSafeValueString(events[i], "name");
        if (eventName.size() == 0) {
            SCLOG_ERROR << "AddEvents: null event name. JSON: " << events[i] << std::endl;
            return false;
        }
        // severity (1: lowest priority, ..., 255: highest priority)
        severity = JsonWrapper::GetSafeValueUInt(events[i], "severity");
        if ((severity == 0) || (severity > 255)) {
            SCLOG_ERROR << "AddEvents: Invalid severity: " << severity << ", JSON: " << events[i] << std::endl;
            return false;
        }
        // state transition
        std::string tname;
        State::TransitionType t;
        Event::TransitionsType ts;

        const JsonWrapper::JsonValue & ts_json = events[i]["state_transition"];
        for (Json::ArrayIndex j = 0; j < ts_json.size(); ++j) {
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
                SCLOG_ERROR << "AddEvents: Invalid transition: " << tname << ", JSON: " << ts_json[i] << std::endl;
                return false;
            }
            ts.push_back(t);
        }

        // create event instance
        event = new Event(eventName, severity, ts);
        if (!AddEvent(componentName, event)) {
            SCLOG_ERROR << "AddEvents: Failed to add event \"" << eventName << "\"" << std::endl;
            delete event;
            return false;
        }
#if VERBOSE
        SCLOG_INFO << "[" << (i + 1) << "/" << events.size() << "] "
                   << "Successfully installed event: \"" << event->GetName() << "\"" << std::endl;
#endif
   }

    return true;
}

bool Coordinator::AddEventFromJSON(const std::string & jsonString)
{
    // Construct JSON structure from JSON string
    JsonWrapper json;
    if (!json.Read(jsonString.c_str())) {
        SCLOG_ERROR << "AddEventFromJSON: Failed to read json string: " << jsonString << std::endl;
        return false;
    }

    const SC::JsonWrapper::JsonValue events = json.GetRoot()["event"];
    const std::string componentName = JsonWrapper::GetSafeValueString(json.GetRoot(), "component");
    bool ret = AddEvents(componentName, events);
    if (!ret) {
        SCLOG_ERROR << "AddEventFromJSON: Failed to add events from JSON: " << jsonString << std::endl;
        return false;
    }

#if VERBOSE
    SCLOG_INFO << "AddEventFromJSON: Successfully added events from JSON: " << jsonString << std::endl;
#endif

    return true;
}

bool Coordinator::AddEventFromJSONFile(const std::string & jsonFileName)
{
    // Construct JSON structure from JSON file
    JsonWrapper json;
    if (!json.ReadFromFile(jsonFileName)) {
        SCLOG_ERROR << "AddEventFromJSONFile: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }

    bool ret = AddEventFromJSON(JsonWrapper::GetJSONString(json.GetRoot()));
    if (!ret) {
        SCLOG_ERROR << "AddEventFromJSONFile: Failed to add events from JSON file: " << jsonFileName << std::endl;
        return false;
    }

#if VERBOSE
    SCLOG_INFO << "AddEventFromJSONFile: Successfully added events from JSON file: " << jsonFileName << std::endl;
#endif

    return true;
}

bool Coordinator::AddEventFromJSONFileToComponent(const std::string & jsonFileName,
                                                  const std::string & targetComponentName)
{
    // Construct JSON structure from JSON file
    JsonWrapper json;
    if (!json.ReadFromFile(jsonFileName)) {
        SCLOG_ERROR << "AddEventFromJSONFileToComponent: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }

    // Insert target component name
    json.GetRoot()["component"] = targetComponentName;

    bool ret = AddEventFromJSON(JsonWrapper::GetJSONString(json.GetRoot()));
    if (!ret) {
        SCLOG_ERROR << "AddEventFromJSONFileToComponent: Failed to add events from JSON file: " << jsonFileName << std::endl;
        return false;
    }

#if VERBOSE
    SCLOG_INFO << "AddEventFromJSONFileToComponent: Successfully added events from JSON file: " << jsonFileName << std::endl;
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
        SCASSERT(events);

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
    JsonWrapper json;
    if (!json.Read(event.c_str())) {
        SCLOG_ERROR << "Coordinator::OnEvent: Failed to read json string: " << event << std::endl;
        return false;
    }
    // Remember information about event occurred
    EventHistory.push_back(json.GetRoot());

    JsonWrapper::JsonValue & jsonEvent = json.GetRoot()["event"];

    const std::string eventName         = JsonWrapper::GetSafeValueString(jsonEvent, "name");
    const TimestampType timestamp       = JsonWrapper::GetSafeValueDouble(jsonEvent, "timestamp");
    const std::string what              = JsonWrapper::GetSafeValueString(jsonEvent, "what");
#if VERBOSE
    const FilterBase::FilterIDType fuid = JsonWrapper::GetSafeValueUInt(jsonEvent, "fuid");
    const unsigned int severity         = JsonWrapper::GetSafeValueUInt(jsonEvent, "severity");
#endif

    // check if event is registered
    const Event * e = GetEvent(eventName);
    if (!e) {
        SCLOG_ERROR << "OnEvent: event \"" << eventName << "\" is not registered" << std::endl;
        return false;
    }
    SCLOG_DEBUG << "OnEvent: Received event: " << *e << std::endl;

    // Construct event object based on json
    Event evt(e->GetName(), e->GetSeverity(), e->GetTransitions());
    evt.SetTimestamp(timestamp ? timestamp : GetCurrentTimeTick());
    evt.SetWhat(what);

    jsonEvent = json.GetRoot()["target"];
    const State::StateMachineType targetStateMachineType = 
        static_cast<State::StateMachineType>(JsonWrapper::GetSafeValueUInt(jsonEvent, "type"));
    const std::string targetComponentName = JsonWrapper::GetSafeValueString(jsonEvent, "component");
    const std::string targetInterfaceName = JsonWrapper::GetSafeValueString(jsonEvent, "interface");

#if VERBOSE
    SCLOG_DEBUG << "fuid: " << fuid << std::endl
                << "name: " << eventName << std::endl
                << "severity: " << severity << std::endl
                << "timestamp: " << timestamp << std::endl
                << "what: " << what << std::endl
                << "targetStateMachineType: " << targetStateMachineType << std::endl
                << "targetComponentName: " << targetComponentName << std::endl
                << "targetInterfaceName: " << targetInterfaceName << std::endl;
#endif

    // Get state machine associated with the event, determine necessary transition 
    // based on the current state of the state machine. This state change may have
    // impact on other states.

    // Check if this event is broadcasted
    const bool broadcast = (evt.GetSeverity() >= Event::SEVERITY_BROADCAST_MIN);
    if (broadcast)
        SCASSERT(targetComponentName.compare("*") == 0);

    // Determine list of components (under this safety coordinator) to be notified of this event
    std::vector<std::string> componentList;
    if (!broadcast) {
        if (!GetGCMInstance(targetComponentName)) {
            SCLOG_ERROR << "OnEvent: no GCM instance found for component \"" << targetComponentName << "\"" << std::endl;
            return false;
        }
        componentList.push_back(targetComponentName);
    } else {
        Coordinator::GCMMapType::const_iterator it = MapGCM.begin();
        const Coordinator::GCMMapType::const_iterator itEnd = MapGCM.end();
        for (; it != itEnd; ++it)
            componentList.push_back(GetComponentName(it->first));
    }

    // Process state transition and get json string that includes the changes
    GCM * gcm;
    for (size_t i = 0; i < componentList.size(); ++i) {
        gcm = GetGCMInstance(componentList[i]);
        SCASSERT(gcm);

        // For error propagation
        JsonWrapper _jsonServiceStateChange;
        JsonWrapper::JsonValue & jsonServiceStateChange = _jsonServiceStateChange.GetRoot();
        const State::TransitionType transition = 
            gcm->ProcessStateTransition(targetStateMachineType, evt, targetInterfaceName, jsonServiceStateChange);
        if (transition == State::INVALID_TRANSITION) {
            SCLOG_WARNING << "OnEvent: invalid transition for event " << evt << std::endl;
            continue;
        } else if (transition == State::NO_TRANSITION) {
            continue;
        }

        // Publish service state change message
        if (jsonServiceStateChange != JsonWrapper::JsonValue::null) {
            PublishMessage(Topic::Control::STATE_UPDATE, JsonWrapper::GetJSONString(jsonServiceStateChange));
            SCLOG_DEBUG << "OnEvent: [ " << targetComponentName << " : " << targetInterfaceName << " ] "
                        << "event [ " << evt << " ] occurred.  Publishing error propagation json:\n"
                        << JsonWrapper::GetJSONString(jsonServiceStateChange) << std::endl;
        }
        else
            SCLOG_DEBUG << "OnEvent: [ " << targetComponentName << " : " << targetInterfaceName << " ] "
                        << "event " << evt << " occurred but has no impact on service state of "
                        << "any of its provided interface.  No further error propagation." << std::endl;
    }

    // Refresh state viewer
    JsonWrapper _jsonRefresh;
    JsonWrapper::JsonValue & jsonRefresh = _jsonRefresh.GetRoot();
    jsonRefresh["target"]["safety_coordinator"] = "*";
    jsonRefresh["target"]["component"] = "*";
    jsonRefresh["request"] = "state_list";
    PublishMessage(Topic::Control::READ_REQ, JsonWrapper::GetJSONString(jsonRefresh));

    // Call event hook for middleware
    return OnEventHandler(&evt);
}

GCM * Coordinator::GetGCMInstance(const std::string & componentName) const
{
    Coordinator::GCMMapType::const_iterator it = MapGCM.find(GetComponentId(componentName));
    if (it == MapGCM.end())
        return 0;
    else
        return it->second;
}

GCM * Coordinator::GetGCMInstance(unsigned int componentId) const
{
    return GetGCMInstance(GetComponentName(componentId));
}

bool Coordinator::AddServiceStateDependencyFromJSON(const std::string & jsonString)
{
    // Construct JSON structure from JSON string
    JsonWrapper json;
    if (!json.Read(jsonString.c_str())) {
        SCLOG_ERROR << "AddServiceStateDependencyFromJSON: Failed to read json string: " << jsonString << std::endl;
        return false;
    }

    const JsonWrapper::JsonValue & services = json.GetRoot()["service"];
    if (services != JsonWrapper::JsonValue::null) {
        const std::string componentName = JsonWrapper::GetSafeValueString(json.GetRoot(), "component");
        GCM * gcm = GetGCMInstance(componentName);
        if (!gcm) {
            SCLOG_ERROR << "AddServiceStateDependencyFromJSON: no component found: \"" << componentName << "\"" << std::endl;
            return false;
        }

        gcm->AddServiceStateDependency(services);
    }

    SCLOG_DEBUG << "AddServiceStateDependencyFromJSON: Successfully added service state dependency using json string: " << jsonString << std::endl;

    return true;
}

bool Coordinator::AddServiceStateDependencyFromJSONFile(const std::string & jsonFileName)
{
    // Construct JSON structure from JSON file
    JsonWrapper json;
    if (!json.ReadFromFile(jsonFileName)) {
        SCLOG_ERROR << "AddServiceStateDependencyFromJSONFile: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }

    bool ret = AddServiceStateDependencyFromJSON(JsonWrapper::GetJSONString(json.GetRoot()));
    if (!ret) {
        SCLOG_ERROR << "AddServiceStateDependencyFromJSONFile: Failed to add service state dependency from JSON file: " << jsonFileName << std::endl;
        return false;
    }

    SCLOG_DEBUG << "AddServiceStateDependencyFromJSONFile: Successfully added service state dependency from JSON file: " << jsonFileName << std::endl;

    return ret;
}

bool Coordinator::OnEventPropagation(const JsonWrapper::JsonValue & json)
{
    if (json.size() == 0) {
        SCLOG_ERROR << "OnEventPropagation: no error propagation info" << std::endl;
        return false;
    }

    std::string scName, componentName, interfaceName;
    for (Json::ArrayIndex i = 0; i < json.size(); ++i) {
        const JsonWrapper::JsonValue & src = json[i];

        // deserialize event information
        State::StateType state  = static_cast<State::StateType>(JsonWrapper::GetSafeValueUInt(src, "state"));
        unsigned int severity   = JsonWrapper::GetSafeValueUInt(src["event"], "severity");
        TimestampType timestamp = JsonWrapper::GetSafeValueDouble(src["event"], "timestamp");
        std::string eventName   = JsonWrapper::GetSafeValueString(src["event"], "name");

        for (Json::ArrayIndex j = 0; j < src["target"].size(); ++j) {
            const JsonWrapper::JsonValue & target = src["target"][j];

            scName = JsonWrapper::GetSafeValueString(target, "safety_coordinator");
            if (this->Name.compare(scName) != 0)
                continue; // this error propagation target is not for this safety coordinator

            componentName = JsonWrapper::GetSafeValueString(target, "component");
            GCM * gcm = GetGCMInstance(componentName);
            if (!gcm) {
                SCLOG_ERROR << "OnEventPropagation: no component found: \"" << componentName << "\"" << std::endl;
                continue;
            }

            interfaceName = JsonWrapper::GetSafeValueString(target, "interface");
            if (!gcm->FindInterface(interfaceName, GCM::REQUIRED_INTERFACE)) {
                SCLOG_ERROR << "OnEventPropagation: no required interface found: \"" << interfaceName << "\"" << std::endl;
                continue;
            }

            // Generate service failure event based on event information received
            JsonWrapper _jsonEvent;
            JsonWrapper::JsonValue & jsonEvent = _jsonEvent.GetRoot();
            jsonEvent["event"]["severity"] = severity;
            jsonEvent["event"]["timestamp"] = timestamp;
            if (state == State::ERROR || state == State::FAILURE)
                jsonEvent["event"]["name"] = "EVT_SERVICE_FAILURE"; // see libs/fdd/filters/json/framework_filters.json
            else if (state == State::NORMAL)
                jsonEvent["event"]["name"] = "/EVT_SERVICE_FAILURE"; // see libs/fdd/filters/json/framework_filters.json
            else {
                SCLOG_ERROR << "OnEventPropagation: Invalid state: " << State::GetStringState(state) << std::endl;
                continue;
            }
            jsonEvent["target"]["type"] = static_cast<unsigned int>(State::STATEMACHINE_REQUIRED);
            jsonEvent["target"]["component"] = componentName;
            jsonEvent["target"]["interface"] = interfaceName;

            SCLOG_DEBUG << "OnEventPropagation: Propagating service failure event:\n" << JsonWrapper::GetJSONString(jsonEvent) << std::endl;

            if (!OnEvent(JsonWrapper::GetJSONString(jsonEvent))) {
                SCLOG_ERROR << "OnEventPropagation: Failed to propagate service failure state:\n"
                            << JsonWrapper::GetJSONString(jsonEvent) << std::endl;
                continue;
            }

            //
            // TODO: add pending event to required interface that maintains what caused
            // service failures.  However, all service failure events are REMAPPED as
            // [/]EVT_SERVICE_FAILURE.
            //
            SCLOG_DEBUG << "OnEventPropagation: " << eventName << ", " << severity << ", " << timestamp << ", " << State::GetStringState(state) << std::endl;
        }
    }

    return true;
}

bool Coordinator::AddConnection(const std::string & clientSCName, const std::string & clientCompName, const std::string & clientIntfcName,
                                const std::string & serverSCName, const std::string & serverCompName, const std::string & serverIntfcName)
{
    // local configuration
    if (clientSCName.compare(serverSCName) == 0) {
        // If name of this safety coordinator does not match neither client or server name
        if (clientSCName.compare(Name) != 0)
            return true;
        // figure out which interface is provided interface
        GCM * gcmClient = GetGCMInstance(clientCompName);
        GCM * gcmServer = GetGCMInstance(serverCompName);
        SCASSERT(gcmClient && gcmServer);
        if (gcmClient->FindInterface(clientIntfcName, GCM::REQUIRED_INTERFACE))// &&
            //gcmServer->FindInterface(serverIntfcName, GCM::PROVIDED_INTERFACE))
        {
            // Client is required, server is provided
            // add connection to server:serverIntfcName
            gcmServer->AddConnection(serverIntfcName, clientSCName, clientCompName, clientIntfcName);
        } else if (gcmClient->FindInterface(clientIntfcName, GCM::PROVIDED_INTERFACE))// &&
                   //gcmServer->FindInterface(serverIntfcName, GCM::REQUIRED_INTERFACE))
        {
            // Server is required, client is provided
            // add connection to client:clientIntfcName
            gcmClient->AddConnection(clientIntfcName, serverSCName, serverCompName, serverIntfcName);
        } else {
            SCLOG_ERROR << "AddConnection: failed to add connection: "
                        << "[ " << clientSCName << " : " << clientCompName << " : " << clientIntfcName << " ] - "
                        << "[ " << serverSCName << " : " << serverCompName << " : " << serverIntfcName << " ]" << std::endl;
            return false;
        }
    }
    // networked configuration
    else {
        // This instance of coordinator is client
        if (clientSCName.compare(Name) == 0) {
            GCM * gcm = GetGCMInstance(clientCompName);
            if (!gcm) {
                SCLOG_ERROR << "AddConnection: no component (as client) found: \"" << clientCompName << "\"" << std::endl;
                return false;
            }
            // Figure out if client interface is required or provided
            if (gcm->FindInterface(clientIntfcName, GCM::REQUIRED_INTERFACE)) {
                // NOP
                ;
            } else {
                SCASSERT(gcm->FindInterface(clientIntfcName, GCM::PROVIDED_INTERFACE));
                // Server is required, client is provided
                // add connection to client:clientIntfcName
                gcm->AddConnection(clientIntfcName, serverSCName, serverCompName, serverIntfcName);
            }
        }
        else {
            // This instance of coordinator is server
            SCASSERT(serverSCName.compare(Name) == 0);

            GCM * gcm = GetGCMInstance(serverCompName);
            if (!gcm) {
                SCLOG_ERROR << "AddConnection: no component (as server) found: \"" << serverCompName << "\"" << std::endl;
                return false;
            }
            // Figure out if server interface is required or provided
            if (gcm->FindInterface(serverIntfcName, GCM::REQUIRED_INTERFACE)) {
                // NOP
                ;
            } else {
                SCASSERT(gcm->FindInterface(serverIntfcName, GCM::PROVIDED_INTERFACE));
                // Client is required, server is provided
                // add connection to server:serverIntfcName
                gcm->AddConnection(serverIntfcName, clientSCName, clientCompName, clientIntfcName);
            }
        }
    }

    SCLOG_DEBUG << "AddConnection: successfully added connection: "
                << "[ " << clientSCName << " : " << clientCompName << " : " << clientIntfcName << " ] - "
                << "[ " << serverSCName << " : " << serverCompName << " : " << serverIntfcName << " ]" << std::endl;

    return true;
}

const std::string Coordinator::GetConnectionList(const std::string & componentName,
                                                 const std::string & prefix) const
{
    std::stringstream ss;

    bool allComponents = (componentName.compare("*") == 0);

    GCMMapType::const_iterator it = MapGCM.begin();
    const GCMMapType::const_iterator itEnd = MapGCM.end();
    for (; it != itEnd; ++it) {
        GCM * gcm = it->second;
        if (!allComponents)
            if (GetComponentName(it->first).compare(componentName) != 0)
                continue;

        ss << "Component: \"" << GetComponentName(it->first) << "\"" << std::endl;
        gcm->PrintConnections(ss, prefix);
    }

    return ss.str();
}

const std::string Coordinator::GetServiceDependencyList(const std::string & componentName,
                                                        const std::string & prefix) const
{
    std::stringstream ss;

    bool allComponents = (componentName.compare("*") == 0);

    GCMMapType::const_iterator it = MapGCM.begin();
    const GCMMapType::const_iterator itEnd = MapGCM.end();
    for (; it != itEnd; ++it) {
        GCM * gcm = it->second;
        if (!allComponents)
            if (GetComponentName(it->first).compare(componentName) != 0)
                continue;

        ss << "Component: \"" << GetComponentName(it->first) << "\"" << std::endl;
        gcm->PrintServiceStateDependencyTable(ss, prefix);
    }

    return ss.str();
}

void Coordinator::GenerateEvent(const std::string &     eventName,
                                State::StateMachineType type,
                                const std::string &     what,
                                const std::string &     componentName,
                                const std::string &     interfaceName)
{
    // get timestamp as early as possible
    // FIXME: After integrating Boost chrono, update this to fetch real timestamp
    TimestampType tick = GetCurrentTimeTick();

    // Look up event dictionary to see if the name of user-provided event is valid
    const Event * e = GetEvent(eventName);
    if (!e) {
        SCLOG_ERROR << "Coordinator::GenerateEvent: no registered event found: \"" << eventName << std::endl;
        return;
    }

    Event evt(*e);

    // Set run-time attributes of event
    evt.SetTimestamp(tick);
    evt.SetWhat(what);

    JsonWrapper _json;
    JsonWrapper::JsonValue & json = _json.GetRoot();
    json["event"]["severity"]   = evt.GetSeverity();
    json["event"]["name"]       = evt.GetName();
    json["event"]["timestamp"]  = evt.GetTimestamp();
    json["event"]["what"]       = evt.GetWhat();
    json["target"]["type"]      = static_cast<int>(type);
    json["target"]["component"] = componentName;
    json["target"]["interface"] = interfaceName;

    SCLOG_INFO << "Coordinator::GenerateEvent: \n" << JsonWrapper::GetJSONString(json) << std::endl;

    if (!OnEvent(JsonWrapper::GetJSONString(json)))
        SCLOG_ERROR << "Coordinator::GenerateEvent: Failed to handle event:\n" << JsonWrapper::GetJSONString(json) << std::endl;
    else
        SCLOG_DEBUG << "Coordinator::GenerateEvent: Successfully generated and handled event:\n" << JsonWrapper::GetJSONString(json) << std::endl;
}

bool Coordinator::BroadcastEvent(const std::string & eventName, const std::string & what)
{
    const Event * e = GetEvent(eventName);
    if (!e) {
        SCLOG_ERROR << "Coordinator::BroadcastEvent: no event found: \"" << eventName << "\"" << std::endl;
        return false;
    }

    // check if event severity is set for broadcast
    if (e->GetSeverity() < Event::SEVERITY_BROADCAST_MIN) {
        SCLOG_ERROR << "Coordinator::BroadcastEvent: event \"" << eventName << "\" is not set for broadcast: " << *e << std::endl;
        return false;
    }

    // broadcast
    GenerateEvent(eventName, State::STATEMACHINE_APP, what, "*");

    return true;
}

#if 0
State::StateType Coordinator::GetState(State::StateMachineType type,
                                       const std::string & componentName,
                                       const std::string & interfaceName) const
{
    GCM * gcm = GetGCMInstance(componentName);
    if (gcm == 0) {
        SCLOG_ERROR << "Coordinator::GetState: No component found: \"" << componentName << "\"" << std::endl;
        return State::INVALID;
    }

    switch (type) {
    case State::STATEMACHINE_FRAMEWORK:
        return gcm->GetComponentState(GCM::FRAMEWORK_VIEW);
    case State::STATEMACHINE_APP:
        return gcm->GetComponentState(GCM::APPLICATION_VIEW);
    case State::STATEMACHINE_PROVIDED:
        return gcm->GetInterfaceState(interfaceName, GCM::PROVIDED_INTERFACE);
    case State::STATEMACHINE_REQUIRED:
        return gcm->GetInterfaceState(interfaceName, GCM::REQUIRED_INTERFACE);
    default:
        return State::INVALID;
    }
}
#endif

State::StateType Coordinator::GetComponentState(const std::string & componentName,
                                                GCM::ViewType view) const
{
    GCM * gcm = GetGCMInstance(componentName);
    if (gcm == 0) {
        SCLOG_ERROR << "Coordinator::GetComponentState: No component found: \"" << componentName << "\"" << std::endl;
        return State::INVALID;
    }

    return gcm->GetComponentState(view);
}

State::StateType Coordinator::GetComponentState(const std::string & componentName,
                                                const Event* & e,
                                                GCM::ViewType view) const
{
    GCM * gcm = GetGCMInstance(componentName);
    if (gcm == 0) {
        SCLOG_ERROR << "Coordinator::GetComponentState (with event): No component found: \"" << componentName << "\"" << std::endl;
        return State::INVALID;
    }

    return gcm->GetComponentState(view, e);
}

State::StateType Coordinator::GetInterfaceState(const std::string & componentName,
                                                const std::string & interfaceName,
                                                GCM::InterfaceType type) const
{
    GCM * gcm = GetGCMInstance(componentName);
    if (gcm == 0) {
        SCLOG_ERROR << "Coordinator::GetInterfaceState: No component found: \"" << componentName << "\"" << std::endl;
        return State::INVALID;
    }

    return gcm->GetInterfaceState(interfaceName, type);
}

State::StateType Coordinator::GetInterfaceState(const std::string & componentName,
                                                const std::string & interfaceName,
                                                const Event* & e,
                                                GCM::InterfaceType type) const
{
    GCM * gcm = GetGCMInstance(componentName);
    if (gcm == 0) {
        SCLOG_ERROR << "Coordinator::GetInterfaceState: No component found: \"" << componentName << "\"" << std::endl;
        return State::INVALID;
    }

    return gcm->GetInterfaceState(interfaceName, type, e);
}

bool Coordinator::SetEventHandlerForComponent(const std::string & componentName,
                                              GCM::ViewType view,
                                              StateEventHandler * handler)
{
    if (view == GCM::SYSTEM_VIEW) {
        SCLOG_ERROR << "Coordinator::SetEventHandlerForComponent: view should be either FRAMEWORK_VIEW or APPLICATION_VIEW" << std::endl;
        return false;
    }
    if (!handler) {
        SCLOG_ERROR << "Coordinator::SetEventHandlerForComponent: NULL handler" << std::endl;
        return false;
    }

    GCM * gcm = GetGCMInstance(componentName);
    if (gcm == 0) {
        SCLOG_ERROR << "Coordinator::SetEventHandlerForComponent: Component \"" << componentName << "\" not found" << std::endl;
        return false;
    }

    return gcm->SetEventHandlerForComponent(view, handler);
}

bool Coordinator::SetEventHandlerForInterface(const std::string & componentName,
                                              const std::string & interfaceName,
                                              GCM::InterfaceType type,
                                              StateEventHandler * handler)
{
    GCM * gcm = GetGCMInstance(componentName);
    if (gcm == 0) {
        SCLOG_ERROR << "Coordinator::SetEventHandlerForInterface: Component \"" << componentName << "\" not found" << std::endl;
        return false;
    }
    if (!handler) {
        SCLOG_ERROR << "Coordinator::SetEventHandlerForInterface: NULL handler" << std::endl;
        return false;
    }

    return gcm->SetEventHandlerForInterface(type, interfaceName, handler);
}


const Event * Coordinator::GetOutstandingEvent(const std::string & componentName,
                                               GCM::ViewType view) const
{
    GCM * gcm = GetGCMInstance(componentName);
    if (gcm == 0)
        return 0;

    const Event * e = 0;
    gcm->GetComponentState(view, e);
    return e;
}

const Event * Coordinator::GetOutstandingEvent(const std::string & componentName,
                                               const std::string & interfaceName,
                                               GCM::InterfaceType type) const
{
    GCM * gcm = GetGCMInstance(componentName);
    if (gcm == 0)
        return 0;

    const Event * e = 0;
    gcm->GetInterfaceState(interfaceName, type, e);
    return e;
}


const std::string Coordinator::GetOutstandingEventName(const std::string & componentName,
                                                       GCM::ViewType view) const
{
    const Event * e = GetOutstandingEvent(componentName, view);

    return (e ? e->GetName() : "");
}

const std::string Coordinator::GetOutstandingEventName(const std::string & componentName,
                                                       const std::string & interfaceName,
                                                       GCM::InterfaceType type) const
{
    const Event * e = GetOutstandingEvent(componentName, interfaceName, type);

    return (e ? e->GetName() : "");
}

bool Coordinator::IsOutstandingEvent(const std::string & eventName,
                                     const std::string & componentName,
                                     GCM::ViewType view) const
{
    const Event * e = GetOutstandingEvent(componentName, view);
    if (e == 0)
        return false;
    return (eventName.compare(e->GetName()) == 0);
}

bool Coordinator::IsOutstandingEvent(const std::string & eventName,
                                     const std::string & componentName,
                                     const std::string & interfaceName,
                                     GCM::InterfaceType type) const
{
    const Event * e = GetOutstandingEvent(componentName, interfaceName, type);
    if (e == 0)
        return false;
    return (eventName.compare(e->GetName()) == 0);
}

void Coordinator::ResetStateMachines(bool resetAll)
{
    {
        boost::mutex::scoped_lock lock(Mutex);

        // Reset all state machines and associated events
        GCMMapType::const_iterator it = MapGCM.begin();
        const GCMMapType::const_iterator itEnd = MapGCM.end();
        for (; it != itEnd; ++it) {
            GCM * gcm = it->second;
            gcm->ResetStatesAndEvents();
        }
    }

    std::stringstream ss;
    ss << "[ " << GetName() << " ] Coordinator::ResetStateMachines: reset ";
    if (resetAll)
        ss << "ALL coordinators";
    else {
        ss << "coordinator \"" << GetName() << "\"";
    }
    ss << std::endl;

    SCLOG_INFO << ss.str();

    // Refresh state viewer
    JsonWrapper _jsonRefresh;
    JsonWrapper::JsonValue & jsonRefresh = _jsonRefresh.GetRoot();
    jsonRefresh["target"]["safety_coordinator"] = "*";
    jsonRefresh["target"]["component"] = "*";
    jsonRefresh["request"] = "state_list";
    PublishMessage(Topic::Control::READ_REQ, JsonWrapper::GetJSONString(jsonRefresh));

    if (resetAll) {
        // Broadcast event to reset state machines of the other safety coordinators as well
        JsonWrapper _json;
        JsonWrapper::JsonValue & json = _json.GetRoot();
        json["target"]["safety_coordinator"] = "*";
        json["source"] = GetName();
        json["request"] = "state_reset";
        PublishMessage(Topic::Control::READ_REQ, JsonWrapper::GetJSONString(json));

        SCLOG_INFO << "[ " << GetName() << " ] " << JsonWrapper::GetJSONString(json) << std::endl;
    }
}

void Coordinator::ResetStateMachines(const std::string & componentName, State::StateMachineType type)
{
    // Find component
    if (!FindComponent(componentName)) {
        SCLOG_ERROR << "[ " << GetName() << " ] Coordinator::ResetStateMachines: no component \"" << componentName << "\" found";
        return;
    }

    // Set string for state machine type
    std::string typeString("invalid");
    if (type == State::STATEMACHINE_INVALID) {
        SCLOG_ERROR << "[ " << GetName() << " ] Coordinator::ResetStateMachines: invalid state machine type";
        return;
    } else {
        switch (type) {
        case State::STATEMACHINE_FRAMEWORK: typeString = "FRAMEWORK"; break;
        case State::STATEMACHINE_APP:       typeString = "APPLICATION"; break;
        case State::STATEMACHINE_PROVIDED:  typeString = "INTERFACE_PROVIDED"; break;
        case State::STATEMACHINE_REQUIRED:  typeString = "INTERFACE_REQUIRED"; break;
        case State::STATEMACHINE_INVALID:
            SCASSERT(false); // must not reach here
        }
    }

    {
        boost::mutex::scoped_lock lock(Mutex);

        // Reset all state machines and associated events
        GCMMapType::const_iterator it = MapGCM.find(GetComponentId(componentName));
        GCM * gcm = it->second;
        SCASSERT(gcm);
        gcm->ResetStatesAndEvents(type);
    }

    std::stringstream ss;
    ss << "[ " << GetName() << " ] Coordinator::ResetStateMachines: reset coordinator \"" << GetName() << "\""
       << ", state machine type: " << typeString << std::endl;
    SCLOG_INFO << ss.str();

    // Refresh state viewer
    JsonWrapper _jsonRefresh;
    JsonWrapper::JsonValue & jsonRefresh = _jsonRefresh.GetRoot();
    jsonRefresh["target"]["safety_coordinator"] = "*";
    jsonRefresh["target"]["component"] = "*";
    jsonRefresh["request"] = "state_list";
    PublishMessage(Topic::Control::READ_REQ, JsonWrapper::GetJSONString(jsonRefresh));
}

const std::string Coordinator::GetStateHistory(const std::string & componentName) const
{
    JsonWrapper _json;
    JsonWrapper::JsonValue & json = _json.GetRoot();

    bool allComponents = (componentName.compare("*") == 0);

    GCM * gcm;
    if (!allComponents) {
        unsigned int cid = GetComponentId(componentName);
        if (cid == 0) {
            std::stringstream ss;
            ss << "[ " << GetName() << " ] Coordinator::GetStateHistory: no component \"" << componentName << "\" found.";
            return ss.str();
        }

        GCMMapType::const_iterator it = MapGCM.find(cid);
        SCASSERT(it != MapGCM.end());

        gcm = it->second;
        gcm->GetStateHistory(json);
    } else {
        GCMMapType::const_iterator it = MapGCM.begin();
        const GCMMapType::const_iterator itEnd = MapGCM.end();
        unsigned int id = 0;
        for (; it != itEnd; ++it) {
#ifdef SC_HAS_CISST
            // TEMP: hide internal components and interfaces in case of cisst
            if (IsInternalComponent(GetComponentName(it->first)))
                continue;
#endif
            gcm = it->second;
            id += gcm->GetStateHistory(json, id);
        }
    }

    return _json.GetJSON();
}

const std::string Coordinator::GetEventHistory(const std::string & componentName)
{
    if (EventHistory.empty())
        return std::string("No event has not occurred yet in the system.");

    bool allComponents = (componentName.compare("*") == 0);

    EventHistoryType::iterator it = EventHistory.begin();
    EventHistoryType::iterator itEnd = EventHistory.end();

    JsonWrapper _historyJson;
    JsonWrapper::JsonValue & historyJson = _historyJson.GetRoot();
    if (allComponents) {
        for (; it != itEnd; ++it) {
            historyJson.append(*it);
        }
    } else {
        for (; it != itEnd; ++it) {
            JsonWrapper::JsonValue & json = (*it)["target"];
            const std::string _componentName = JsonWrapper::GetSafeValueString(json, "component");
            if (componentName.compare(_componentName) != 0)
                continue;

            historyJson.append(*it);
        }
    }

    return _historyJson.GetJSON();
}
