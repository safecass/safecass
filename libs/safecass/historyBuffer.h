//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Mar 15, 2016
// Last revision: Mar 23, 2016
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/safecass/safecass
//
#ifndef _HistoryBuffer_h
#define _HistoryBuffer_h

#include "historyBufferBase.h"
#include <boost/lockfree/spsc_queue.hpp>

namespace SC {

class SCLIB_EXPORT HistoryBuffer: public HistoryBufferBase
{
public:
    typedef HistoryBufferBase BaseType;

protected:
    boost::lockfree::spsc_queue<int> * Queue;

public:
    HistoryBuffer(void);
    virtual ~HistoryBuffer();

    //
    // Methods required by base class
    //
    // For active filtering
    void GetNewValueScalar(SignalElement::HistoryBufferIndexType index,
                           SignalElement::ScalarType & value,
                           TimestampType & timestamp) {}
    void GetNewValueVector(SignalElement::HistoryBufferIndexType index,
                           SignalElement::VectorType & value,
                           TimestampType & timestamp) {}

    // For passive filtering
    void GetNewValueScalar(SignalElement::ScalarType & value, TimestampType & timestamp) {}
    void GetNewValueVector(SignalElement::VectorType & value, TimestampType & timestamp) {}

    // For deep fault injection support
    void PushNewValueScalar(SignalElement::HistoryBufferIndexType index,
                            SignalElement::ScalarType & value) {}
    void PushNewValueVector(SignalElement::HistoryBufferIndexType index,
                            const SignalElement::VectorType & value) {}
};



};

#endif // _HistoryBufferBase_h
