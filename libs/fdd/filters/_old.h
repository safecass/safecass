/*

  Safety Framework for Component-based Robotics

  Created on: January 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

/*!
  \file
  \brief Defines basic types of filters for fault detection and diagnosis
*/

#ifndef _FilterBypass_h
#define _FilterBypass_h

#include "filterBase.h"

namespace SF {

//-----------------------------------------------------------------------------
//  Bypass Filter
//
//  Output Y = X
//
//  X, Y can be either scalar or vector.
//
class SFLIB_EXPORT FilterBypass : public FilterBase
{
protected:
    BaseType::SignalElement::SIGNAL_TYPE SignalType;

public:
    /*! Default constructor is provided only to satisfy the requirement of 
        cmnGenericObject.  DO NOT USE THIS. */
    FilterBypass();
    FilterBypass(BaseType::FILTER_TYPE filterType, 
                           const std::string & inputName,
                           BaseType::SignalElement::SIGNAL_TYPE signalType);
    ~FilterBypass();

    /*! Implements bypass */
    void DoFiltering(bool debug);

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(FilterBypass);


//-----------------------------------------------------------------------------
//  Trend Velocity Filter (1st-order differentiator)
//
//  Output Y = (x(t+1) - x(t)) / (delta_t)
//
//  delta_t: Period of monitoring component.  Usually sampling rate of 
//           the monitoring component (mtsMonitorComponent).
//  MJ: delta_t can be also calculated as actual time difference between 
//      two samples but this requires samples to contain timestamp information.
//
class FilterTrendVel : public FilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    BaseType::SignalElement::SIGNAL_TYPE SignalType;
    BaseType::PlaceholderType OldValueScalar;
    BaseType::PlaceholderVectorType OldValueVector;
    double OldTimestamp;
    bool OldValueVectorInitialized;

public:
    /*! Default constructor is provided only to satisfy the requirement of 
        cmnGenericObject.  DO NOT USE THIS. */
    FilterTrendVel();
    FilterTrendVel(BaseType::FILTER_TYPE filterType, 
                             const std::string & inputName,
                             BaseType::SignalElement::SIGNAL_TYPE signalType);
    ~FilterTrendVel();

    /*! Implements 1st-order differentiator */
    void DoFiltering(bool debug);

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(FilterTrendVel);


//-----------------------------------------------------------------------------
//  Vectorize Filter
//
//  Output Y_n = (X1, X2, ..., Xn)
//
//  Y_n: vector of size n (of type mtsDoubleVec)
//   Xi: i-th scalar input
//
class FilterVectorize: public FilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    unsigned int InputSize;

public:
    /*! Default constructor is provided only to satisfy the requirement of 
        cmnGenericObject.  DO NOT USE THIS. */
    FilterVectorize();
    FilterVectorize(BaseType::FILTER_TYPE filterType, const BaseType::SignalNamesType & inputNames);
    ~FilterVectorize();

    /*! Convert n scalars into one vector of size n */
    void DoFiltering(bool debug);

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(FilterVectorize);


//-----------------------------------------------------------------------------
//  Norm Filter
//
//  Output Y = (norm of X)
//
//  Three types of norm are supported: L1 norm, L2 norm, Linfinity norm
//
//  L1 Norm: Weisstein, Eric W. "L1-Norm." From MathWorld--A Wolfram Web Resource. 
//           http://mathworld.wolfram.com/L1-Norm.html
//  L2 Norm: Weisstein, Eric W. "L2-Norm." From MathWorld--A Wolfram Web Resource. 
//           http://mathworld.wolfram.com/L2-Norm.html
//  Linf Norm: Weisstein, Eric W. "Linfty-Norm." From MathWorld--A Wolfram Web Resource. 
//             http://mathworld.wolfram.com/L-Infinity-Norm.html
//
class FilterNorm: public FilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef enum { L1NORM, L2NORM, LINFNORM } NORM_TYPE;

protected:
    NORM_TYPE NormType;

public:
    /*! Default constructor is provided only to satisfy the requirement of 
        cmnGenericObject.  DO NOT USE THIS. */
    FilterNorm();
    FilterNorm(BaseType::FILTER_TYPE filterType, 
                         const std::string & inputName,
                         NORM_TYPE normType);
    ~FilterNorm();

    /*! Calculate norm of input */
    void DoFiltering(bool debug);

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(FilterNorm);


//-----------------------------------------------------------------------------
//  Arithmetic Operation Filter
//
//  Output Y = X_1 [+,-,*,/] X_2
//
//  X_1 and X_2 can be either scalar or vector.
//
class FilterArithmetic: public FilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef enum { INVALID, SUBTRACTION } OPERATION_TYPE;

protected:
    const OPERATION_TYPE OperationType;

    const bool IsOffsetScalar;
    const BaseType::PlaceholderType OffsetScalar;
    const BaseType::PlaceholderVectorType OffsetVector;

    bool IsInputScalar;
    bool IsOutputScalar;

public:    
    /*! Default constructor is provided only to satisfy the requirement of 
        cmnGenericObject.  DO NOT USE THIS. */
    FilterArithmetic();
    FilterArithmetic(BaseType::FILTER_TYPE filterType, 
                               OPERATION_TYPE operationType,
                               const std::string & input1Name,
                               BaseType::SignalElement::SIGNAL_TYPE input1Type,
                               const BaseType::PlaceholderVectorType input2);
    FilterArithmetic(BaseType::FILTER_TYPE filterType, 
                               OPERATION_TYPE operationType,
                               const std::string & input1Name,
                               BaseType::SignalElement::SIGNAL_TYPE input1Type,
                               const BaseType::PlaceholderType input2);
    ~FilterArithmetic();

    /*! Calculate norm of input */
    void DoFiltering(bool debug);

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(FilterArithmetic);


//-----------------------------------------------------------------------------
//  Mean (Moving Average) Filter
//
//  Output Y(t+1) = lambda * x(t) + (1 - ramda) * Y(t)
//  
//  where Y(t): moving average at time t 
//        x(t): sample at time t
//      lambda: filter constant (0 < lambda <= 1, determines degree of filtering)
//              (smaller value is more useful for detecting small shifts)
//
//  MJ: Moving average can be defined in various ways: simple moving average, 
//  cumulative moving average, weighted moving average, exponential moving 
//  average (EMA) (a.k.a, exponentially weighted moving average (EWMA)),
//  modified moving average (MMA), running moving average (RMA), or smoothed 
//  moving average.  For now, only EWMA is implemented.
//
class FilterAverage: public FilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    const BaseType::SignalElement::SIGNAL_TYPE SignalType;
    const double Lambda;

    bool OldValueInitialized;
    BaseType::PlaceholderType       OldValueScalar;
    BaseType::PlaceholderVectorType OldValueVector;

public:
    /*! Default constructor is provided only to satisfy the requirement of 
        cmnGenericObject.  DO NOT USE THIS. */
    FilterAverage();
    FilterAverage(BaseType::FILTER_TYPE filterType, 
                            const std::string & inputName,
                            BaseType::SignalElement::SIGNAL_TYPE inputType,
                            double Lambda);
    ~FilterAverage();

    /*! Implements 1st-order differentiator */
    void DoFiltering(bool debug);

    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(FilterAverage);

};

#endif // _FilterBasics_h

