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

class SignalElement {
public:
    /*! Typedef for scalar and vector types */
    typedef double ScalarType;
    typedef std::vector<double> VectorType;

    /*! Typedef for signal type */
    typedef enum { SCALAR, VECTOR } SignalType;

    /*! Typedef for index of the history buffer.
        In cisst, defined as mtsStateDataId which is typedef-ed as int
        (see cisst/cisstMultiTask/mtsStateTable.h) */
    typedef int HistoryBufferIndexType;

    /*! Const definitions */
    const static HistoryBufferIndexType INVALID_HISTORY_BUFFER_INDEX;

protected:
    /*! Name of this filter */
    std::string Name;

    /*! Type of signal */
    SignalType Type;

    /*! Index of signal in the history buffer */
    HistoryBufferIndexType BufferIndex; // was StateDataId (int)

    /*! Placeholders for each signal type.  Updated when fetching new values from the
        history buffer. */
    // MJ: this could be public
    ScalarType PlaceholderScalar;
    VectorType PlaceholderVector;

    /*! Initialize internal variables */
    void Init(void);

public:
    //-------------------------------------------------- 
    //  Constructors
    //-------------------------------------------------- 
    SignalElement();
    SignalElement(SignalType type, const std::string & name = "NONAME");

    //-------------------------------------------------- 
    //  Getters and Setters
    //-------------------------------------------------- 
    /*! Returns the name of this filter */
    inline const std::string & GetName(void) const { return Name; }

    /*! Returns the type of this filter */
    inline SignalType GetSignalType(void) const { return Type; }

    /*! Returns the index of signal in the history buffer, which this filter uses  */
    inline HistoryBufferIndexType GetHistoryBufferIndex(void) const { return BufferIndex; }

    /*! Sets the index of signal in the history buffer, which this filter uses */
    inline void SetStateDataId(HistoryBufferIndexType index) { BufferIndex = index; }
};

};

#endif // _SignalElement_h
