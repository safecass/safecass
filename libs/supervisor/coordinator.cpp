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

using namespace SF;

Coordinator::Coordinator(const std::string & name): Name(name), ComponentIdCounter(0)
{}

Coordinator::~Coordinator()
{
    // TODO: cleanup: FilterMapType
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

    SFLOG_INFO << "AddFilter: successfully added filter \"" << *filter << "\" to component \"" << componentName << "\"" << std::endl;

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
