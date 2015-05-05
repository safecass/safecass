//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 5, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// URL          : https://github.com/minyang/safecass
//
#ifndef _jsonwrapper_h
#define _jsonwrapper_h

#include "common.h"

#include <json.h> // jsoncpp
#include <reader.h>
#include <string>

namespace SC {

class SCLIB_EXPORT JsonWrapper {
    // for unit-tests
    friend class JSONTest;
public:
    //! Typedef for convenience 
    /*! In this way, users don' have to directly access Json::Value */
    typedef Json::Value  JsonValue;
    typedef Json::Reader JsonReader;

protected:
    JsonValue  * _JsonValue;
    JsonReader * _JsonReader;

    //! Internal initialization
    void Initialize(void);

    //! Internal cleanup
    void Cleanup(void);

public:
    //! Default constructor
    JsonWrapper(void);
    //! Constructor with JSON file
    //JsonWrapper(const std::string & jsonFile);
    //! Destructor
    virtual ~JsonWrapper();

    //! Populate JSON structure with string
    virtual bool Read(const char * json, bool printStdErr = false);
    //! Read JSON file to populate JSON structure
    virtual bool ReadFromFile(const std::string & fileName);
    //! Write JSON structure to file
    virtual bool WriteToFile(const std::string & fileName) const;

    //! Returns JSON in std::string
    virtual std::string GetJSON(void) const;

    //! Returns root of JSON structure
    JsonValue & GetRoot(void) { return *_JsonValue; }

    //! Returns JSON in std::string
    static std::string GetJSONString(const JsonValue & jsonValue);

    //! Fetches values safely from JSON
    /*! If not careful, seg faults or crashes due to deallocation of non-allocated memory
        may occur.
    */
    static bool         GetSafeValueBool  (const JsonValue & json, const std::string & key);
    static int          GetSafeValueInt   (const JsonValue & json, const std::string & key);
    static unsigned int GetSafeValueUInt  (const JsonValue & json, const std::string & key);
    static double       GetSafeValueDouble(const JsonValue & json, const std::string & key);
    static std::string  GetSafeValueString(const JsonValue & json, const std::string & key);

    virtual void ToStream(std::ostream & outputStream) const;
};

inline std::ostream & operator << (std::ostream & outputStream, const JsonWrapper & json) {
    json.ToStream(outputStream);
    return outputStream;
}

};

#endif // _jsonwrapper_h
