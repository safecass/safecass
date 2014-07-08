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
#ifndef _coordinator_h
#define _coordinator_h

#include "common.h"
#include "monitor.h"
#include "gcm.h"
#include "filterBase.h"

#include <map>
//#include <queue>

namespace SF {

// TODO: casros accessor is not included here.  If accessor is implemented without cisst,
// it should be moved to this base class.

class SFLIB_EXPORT Coordinator {
public:
    //! Typedef for map of monitoring targets
    /*! key: string of monitoring target UID
     *  value: string of monitoring target in JSON representation
     */
    typedef std::map<std::string, std::string> MonitorTargetMapType;

    // Dictionary to convert numeric component id to its name or vice versa
    typedef std::map<unsigned int, std::string> ComponentIdToNameMapType;
    typedef std::map<std::string, unsigned int> ComponentNameToIdMapType;

    // STATES: Container to manage the entire set of states of the current process
    typedef std::map<unsigned int, GCM*> GCMMapType;
    
    // EVENTS 
    // key: event name
    typedef std::map<std::string, Event*> EventsType;
    // key: component name
    typedef std::map<std::string, EventsType*> EventMapType;

    // FILTERS
    typedef std::map<FilterBase::FilterIDType, FilterBase*> FiltersType;
    // key: component name, value: filter container
    typedef std::map<std::string, FiltersType*> FilterMapType;

    // TODO: CONNECTIONS
    // - do this with graph

protected:
    // Name of this coordinator
    const std::string Name;

    // Monitor map
    //! Map of monitoring targets
    MonitorTargetMapType MapMonitorTarget;

    // component id counter
    unsigned int ComponentIdCounter;

    /*! \addtogroup Management of monitoring targets
     * @{
     */
    //! Install monitoring target
    virtual bool AddMonitoringTarget(const std::string & targetUID, 
                                     const std::string & targetJSON);
    //! Check if given uid already exists
    bool FindMonitoringTarget(const std::string & targetUID) const;
    //! Remove monitoring target
    bool RemoveMonitoringTarget(const std::string & targetUID);
    //! Print out monitoring targets
    void PrintMonitoringTargets(std::ostream & outputStream) const;
    /* @} */

    // Dictionaries to convert numeric component id to its name or vice versa
    ComponentIdToNameMapType MapComponentIdToName;
    ComponentNameToIdMapType MapComponentNameToId;

    // STATES
    GCMMapType MapGCM;
    // EVENTS
    EventMapType MapEvent;
    // FILTERS
    FilterMapType MapFilter;
    // CONNECTIONS
    // TODO

protected:
    // Don't allow to create this object without its name
    Coordinator();
public:
    //! Constructor
    Coordinator(const std::string & name);
    //! Destructor
    virtual ~Coordinator();

    //! Deploy all monitors and filter pipelines
    /*! Middleware specific: This method should be overriden by derived class
     * because each middleware has different component composition processes.
     */
    virtual bool DeployMonitorsAndFDDs(void) = 0;

    //! Handle event (fault, error, failure)
    /*! Middleware specific: This method should be overriden by derived class
     */
    virtual void OnFaultEvent(const std::string & json) = 0;

    //! For human-readable logging and debugging
    /*! \param outputStream output stream
        \param includeLocation target location is printed if yes (default: yes)
    */
    virtual void ToStream(std::ostream & outputStream) const;

    //
    // STATES
    //
    // Add component. Returns non-zero component id for success, zero for failure.
    unsigned int AddComponent(const std::string & componentName);
    // Get component id using its name
    unsigned int GetComponentId(const std::string & componentName) const;
    // Get component name using its id
    const std::string GetComponentName(unsigned int componentId) const;
    // Add interface
    bool AddInterface(const std::string & componentName, 
                      const std::string & interfaceName,
                      const GCM::InterfaceTypes type);
    bool RemoveInterface(const std::string & componentName, 
                         const std::string & interfaceName,
                         const GCM::InterfaceTypes type);
    // Get state information of the entire system
    const std::string GetStateSnapshot(const std::string & componentName = "*") const;

    //
    // FILTERS
    //
    // Get all filters installed on the component specified
    FiltersType * GetFilters(const std::string & componentName) const;
    // Add filter instance
    bool AddFilter(const std::string & componentName, FilterBase * filter);
    // Get information about all the filters installed on the component specified
    const std::string GetFilterList(const std::string & componentName = "*") const;
    // Get information about all the filters installed on the component specified
    bool InjectInputToFilter(FilterBase::FilterIDType fuid, const DoubleVecType & inputs);

    //
    // EVENTS
    //
    // Add event using event instance
    bool AddEvent(const std::string & componentName, Event * event);
    // Add event using JSON array object
    bool AddEvents(const std::string & componentName, const JSON::JSONVALUE & events);
    // Add event using JSON string
    bool AddEventFromJSON(const std::string & jsonString);
    // Add event using file containing JSON string
    bool AddEventFromJSONFile(const std::string & jsonFileName);
    // Given an event name, check if event exists
    bool FindEvent(const std::string & componentName, const std::string & eventName) const;
    bool FindEvent(const std::string & eventName) const;
    // Given an event name, returns event instance
    const Event * GetEvent(const std::string & componentName, const std::string & eventName) const;
    const Event * GetEvent(const std::string & eventName) const;
    // Get information about all the events installed on the component specified
    const std::string GetEventList(const std::string & componentName = "*") const;
    // Called by filter when event is generated.  Internally calls OnEventHandler()
    bool OnEvent(const std::string & eventName);
    // Called by OnEvent() to inform the derived class (e.g., mtsSafetyCoordinator in case
    // of cisst) of the event
    virtual bool OnEventHandler(const Event * e) = 0;
};

inline std::ostream & operator << (std::ostream & outputStream, const Coordinator & coordinator)
{
    coordinator.ToStream(outputStream);
    return outputStream;
}

};

#endif // _coordinator_h
