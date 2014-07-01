//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jan 7, 2012
// Last revision: Jul 1, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _FilterBase_h
#define _FilterBase_h

#include <iostream>
#include <sstream>

#include "signal.h"
#include "eventLocationBase.h"
#include "json.h"
#include "event.h"

namespace SF {

class HistoryBufferBase;
class EventPublisherBase;

// Helper macros to ease implementation of Create() function
#define SF_DEFINE_FACTORY_CREATE(_className)\
    static const std::string Name;\
    static FilterBase * Create(const SF::JSON::JSONVALUE & jsonNode) {\
        return new _className(jsonNode);\
    }\
    static _className __instance;

#define SF_IMPLEMENT_FACTORY(_className)\
    const std::string _className::Name = #_className;\
    SFLIB_EXPORT _className _className::__instance;

#define SF_REGISTER_FILTER_TO_FACTORY(_className)\
    FilterFactory::GetInstance()->RegisterFilter(#_className, &_className::Create);

class SFLIB_EXPORT FilterBase
{
public:
    //! Typedef of numerical representation of unique filter id
    typedef unsigned int FilterIDType;

    //! Typedef for filtering type
    /*! ACTIVE:  filter is processed by the target component and thus the execution time of 
                 the target component is consumed.
                 - Pros: Fastest and guaranteed detection of events
                 - Cons: Run-time overhead to run FDD pipelines and to propagate events
        PASSIVE: filter is processed by the monitoring component which does not consume the 
                 execution time of the target component.
                 - Pros: No run-time performance impact on the target component
                 - Cons: Delay in detecting an event (currently, monitoring component is 
                 implemented as a periodic task) 
    */
    typedef enum {
        ACTIVE,  /*!< active filtering */
        PASSIVE  /*!< passive filtering */
    } FilteringType;

#if 0
    /*! \enum SF::FilterBase::FilterCategory 
     * Typedef for filter categories 
     */
    typedef enum {
        INVALID,        /*!< invalid filter is not processed */
        FEATURE,        /*!< raw measurement processed */
        FEATURE_VECTOR, /*!< collection of features */
        SYMPTOM,        /*!< feature vectors processed */
        SYMPTOM_VECTOR, /*!< collection of symptoms */
        FAULT_DETECTOR  /*!< fault detector */
    } FilterCategory;
#endif

    //! Typedef for inputs
    /*! A filtering algorithm may require more than one input signal.
     *  Each input signal is represented as SignalElement and this container maintains
     *  all inputs of this filter instance. 
     */
    typedef std::vector<SignalElement*> SignalElementsType;
    // not used now: TODO: remove this if not used anymore
    //typedef std::vector<std::string> SignalNamesType;

    //! Typedef for filter factory class registration
    typedef FilterBase * (*CreateFilterFuncType)(const JSON::JSONVALUE & jsonNode);

    /*! \enum SF::FilterBase::FilterState
     * Typedef for internal state of filter.  A filter has to be explicitly enabled to be
     * used because its internal state is initialized as DISABLED.
     */
    typedef enum {
        DISABLED, /*!< disabled (default state) */
        ENABLED,  /*!< enabled, no pending event, new event can be detected */
        DETECTED  /*!< enabled with event detected, waiting for the event to be cleared,
                       no new event can be detected */
    } FilterStateType;

private:
    //! UID of this filters
    /*! Multiple filters of the same type may be deployed
     */
    static FilterIDType FilterUID;

protected:
    //! Typedef for derived classes
    typedef FilterBase BaseType;

    //! UID of this filter (copy of FilterUID for this particular filter)
    const FilterIDType UID;

    //! Name of this filter
    const std::string Name;

    //! Class name of this filter
    /*! Derived filter is instantiated based on this class name string 
     */
    const std::string ClassName;

    //! Category of this filter
    //const FilterCategory Category;

    //! Name of target component
    // TODO: this can be replaced with MonitorTarget structure(?)
    const std::string NameOfTargetComponent;

    //! Filtering type (active or passive)
    const FilteringType Type;

    //! Is this filter the last filter (of FDD pipeline)?
    bool LastFilterOfPipeline;

    //! Print out internal debug log if true
    bool PrintDebugLog;

    //! Event detection state
    FilterStateType FilterState;

    //! Event that this filter detected, which needs to be handled (or removed)
    Event * EventDetected;

    //! Queue for fault injection
    DoubleVecType InputQueue;

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
    virtual const std::string GenerateFDIJSON(double severity, double timestamp) const;

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

    //! Initialize internal variables
    void Initialize(void);

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
    FilterBase(const std::string  & filterName,
               const std::string  & targetComponentName,
               const FilteringType  monitoringType);
    //! Constructor using instance of JSON structure
    FilterBase(const std::string & filterName,
               const JSON::JSONVALUE & jsonNode);
    //! Destructor
    virtual ~FilterBase();

    //-------------------------------------------------- 
    //  Pure virtual methods
    //-------------------------------------------------- 
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
    inline void DeclareLastFilterOfPipeline(void) { LastFilterOfPipeline = true; }

    //-------------------------------------------------- 
    //  Fault injection
    //-------------------------------------------------- 
    // Queue of next inputs.  If this queue is empty, the filter runs with the actual
    // values.  If not, the filter dequeues an element (FIFO) to use it as the next input.
    // This feature is particularly useful for unit-testing or fault injection.
    // TODO: The current implementation assumes that the total number of inputs is 1.
    // This should be extended to support multiple-input cases.
    void InjectInput(SignalElement::ScalarType input);
    void InjectInput(const DoubleVecType & inputs);
    const std::string ShowInputQueue(void) const;

    //-------------------------------------------------- 
    //  Getters and Setters
    //-------------------------------------------------- 
    inline FilterIDType        GetFilterUID(void) const { return UID; }
    inline const std::string & GetFilterName(void) const { return Name; }
    inline const std::string & GetNameOfTargetComponent(void) const { return NameOfTargetComponent; }
    inline FilteringType       GetFilteringType(void) const { return Type; }
    inline bool IsLastFilterOfPipeline(void) const { return LastFilterOfPipeline; }

    // queries for filter state 
    //! Is this filter disabled?
    bool IsDisabled(void) const;
    //! Is this filter enabled?
    bool IsEnabled(void) const;
    //! Enable or disable this filter
    void Enable(bool enable = true);

    //! Set event detected
    bool SetEventDetected(const std::string & json);
    bool SetEventDetected(Event * event);
    //! Does this filter have any pending event?
    /*! TODO: if filter needs to handle multiple types of events, HasPendingEvent() has to
     *  accpet parameter(s) to specify type(s) of events.
     */
    bool HasPendingEvent(void) const;

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

    //! Returns human readable outputs
    virtual std::string ToString(void) const;
    virtual void        ToStream(std::ostream & outputStream) const;


    /*! \addtogroup Misc. Getters
        @{
     */
    //! Convert filtering type to string
    static const std::string GetFilteringTypeString(const FilteringType type);
    //! Convert string to filtering type
    static FilteringType GetFilteringTypeFromString(const std::string & str);

    //! Convert filter state to string
    static const std::string GetFilterStateString(const FilterStateType state);
    //! Convert string to filter state
    static FilterStateType GetFilterStateFromString(const std::string & str);
    /* @} */


    // invalid filter UID
    static const FilterIDType INVALID_FILTER_UID;
};

inline std::ostream & operator << (std::ostream & outputStream, const FilterBase & filter)
{
    filter.ToStream(outputStream);
    return outputStream;
}

};

#endif // _FilterBase_h
