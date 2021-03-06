/*

  Safety Framework for Component-based Robotics

  Created on: Sep 14, 2012

  Copyright (C) 2012-2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _cisstEventLocation_h
#define _cisstEventLocation_h

#include "eventLocationBase.h"

namespace SC {

class SCLIB_EXPORT cisstEventLocation: public EventLocationBase {
protected:
    // Middleware-specific fields
    std::string CommandName;
    std::string FunctionName;
    std::string EventGeneratorName;
    std::string EventHandlerName;

public:
    //! Default constructor
    cisstEventLocation(void);
    //! Constructor with explicit arguments
    cisstEventLocation(const std::string & processName,
                       const std::string & componentName,
                       const std::string & interfaceProvidedName,
                       const std::string & interfaceRequiredName,
                       const std::string & commandName,
                       const std::string & functionName,
                       const std::string & eventGeneratorName,
                       const std::string & eventHandlerName);
    //! Destructor
    ~cisstEventLocation();

    // Getters and Setters
#define DEFINE_ACCESSORS(_var)\
    inline const std::string Get##_var(void) const { return _var; }\
    inline void Set##_var(const std::string & _##_var) { _var = _##_var; }

    DEFINE_ACCESSORS(CommandName);
    DEFINE_ACCESSORS(FunctionName);
    DEFINE_ACCESSORS(EventGeneratorName);
    DEFINE_ACCESSORS(EventHandlerName);
#undef DEFINE_ACCESSORS

    const std::string GetIDString(void) const;

    //! Export internal data into JSON container
    void ExportToJSON(::Json::Value & root) const;
    //! Import JSON to update internal data
    void ImportFromJSON(const ::Json::Value & value);

    //! Human-readable output of this class
    void ToStream(std::ostream & outputStream) const;

    //! Operator= overloading
    virtual cisstEventLocation & operator=(const cisstEventLocation & rhs);
};

inline std::ostream & operator << (std::ostream & outputStream, const cisstEventLocation & location)
{
    location.ToStream(outputStream);
    return outputStream;
}

};

#endif // _cisstEventLocation_h
