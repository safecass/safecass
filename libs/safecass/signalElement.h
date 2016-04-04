//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jan 7, 2012
// Last revision: Apr 3, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//

#ifndef _SignalElement_h
#define _SignalElement_h

#include "common/common.h"
#include "safecass/historyBufferBase.h"

namespace SC {

class SCLIB_EXPORT SignalElement
{
private:
    //! Placeholder that is used to initialize ParamPrototype
    /*!
        This placeholder should be only used by the default constructor,
        where ParamPrototype cannot be determined properly.  bool is
        an arbitrary type.
    */
    const ParamEigen<bool> _ParamPrototypeDummy;

protected:
    //! Name of this signal
    const std::string Name;

    //! Parameter prototype associated with this signal
    const ParamBase & ParamPrototype;

    //! Instance of history buffer that this signal is associated with
    HistoryBufferBase * HistoryBufferInstance;

    //! Index of signal in the history buffer
    /*!
        SignalIndex corresponds to StateDataId (int) in case of cisst
    */
    HistoryBufferBase::IndexType SignalIndex;

    //! Timestamp of last sample fetched
    HistoryBufferBase::TimestampType TimeLastSampleFetched;

private:
    //! Signal requires explicit specifications of its name and parameter type
    SignalElement(void);

public:
    //! Constructors
    SignalElement(const std::string & signalName,
                  ParamBase & paramType,
                  HistoryBufferBase * historyBuffer = 0);

    //! Destructor
    ~SignalElement();

    //! Fetch latest value from history buffer
    // FIXME argument is not used now -- update this method when working on
    // internal vs. external filtering
    bool FetchNewValue(void) const;

    //
    //  Getters and Setters
    //
    //! Returns name of this signal
    inline const std::string & GetName(void) const { return Name; }

    //! Returns copy of parameter prototype of this signal
    /*!
        Note this method dynamically creates a copy of the parameter prototype
        registered to this signal object.  The caller of this method should
        properly deallocate the memory.
    */
    inline ParamBase * GetParamPrototype(void) const {
        return ParamPrototype.Clone();
    }

    //! Return history buffer instance
    inline HistoryBufferBase * GetHistoryBufferInstance(void) const {
        return HistoryBufferInstance;
    }
    //! Sets history buffer instance.  Should be called before activating filter (or signal)
    inline void SetHistoryBufferInstance(HistoryBufferBase * historyBuffer) {
        HistoryBufferInstance = historyBuffer;
    }

    //! Returns index of this signal in the history buffer
    inline HistoryBufferBase::IndexType GetSignalIndex(void) const {
        return SignalIndex;
    }
    //! Sets index of this signal in the history buffer
    inline void SetSignalIndex(HistoryBufferBase::IndexType index) {
        SignalIndex = index;
    }
    //! Returns timestamp of last sample fetched
    inline HistoryBufferBase::TimestampType GetTimeLastSampleFetched(void) {
        return TimeLastSampleFetched;
    }
    //! Sets when the last sample was fetched
    inline void SetTimeLastSampleFetched(HistoryBufferBase::TimestampType timestamp) {
        TimeLastSampleFetched = timestamp;
    }

    // TODO May not be necessary?
#if 0
    /*! Returns placeholders */
    ScalarType & GetPlaceholderScalar(void) { return PlaceholderScalar; }
    VectorType & GetPlaceholderVector(void) { return PlaceholderVector; }
    /*! Returns pointers of placeholders */
    ScalarType * GetPlaceholderScalarPointer(void) { return &PlaceholderScalar; }
    VectorType * GetPlaceholderVectorPointer(void) { return &PlaceholderVector; }
    /*! Sets placeholders */
    void SetPlaceholderScalar(ScalarType value) { PlaceholderScalar = value; }
    void SetPlaceholderVector(VectorType value) { PlaceholderVector = value; }
#endif

    //! Push new value to history buffer (used for deep fault injection)
    void PushNewValue(const ParamBase & paramType);

    //! Serialized representation of this signal object
    void ToStream(std::ostream & os) const;
};

inline std::ostream & operator << (std::ostream & os, const SignalElement & signal)
{
    signal.ToStream(os);
    return os;
}

};

#endif // _SignalElement_h
