//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jan 7, 2012
// Last revision: May 6, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _SignalElement_h
#define _SignalElement_h

#include <vector>

#include "common.h"
#include "utils.h"

namespace SF {

class HistoryBufferBase;

class SFLIB_EXPORT SignalElement {

public:
    /*! Typedef for scalar and vector types */
    typedef double ScalarType;
    typedef std::vector<double> VectorType;

    /*! Typedef for signal type */
    typedef enum {
        SCALAR, // scalar (double)
        VECTOR, // vector of doubles
        CUSTOM  // custom (user-defined) structure
    } SignalType;

    /*! Typedef for id of the history buffer that maintains the history of this signal */
    typedef std::string HistoryBufferIDType;

    /*! Typedef for index of the history buffer.
        In cisst, defined as mtsStateDataId which is typedef-ed as int
        (see cisst/cisstMultiTask/mtsStateTable.h) */
    typedef int HistoryBufferIndexType;

    /*! Const definitions */
    const static HistoryBufferIndexType INVALID_HISTORY_BUFFER_INDEX;

protected:
    /*! Name of this signal */
    const std::string Name;

    /*! Type of signal */
    const SignalType Type;

    /*! Instance of history buffer that this filter runs on.  Should be dynamically
        allocated as middleware-specific plug-in (i.e., history buffer accessor adapter) */
    HistoryBufferBase * HistoryBuffer;

    /*! Id of the history buffer that this signal is registered to */
    //HistoryBufferIDType HistoryBufferID;

    /*! Index of signal in the history buffer */
    HistoryBufferIndexType HistoryBufferIndex; // MJ TEMP: was StateDataId (int) in cisst

    /*! Timestamp of the last sample fetched */
    TimestampType TimeLastSampleFetched;

    /*! Placeholders for each signal type.  Updated when fetching new values from the
        history buffer. */
    ScalarType PlaceholderScalar;
    VectorType PlaceholderVector;

    /*! Initialize internal variables */
    void Init(void);

public:
    //-------------------------------------------------- 
    //  Constructors
    //-------------------------------------------------- 
    SignalElement();
    SignalElement(const std::string &             signalName, 
                  const SignalElement::SignalType signalType);
    ~SignalElement();

    /*! Fetch latest scalar-type value from history buffer */
    bool FetchNewValueScalar(bool activeFiltering);
    /*! Fetch latest vector-type value from history buffer */
    bool FetchNewValueVector(bool activeFiltering);

    //-------------------------------------------------- 
    //  Getters and Setters
    //-------------------------------------------------- 
    /*! Returns the name of this signal */
    inline const std::string & GetName(void) const { return Name; }

    /*! Returns the type of this signal */
    inline SignalType GetSignalType(void) const { return Type; }

    /*! Sets history buffer instance.  Should be called before activating filter (or signal) */
    void SetHistoryBufferInstance(HistoryBufferBase * buffer);
    /*! Returns the index of signal in the history buffer */
    inline HistoryBufferIndexType GetHistoryBufferIndex(void) const { return HistoryBufferIndex; }
    /*! Sets the index of signal in the history buffer */
    inline void SetHistoryBufferIndex(HistoryBufferIndexType index) { HistoryBufferIndex = index; }

    /*! Returns when the signal value was updated last time */
    inline TimestampType GetTimeLastSampleFetched(void) { return TimeLastSampleFetched; }
    /*! Sets when the signal value was updated last time */
    inline void SetTimeLastSampleFetched(TimestampType timestamp) { TimeLastSampleFetched = timestamp; }

    /*! Returns placeholders */
    ScalarType & GetPlaceholderScalar(void) { return PlaceholderScalar; }
    VectorType & GetPlaceholderVector(void) { return PlaceholderVector; }
    /*! Returns pointers of placeholders */
    ScalarType * GetPlaceholderScalarPointer(void) { return &PlaceholderScalar; }
    VectorType * GetPlaceholderVectorPointer(void) { return &PlaceholderVector; }
    /*! Sets placeholders */
    void SetPlaceholderScalar(ScalarType value) { PlaceholderScalar = value; }
    void SetPlaceholderVector(VectorType value) { PlaceholderVector = value; }

    /*! Returns human readable outputs */
    virtual std::string ToString(void) const  {
        std::stringstream ss;
        ToStream(ss);
        return ss.str();
    };
    void ToStream(std::ostream & outputStream) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const SignalElement & signal)
{
    signal.ToStream(outputStream);
    return outputStream;
}

};

#endif // _SignalElement_h
