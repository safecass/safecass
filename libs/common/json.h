/*

  Safety Framework for Component-based Robotics

  Created on: July 5, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _json_h
#define _json_h

#include "common.h"

#include <jsoncpp/json.h>
#include <jsoncpp/reader.h>
#include <string>

namespace SF {

class SFLIB_EXPORT JSON {
    // for unit-tests
    friend class JSONTest;
public:
    //! Typedef for convenience 
    /*! In this way, users don' have to directly access Json::Value */
    typedef Json::Value  JSONVALUE;
    typedef Json::Reader JSONREADER;

protected:
    JSONVALUE  * JSONValues;
    JSONREADER * JSONReader;

    //! Internal initialization
    void Initialize(void);

    //! Internal cleanup
    void Cleanup(void);

public:
    //! Default constructor
    JSON(void);
    //! Constructor with JSON file
    //JSON(const std::string & jsonFile);
    //! Destructor
    virtual ~JSON();

    //! Populate JSON structure with string
    virtual bool Read(const char * json);
    //! Read JSON file to populate JSON structure
    virtual bool ReadFromFile(const std::string & fileName);
    //! Write JSON structure to file
    virtual bool WriteToFile(const std::string & fileName) const;

    //! Returns JSON in std::string
    virtual std::string GetJSON(void) const;

    //! Returns root of JSON structure
    JSONVALUE & GetRoot(void) { return *JSONValues; }

    //! Returns JSON in std::string
    static std::string GetJSONString(const JSONVALUE & jsonValue);

    //! Fetches values safely from JSON
    /*! If not careful, seg faults or crashes due to deallocation of non-allocated memory
        may occur.
    */
    static bool         GetSafeValueBool  (const JSONVALUE & json, const std::string & key);
    static int          GetSafeValueInt   (const JSONVALUE & json, const std::string & key);
    static unsigned int GetSafeValueUInt  (const JSONVALUE & json, const std::string & key);
    static double       GetSafeValueDouble(const JSONVALUE & json, const std::string & key);
    static std::string  GetSafeValueString(const JSONVALUE & json, const std::string & key);

    virtual void ToStream(std::ostream & outputStream) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const JSON & json) {
    json.ToStream(outputStream);
    return outputStream;
}

};

#endif // _json_h
