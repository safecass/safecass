/*

  Safety Framework for Component-based Robotics

  Created on: September 14, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _EventLocationBase_h
#define _EventLocationBase_h

#include "common/common.h"
#include "json/json.h"

namespace SC {

class SCLIB_EXPORT EventLocationBase
{
protected:
    //! Middleware independent fields
    std::string ProcessName;
    std::string ComponentName;
    std::string InterfaceProvidedName;
    std::string InterfaceRequiredName;

public:
    //! Default constructor
    EventLocationBase(void);
    //! Constructor with explicit arguments
    EventLocationBase(const std::string & processName,
                      const std::string & componentName,
                      const std::string & interfaceProvidedName,
                      const std::string & interfaceRequiredName);
    //! Destructor
    virtual ~EventLocationBase();

    // Getters and Setters
#define DEFINE_ACCESSORS(_var)\
    inline const std::string Get##_var(void) const { return _var; }\
    inline void Set##_var(const std::string & _##_var) { _var = _##_var; }

    DEFINE_ACCESSORS(ProcessName);
    DEFINE_ACCESSORS(ComponentName);
    DEFINE_ACCESSORS(InterfaceProvidedName);
    DEFINE_ACCESSORS(InterfaceRequiredName);
#undef DEFINE_ACCESSORS

    //! Get string representation of this class
    virtual const std::string GetIDString(void) const;

    //! Export internal data into JSON container
    virtual void ExportToJSON(::Json::Value & root) const;
    //! Import JSON to update internal data
    virtual void ImportFromJSON(const ::Json::Value & value);

    //! Human-readable output of this class
    virtual void ToStream(std::ostream & outputStream) const;

    //! Operator= overloading
    virtual EventLocationBase & operator=(const EventLocationBase & rhs);
};

inline std::ostream & operator << (std::ostream & outputStream, const EventLocationBase & location)
{
    location.ToStream(outputStream);
    return outputStream;
}

};

#endif // _EventLocationBase_h
