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
    /*! Typedef of numerical representation of unique id of monitor targets */
    typedef unsigned int UIDType;
    enum { INVALID_UID = 0 };

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
    /*! Numerical representation of unique id */
    UIDType UID;

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

    /*! Timestamp of last sampled data */
    double LastSamplingTick;

public:
    Monitor();
    virtual ~Monitor();

    /*! Placeholders for samples */
    struct {
        double Period;
        double ExecTimeUser;
        double ExecTimeTotal;
    } Samples;

    /*! Assignment operator overloading */
    //Monitor & operator=(const Monitor & rhs);

    /*! Returns numeric unique id (unique within process) */

    /*! Returns string representation of unique id of monitoring target.
        Used to check duplicate monitoring target with the same monitoring
        target type. */
    const std::string GetUIDAsString(void) const;

    /*! Returns numerical representation of unique id of monitoring target */
    UIDType GetUIDAsNumber(void) const;

    /*! Check if it is necessary to sample new data */
    bool IsSamplingNecessary(double currentTick) const;

    /*! Update last sampling timestamp */
    inline void UpdateLastSamplingTick(double tick) { LastSamplingTick = tick; }

    //
    //  Getters
    //
    inline bool IsActive(void) const { return (State == STATE_ON); }
    inline bool IsStream(void) const { return (Output == OUTPUT_STREAM); }
    inline bool IsEvent(void) const  { return (Output == OUTPUT_EVENT); }

    inline TargetType         GetTargetType(void) const     { return Target; }
    inline TargetIDBase *     GetTargetID(void) const       { return TargetID; }
    inline StateType          GetState(void) const          { return State; }
    inline OutputType         GetOutputType(void) const     { return Output; }
    inline SamplingRateType   GetSamplingRate(void) const   { return SamplingRate; }
    inline SamplingPeriodType GetSamplingPeriod(void) const { return (1.0 / (SamplingPeriodType) SamplingRate); }

    //
    //  Setters
    //
    inline void SetTargetType(const TargetType target)       { Target = target; }
    inline void SetTargetID(TargetIDBase * targetId)         { TargetID = targetId; }
    inline void SetState(const StateType state)              { State = state; }
    inline void SetOutputType(const OutputType output)       { Output = output; }
    inline void SetSamplingRate(const SamplingRateType rate) { SamplingRate = rate; }

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
