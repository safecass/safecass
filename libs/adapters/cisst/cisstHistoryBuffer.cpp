/*

  Safety Framework for Component-based Robotics

  Created on: September 2, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "cisstHistoryBuffer.h"

namespace SF {

cisstHistoryBuffer::cisstHistoryBuffer(mtsStateTable * stateTable)
    : StateTable(stateTable)
{
    CMN_ASSERT(StateTable);
}

void cisstHistoryBuffer::GetNewValueScalar(SignalElement::HistoryBufferIndexType index,
                                           SignalElement::ScalarType & value,
                                           SignalElement::TimestampType & timestamp)
{
    CMN_ASSERT(StateTable); // MJ TEMP
    value = StateTable->GetNewValueScalar(index, timestamp);
}

void cisstHistoryBuffer::GetNewValueVector(SignalElement::HistoryBufferIndexType index,
                                           SignalElement::VectorType & value,
                                           SignalElement::TimestampType & timestamp)
{
    CMN_ASSERT(StateTable); // MJ TEMP
    value = StateTable->GetNewValueVector(index, timestamp);
}

};
