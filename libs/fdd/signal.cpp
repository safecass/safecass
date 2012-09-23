/*

  Safety Framework for Component-based Robotics

  Created on: January 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "signal.h"
#include "historyBufferBase.h"

namespace SF {

const SignalElement::HistoryBufferIndexType SignalElement::INVALID_HISTORY_BUFFER_INDEX = -1;

SignalElement::SignalElement()
    : Name("NONAME"),
      Type(SCALAR),
      HistoryBuffer(0),
      HistoryBufferIndex(INVALID_HISTORY_BUFFER_INDEX)
{
    Init();
}

SignalElement::SignalElement(const std::string &       signalName, 
                             SignalElement::SignalType signalType)
    : Name(signalName),
      Type(signalType),
      HistoryBuffer(0),
      HistoryBufferIndex(INVALID_HISTORY_BUFFER_INDEX)
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

    TimeLastSampleFetched =
#ifdef SF_HAS_CISST
        0.0;
#else
        0; // different middlewares may use different types to represent timestamp
#endif
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
        SFLOG_ERROR << "FetchNewValueScalar: Failed to fetch new value - NULL history buffer" << std::endl;
        PlaceholderScalar = 0.0;
        TimeLastSampleFetched = 0.0;
        return false;
    }

    if (activeFiltering)
        HistoryBuffer->GetNewValueScalar(HistoryBufferIndex, PlaceholderScalar, TimeLastSampleFetched);
    else
        HistoryBuffer->GetNewValueScalar(PlaceholderScalar, TimeLastSampleFetched);

    return true;
}

bool SignalElement::FetchNewValueVector(bool activeFiltering)
{
    if (!HistoryBuffer) {
        SFLOG_ERROR << "FetchNewValueVector: Failed to fetch new value - NULL history buffer" << std::endl;
        PlaceholderVector.clear();
        TimeLastSampleFetched = 0.0;
        return false;
    }

    if (activeFiltering)
        HistoryBuffer->GetNewValueScalar(HistoryBufferIndex, PlaceholderScalar, TimeLastSampleFetched);
    else
        HistoryBuffer->GetNewValueScalar(PlaceholderScalar, TimeLastSampleFetched);

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
        outputStream << "Values: ";
        for (size_t i = 0; i < PlaceholderVector.size(); ++i) {
             outputStream << PlaceholderVector[i] << ", ";
        }
    }
    outputStream << "Timestamp: " << TimeLastSampleFetched;
}

};
