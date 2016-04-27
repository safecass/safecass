//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 5, 2012
// Last revision: Apr 26, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// URL          : https://github.com/safecass/safecass
//
// JSONCPP on github:
//      https://github.com/open-source-parsers/jsoncpp
// JSONCPP API:
//      https://github.com/open-source-parsers/jsoncpp/wiki/API
// JSONCPP tutorial:
//      https://github.com/open-source-parsers/jsoncpp/wiki
//
#ifndef _JsonWrapper_h
#define _JsonWrapper_h

#include "common/common.h"
#include "json/json.h"
#include "json/reader.h"

namespace SC {

class SCLIB_EXPORT JsonWrapper
{
protected:
    Json::Value  JsonValue;
    Json::Reader JsonReader;

    //! Internal initialization
    void Initialize(void);

    //! Internal cleanup
    void Cleanup(void);

public:
    //! Constructor and destructor (do nothing)
    JsonWrapper(void) {}
    ~JsonWrapper(void) {}

    //! Read and parse JSON document from string
    /*!
        \param json An instance of std::string that contains a JSON document
        \return true if success; false otherwise
    */
    bool Read(const std::string & json);

    //! Read and parse JSON document from file
    /*!
        \param fileName file name that contains a JSON document
        \return true if success; false otherwise
    */
    bool ReadFromFile(const std::string & fileName);

    //! Export JSON document to file
    bool WriteToFile(const std::string & fileName) const;

    //
    // Getters
    //
    //! Returns JSON in std::string format
    std::string GetJsonString(void) const;

    //! Returns root of JSON structure
    inline Json::Value & GetJsonRoot(void) { return JsonValue; }

    //
    // Utils
    //
    //! Returns JSON in std::string
    static std::string GetJsonString(const Json::Value & jsonValue);

    //! Export content of this class in human-readable format
    void ToStream(std::ostream & os) const;
};

inline std::ostream & operator << (std::ostream & os, const JsonWrapper & json)
{
    json.ToStream(os);
    return os;
}

};

#endif // _JsonWrapper_h
