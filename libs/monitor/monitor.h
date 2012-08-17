/*

  Safety Framework for Component-based Robotics

  Created on: July 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _monitor_h
#define _monitor_h

#include "common.h"

//#include <string>
//#include <map>

namespace SF {

class TargetIDBase {
public:
    virtual const std::string GetTargetID(void) const = 0;
};

class SFLIB_EXPORT Monitor {
public:
    /*! Typedef for monitor target (what to monitor) */
    typedef enum {
        TARGET_INVALID          = 0,
        // Measure period of periodic task (see FAULT_COMPONENT_PERIOD)
        TARGET_THREAD_PERIOD    = 1,
        // Measure duty cycle of periodic task (see FAULT_COMPONENT_OVERRUN)
        TARGET_THREAD_DUTYCYCLE = 2
        // [SFUPDATE]
    } TargetType;

    /*! Typedef for set of monitor targets */
    typedef unsigned int TargetSetType;

    /*! Typedef for monitor state */
    typedef enum {
        STATE_INVALID, 
        STATE_OFF, 
        STATE_ON
    } StateType;

    /*! Typedef for monitoring types (how to monitor) */
    typedef enum {
        OUTPUT_INVALID, 
        OUTPUT_STREAM,
        OUTPUT_EVENT 
    } OutputType;

protected:
    /*! Monitoring target */
    TargetType Target;

    /*! Target information (declared as pointer to support middleware-specific classes
        which derive from SF::TargetIDBase).  An instance of TargetIDBase needs to be 
        created outside of this class but should be cleaned up by this class, i.e.,
        the instance should not be deleted outside of this class. */
    TargetIDBase * TargetID;

    /*! Monitoring state */
    StateType State;

    /*! Monitor output type */
    OutputType Output;

    /*! Sampling rate (valid only for STREAM-type monitors) */
    SamplingRateType SamplingRate;

    /*! Addresses to publish monitoring data */
    // MJ TEMP: don't use this field for now
    //StrVecType AddressesToPublish;

public:
    Monitor();
    virtual ~Monitor();

    /*! Assignment operator overloading */
    //Monitor & operator=(const Monitor & rhs);

    /*! Returns unique id of monitoring target */
    const std::string GetTargetUID(void) const;

    //
    //  Getters
    //
    inline bool IsActive(void) const { return (State == STATE_ON); }
    inline bool IsStream(void) const { return (Output == OUTPUT_STREAM); }
    inline bool IsEvent(void) const  { return (Output == OUTPUT_EVENT); }

    TargetType         GetTargetType(void) const     { return Target; }
    TargetIDBase *     GetTargetID(void) const       { return TargetID; }
    StateType          GetState(void) const          { return State; }
    OutputType         GetOutputType(void) const     { return Output; }
    SamplingRateType   GetSamplingRate(void) const   { return SamplingRate; }
    SamplingPeriodType GetSamplingPeriod(void) const { return (1.0 / (SamplingPeriodType) SamplingRate); }

    //
    //  Setters
    //
    void SetTargetType(const TargetType target)       { Target = target; }
    void SetTargetId(TargetIDBase * targetId)         { TargetID = targetId; }
    void SetState(const StateType state)              { State = state; }
    void SetOutputType(const OutputType output)       { Output = output; }
    void SetSamplingRate(const SamplingRateType rate) { SamplingRate = rate; }

    //
    //  Misc. Getters
    //
    /*! Return string that corresponds to the monitor target */
    static const std::string GetTargetTypeString(const TargetType type);
    /*! Return target type from string */
    static TargetType GetTargetTypeFromString(const std::string & str);

    /*! Return string that corresponds to the monitor state */
    static const std::string GetStateTypeString(const StateType type);
    /*! Return target type from string */
    static StateType GetStateTypeFromString(const std::string & str);

    /*! Return string that corresponds to the monitor output */
    static const std::string GetOutputTypeString(const OutputType output);
    /*! Return output type from string */
    static OutputType GetOutputTypeFromString(const std::string & str);

    /*! For human-readable logging and debugging */
    virtual void ToStream(std::ostream & outputStream) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const Monitor & monitor) {
    monitor.ToStream(outputStream);
    return outputStream;
}

};

#endif // _monitor_h
