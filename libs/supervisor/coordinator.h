//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 14, 2012
// Last revision: Jul 22, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _coordinator_h
#define _coordinator_h

#include "common.h"
#include "monitor.h"
#include "gcm.h"
#include "filterBase.h"
#include "topic_def.h"

#include <map>

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
    typedef std::map<unsigned int, GCM*> GCMMapType; // key: component id
    
    // EVENTS
    typedef std::map<std::string, Event*> EventsType; // key: event name
    typedef std::map<std::string, EventsType*> EventMapType; // key: component name

    // FILTERS
    typedef std::map<FilterBase::FilterIDType, FilterBase*> FiltersType;
    typedef std::map<std::string, FiltersType*> FilterMapType; // key: component name

protected:
    // Name of this coordinator
    std::string Name;

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

    GCM * GetGCMInstance(const std::string & componentName) const;

protected:
    // Don't allow to create this object without its name
    Coordinator();
public:
    //! Constructor
    Coordinator(const std::string & name);
    //! Destructor
    virtual ~Coordinator();

    // Get name of instance
    inline const std::string & GetName(void) const { return Name; }
    // Set name of instance
    inline void SetName(const std::string & name) { Name = name; }

    // Read and process configuration file that contains definitions for events, filters,
    // and service states dependency information.
    bool ReadConfigFile(const std::string & jsonFileName);
    bool ReadConfigFileFramework(const std::string & jsonFileName, const std::string & componentName);

    //! Deploy all monitors and filter pipelines
    /*! Middleware specific: This method should be overriden by derived class
     * because each middleware has different component composition processes.
     */
    virtual bool DeployMonitorsAndFDDs(void) = 0;

    //! Handle event (fault, error, failure)
    /*! Middleware specific: This method should be overriden by derived class
     */
    virtual void OnFaultEvent(const std::string & json) = 0;

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
    // Install filter from JSON
    bool AddFilters(const SF::JSON::JSONVALUE & filters);
    // Install filter from JSON string
    bool AddFilterFromJSON(const std::string & jsonString);
    // Install filter from JSON file
    bool AddFilterFromJSONFile(const std::string & jsonFileName);
    // Install filter from JSON file with target component specified
    bool AddFilterFromJSONFileToComponent(const std::string & jsonFileName,
                                          const std::string & targetComponentName);
    // Install filter using filter instance (Component-based framework-specific)
    virtual bool AddFilter(SF::FilterBase * filter) = 0;
    // Register filter instance to Coordinator
    bool AddFilter(const std::string & componentName, FilterBase * filter);
    // Get information about all the filters installed on the component specified
    const std::string GetFilterList(const std::string & componentName = "*", bool verbose = true) const;
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
    // Add event to specific component using file containing JSON string
    bool AddEventFromJSONFileToComponent(const std::string & jsonFileName,
                                         const std::string & targetComponentName);
    // Given an event name, check if event exists
    bool FindEvent(const std::string & componentName, const std::string & eventName) const;
    bool FindEvent(const std::string & eventName) const;
    // Given an event name, returns event instance
    const Event * GetEvent(const std::string & componentName, const std::string & eventName) const;
    const Event * GetEvent(const std::string & eventName) const;
    // Get information about all the events installed on the component specified
    const std::string GetEventList(const std::string & componentName = "*") const;
    // Called by filter when event is generated.  Event information such as timestamp,
    // location, and severity is encoded in JSON.
    bool OnEvent(const std::string & event);
    // Called by subscriber when service state change is propagated from other component.
    bool OnEventPropagation(const JSON::JSONVALUE & json);
    // TEMP: Coordinator does not have casros accessor and cannot publish messages. As
    // temporary solution, we use pure virtual method for publishing messages.
    virtual bool PublishMessage(Topic::Control::CategoryType category, const std::string & msg) = 0;
    // Called by OnEvent() to inform the derived class (e.g., mtsSafetyCoordinator in case
    // of cisst) of the event
    virtual bool OnEventHandler(const Event * e) = 0;

    //
    // SERVICE STATES
    //
    bool AddServiceStateDependencyFromJSON(const std::string & jsonString);
    bool AddServiceStateDependencyFromJSONFile(const std::string & jsonFileName);
    // Get information about service dependency specifications
    const std::string GetServiceDependencyList(const std::string & componentName = "*", const std::string & prefix = "") const;

    //
    // CONNECTIONS
    //
    bool AddConnection(const std::string & clientSCName, const std::string & clientCompName, const std::string & clientIntfcName,
                       const std::string & serverSCName, const std::string & serverCompName, const std::string & serverIntfcName);
    // Get information about all connections that the component specified is involved with
    const std::string GetConnectionList(const std::string & componentName = "*", const std::string & prefix = "") const;

    //
    // APIs for Users
    //
    // Generate event
    void GenerateEvent(const std::string &     eventName,
                       State::StateMachineType type,
                       const std::string &     what,
                       const std::string &     componentName,
                       const std::string &     interfaceName = "");
    // Get state
    State::StateType GetState(State::StateMachineType type,
                              const std::string & componentName,
                              const std::string & interfaceName) const;

    //! For human-readable logging and debugging
    /*! \param outputStream output stream
        \param includeLocation target location is printed if yes (default: yes)
    */
    virtual void ToStream(std::ostream & outputStream) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const Coordinator & coordinator)
{
    coordinator.ToStream(outputStream);
    return outputStream;
}

};

#endif // _coordinator_h
