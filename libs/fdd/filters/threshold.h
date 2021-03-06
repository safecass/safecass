//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Sep 3, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#ifndef _FilterThreshold_h
#define _FilterThreshold_h

#include "filterBase.h"
#include "event.h"

namespace SC {

/*
   Threshold Filter

   Given constant threshold T and tolerance t,

      Output Y = Y1 if X > (T + t)
                 Y0 otherwise

   where Y1 and Y0 are two const values for outputs.  All inputs and outputs are scalar.
*/

class SCLIB_EXPORT FilterThreshold: public FilterBase
{
public:
    typedef enum { BELOW_THRESHOLD, ABOVE_THRESHOLD } EVENT_TYPE;

protected:
    // Filter should be instantiated with explicit arguments
    FilterThreshold(void);

    void Initialize(void);

    const std::string GenerateEventInfo(EVENT_TYPE eventType) const;

    //--------------------------------------------------
    //  Filter-specific parameters
    //--------------------------------------------------
    //! Name of input signal
    const std::string NameOfInputSignal;
    //! Threshold
    SignalElement::ScalarType Threshold;
    //! Threshold tolerance
    SignalElement::ScalarType Tolerance;
    //! Output when input exceeds threshold by more than margin of tolerance
    SignalElement::ScalarType OutputAbove;
    //! Output when input does not exceed threshold with margin of tolerance
    SignalElement::ScalarType OutputBelow;
    //! Names of events generated
    std::string EventNameAbove;
    std::string EventNameBelow;

    //--------------------------------------------------
    //  Methods required by the base class
    //--------------------------------------------------
    bool ConfigureFilter(const JsonWrapper::JsonValue & jsonNode);
    bool InitFilter(void);
    void RunFilter(void); //< Implements filtering algorithm
    void CleanupFilter(void);

public:
    //! Constructor with explicit arguments
    // Constructor to deploy filter to interface (only component name required)
    FilterThreshold(FilterBase::FilteringType monitoringType,
                    State::StateMachineType   targetStateMachineType,
                    const std::string &       targetComponentName,
                    const std::string &       inputSignalName,
                    SignalElement::ScalarType threshold,
                    SignalElement::ScalarType tolerance,
                    SignalElement::ScalarType outputBelow,
                    SignalElement::ScalarType outputAbove,
                    const std::string &       eventNameAbove,
                    const std::string &       eventNameBelow);
    // Constructor to deploy filter to interface (component name and
    // interface name are required)
    FilterThreshold(FilterBase::FilteringType monitoringType,
                    State::StateMachineType   targetStateMachineType,
                    const std::string &       targetComponentName,
                    const std::string &       targetInterfaceName, // additional param
                    const std::string &       inputSignalName,
                    SignalElement::ScalarType threshold,
                    SignalElement::ScalarType tolerance,
                    SignalElement::ScalarType outputBelow,
                    SignalElement::ScalarType outputAbove,
                    const std::string &       eventNameAbove,
                    const std::string &       eventNameBelow);
    //! Constructor using JSON
    FilterThreshold(const JsonWrapper::JsonValue & jsonNode);
    //! Destructor
    ~FilterThreshold();

    //! Getters
    inline const std::string & GetNameOfInputSignal(void) const { return NameOfInputSignal; }
    inline SignalElement::ScalarType GetThreshold(void) const   { return Threshold; }
    inline SignalElement::ScalarType GetTolerance(void) const   { return Tolerance; }
    inline SignalElement::ScalarType GetOutputBelow(void) const { return OutputBelow; }
    inline SignalElement::ScalarType GetOutputAbove(void) const { return OutputAbove; }
    inline const std::string & GetEventNameAbove(void) const    { return EventNameAbove; }
    inline const std::string & GetEventNameBelow(void) const    { return EventNameBelow; }

    /*! Returns human readable representation of this filter */
    void ToStream(std::ostream & outputStream, bool verbose = false) const;

    //! For filter factory
    SC_DEFINE_FACTORY_CREATE(FilterThreshold);
};

};

#endif // _FilterThreshold_h
