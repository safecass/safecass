//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Aug 20, 2014
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#ifndef _FilterNOP_h
#define _FilterNOP_h

#include "filterBase.h"

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
    bool ConfigureFilter(const JsonWrapper::JsonValue & UNUSED(json));
    bool InitFilter(void);
    void RunFilter(void);
    void CleanupFilter(void) {}

public:
    FilterNOP(const std::string &       targetComponentName,
              const std::string &       inputSignalName,
              SignalElement::SignalType inputSignalType);
    FilterNOP(const JsonWrapper::JsonValue & json);
    ~FilterNOP();

    inline const std::string & GetNameOfInputSignal(void) const { return NameOfInputSignal; }
    inline SignalElement::SignalType GetSignalType(void) const  { return InputSignalType; }

    SF_DEFINE_FACTORY_CREATE(FilterNOP);
};

};

#endif // _FilterNOP_h
