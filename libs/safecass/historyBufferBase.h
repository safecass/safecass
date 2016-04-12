//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Sep 12, 2012
// Last revision: Apr 2, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
// TODO:
// - When working on cisst integration, this base file needs updates

#ifndef _HistoryBufferBase_h
#define _HistoryBufferBase_h

#include "SCDLLDefines.h"
#include "safecass/paramEigen.h"

namespace SC {

//! Base class that defines an interface to access underlying time-indexed buffer
/*!
    The history buffer is conceptually a time-indexed circular buffer that contains
    a history of data.  In practice, however, the history buffer is an abstract interface
    that provides an access to an underlying time-indexed buffer, which is typically
    implemented as a circular or ring buffer.  Because different middlewares or
    frameworks have their own design of maintaining data, it is required for safecass
    to define a minimal set of APIs that are generic enough to represnt different
    designs, while being able to read/write accesses to actual data in the buffer.

    References:

      * Filters and History Buffer:
          Secs. 4.3.6, 5.6.5
          Figs. 4.11, 5.11
      * Filtering modes (internal vs. external filtering):
          Sec. 5.6.5.2
          Fig. 5.12
*/
class SCLIB_EXPORT HistoryBufferBase
{
public:
    //! Typedef for identifier defined by underlying time-indexed buffer
    /*!
        cisst, for example, uses std::string as identifier
    */
    typedef std::string IDType;

    //! Typedef for index type defined by underlying time-indexed buffer
    /*!
        cisst, for example, uses index of type mtsStateDataId, which is typedefed to int
        (see mtsStateTable.h in cisstMultiTask library)
    */
    typedef int IndexType;

    //! Invalid signal index in history buffer
    enum { INVALID_HISTORY_BUFFER_INDEX = -1 };

    //! Typedef for timestamp type defined by underlying time-indexed buffer
    /*!
        cisst, for example, uses double as timestamp
        (see osaGetTime.h in cisstOSAbstraction library)

        This may change in the future when Boost.Chrono is introduce.
    */
    typedef double TimestampType;

public:
    //! Constructor
    HistoryBufferBase(void) {}
    //! Destructor
    virtual ~HistoryBufferBase() {}

    //! Get latest value from history buffer via direct access to history buffer
    /*!
        \param [in] id identifier of data in history buffer
        \param [out] arg latest snapshot of the signal specified (read from history buffer)
        \return true if success; false otherwise
        \sa FilterBase.h
    */
    virtual bool GetNewValue(const IDType & id, ParamBase & arg) = 0;

    //! Get latest value from history buffer via direct access to history buffer
    /*!
        \param [in] index index identifying data in history buffer
        \param [out] arg latest snapshot of the signal specified (read from history buffer)
        \return true if success; false otherwise
        \sa FilterBase.h
    */
    virtual bool GetNewValue(const IndexType & index, ParamBase & arg) = 0;

    //! Push value to history buffer
    /*!
        This method writes a value to the history buffer, thereby enabling safecass's
        deep fault injection that modifies values in the history buffer.
        Because this feature relies on a particular implementation of the underlying
        history buffer, the deep fault injection may not be available if the history
        buffer does not support such a "write-to-buffer" feature (e.g., ROS).  For this
        reason, this method is not declared as pure virtual and returns false by default.
    */
    virtual bool PushNewValue(const IndexType & index, const ParamBase & arg) {
        return false;
    }

#if 0
    //
    // For active filtering
    //
    //! Get latest value from history buffer (direct access)
    virtual void GetNewValueScalar(SignalElement::HistoryBufferIndexType index,
                                   SignalElement::ScalarType & value,
                                   TimestampType & timestamp) = 0;
    virtual void GetNewValueVector(SignalElement::HistoryBufferIndexType index,
                                   SignalElement::VectorType & value,
                                   TimestampType & timestamp) = 0;

    //
    // For passive filtering
    //
    //! Fetch latest value via middleware-specific data exchange mechanism (indirect access)
    virtual void GetNewValueScalar(SignalElement::ScalarType & value, TimestampType & timestamp) = 0;
    virtual void GetNewValueVector(SignalElement::VectorType & value, TimestampType & timestamp) = 0;

    // Support for "deep" fault injection: modify actual values in the buffer
    virtual void PushNewValueScalar(SignalElement::HistoryBufferIndexType index,
                                    SignalElement::ScalarType & value) = 0;
    virtual void PushNewValueVector(SignalElement::HistoryBufferIndexType index,
                                    const SignalElement::VectorType & value) = 0;
#endif

    virtual void ToStream(std::ostream & os) const = 0;
};

inline std::ostream & operator<< (std::ostream & os, const HistoryBufferBase & instance)
{
    instance.ToStream(os);
    return os;
}

};

#endif // _HistoryBufferBase_h
