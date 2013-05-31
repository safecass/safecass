/*

  Safety Framework for Component-based Robotics

  Created on: January 7, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _FilterBase_h
#define _FilterBase_h

#include <iostream>
#include <sstream>

#include "signal.h"
#include "eventLocationBase.h"
#include "json.h"

namespace SF {

class HistoryBufferBase;
class EventPublisherBase;

class SFLIB_EXPORT FilterBase {
public:
    //! Typedef of numerical representation of unique filter id
    typedef int FilterIDType;

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

    //! Typedef for inputs
    /*! A filtering algorithm may require more than one input signal.
     *  Each input signal is represented as SignalElement and this container maintains
     *  all inputs of this filter instance. 
     */
    typedef std::vector<SignalElement*> SignalElementsType;
    // not used now: TODO: remove this if not used anymore
    //typedef std::vector<std::string> SignalNamesType;

private:
    //! UID of this filters
    /*! Multiple filters of the same type may be deployed
     */
    static FilterIDType FilterUID;

protected:
    //! Typedef for derived classes
    typedef FilterBase BaseType;

    //! UID of this filter
    const FilterIDType UID;

    //! Name of this filter
    std::string Name;

    //! Class name of this filter
    /*! Derived filter is instantiated based on this class name string 
     */
    std::string ClassName;

    //! Category of this filter
    const FilterCategory Category;

    //! Name of target component
    // MJTODO: this can be replaced with MonitorTarget structure(?)
    const std::string NameOfTargetComponent;

    //! Filtering type (active or passive)
    const FilteringType Type;

    //! Is this filter the last filter (of FDD pipeline)?
    bool LastFilterOfPipeline;

    //! Print out internal debug log if true
    bool PrintDebugLog;

    //! State of this filter (enabled or disabled)
    bool Enabled;

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
    // REMOVE THE BELOW STRUCTURE IF I DON'T USE IT
    //! Instance of history buffer that this filter runs on
    /*! Dynamically allocated as middleware-specific plug-in (i.e., history buffer accessor 
        adapter)
    */
    //HistoryBufferBase * HistoryBuffer; // am I using this??? Probably not...

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
    FilterBase(const std::string & filterName,
               FilterCategory      filterCategory,
               const std::string & targetComponentName,
               FilteringType       monitoringType);
    //! Constructor using instance of JSON structure
    FilterBase(const std::string & filterName,
               const JSON::JSONVALUE & jsonNode);
    //! Destructor
    virtual ~FilterBase();

#if 0 // MJ: future improvements
    /*! Add input signal to this filter.  A placeholder of appropriate type 
        is created internally to fetch new values from the state table. */
    bool AddInput(const std::string & componentName, const std::string & signalName);
    // TODO: push_back (name), prepare placeholder to fetch new input values from state table

    // TODO: add an option to expose an output to the standardized provided interface.
    // This can be useful if the monitoring component wants to fetch output values when a filter 
    // is attached to the source component.
    bool AddOutput(const std::string & signalName); 
#endif
    //! Run filtering algorithm (pure virtual)
    /*! This method should be re-defined and implemented by derived filters
     */
    virtual void DoFiltering(void) = 0;

    //! Declare this filter as the last filter of FDD pipeline
    /*! This internally creates a monitor to publish filtering results (e.g., events or 
     *  faults) to the Safety Framework, and attaches the monitor to this filter.
     */
    inline void DeclareLastFilterOfPipeline(void) { LastFilterOfPipeline = true; }

    //-------------------------------------------------- 
    //  Getters and Setters
    //-------------------------------------------------- 
    inline FilterIDType        GetFilterUID(void) const { return UID; }
    inline const std::string & GetFilterName(void) const { return Name; }
    inline FilterCategory      GetFilterCategory(void) const { return Category; }
    inline const std::string & GetNameOfTargetComponent(void) const { return NameOfTargetComponent; }
    inline FilteringType       GetFilteringType(void) const { return Type; }
    inline bool IsLastFilterOfPipeline(void) const { return LastFilterOfPipeline; }

    inline bool IsEnabled(void) const { return Enabled; }
    inline void Enable(bool enable = true) { this->Enabled = enable; }

    inline size_t GetNumberOfInputs(void) const { return InputSignals.size(); }
    inline size_t GetNumberOfOutputs(void) const { return OutputSignals.size(); }

    std::string GetInputSignalName(size_t index) const;
    std::string GetOutputSignalName(size_t index) const;

    // not used now
    //SignalNamesType GetInputSignalNames(void) const;
    //SignalNamesType GetOutputSignalNames(void) const;

    SignalElement * GetInputSignalElement(size_t index) const;
    SignalElement * GetOutputSignalElement(size_t index) const;

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
    //! Convert filter category to string
    static const std::string GetFilterCategoryString(const FilterCategory type);
    //! Convert string to filter category
    static FilterCategory GetFilterCategoryFromString(const std::string & str);

    //! Convert filtering type to string
    static const std::string GetFilteringTypeString(const FilteringType type);
    //! Convert string to filtering type
    static FilteringType GetFilteringTypeFromString(const std::string & str);
};

inline std::ostream & operator << (std::ostream & outputStream, const FilterBase & filter)
{
    filter.ToStream(outputStream);
    return outputStream;
}

};

#endif // _FilterBase_h
