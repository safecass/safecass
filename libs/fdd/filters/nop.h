//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Aug 20, 2014
// Last revision: Aug 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _FilterNOP_h
#define _FilterNOP_h

#include "filterBase.h"
#include "event.h"

namespace SF {

/* 
   No-operation (NOP) filter

   This filter does nothing and is inteded to be used for fault injection.

   Input : x(t) (scalar or vector)

   Output: none
*/

class SFLIB_EXPORT FilterNOP: public FilterBase
{
protected:
    // Filter should be instantiated with explicit arguments
    FilterNOP(void);

    void Initialize(void);

    //--------------------------------------------------
    //  Filter-specific parameters
    //--------------------------------------------------
    // Name of input signal
    const std::string NameOfInputSignal;

    // Input signal type
    SignalElement::SignalType InputSignalType;

    //-------------------------------------------------- 
    //  Methods required by the base class
    //-------------------------------------------------- 
    bool ConfigureFilter(const JSON::JSONVALUE & json);
    bool InitFilter(void);
    void RunFilter(void);
    void CleanupFilter(void) {}

public:
    FilterNOP(const std::string &       targetComponentName, 
              const std::string &       inputSignalName,
              SignalElement::SignalType inputSignalType);
    FilterNOP(const JSON::JSONVALUE & json);
    ~FilterNOP();

    inline const std::string & GetNameOfInputSignal(void) const { return NameOfInputSignal; }
    inline SignalElement::SignalType GetSignalType(void) const  { return InputSignalType; }

    SF_DEFINE_FACTORY_CREATE(FilterNOP);
};

};

#endif // _FilterNOP_h
