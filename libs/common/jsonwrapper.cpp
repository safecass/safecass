//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 6, 2012
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
// URL          : https://github.com/minyang/safecass
//
#include "jsonwrapper.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace SF;

JsonWrapper::JsonWrapper(void): _JsonValue(0), _JsonReader(0)
{
    Initialize();
}

JsonWrapper::~JsonWrapper()
{
}

void JsonWrapper::Initialize(void)
{
    _JsonValue = new Json::Value;
    _JsonReader = new Json::Reader;
}

void JsonWrapper::Cleanup(void)
{
    if (_JsonValue) delete _JsonValue;
    if (_JsonReader) delete _JsonReader;
    _JsonValue = 0;
    _JsonReader = 0;
}

bool JsonWrapper::Read(const char * json, bool printStdErr)
{
    if (!json) return false;

    // MJTODO: if json file is corrupted or contains invalid syntax,
    // following call to parse() will crash (points being freed is not allocated).
    // Json syntax validation would be a good plus.
    if (!_JsonReader->parse(json, *_JsonValue)) {
        if (printStdErr)
            std::cout << "JsonWrapper::Read - Failed to parse json:\n" 
                    //<< _JsonReader->getFormatedErrorMessages() << std::endl;
                    << json << std::endl;
        Cleanup();
        return false;
    }

    return true;
}

bool JsonWrapper::ReadFromFile(const std::string & fileName)
{
    std::string jsonString;

    std::ifstream input(fileName.c_str());
    if (input.is_open()) {
        std::string line;
        while (input.good()) {
            getline(input, line);
            jsonString += line;
            jsonString += "\n";
        }
        input.close();
    } else {
        return false;
    }

    return Read(jsonString.c_str());
}

std::string JsonWrapper::GetJSON(void) const
{
    std::stringstream ss;
    Json::StyledWriter writer;
    ss << writer.write(*_JsonValue);

    return ss.str();
}

std::string JsonWrapper::GetJSONString(const Json::Value & jsonValue)
{
    std::stringstream ss;
    Json::StyledWriter writer;
    ss << writer.write(jsonValue);

    return ss.str();
}

bool JsonWrapper::WriteToFile(const std::string & fileName) const
{
    std::ofstream output(fileName.c_str());
    if (!output.is_open()) {
        return false;
    }

    output << GetJSON();
    output.close();

    return true;
}

bool JsonWrapper::GetSafeValueBool(const JsonValue & json, const std::string & key)
{
    JsonValue val = json.get(key, Json::nullValue);
    return (val.isNull() ? false : val.asBool());
}

int JsonWrapper::GetSafeValueInt(const JsonValue & json, const std::string & key)
{
    JsonValue val = json.get(key, Json::nullValue);
    return (val.isNull() ? 0 : val.asInt());
}

unsigned int JsonWrapper::GetSafeValueUInt(const JsonValue & json, const std::string & key)
{
    JsonValue val = json.get(key, Json::nullValue);
    return (val.isNull() ? 0 : val.asUInt());
}

double JsonWrapper::GetSafeValueDouble(const JsonValue & json, const std::string & key)
{
    JsonValue val = json.get(key, Json::nullValue);
    return (val.isNull() ? 0.0 : val.asDouble());
}

std::string JsonWrapper::GetSafeValueString(const JsonValue & json, const std::string & key)
{
    JsonValue val = json.get(key, Json::nullValue);
    return (val.isNull() ? "" : val.asString());
}

void JsonWrapper::ToStream(std::ostream & outputStream) const
{
    Json::StyledWriter writer;
    outputStream << writer.write(*_JsonValue);
}
