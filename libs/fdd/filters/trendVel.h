/*

  Safety Framework for Component-based Robotics

  Created on: October 8, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _FilterTrendVel_h
#define _FilterTrendVel_h

#include "filterBase.h"

namespace SF {

/* 
   Trend Velocity Filter (1st-order differentiator)

   Output Y = (x(t(k+1)) - x(t(k))) / (t(k+1) - t(k))
*/

class SFLIB_EXPORT FilterTrendVel: public FilterBase
{
protected:
    // Filter should be instantiated with explicit arguments
    FilterTrendVel();

    //--------------------------------------------------
    //  Filter-specific parameters
    //--------------------------------------------------
    /*! Name of input signal */
    std::string NameOfInputSignal;
    /*! Input(output) signal type */
    SignalElement::SignalType SignalType;
    /*! time scaling.  If enabled, output -- difference of the current and the last input -- is
        divided by actual time difference of the two samples */
    bool TimeScaling;

    /*! Local cache of previous value */
    // MJ: Could use separate API that fetches value history of length 1
    struct {
        bool Initialized; // to mask first output as first input (otherwise, first output becomes invalid)
        TimestampType Timestamp;
        SignalElement::ScalarType Scalar;
        SignalElement::VectorType Vector;
    } PreviousValue;

public:
    FilterTrendVel(BaseType::FilterCategory      category, 
                   const std::string &           targetComponentName,
                   SF::FilterBase::FilteringType monitoringType,
                   // below are filter-specific arguments
                   const std::string &           inputSignalName,
                   SignalElement::SignalType     signalType,
                   bool                          timeScaling);
    ~FilterTrendVel();

    /*! Implements thresholding algorithm */
    void DoFiltering(void);

    /*! Getters */
    inline const std::string & GetNameOfInputSignal(void) const { return NameOfInputSignal; }
    inline SignalElement::SignalType GetSignalType(void) const { return SignalType; }

    /*! Returns human readable representation of this filter */
    void ToStream(std::ostream & outputStream) const;

    /*! Name of this filter */
    static const std::string Name;
};

};

#endif // _FilterTrendVel_h
