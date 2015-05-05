//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 7, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _monitor_h
#define _monitor_h

#include "common.h"
#include "jsonwrapper.h"
#include "eventLocationBase.h"

namespace SF {

//! Class Monitor
/** 
    Monitor defines and implements a monitoring instance that monitors a quantity
    in the system.
*/
class SFLIB_EXPORT Monitor {
public:
    //! Typedef of numerical representation of unique id of monitor targets
    typedef unsigned int UIDType;
    enum { INVALID_UID = 0 };

    //! Typedef for monitor target (what to monitor)
    /*! In JSON format, each row can be specified as
        TARGET_INVALID                : "invalid"
        TARGET_THREAD_PERIOD          : "period"
        TARGET_THREAD_DUTYCYCLE_USER  : "dutycycle_user"
        TARGET_THREAD_DUTYCYCLE_TOTAL : "dutycycle_total"
        TARGET_FILTER_EVENT           : "filter_event"
        TARGET_CUSTOM                 : "custom"
        \sa SF::Dict::Json (libs/common/dict.h)
    */
    typedef enum {
        TARGET_INVALID,                /*!< initial value (invalid) */
        TARGET_THREAD_PERIOD,          /*!< period of periodic task (see FAULT_COMPONENT_PERIOD) */
        TARGET_THREAD_DUTYCYCLE_USER,  /*!< duty cycle of user thread (see FAULT_COMPONENT_OVERRUN) */
        TARGET_THREAD_DUTYCYCLE_TOTAL, /*!< duty cycle of thread (see FAULT_COMPONENT_OVERRUN) */
        TARGET_FILTER_EVENT,           /*!< MJTEMP: events from filter (usually faults) */
        TARGET_CUSTOM                  /*!< custom monitoring target */
        // [SFUPDATE]
    } TargetType;

    //! Typedef for set of monitor targets
    //typedef unsigned int TargetSetType;

    //! Typedef for monitor state
    /*! In JSON format, each row can be specified as
        STATE_INVALID : "invalid"
        STATE_OFF     : "off"
        STATE_ON      : "on"
        \sa SF::Dict::Json (libs/common/dict.h)
    */
    typedef enum {
        STATE_INVALID, /*!< initial value (invalid) */
        STATE_OFF,     /*!< monitoring disabled */
        STATE_ON       /*!< monitoring enabled */
    } StateType;

    //! Typedef for monitoring types (how to monitor)
    /*! In JSON format, each row can be specified as
        OUTPUT_INVALID : "invalid"
        OUTPUT_STREAM  : "stream"
        OUTPUT_EVENT   : "event"
        \sa SF::Dict::Json (libs/common/dict.h)
    */
    typedef enum {
        OUTPUT_INVALID, /*!< initial value (invalid) */
        OUTPUT_STREAM,  /*!< continuous sampling (stream of samples) */
        OUTPUT_EVENT    /*!< event-based sampling */
    } OutputType;

protected:
    //! Numerical representation of unique id
    UIDType UID;

    //! Monitoring target
    TargetType Target;

    //! Target location 
    /*! Declared as pointer to support middleware-specific classes
        which derive from SF::EventLocationBase).  An instance of EventLocationBase 
        needs to be created outside of this class and it will be cleaned up by this 
        class, i.e., the instance should not be deleted outside of this class.
    */
    EventLocationBase * LocationID;

    //! Monitoring state
    StateType State;

    //! Monitor output type
    OutputType Output;

    //! Sampling rate (valid only for stream type monitors)
    SamplingRateType SamplingRate;

    //! Addresses to publish monitoring data
    // MJ TEMP: don't use this field for now
    //StrVecType AddressesToPublish;

    //! Timestamp of last sampled data
    double LastSamplingTick;

    //! Monitor type
    /*! Either individual monitor (for passive filtering) or monitor attached 
        to filter (for active filtering)
    */
    bool AttachedToActiveFilter; // MJTEMP: is this being used????

