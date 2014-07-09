//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 6, 2014
// Last revision: Jul 8, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _FilterChangeDetection_h
#define _FilterChangeDetection_h

#include "filterBase.h"

namespace SF {

/* 
   Change detection filter

   This filter detects if the current input is different from its previous value.

   Input : x(t) (a double scalar)

   Output: 1 if x(t) is different from x(t-1), 0 otherwise
*/

class SFLIB_EXPORT FilterChangeDetection: public FilterBase
{
protected:
    // Filter should be instantiated with explicit arguments
    FilterChangeDetection();

    void Initialize(void);

    //--------------------------------------------------
    //  Filter-specific parameters
    //--------------------------------------------------
    // TODO: These could be moved down to the base class..?
    /*! Name of input signal */
    const std::string NameOfInputSignal;
    // Name of output event
    std::string EventName;

    /*! Local cache of the last value */
    SignalElement::ScalarType LastValue;

    //-------------------------------------------------- 
    //  Methods required by the base class
    //-------------------------------------------------- 
    bool ConfigureFilter(const JSON::JSONVALUE & jsonNode);
    bool InitFilter(void);
    void RunFilter(void); //< Implements filtering algorithm to detect change
    void CleanupFilter(void);

public:
    //! Default constructor
    FilterChangeDetection(const std::string &             targetComponentName,
                          const FilterBase::FilteringType monitoringType,
                          // below are filter-specific arguments
                          const std::string &             inputSignalName);
    //! Constructor using JSON
    FilterChangeDetection(const JSON::JSONVALUE & jsonNode);

    //! Destructor
    ~FilterChangeDetection();

    /*! Getters */
    inline const std::string & GetNameOfInputSignal(void) const { return NameOfInputSignal; }
    // TODO: this filter only supports scalar-type input/output for now.
    inline SignalElement::SignalType GetSignalType(void) const  { return SignalElement::SCALAR; }

    /*! Returns human readable representation of this filter */
    void ToStream(std::ostream & outputStream) const;

    //! For filter factory
    SF_DEFINE_FACTORY_CREATE(FilterChangeDetection);
};

};

#endif // _FilterChangeDetection_h
