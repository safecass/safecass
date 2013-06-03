/*

  Safety Framework for Component-based Robotics

  Created on: September 3, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

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
    //! Default constructor: Filter should be created with explicit arguments
    FilterThreshold(void);

    //--------------------------------------------------
    //  Filter-specific parameters
    //--------------------------------------------------
    //! Name of input signal
    std::string NameOfInputSignal;
    //! Threshold
    SignalElement::ScalarType Threshold;
    //! Threshold margin
    SignalElement::ScalarType Margin;
    //! Output when input does not exceed threshold specified
    SignalElement::ScalarType Output0;
    //! Output when input exceeds threshold specified
    SignalElement::ScalarType Output1;

    //! initialize internal variables (called by constructor)
    void Initialize(void);

public:
    //! Constructor with explicit arguments
    FilterThreshold(BaseType::FilterCategory      category, 
                    const std::string &           targetComponentName,
                    SF::FilterBase::FilteringType monitoringType,
                    // below are filter-specific arguments
                    const std::string &           inputSignalName,
                    SignalElement::ScalarType     threshold,
                    SignalElement::ScalarType     margin,
                    SignalElement::ScalarType     output0,
                    SignalElement::ScalarType     output1);
    //! Constructor using instance of JSON structure
    FilterThreshold(const JSON::JSONVALUE & jsonNode);
    //! Destructor
    ~FilterThreshold();

    //! Implements pure virtual methods including thresholding algorithm
    bool InitFilter(void);
    void RunFilter(void);
    void CleanupFilter(void);

    //! Implements string representation of fault diagnosis and identification
    const std::string GenerateFDIJSON(double severity, double timestamp) const;

    //! Getters
    inline const std::string & GetNameOfInputSignal(void) const { return NameOfInputSignal; }
    inline SignalElement::ScalarType GetThreshold(void) const { return Threshold; }
    inline SignalElement::ScalarType GetMargin(void) const { return Margin; }
    inline SignalElement::ScalarType GetOutput0(void) const { return Output0; }
    inline SignalElement::ScalarType GetOutput1(void) const { return Output1; }

    //! Returns human readable representation of this filter
    void ToStream(std::ostream & outputStream) const;

    //! Name of this filter
    static const std::string Name;

    //! For filter factory 
    SF_IMPLEMENT_FACTORY_CREATE(FilterThreshold);
};

};

#endif // _FilterThreshold_h
