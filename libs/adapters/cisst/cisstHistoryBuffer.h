/*

  Safety Framework for Component-based Robotics

  Created on: September 2, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _cisstHistoryBufferBase_h
#define _cisstHistoryBufferBase_h


#include <cisstMultiTask/mtsStateTable.h>

namespace SF {

class SFLIB_EXPORT cisstHistoryBuffer: public HistoryBufferBase
{
protected:
    mtsStateTable * StateTable;

public:
    cisstHistoryBuffer(mtsStateTable * stateTable);
    virtual ~cisstHistoryBuffer() {}

    /*! Fetch latest scalar-type value from history buffer */
    void GetNewValueScalar(SignalElement::HistoryBufferIndexType index,
                           SignalElement::ScalarType & value,
                           SignalElement::TimestampType & timestamp);

    /*! Fetch latest vector-type value from history buffer */
    void GetNewValueVector(SignalElement::HistoryBufferIndexType index,
                           SignalElement::VectorType & value,
                           SignalElement::TimestampType & timestamp);
};

};

#endif // _cisstHistoryBufferBase_h
