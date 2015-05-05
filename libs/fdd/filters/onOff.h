//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jun 30, 2014
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#ifndef _FilterOnOff_h
#define _FilterOnOff_h

#include "filterBase.h"
#include "event.h"

namespace SC {

/*
   On/off (1-bit change) detection filter

   This filter detects if the input bit is zero or non-zero.

   Input : x(t) (double scalar)

   Output: 1  if x(t) is different from x(t-1) and x(t) is non-zero
           0  otherwise

   If output is non-zero, i.e., change in the input is detected, an event associated
   with each case is generated via the event publisher.
*/

class SCLIB_EXPORT FilterOnOff: public FilterBase
{
public:
    typedef enum { ONSET, OFFSET } EVENT_TYPE;

protected:
    // Filter should be instantiated with explicit arguments
    FilterOnOff(void);

    void Initialize(void);

    const std::string GenerateEventInfo(EVENT_TYPE eventType) const;

    //--------------------------------------------------
    //  Filter-specific parameters
    //--------------------------------------------------
    /*! Name of input signal */
    const std::string NameOfInputSignal;

    // Local cache of the last value. Note the type is integer, not double.
    int LastValue;

    // Name of output event for event onset.  Defined by JSON.
    std::string EventNameOn;
    // Name of output event for event offset.  Defined by JSON.
    std::string EventNameOff;

    //-------------------------------------------------- 
    //  Methods required by the base class
    //-------------------------------------------------- 
    bool ConfigureFilter(const JsonWrapper::JsonValue & jsonNode);
    bool InitFilter(void);
    void RunFilter(void); //< Implements filtering algorithm
    void CleanupFilter(void);

public:
    //! Constructor
    FilterOnOff(FilterBase::FilteringType monitoringType,
                State::StateMachineType   targetStateMachineType,
                const std::string &       targetComponentName,
                const std::string &       targetInterfaceName,
                const std::string &       inputSignalName);
    //! Constructor using JSON
    FilterOnOff(const JsonWrapper::JsonValue & jsonNode);
    //! Destructor
    ~FilterOnOff();

    /*! Getters */
    inline const std::string & GetNameOfInputSignal(void) const { return NameOfInputSignal; }
    inline SignalElement::SignalType GetSignalType(void) const  { return SignalElement::SCALAR; }

    /*! Returns human readable representation of this filter */
    void ToStream(std::ostream & outputStream, bool verbose = false) const;

    //! For filter factory
    SC_DEFINE_FACTORY_CREATE(FilterOnOff);
};

};

#endif // _FilterOnOff_h
