/*

  Safety Framework for Component-based Robotics

  Created on: January 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _FilterBypass_h
#define _FilterBypass_h

#include "filterBase.h"

namespace SF {

/* 
   Bypass Filter

   Output Y = X

    X can be either scalar or vector.
    Y is determined by the type of X.
*/

class SFLIB_EXPORT FilterBypass : public FilterBase
{
protected:
    SignalElement::SignalType InputType;

    // Filter should be created with explicit arguments
    FilterBypass();

public:
    FilterBypass(BaseType::FilterCategory  category, 
                 const std::string &       inputName,
                 SignalElement::SignalType inputType,
                 HistoryBufferBase * historyBuffer);
    ~FilterBypass();

    /*! Implements bypass algorithm */
    void DoFiltering(bool debug);

    /*! Returns human readable representation of this filter */
    void ToStream(std::ostream & outputStream) const;
};

};

#endif // _FilterBasics_h

