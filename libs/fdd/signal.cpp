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
      ActiveFiltering(true),
      HistoryBuffer(0),
      HistoryBufferIndex(INVALID_HISTORY_BUFFER_INDEX)
{
    Init();
}

SignalElement::SignalElement(const std::string &       signalName, 
                             SignalElement::SignalType signalType, 
                             bool                      activeFiltering)
    : Name(signalName),
      Type(signalType),
      ActiveFiltering(activeFiltering),
      HistoryBuffer(0),
      HistoryBufferIndex(INVALID_HISTORY_BUFFER_INDEX)
{
    Init();
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

bool SignalElement::FetchNewValueScalar(void)
{
    if (!HistoryBuffer) {
        SFLOG_ERROR << "FetchNewValueScalar: Failed to fetch new value - NULL history buffer" << std::endl;
        PlaceholderScalar = 0.0;
        TimeLastSampleFetched = 0.0;
        return false;
    }

    HistoryBuffer->GetNewValueScalar(HistoryBufferIndex, PlaceholderScalar, TimeLastSampleFetched);

    return true;
}

bool SignalElement::FetchNewValueVector(void)
{
    if (!HistoryBuffer) {
        SFLOG_ERROR << "FetchNewValueVector: Failed to fetch new value - NULL history buffer" << std::endl;
        PlaceholderVector.clear();
        TimeLastSampleFetched = 0.0;
        return false;
    }

    HistoryBuffer->GetNewValueScalar(HistoryBufferIndex, PlaceholderScalar, TimeLastSampleFetched);

    return true;
}

};
