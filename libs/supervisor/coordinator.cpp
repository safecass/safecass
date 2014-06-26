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

namespace SF {

Coordinator::Coordinator(): ComponentIdCounter(0)
{}

Coordinator::~Coordinator()
{}

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
    if (MapComponentNameToId.find(componentName) != MapComponentNameToId.end())
        return 0;

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

};
