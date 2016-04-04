//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jan 7, 2012
// Last revision: Mar 31, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//

#include "safecass/signalElement.h"

using namespace SC;

SignalElement::SignalElement(void)
    : Name("UNNAMED"),
      ParamPrototype(_ParamPrototypeDummy),
      HistoryBufferInstance(0),
      SignalIndex(HistoryBufferBase::INVALID_HISTORY_BUFFER_INDEX),
      TimeLastSampleFetched(0.0)
{
}

SignalElement::SignalElement(const std::string & signalName,
                             ParamBase & paramType,
                             HistoryBufferBase * historyBuffer)
    : Name(signalName),
      ParamPrototype(paramType),
      HistoryBufferInstance(historyBuffer),
      SignalIndex(HistoryBufferBase::INVALID_HISTORY_BUFFER_INDEX),
      TimeLastSampleFetched(0.0)
{
}

SignalElement::~SignalElement()
{
    //
    // FIXME this may not be the case anymore..?
    //
    // Each signal element has an instance of a derived class of HistoryBufferBase which 
    // is created dynamically and thus should be clean up here.
}

#if 0
void SignalElement::SetHistoryBufferInstance(HistoryBufferBase * buffer)
{
    if (!buffer) return;
    if (HistoryBuffer)
        delete HistoryBuffer;

    HistoryBuffer = buffer;
}
#endif

//
// FIXME
//
bool SignalElement::FetchNewValue(void) const
{
    /*
    if (!HistoryBuffer) {
        SCLOG_ERROR << "FetchNewValueScalar: Failed to fetch new value - NULL history buffer" << std::endl;
        PlaceholderScalar = 0.0;
        TimeLastSampleFetched = 0.0;
        return false;
    }

    // FIXME Restore the following code when handling internal vs. external filtering
#if 0
    if (activeFiltering)
        HistoryBuffer->GetNewValueScalar(HistoryBufferIndex, PlaceholderScalar, TimeLastSampleFetched);
    else
        HistoryBuffer->GetNewValueScalar(PlaceholderScalar, TimeLastSampleFetched);
#endif

    // TEMP: Should review which timestamp (now or elapsed time from origin
    // in case of cisst) must be used.
    TimeLastSampleFetched = GetCurrentTimeTick();
    */

    return true;
}

void SignalElement::ToStream(std::ostream & os) const
{
    os << "Signal: "
       << "\"" << Name << "\", "
       << "history buffer: " << (HistoryBufferInstance == 0 ? "n/a" : "OK") << ", "
       << "signal index: " << SignalIndex << ", "
       << "last sample time: " << TimeLastSampleFetched << ", "
       << "parameter prototype: " << ParamPrototype;
}

void SignalElement::PushNewValue(const ParamBase & paramType)
{
    //
    // FIXME
    //
    std::cout << paramType << std::endl;
}

/*
void SignalElement::PushNewValueScalar(ScalarType value)
{
    SCASSERT(HistoryBuffer);
    // FIXME Restore this code when handling deep fault injection
#if 0
    HistoryBuffer->PushNewValueScalar(HistoryBufferIndex, value);
#endif
}

void SignalElement::PushNewValueVector(const VectorType & value)
{
    SCASSERT(HistoryBuffer);
    // FIXME Restore this code when handling deep fault injection
#if 0
    HistoryBuffer->PushNewValueVector(HistoryBufferIndex, value);
#endif
}
*/
