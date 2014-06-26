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

#include <map>

namespace SF {

// TODO: casros accessor is not included here.  If accessor is implemented without cisst,
// it should be moved to this base class.

// Adapter class: almost empty but pure virtual methods
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

    // Container to manage the entire set of states of the current process
    typedef std::map<unsigned int, GCM*> GCMMapType;

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

    // Container to manage the entire set of states of the current process
    GCMMapType MapGCM;

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

    // Get state information in its entirety
    const std::string GetStateSnapshot(const std::string & componentName = "*") const;
};

inline std::ostream & operator << (std::ostream & outputStream, const Coordinator & coordinator)
{
    coordinator.ToStream(outputStream);
    return outputStream;
}

};

#endif // _coordinator_h
