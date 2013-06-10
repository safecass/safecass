/*

  Safety Framework for Component-based Robotics

  Created on: July 7, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _event_h
#define _event_h

#include "common.h"
#include "eventLocationBase.h"

namespace SF {

//! Class Event
/** 
    Event defines and implements an event (e.g., faults, errors, failures) with the
    complete description of the event.  Any event represetned by this class can be
    exchanged within the system by json format.  JSONSerializer provides services
    for serialization and deserialization of this class.
*/
class SFLIB_EXPORT Event
{
public:
    typedef enum {
        INVALID,
        FAULT,
        ERROR,
        FAILURE
    } EventType;

    //! Typedef for FAULT events
    typedef enum {
        FAULT_INVALID,           /*!< initial value (invalid) */
        FAULT_COMPONENT_PERIOD,  /*!< predefined: timing fault: OS thread scheduling latency */
        FAULT_COMPONENT_OVERRUN, /*!< predefined: timing fault: thread overrun */
        FAULT_APPLICATION        /*!< user-defined: application faults */
    } FaultType;

    //! Typedef for ERROR events
    typedef enum {
        ERROR_INVALID,           /*!< initial value (invalid) */
        ERROR_APPLICATION        /*!< user-defined: application errors */
    } ErrorType;

    //! Typedef for FAILURE events
    typedef enum {
        FAILURE_INVALID,         /*!< initial value (invalid) */
        FAILURE_APPLICATION      /*!< user-defined: application failures */
    } FailureType;

    //! Typdef for common information
protected:
    //! Type of this event
    const EventType   Type;
    //! Subtype of each event type
    const FaultType   TypeFault;
    const ErrorType   TypeError;
    const FailureType TypeFailure;
    //! Name of this event
    const std::string Name;
    //! Location of this event
    EventLocationBase * Location;
    //! Timestamp of event
    double Timestamp;

    //! Constructors
protected:
    // Event instance should be created with explicit arguments
    Event(void);
public:
    //! Constructor with explicit arguments for FAULT
    explicit Event(EventType           type, 
                   FaultType           typeFault,
                   const std::string & name,
                   EventLocationBase * location,
                   double              timestamp);
    //! Constructor with explicit arguments for ERROR
    explicit Event(EventType           type, 
                   ErrorType           typeError,
                   const std::string & name,
                   EventLocationBase * location,
                   double              timestamp);
    //! Constructor with explicit arguments for FAILURE
    explicit Event(EventType           type, 
                   FailureType         typeFailure,
                   const std::string & name,
                   EventLocationBase * location,
                   double              timestamp);

    /*! \addtogroup Getters
        @{
     */
    inline EventType           GetEventType(void) const   { return Type; }
    inline FaultType           GetFaultType(void) const   { return TypeFault; }
    inline ErrorType           GetErrorType(void) const   { return TypeError; }
    inline FailureType         GetFailureType(void) const { return TypeFailure; }
    inline const std::string & GetName(void) const        { return Name; }
    inline EventLocationBase * GetLocation(void) const    { return Location; }
    inline double              GetTimestamp(void) const   { return Timestamp; }

    static const std::string GetEventTypeString(EventType Type);
    static const std::string GetFaultTypeString(FaultType faultType);

    static EventType GetEventTypeFromString(const std::string & eventString);
    static FaultType GetFaultTypeFromString(const std::string & faultString);
    /*! @} */
};

};

#endif // _event_h
