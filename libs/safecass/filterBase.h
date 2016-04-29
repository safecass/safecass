//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jan 7, 2012
// Last revision: Apr 28, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#ifndef _FilterBase_h
#define _FilterBase_h

#include <iostream>
#include <sstream>
#include <queue> // for fault injection

#include "common/jsonwrapper.h"

#include "safecass/state.h"
#include "safecass/signalElement.h"
#include "safecass/event.h"
#include "safecass/eventLocationBase.h"

namespace SC {

class EventPublisherBase;

class Coordinator;

class SCLIB_EXPORT FilterBase
{
public:
    //! Typedef of unique filter id
    typedef size_t FilterIDType;

    //! Invalid filter UID
    static const FilterIDType INVALID_FILTER_UID;

    //! Typedef of identifier of the state machine associated with this filter.
    /*!
        When this filter detects and generates a GCM event, this identifier is
        used for identifying an instance of the GCM state machine that is
        notified of the event.
    */
    class StateMachineInfo {
    public:
        //! State machine type (refer to State::StateMachineType)
        State::StateMachineType StateMachineType;
        //! Name of component
        std::string ComponentName;
        //! Name of interface
        std::string InterfaceName;

        StateMachineInfo(void) : StateMachineType(State::STATEMACHINE_INVALID) {}
    };

    //! Typedef of filtering type
    /*!
        Two types of filtering

        1) Internal filtering: The filter is processed by the target component using its
        own execution time.

            a) Pros: Event detection with a minial delay is guranteed during the
            lifecycle of the target component

            b) Cons: Run-time overhead for filter processing

        2) External filtering: The filter is processed by the monitoring component, which
        is an external component

            a) Pros: No run-time overhead on the target component

            b) Cons: Delay in the detection and generation of events.  Such a delay is
            defined and bounded by the thread execution model of the monitoring component.
            For example, a max possible delay in the external filtering can be one second
            if the monitoring component is running at 1 Hz.
    */
    typedef enum {
        FILTERING_INTERNAL, /*!< Internal filtering */
        FILTERING_EXTERNAL  /*!< External filtering */
    } FilteringType;

    //! Typedef of (a list of) signals
    /*!
        A filter is comprised of a set of input signals and output signals where each
        signal is represented as an SignalElement object.  This container maintains
        a set of SignalElement objects that this filter defines.
    */
    typedef std::vector<SignalElement*> SignalElementsType;

    //! Typedef for filter factory class registration
    typedef FilterBase * (*CreateFilterFuncType)(const Json::Value & json);

    //! Typedef of filter state
    typedef enum {
        STATE_INIT,     /*!< Initial state; not yet ready to be used */
        STATE_DISABLED, /*!< Initialized but disabled; ready to be used */
        STATE_ENABLED,  /*!< Enabled, no pending event, new event can be detected */
        STATE_DETECTED  /*!< Enabled, outstanding event exists, no new event can be detected */
    } FilterStateType;

    //! Typedef of queue for fault injection
    /*!
        See comments in HistoryBuffer::PushNewValue()
        \sa HistoryBuffer::PushNewValue()
    */
    typedef std::queue<ParamBase*> InjectionQueueType;
#if 0
    template<typename _type>
    BaseType::IndexType AddSignal(const ParamEigen<_type> & arg, const BaseType::IDType & name)
    {
        // Check duplicate name
        if (FindSignal(name)) {
            SCLOG_ERROR << "AddSignal() failed: duplicate name \"" << name << "\"" << std::endl;
            return BaseType::INVALID_SIGNAL_INDEX;
        }

        typedef ParamEigen<_type> ParamType;
        SignalAccessor<ParamType> * accessor = new SignalAccessor<ParamType>(arg, name, BufferSize);

        BaseType::IndexType accessorId = (BaseType::IndexType) SignalAccessors.size();

        SignalAccessors.push_back(accessor);
        SignalAccessorsMap.insert(std::make_pair(name, accessorId));

        SCLOG_INFO << "Created accessor (id=" << accessorId << "): " << *accessor << std::endl;

        return accessorId;
    }
#endif

    //! Typedef of event detection mode
    typedef enum {
        EVENT_DETECTION_LEVEL, /*!< Level triggered */
        EVENT_DETECTION_EDGE   /*!< Edge triggered */
    } EventDetectionModeType;

private:
    //! Static counter for unique filter id
    static FilterIDType FilterUID;

