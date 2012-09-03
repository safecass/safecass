/*

  Safety Framework for Component-based Robotics

  Created on: January 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _FilterBase_h
#define _FilterBase_h

#include <iostream>
#include <sstream>

#include "signal.h"

namespace SF {

class SFLIB_EXPORT FilterBase
{
public:
    /*! Typedef for filtering type.
        ACTIVE:  filter is processed by the target component and thus the execution time of 
                 the target component is consumed.
                 - Pros: Fastest and guaranteed detection of an event
                 - Cons: Run-time overhead to execute FDD pipelines and generate and propagate events
        PASSIVE: filter is processed by the monitoring component which does not require the 
                 execution time of the target component.
                 - Pros: No run-time performance impact on the target component
                 - Cons: Delay in detecting an event (currently, monitoring component is periodic task) */
    typedef enum { ACTIVE, PASSIVE } FilteringType;

    /*! Typedef for filter categories */
    typedef enum {
        INVALID,        // invalid filter is not processed
        FEATURE,        // raw measurement processed 
        FEATURE_VECTOR, // collection of features
        SYMPTOM,        // feature vectors processed
        SYMPTOM_VECTOR, // collection of symptoms
        FAULT_DETECTOR  // fault detector
    } FilterCategory;

    /*! Input signal element.  A filter may need more than one signal to run 
        its filtering algorithm and this class represents a signal. */
    typedef std::vector<SignalElement*> SignalElementsType;
    // not used now
    //typedef std::vector<std::string> SignalNamesType;

    /*! Typedef for filter id */
    typedef int FilterIdType;

private:
    /*! ID of this filter which allows multiple filters of the same type to be 
        used in the same state table. */
    static FilterIdType FilterUID;

protected:
    /*! Typedef of base class for derived classes */
    typedef FilterBase BaseType;

    /*! UID of this filter */
    const FilterIdType UID;
    /*! Category of this filter */
    const FilterCategory Category;
    /*! Name of this filter */
    const std::string Name;

    /*! State of this filter (enabled or disabled) */
    bool Enabled;

    /*! Input signals that this filter uses */
    SignalElementsType InputSignals;
    /*! Output signals that this filter generates */
    SignalElementsType OutputSignals;

    /*! Helper function to register input signal to this filter (used by derived filters) */
    bool AddInputSignal(const std::string & signalName, SignalElement::SignalType type);
    /*! Helper function to register output signal generated by filter (used by derived filters) */
    bool AddOutputSignal(const std::string & signalName, SignalElement::SignalType type);

    //-------------------------------------------------- 
    //  Constructors and Destructor
    //-------------------------------------------------- 
    // Filter has to be created with explicit type and name.
protected:
    FilterBase(void);
public:
    FilterBase(FilterCategory category, const std::string & name);
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
    /*! Run filtering algorithm which should be defined and implemented by derived filters */
    virtual void DoFiltering(bool debug = false) = 0;

    //-------------------------------------------------- 
    //  Getters and Setters
    //-------------------------------------------------- 
    inline FilterIdType GetFilterUID(void) const {
        return UID;
    }
    inline FilterCategory GetFilterCategory(void) const {
        return Category;
    }
    inline const std::string & GetFilterName(void) const {
        return Name;
    }

    inline bool IsEnabled(void) const {
        return Enabled;
    }
    inline void Enable(bool enable = true) {
        this->Enabled = enable;
    }

    inline size_t GetNumberOfInputs(void) const {
        return InputSignals.size();
    }
    inline size_t GetNumberOfOutputs(void) const {
        return OutputSignals.size();
    }

    std::string GetInputSignalName(size_t index) const;
    std::string GetOutputSignalName(size_t index) const;

    // not used now
    //SignalNamesType GetInputSignalNames(void) const;
    //SignalNamesType GetOutputSignalNames(void) const;

    SignalElement * GetInputSignalElement(size_t index) const;
    SignalElement * GetOutputSignalElement(size_t index) const;

    /*! Returns human readable outputs (for debugging purpose) */
    virtual std::string ToString(void) const  {
        std::stringstream ss;
        ToStream(ss);
        return ss.str();
    };
    virtual void ToStream(std::ostream & outputStream) const = 0;
};

inline std::ostream & operator << (std::ostream & outputStream, const FilterBase & filter)
{
    filter.ToStream(outputStream);
    return outputStream;
}

};

#endif // _FilterBase_h
