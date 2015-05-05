//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
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
#include "signal.h"
#include "historyBufferBase.h"

using namespace SC;

const SignalElement::HistoryBufferIndexType SignalElement::INVALID_HISTORY_BUFFER_INDEX = -1;

SignalElement::SignalElement(void)
    : Name("NONAME"),
      Type(SCALAR),
      HistoryBuffer(0),
      HistoryBufferIndex(INVALID_HISTORY_BUFFER_INDEX),
      TimeLastSampleFetched(0.0),
      PlaceholderScalar(0.0)
{
    Init();
}

SignalElement::SignalElement(const std::string & signalName, 
                             const SignalElement::SignalType signalType)
    : Name(signalName),
      Type(signalType),
      HistoryBuffer(0),
      HistoryBufferIndex(INVALID_HISTORY_BUFFER_INDEX),
      TimeLastSampleFetched(0.0),
      PlaceholderScalar(0.0)
{
    Init();
}

SignalElement::~SignalElement()
{
    // Each signal element has an instance of a derived class of HistoryBufferBase which 
    // is created dynamically and thus should be clean up here.
}

void SignalElement::Init(void)
{
    PlaceholderScalar = 0.0;
    PlaceholderVector.clear();

    // TODO: different middlewares may use different types to represent timestamp
    TimeLastSampleFetched = 0.0;
}

void SignalElement::SetHistoryBufferInstance(HistoryBufferBase * buffer)
{
    if (!buffer) return;
    if (HistoryBuffer)
        delete HistoryBuffer;

    HistoryBuffer = buffer;
}

bool SignalElement::FetchNewValueScalar(bool activeFiltering)
{
    if (!HistoryBuffer) {
        SCLOG_ERROR << "FetchNewValueScalar: Failed to fetch new value - NULL history buffer" << std::endl;
        PlaceholderScalar = 0.0;
        TimeLastSampleFetched = 0.0;
        return false;
    }

    if (activeFiltering)
        HistoryBuffer->GetNewValueScalar(HistoryBufferIndex, PlaceholderScalar, TimeLastSampleFetched);
    else
        HistoryBuffer->GetNewValueScalar(PlaceholderScalar, TimeLastSampleFetched);

    // TEMP: Should review which timestamp (now or elapsed time from origin
    // in case of cisst) must be used.
    TimeLastSampleFetched = GetCurrentTimeTick();

    return true;
}

bool SignalElement::FetchNewValueVector(bool activeFiltering)
{
    if (!HistoryBuffer) {
        SCLOG_ERROR << "FetchNewValueVector: Failed to fetch new value - NULL history buffer" << std::endl;
        PlaceholderVector.clear();
        TimeLastSampleFetched = 0.0;
        return false;
    }

    if (activeFiltering)
        HistoryBuffer->GetNewValueVector(HistoryBufferIndex, PlaceholderVector, TimeLastSampleFetched);
    else
        HistoryBuffer->GetNewValueVector(PlaceholderVector, TimeLastSampleFetched);

    return true;
}

void SignalElement::ToStream(std::ostream & outputStream) const
{
    outputStream << "\"" << Name << "\", "
                 << "Type: " << (Type == SCALAR ? "SCALAR" : "VECTOR") << ", "
                 << "History buffer: " << (HistoryBuffer == 0 ? "Unavailable" : "Available") << ", "
                 << "History buffer index: " << HistoryBufferIndex << ", ";
    if (Type == SCALAR) {
        outputStream << "Value: " << PlaceholderScalar << ", ";
    } else {
        outputStream << "Values: [ ";
        for (size_t i = 0; i < PlaceholderVector.size(); ++i) {
             outputStream << PlaceholderVector[i] << ", ";
        }
        std::cout << " ], ";
    }
    outputStream << "Timestamp: " << TimeLastSampleFetched;
}

void SignalElement::PushNewValueScalar(ScalarType value)
{
    SCASSERT(HistoryBuffer);
    HistoryBuffer->PushNewValueScalar(HistoryBufferIndex, value);
}

void SignalElement::PushNewValueVector(const VectorType & value)
{
    SCASSERT(HistoryBuffer);
    HistoryBuffer->PushNewValueVector(HistoryBufferIndex, value);
}
