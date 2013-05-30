/*

  Safety Framework for Component-based Robotics

  Created on: September 2, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

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
                                   SignalElement::TimestampType & timestamp) = 0;
    virtual void GetNewValueVector(SignalElement::HistoryBufferIndexType index,
                                   SignalElement::VectorType & value,
                                   SignalElement::TimestampType & timestamp) = 0;

    //! For passive filtering
    /*! Fetch latest value from history buffer via middleware-specific data 
     *  exchange mechanism (indirect access)
     */
    virtual void GetNewValueScalar(SignalElement::ScalarType & value,
                                   SignalElement::TimestampType & timestamp) = 0;
    virtual void GetNewValueVector(SignalElement::VectorType & value,
                                   SignalElement::TimestampType & timestamp) = 0;
};

};

#endif // _HistoryBufferBase_h