    //! Initialize internal variables
    void Initialize(void);

public:
    //! Default constructor
    Monitor(void);
    //! Constructor with explicit arguments
    Monitor(const TargetType target, 
            EventLocationBase * locationId,
            const StateType state,
            const OutputType outputType,
            const SamplingRateType samplingRate = 0, // unsigned int type (Hz)
            double lastSamplingTick = 0.0);
    //! Constructor using instance of JSON structure
    Monitor(const JsonWrapper::JsonValue & jsonNode);
    //! Constructor using JSON string.
    /*! Internally calls constructor with JSON structure */
    //Monitor(const std::string & jsonMonitorSpecString);
    //! Destructor
    virtual ~Monitor();

    //! Placeholders for predefined monitoring targets
    struct {
        double PeriodActual;  // period measurement
        double PeriodNominal; // period measurement
        double ExecTimeUser;  // execution time for user codes
        double ExecTimeTotal; // execution time for user codes with internal processing
    } Samples;

    //! Assignment operator overloading
    //Monitor & operator=(const Monitor & rhs);

    //! Returns numeric unique id (unique within process)
    // TODO

    //! Returns string representation of unique id of monitoring target.
    /*! This is used to check duplicate monitoring target with the same monitoring
        target type.
    */
    const std::string GetUIDAsString(void) const;

    //! Returns numerical representation of unique id of monitoring target
    UIDType GetUIDAsNumber(void) const;

    //! Check if it is necessary to sample new data
    bool IsSamplingNecessary(double currentTick) const;

    //! Update last sampling timestamp
    inline void UpdateLastSamplingTick(double tick) { LastSamplingTick = tick; }

    /*! \addtogroup Getters
        @{
     */
    inline bool IsActive(void) const { return (State == STATE_ON); }
    inline bool IsStream(void) const { return (Output == OUTPUT_STREAM); }
    inline bool IsEvent(void) const  { return (Output == OUTPUT_EVENT); }
    inline bool IsAttachedToActiveFilter(void) const { return AttachedToActiveFilter; }

    inline TargetType         GetTargetType(void) const     { return Target; }
    virtual inline EventLocationBase *GetLocationID(void) const     { return LocationID; }
    inline StateType          GetState(void) const          { return State; }
    inline OutputType         GetOutputType(void) const     { return Output; }
    inline SamplingRateType   GetSamplingRate(void) const   { return SamplingRate; }
    inline SamplingPeriodType GetSamplingPeriod(void) const { return (1.0 / (SamplingPeriodType) SamplingRate); }
    /*! @} */

    /*! \addtogroup Setters
        @{
     */
    inline void SetTargetType(const TargetType target)       { Target = target; }
    inline void SetLocationID(EventLocationBase * locationId){ LocationID = locationId; }
    inline void SetState(const StateType state)              { State = state; }
    inline void SetOutputType(const OutputType output)       { Output = output; }
    inline void SetSamplingRate(const SamplingRateType rate) { SamplingRate = rate; }
    inline void SetAttachedToActiveFilter(bool attachedToActiveFilter) {
        AttachedToActiveFilter = attachedToActiveFilter;
    }
    /*! @} */

    /*! \addtogroup Misc. Getters
        @{
     */
    //! Return string that corresponds to the monitor target
    static const std::string GetTargetTypeString(const TargetType type);
    //! Return target type from string
    static TargetType GetTargetTypeFromString(const std::string & str);

    //! Return string that corresponds to the monitor state
    static const std::string GetStateTypeString(const StateType type);
    //! Return state type from string
    static StateType GetStateTypeFromString(const std::string & str);

    //! Return string that corresponds to the monitor output
    static const std::string GetOutputTypeString(const OutputType output);
    //! Return output type from string
    static OutputType GetOutputTypeFromString(const std::string & str);
    /*! @} */

    //! For human-readable logging and debugging
    /*! \param outputStream output stream
        \param includeLocation target location is printed if yes (default: yes)
    */
    virtual void ToStream(std::ostream & outputStream, bool includeLocation = true) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const Monitor & monitor) {
    monitor.ToStream(outputStream);
    return outputStream;
}

};

#endif // _monitor_h
