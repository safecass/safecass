/*

  Safety Framework for Component-based Robotics

  Created on: July 14, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _coordinator_h
#define _coordinator_h

#include "common.h"
#include "monitor.h"

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

protected:
    // Monitor map
    //! Map of monitoring targets
    MonitorTargetMapType MonitorTargetMap;

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

public:
    //! Constructor
    Coordinator();
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
    //virtual void ToStream(std::ostream & outputStream) const;
};

#if 0
inline std::ostream & operator << (std::ostream & outputStream, const Coordinator & coordinator) {
    coordinator.ToStream(outputStream);
    return outputStream;
}
#endif

};

#endif // _coordinator_h