    //! Specify state machine associated with this filter
    StateMachineInfo RegisterStateMachine(State::StateMachineType targetStateMachineType,
                                          const std::string &     targetComponentName,
                                          const std::string &     targetInterfaceName);

    //! Specify state machine associated with this filter
    StateMachineInfo RegisterStateMachine(const std::string & targetStateMachineTypeName,
                                          const std::string & targetComponentName,
                                          const std::string & targetInterfaceName);

protected:
    //! Typedef for derived classes
    typedef FilterBase BaseType;

    //! UID of this filter
    const FilterIDType FilterID;

    //! Name of this filter
    const std::string Name;

    //! Specification of state machine associated with this filter
    const StateMachineInfo StateMachineRegistered;

    //! Filtering type
    const FilteringType FilterType;

    //! Filter state
    FilterStateType FilterState;

    // FIXME design for composite filter (filter pipeline, cascaded filters)
    //! Is this filter the last filter (of FDD pipeline)?
    // bool LastFilterOfPipeline;

    //! Event detection type
    EventDetectionModeType EventDetectionMode;

    //! Queue for fault injection
    InjectionQueueType InjectionQueue;

    //! Pointer to Safety Coordinator instance
    Coordinator * SafetyCoordinator;

    //! Initialize this filter
    virtual void Initialize(void);

    //! Refresh samples
    /*!
        IMPORTANT: All classes derived from this class must call this method
        inside its RunFilter() method

        \return false if the filter is not initialized or disabled.
                FIXME => or in the external filtering mode (?)
    */
    bool RefreshSamples(void);

    // Serialize information of the event detected in JSON format
    virtual void GenerateEventInfo(Json::Value & json) const;

    //--------------------------------------------------
    //  Filter Inputs and Outputs
    //--------------------------------------------------
    /*! \addtogroup Filter input and output signals
        @{
    */
    //! Input signals
    SignalElementsType InputSignals;
    //! Output signals
    SignalElementsType OutputSignals;

    //! Add input signal to this filter (used by derived filters)
    bool AddInputSignal(const std::string & signalName);

    //! Add output signal to this filter (used by derived filters)
    bool AddOutputSignal(const std::string & signalName);

    //! Generate output signal name
    /*!
        Prefix: Name of input signal or input-specific word
        Root1 : Filter name
        Root2 : Filter UID
        Suffix: Signal id
    */
    std::string GenerateOutputSignalName(const std::string & prefix,
                                         const std::string & root1,
                                         const FilterIDType  root2,
                                         size_t              suffix) const;

    inline size_t GetNumberOfInputSignal(void) const  { return InputSignals.size(); }
    inline size_t GetNumberOfOutputSignal(void) const { return OutputSignals.size(); }

    std::string GetInputSignalName(size_t index) const;
    std::string GetOutputSignalName(size_t index) const;

    SignalElement * GetInputSignalElement(size_t index) const;
    SignalElement * GetOutputSignalElement(size_t index) const;
    /* @} */


    //--------------------------------------------------
    //  Middleware-specific: cisst
    //--------------------------------------------------
    //! Event publisher to propagate events to the Safety Framework
    /*!
        Dynamically allocated as middleware-specific plug-in (event propagation
        accessor adapter
    */
    EventPublisherBase * EventPublisher;

    //! Event location to identify source of event
    /*!
        If EventLocationBase class does not fully capture the structure of the
        system, a middleware-specific class derived from EventLocationBase can
        be used.
    */
    EventLocationBase * EventLocation;

    //--------------------------------------------------
    //  Constructors and Destructor
    //--------------------------------------------------
private:
    //! Default constructor
    /*!
        Disable default constructor: All filters have to be created with
        explicit parameters.
    */
    FilterBase(void);

public:
    //! Constructor with explicit arguments
    FilterBase(const std::string     & filterName,
               FilteringType           filteringType,
               // FIXME use StateMachineInfo instead?
               State::StateMachineType targetStateMachineType,
               const std::string     & targetComponentName,
               const std::string     & targetInterfaceName,
               EventDetectionModeType  eventDetectionMode = EVENT_DETECTION_EDGE);
    //! Constructor using JSON
    FilterBase(const std::string & filterName, const Json::Value & jsonNode);

    //! Destructor
    virtual ~FilterBase();

    //--------------------------------------------------
    //  Pure virtual methods
    //--------------------------------------------------
    /*! \addtogroup Pure virtual methods
        @{
    */
    //! Configure filter-specific arguments
    virtual bool ConfigureFilter(const Json::Value & jsonNode) = 0;
    //! Initialize filter
    virtual bool InitFilter(void) = 0;
    //! Run filtering algorithm
    virtual void RunFilter(void) = 0;
    //! Clean up
    virtual void CleanupFilter(void) = 0;
    /* @} */

