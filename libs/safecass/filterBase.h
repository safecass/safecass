//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jan 7, 2012
// Last revision: Apr 26, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#ifndef _FilterBase_h
#define _FilterBase_h

#include <iostream>
#include <sstream>
#include <queue> // for fault injection
#include <list>

#include "safecass/state.h"
#include "safecass/signalElement.h"

#include "signal.h"
#include "eventLocationBase.h"
#include "jsonwrapper.h"
#include "event.h"

namespace SC {

class EventPublisherBase;

class Coordinator;

class SCLIB_EXPORT FilterBase
{
public:
    //! Typedef of unique filter id
    typedef size_t FilterIDType;

    //! Typedef of identifier of the state machine associated with this filter.
    /*!
        When this filter detects and generates a GCM event, this identifier is
        used for identifying an instance of the GCM state machine that is
        notified of the event.
    */
    class FilterTargetType {
    public:
        State::StateMachineType StateMachineType; /*!< state machine type (State::StateMachineType) */
        std::string ComponentName; /*!< Name of component */
        std::string InterfaceName; /*!< Name of interface */

        FilterTargetType(void) : StateMachineType(STATEMACHINE_INVALID) {}
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
    typedef FilterBase * (*CreateFilterFuncType)(const JsonWrapper::JsonValue & jsonNode);

    /*! \enum SC::FilterBase::FilterState
     * Typedef for internal state of filter.  A filter has to be explicitly enabled to be
     * used because its internal state is initialized as DISABLED.
     */
    typedef enum {
        DISABLED, /*!< disabled (default state) */
        ENABLED,  /*!< enabled, no pending event, new event can be detected */
        DETECTED  /*!< enabled with event detected, waiting for the event to be cleared,
                       no new event can be detected */
    } FilterStateType;

    // typedef for fault injection
    typedef std::queue<SignalElement::ScalarType> InputQueueScalarType;
    typedef std::queue<SignalElement::VectorType> InputQueueVectorType;

    // typedef for event detection and generation
    typedef enum {
        EVENT_DETECTION_LEVEL,
        EVENT_DETECTION_EDGE
    } EventDetectionModeType;
private:
    //! UID of this filters
    /*! Multiple filters of the same type may be deployed
     */
    static FilterIDType FilterUID;

    FilterTargetType CreateFilterTarget(State::StateMachineType targetStateMachineType,
                                        const std::string     & targetComponentName,
                                        const std::string     & targetInterfaceName);

    FilterTargetType CreateFilterTarget(const std::string & targetStateMachineTypeName,
                                        const std::string & targetComponentName,
                                        const std::string & targetInterfaceName);

protected:
    //! Typedef for derived classes
    typedef FilterBase BaseType;

    //! UID of this filter (copy of FilterUID for this particular filter)
    const FilterIDType UID;

    //! Name of this filter
    const std::string Name;

    //! Name of target component
    //const std::string NameOfTargetComponent;
    const FilterTargetType FilterTarget;

    //! Filtering type (active or passive)
    const FilteringType FilterType;

    //! If this filter is initialized
    bool Initialized;

    //! Is this filter the last filter (of FDD pipeline)?
    // bool LastFilterOfPipeline;

    //! Print out internal debug log if true
    bool PrintDebugLog;

    //! Event detection state
    FilterStateType FilterState;

    //! Event detection type
    EventDetectionModeType EventDetectionMode;

    //! Event that this filter detected, which needs to be handled (or removed)
    Event * EventDetected;

    //! Queue for fault injection
    InputQueueScalarType InputQueueScalar;
    InputQueueVectorType InputQueueVector;

    // For event generation and broadcasting
    Coordinator * SafetyCoordinator;

    //! Initialize this filter
    virtual void Initialize(void);

    //-------------------------------------------------- 
    //  Filter Inputs and Outputs
    //-------------------------------------------------- 
    //! Input signals that this filter uses
    SignalElementsType InputSignals;
    //! Output signals that this filter generates
    SignalElementsType OutputSignals;

    //! Add input signal to this filter (used by derived filters)
    bool AddInputSignal(const std::string &       signalName, 
                        SignalElement::SignalType signalType);
    //! Add output signal to this filter (used by derived filters)
    bool AddOutputSignal(const std::string &       signalName, 
                        SignalElement::SignalType  signalType);

    //! Refresh samples.  Returns false if the filter is disabled or it is of passive type.
    // IMPORTANT: All derived classes must call this method inside its RunFilter() method.
    bool RefreshSamples(void);

    //! Generate output signal name
    /*! Prefix: Name of input signal or input-specific word
        Root1 : Filter name
        Root2 : Filter UID
        Suffix: Signal id 

        TODO: this may need to be deprecated if user provides output signals name via json
     */
    std::string GenerateOutputSignalName(const std::string & prefix,
                                         const std::string & root1,
                                         const FilterIDType  root2,
                                         size_t              suffix) const;

    //! Generate fault diagnosis and identification (FDI) string in JSON format
    /*!
        Filter-specific.  String to be returned contains the following information:
        - Fault type
        - Fault location (spatial localization)
        - Fault time (temporal localization)
        - Fault severity
    */
    //virtual const std::string GenerateFDIJSON(double severity, double timestamp) const;

    // Encode event information in JSON format
    void GenerateEventInfo(JsonWrapper::JsonValue & json) const;

    //-------------------------------------------------- 
    //  Middleware-specific Instances (cisst)
    //-------------------------------------------------- 
    //! Event publisher to propagate events to the Safety Framework
    /*! Dynamically allocated as middleware-specific plug-in (event propagation 
        accessor adapter)
    */
    EventPublisherBase * EventPublisher;

