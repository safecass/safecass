/*

  Safety Framework for Component-based Robotics

  Created on: January 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _SignalElement_h
#define _SignalElement_h

#include <vector>

#include "common.h"

namespace SF {

class HistoryBufferBase;

class SFLIB_EXPORT SignalElement {
public:
    /*! Typedef for scalar and vector types */
    typedef double ScalarType;
    typedef std::vector<double> VectorType;

    /*! Typedef for signal type */
    typedef enum { SCALAR, VECTOR } SignalType;

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
    std::string Name;

    /*! Type of signal */
    SignalType Type;

    /*! Id of the history buffer that this signal is registered to */
    //HistoryBufferIDType HistoryBufferID;

    /*! Index of signal in the history buffer */
    HistoryBufferIndexType HistoryBufferIndex; // MJ TEMP: was StateDataId (int) in cisst

    /*! Placeholders for each signal type.  Updated when fetching new values from the
        history buffer. */
    // MJ: this could be public
    ScalarType PlaceholderScalar;
    VectorType PlaceholderVector;

    /*! Timestamp of the last sample fetched */
    TimestampType TimeLastSampleFetched;

    /*! Instance of history buffer that this filter runs on.  Should be dynamically
        allocated as middleware-specific plug-in (i.e., history buffer accessor adapter) */
    HistoryBufferBase * HistoryBuffer;

    /*! Initialize internal variables */
    void Init(void);

public:
    //-------------------------------------------------- 
    //  Constructors
    //-------------------------------------------------- 
    SignalElement();
    //SignalElement(HistoryBufferIDType historyBufferId, SignalType type, const std::string & name = "NONAME");
    SignalElement(SignalType type, const std::string & name = "NONAME");

    /*! Fetch latest scalar-type value from history buffer */
    bool FetchNewValueScalar(void);
    /*! Fetch latest vector-type value from history buffer */
    bool FetchNewValueVector(void);

    //-------------------------------------------------- 
    //  Getters and Setters
    //-------------------------------------------------- 
    /*! Returns the name of this filter */
    inline const std::string & GetName(void) const { return Name; }

    /*! Returns the type of this filter */
    inline SignalType GetSignalType(void) const { return Type; }

    /*! Returns the id of the history buffer that this signal is registered to */
    //inline HistoryBufferIDType GetHistoryBufferID(void) const { return HistoryBufferID; }
    /*! Sets the history buffer instance */
    inline void SetHistoryBufferInstance(HistoryBufferBase * historyBufferInstance) {
        HistoryBuffer = historyBufferInstance;
    }

    /*! Returns the index of signal in the history buffer */
    inline HistoryBufferIndexType GetHistoryBufferIndex(void) const { return HistoryBufferIndex; }
    /*! Sets the index of signal in the history buffer */
    inline void SetHistoryBufferIndex(HistoryBufferIndexType index) { HistoryBufferIndex = index; }

    /*! Returns when the signal value was updated last time */
    inline TimestampType GetTimeLastSampleFetched(void) { return TimeLastSampleFetched; }
    /*! Sets when the signal value was updated last time */
    inline void SetTimeLastSampleFetched(TimestampType timestamp) { TimeLastSampleFetched = timestamp; }

    /*! Returns placeholders */
    ScalarType GetPlaceholderScalar(void) const { return PlaceholderScalar; }
    VectorType & GetPlaceholderVector(void) { return PlaceholderVector; }
    /*! Sets placeholders */
    void SetPlaceholderScalar(ScalarType value) { PlaceholderScalar = value; }
    void SetPlaceholderVector(VectorType value) { PlaceholderVector = value; }
};

};

#endif // _SignalElement_h
