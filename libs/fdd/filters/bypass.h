//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jan 7, 2012
// Last revision: Aug 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _FilterBypass_h
#define _FilterBypass_h

#include "filterBase.h"
#include "event.h"

namespace SC {

/* 
   Bypass Filter

   Output Y = X

    X can be either scalar or vector.
    Y is determined by the type of X.
*/

class SCLIB_EXPORT FilterBypass : public FilterBase
{
protected:
    // Filter should be instantiated with explicit arguments
    FilterBypass(void);

    void Initialize(void);

    const std::string GenerateEventInfo(EVENT_TYPE eventType) const;

    //--------------------------------------------------
    //  Filter-specific parameters
    //--------------------------------------------------
    /*! Name of input signal */
    const std::string NameOfInputSignal;

    //-------------------------------------------------- 
    //  Methods required by the base class
    //-------------------------------------------------- 
    bool ConfigureFilter(const JSON::JSONVALUE & jsonNode);
    bool InitFilter(void);
    void RunFilter(void); //< Implements filtering algorithm
    void CleanupFilter(void);

public:
    //! Constructor
    FilterBypass(FilterBase::FilteringType monitoringType,
                 State::StateMachineType   targetStateMachineType,
                 const std::string &       targetComponentName,
                 const std::string &       targetInterfaceName,
                 const std::string &       inputSignalName);
    //! Constructor using JSON
    FilterBypass(const JSON::JSONVALUE & jsonNode);
    //! Destructor
    ~FilterBypass();

    /*! Getters */
    inline const std::string & GetNameOfInputSignal(void) const { return NameOfInputSignal; }
    //inline SignalElement::SignalType GetSignalType(void) const  { return SignalElement::SCALAR; }

    /*! Returns human readable representation of this filter */
    void ToStream(std::ostream & outputStream, bool verbose = false) const;

    //! For filter factory
    SC_DEFINE_FACTORY_CREATE(FilterBypass);
};

};

#endif // _FilterBasics_h

