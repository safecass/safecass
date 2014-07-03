//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jun 30, 2014
// Last revision: Jun 30, 2014
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
           -1 if x(t) is different from x(t-1) and x(t) is zero
           0  otherwise

   If output is non-zero, i.e., change in the input is detected, an event associated
   with each case is generated via the event publisher.
*/

class SFLIB_EXPORT FilterOnOff: public FilterBase
{
protected:
    // Filter should be instantiated with explicit arguments
    FilterOnOff();

    //--------------------------------------------------
    //  Filter-specific parameters
    //--------------------------------------------------
    /*! Name of input signal */
    const std::string NameOfInputSignal;

    /*! Local cache of the last value */
    SignalElement::ScalarType LastValue;

    // Output when event is on.  Allocated by ConfigureFilter() via JSON
    std::string EventNameOn;
    std::string EventNameOff;

    // Output when event becomes off

    /*! If this filter is initialized */
    bool Initialized;

    /*! Initialize this filter */
    void Initialize(void);

    //-------------------------------------------------- 
    //  Methods required by the base class
    //-------------------------------------------------- 
    bool ConfigureFilter(const JSON::JSONVALUE & jsonNode);
    bool InitFilter(void);
    void RunFilter(void); //< Implements filtering algorithm
    void CleanupFilter(void);

public:
    //! Default constructor
    FilterOnOff(const std::string &             targetComponentName,
                const FilterBase::FilteringType monitoringType,
                // below are filter-specific arguments
                const std::string &             inputSignalName);
    //! Constructor using JSON
    FilterOnOff(const JSON::JSONVALUE & jsonNode);

    //! Destructor
    ~FilterOnOff();

    /*! Getters */
    inline const std::string & GetNameOfInputSignal(void) const { return NameOfInputSignal; }
    inline SignalElement::SignalType GetSignalType(void) const  { return SignalElement::SCALAR; }

    /*! Returns human readable representation of this filter */
    void ToStream(std::ostream & outputStream) const;

    //! For filter factory
    SF_DEFINE_FACTORY_CREATE(FilterOnOff);
};

};

#endif // _FilterOnOff_h