    //! Declare this filter as the last filter of FDD pipeline
    // FIXME design for composite filter (filter pipeline, cascaded filters)
    /*! This internally creates a monitor to publish filtering results (e.g., events or 
     *  faults) to the Safety Framework, and attaches the monitor to this filter.
     */
    // inline void DeclareLastFilterOfPipeline(void) { LastFilterOfPipeline = true; }

    //--------------------------------------------------
    //  Fault injection
    //--------------------------------------------------
    // Queue of next inputs.  If this queue is empty, the filter runs with the actual
    // values.  If not, the filter dequeues an element (FIFO) to use it as the next input.
    // This feature is particularly useful for unit-testing or fault injection.
    /*! \addtogroup Fault injection
        @{
    */
    // FIXME add description about deep vs. fault injection
    //! Injection of one element
    void InjectInput(const std::string & inputSignalName,
                     const ParamBase & arg,
                     bool deepInjection);

    // For debugging
    const std::string PrintInjectionQueue(void) const;
    /* @} */

    //--------------------------------------------------
    //  Getters and Setters
    //--------------------------------------------------
    /*! \addtogroup Miscellaneous getters and setters
        @{
    */
    inline FilterIDType  GetFilterID(void) const   { return FilterID; }
    inline std::string   GetFilterName(void) const { return Name; }
    // FIXME deprecate this one?
    inline std::string   GetNameOfTargetComponent(void) const { return StateMachineRegistered.ComponentName; }
    inline FilteringType GetFilteringType(void) const { return FilterType; }
    inline const StateMachineInfo & GetStateMachineInfo(void) const { return StateMachineRegistered; }

    //! Returns if this filter is disabled
    inline bool IsDisabled(void) const { return (FilterState == STATE_DISABLED); }
    //! Returns if this filter is enabled
    inline bool IsEnabled(void) const { return (FilterState == STATE_ENABLED); }
    // FIXME What about STATE_INIT and STATE_DETECTED?

    //! Enable or disable this filter
    void Enable(bool enable = true);

    // Filter state should be maintained by itself
    // inline FilterStateType GetFilterState(void) const { return FilterState; }
    // inline void SetFilterState(FilterStateType newFilterState) { FilterState = newFilterState; }

    //! Sets event publisher instance
    /*!
        This method should be called BEFORE enabling filter.
    */
    void SetEventPublisherInstance(EventPublisherBase * publisher);

    //! Sets event location instance
    /*!
        This method should be called BEFORE enabling filter.
    */
    void SetEventLocationInstance(EventLocationBase * location);

    // FIXME shouldn't this be specified as constructor's argument?
    //! Sets Safety Coordinator instance
    inline void SetSafetyCoordinator(Coordinator * instance) { SafetyCoordinator = instance; }

    //! Returns content of this filter in human readable format
    virtual const std::string ToString(bool verbose = false) const;

    //! Returns content of this filter in human readable format
    virtual void ToStream(std::ostream & out, bool verbose = true) const;

    //! Convert filtering type to string
    static const std::string GetFilteringTypeString(const FilteringType type);
    //! Convert string to filtering type
    static FilteringType GetFilteringTypeFromString(const std::string & str);
    //! Convert string to event detection type
    EventDetectionModeType GetEventDetectionTypeFromString(const std::string & str);

    //! Convert filter state to string
    static const std::string GetFilterStateString(const FilterStateType state);
    //! Convert string to filter state
    static FilterStateType GetFilterStateFromString(const std::string & str);
    /* @} */
};

inline std::ostream & operator << (std::ostream & out, const FilterBase & filter)
{
    filter.ToStream(out);
    return out;
}

};

// Helper macros for filter factory implementation
#define SC_DEFINE_FACTORY_CREATE(_className)\
    static const std::string Name;\
    static FilterBase * Create(const Json::Value & jsonNode) {\
        return new _className(jsonNode);\
    }\
    static _className __instance;

#define SC_IMPLEMENT_FACTORY(_className)\
    const std::string _className::Name = #_className;\
    SCLIB_EXPORT _className _className::__instance;

#define SC_REGISTER_FILTER_TO_FACTORY(_className)\
    FilterFactory::GetInstance()->RegisterFilter(#_className, &_className::Create);

#endif // _FilterBase_h