    //! Event location to uniquely identify source of event
    /*! If EventLocationBase class is not expressive enough to uniquely identify an event 
     *  location in the system, a middleware-specific event location class that is derived 
     *  from this class can be used.
     */
    EventLocationBase * EventLocation;

    //-------------------------------------------------- 
    //  Constructors and Destructor
    //-------------------------------------------------- 
protected:
    //! Default constructor 
    /*! All filters have to be created with explicit type and name
     */
    FilterBase(void);
public:
    //! Constructor with explicit arguments
    FilterBase(const std::string     & filterName,
               FilteringType           filteringType,
               State::StateMachineType targetStateMachineType,
               const std::string     & targetComponentName,
               const std::string     & targetInterfaceName,
               EventDetectionModeType  eventDetectionMode = EVENT_DETECTION_EDGE);
    //! Constructor using JSON
    FilterBase(const std::string & filterName, const JsonWrapper::JsonValue & jsonNode);
    //! Destructor
    virtual ~FilterBase();

    //-------------------------------------------------- 
    //  Pure virtual methods
    //-------------------------------------------------- 
    //! Configure filter-specific arguments
    virtual bool ConfigureFilter(const JsonWrapper::JsonValue & jsonNode) = 0;

    //! Initialize filter
    virtual bool InitFilter(void) = 0;

    //! Run filtering algorithm
    virtual void RunFilter(void) = 0;

    //! Release
    virtual void CleanupFilter(void) = 0;

    //! Declare this filter as the last filter of FDD pipeline
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
    // TODO: The current implementation assumes that the total number of inputs is 1.
    // This should be extended to support multiple-input cases.

    // For scalar-type fault injection
    void InjectInputScalar(SignalElement::ScalarType input, bool deepInjection);
    void InjectInputScalar(const std::vector<SignalElement::ScalarType> & inputs, bool deepInjection);
    void InjectInputScalar(const std::list<SignalElement::ScalarType> & inputs, bool deepInjection);
    // For vector-type fault injection
    void InjectInputVector(const SignalElement::VectorType & input, bool deepInjection);
    void InjectInputVector(const std::vector<SignalElement::VectorType> & inputs, bool deepInjection);
    // For debugging
    const std::string ShowInputQueueScalar(void) const;
    const std::string ShowInputQueueVector(void) const;

    //-------------------------------------------------- 
    //  Getters and Setters
    //-------------------------------------------------- 
    inline FilterIDType        GetFilterUID(void) const { return UID; }
    inline const std::string & GetFilterName(void) const { return Name; }
    inline const FilterTargetType & GetFilterTarget(void) const { return FilterTarget; }
    inline const std::string & GetNameOfTargetComponent(void) const { return FilterTarget.ComponentName; }
    inline FilteringType       GetFilteringType(void) const { return FilterType; }
    // inline bool IsLastFilterOfPipeline(void) const { return LastFilterOfPipeline; }

    // queries for filter state 
    //! Is this filter disabled?
    bool IsDisabled(void) const;
    //! Is this filter enabled?
    bool IsEnabled(void) const;
    //! Enable or disable this filter
    void Enable(bool enable = true);

    //! Set event detected: obsolete
    //bool SetEventDetected(const std::string & json);
    //bool SetEventDetected(Event * event);
    //! Does this filter have any pending event?
    /*! TODO: if filter needs to handle multiple types of events, HasPendingEvent() has to
     *  accpet parameter(s) to specify type(s) of events.
     */
    //bool HasPendingEvent(void) const;

    inline size_t GetNumberOfInputs(void) const { return InputSignals.size(); }
    inline size_t GetNumberOfOutputs(void) const { return OutputSignals.size(); }

    std::string GetInputSignalName(size_t index) const;
    std::string GetOutputSignalName(size_t index) const;

    // not used now
    //SignalNamesType GetInputSignalNames(void) const;
    //SignalNamesType GetOutputSignalNames(void) const;

    SignalElement * GetInputSignalElement(size_t index) const;
    SignalElement * GetOutputSignalElement(size_t index) const;

    inline FilterStateType GetFilterState(void) const { return FilterState; }
    inline void SetFilterState(FilterStateType newFilterState) { FilterState = newFilterState; }

    //! Sets event publisher instance
    /*! This method should be called BEFORE activating filter
     */
    void SetEventPublisherInstance(EventPublisherBase * publisher);
    //! Sets event location instance
    /*! This method should be called BEFORE activating filter
     */
    void SetEventLocationInstance(EventLocationBase * location);

    //! Enable or disable internal debug log
    inline void EnableDebugLog(bool enable = true) { PrintDebugLog = enable; }

    // TODO: Set instance of Safety Coordinator 
    inline void SetSafetyCoordinator(Coordinator * instance) { SafetyCoordinator = instance; }

    //! Returns human readable outputs
    virtual const std::string ToString(bool verbose = false) const;
    virtual void ToStream(std::ostream & out, bool verbose = true) const;


    /*! \addtogroup Misc. Getters
        @{
     */
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


    // invalid filter UID
    static const FilterIDType INVALID_FILTER_UID;
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
    static FilterBase * Create(const JsonWrapper::JsonValue & jsonNode) {\
        return new _className(jsonNode);\
    }\
    static _className __instance;

#define SC_IMPLEMENT_FACTORY(_className)\
    const std::string _className::Name = #_className;\
    SCLIB_EXPORT _className _className::__instance;

#define SC_REGISTER_FILTER_TO_FACTORY(_className)\
    FilterFactory::GetInstance()->RegisterFilter(#_className, &_className::Create);

#endif // _FilterBase_h
