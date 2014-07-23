//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jun 30, 2014
// Last revision: Jul 8, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _FilterOnOff_h
#define _FilterOnOff_h

#include "filterBase.h"
#include "event.h"

namespace SF {

/* 
   On/off (1-bit change) detection filter

   This filter detects if the input bit is zero or non-zero.

   Input : x(t) (double scalar)

   Output: 1  if x(t) is different from x(t-1) and x(t) is non-zero
           0  otherwise

   If output is non-zero, i.e., change in the input is detected, an event associated
   with each case is generated via the event publisher.
*/

class SFLIB_EXPORT FilterOnOff: public FilterBase
{
public:
    typedef enum { ONSET, OFFSET } EVENT_TYPE;

protected:
    // Filter should be instantiated with explicit arguments
    FilterOnOff();

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
    bool ConfigureFilter(const JSON::JSONVALUE & jsonNode);
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
    FilterOnOff(const JSON::JSONVALUE & jsonNode);
    //! Destructor
    ~FilterOnOff();

    /*! Getters */
    inline const std::string & GetNameOfInputSignal(void) const { return NameOfInputSignal; }
    inline SignalElement::SignalType GetSignalType(void) const  { return SignalElement::SCALAR; }

    /*! Returns human readable representation of this filter */
    void ToStream(std::ostream & outputStream, bool verbose = false) const;

    //! For filter factory
    SF_DEFINE_FACTORY_CREATE(FilterOnOff);
};

};

#endif // _FilterOnOff_h
