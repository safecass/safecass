//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Sep 12, 2012
// Last revision: Aug 11, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _HistoryBufferBase_h
#define _HistoryBufferBase_h

#include "signal.h"

namespace SF {

class SFLIB_EXPORT HistoryBufferBase
{
public:
    HistoryBufferBase(void) {}
    virtual ~HistoryBufferBase() {}

    //! For active filtering
    /*! Get latest value from history buffer (direct access)
     */
    virtual void GetNewValueScalar(SignalElement::HistoryBufferIndexType index,
                                   SignalElement::ScalarType & value,
                                   TimestampType & timestamp) = 0;
    virtual void GetNewValueVector(SignalElement::HistoryBufferIndexType index,
                                   SignalElement::VectorType & value,
                                   TimestampType & timestamp) = 0;

    //! For passive filtering
    /*! Fetch latest value from history buffer via middleware-specific data 
     *  exchange mechanism (indirect access)
     */
    virtual void GetNewValueScalar(SignalElement::ScalarType & value,
                                   TimestampType & timestamp) = 0;
    virtual void GetNewValueVector(SignalElement::VectorType & value,
                                   TimestampType & timestamp) = 0;

    // Support for "deep" fault inject: modify actual values in the buffer
    virtual void PushNewValueScalar(SignalElement::HistoryBufferIndexType index,
                                    SignalElement::ScalarType & value) = 0;
};

};

#endif // _HistoryBufferBase_h
