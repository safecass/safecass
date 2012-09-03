/*

  Safety Framework for Component-based Robotics

  Created on: September 3, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _FilterThreshold_h
#define _FilterThreshold_h

#include "filterBase.h"

namespace SF {

/* 
   Threshold Filter

   Given constant threshold T and two constant output values Y1 and Y0,
   
      Output Y = Y1 if X >= T
                 Y0 otherwise

    X should be scalar and thus Y is scalar as well.
*/

class SFLIB_EXPORT FilterThreshold: public FilterBase
{
protected:
    // Filter should be created with explicit arguments
    FilterThreshold();

    /*! Threshold */
    SignalElement::ScalarType Threshold;
    /*! Output when input exceeds threshold specified */
    SignalElement::ScalarType Output0;
    /*! Output when input does not exceed threshold specified */
    SignalElement::ScalarType Output1;

public:
    FilterThreshold(BaseType::FilterCategory category, 
                    const std::string & inputName,
                    SignalElement::ScalarType threshold,
                    SignalElement::ScalarType output0,
                    SignalElement::ScalarType output1);
    ~FilterThreshold();

    /*! Implements thresholding algorithm */
    void DoFiltering(bool debug);

    /*! Returns human readable representation of this filter */
    void ToStream(std::ostream & outputStream) const;
};

};

#endif // _FilterThreshold_h
