//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 6, 2012
// Last revision: Aug 7, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "json.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace SF {

JSON::JSON(void): JSONValues(0), JSONReader(0)
{
    Initialize();
}

JSON::~JSON()
{
}

void JSON::Initialize(void)
{
    JSONValues = new Json::Value;
    JSONReader = new Json::Reader;
}

void JSON::Cleanup(void)
{
    if (JSONValues) delete JSONValues;
    if (JSONReader) delete JSONReader;
    JSONValues = 0;
    JSONReader = 0;
}

bool JSON::Read(const char * json, bool printStdErr)
{
    if (!json) return false;

    // MJTODO: if json file is corrupted or contains invalid syntax,
    // following call to parse() will crash (points being freed is not allocated).
    // Json syntax validation would be a good plus.
    if (!JSONReader->parse(json, *JSONValues)) {
        if (printStdErr)
            std::cout << "JSON::Read - Failed to parse json:\n" 
                    //<< JSONReader->getFormatedErrorMessages() << std::endl;
                    << json << std::endl;
        Cleanup();
        return false;
    }

    return true;
}

bool JSON::ReadFromFile(const std::string & fileName)
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

std::string JSON::GetJSON(void) const
{
    std::stringstream ss;
    Json::StyledWriter writer;
    ss << writer.write(*JSONValues);

    return ss.str();
}

std::string JSON::GetJSONString(const Json::Value & jsonValue)
{
    std::stringstream ss;
    Json::StyledWriter writer;
    ss << writer.write(jsonValue);

    return ss.str();
}

bool JSON::WriteToFile(const std::string & fileName) const
{
    std::ofstream output(fileName.c_str());
    if (!output.is_open()) {
        return false;
    }

    output << GetJSON();
    output.close();

    return true;
}

bool JSON::GetSafeValueBool(const JSONVALUE & json, const std::string & key)
{
    JSONVALUE val = json.get(key, Json::nullValue);
    return (val.isNull() ? false : val.asBool());
}

int JSON::GetSafeValueInt(const JSONVALUE & json, const std::string & key)
{
    JSONVALUE val = json.get(key, Json::nullValue);
    return (val.isNull() ? 0 : val.asInt());
}

unsigned int JSON::GetSafeValueUInt(const JSONVALUE & json, const std::string & key)
{
    JSONVALUE val = json.get(key, Json::nullValue);
    return (val.isNull() ? 0 : val.asUInt());
}

double JSON::GetSafeValueDouble(const JSONVALUE & json, const std::string & key)
{
    JSONVALUE val = json.get(key, Json::nullValue);
    return (val.isNull() ? 0.0 : val.asDouble());
}

std::string JSON::GetSafeValueString(const JSONVALUE & json, const std::string & key)
{
    JSONVALUE val = json.get(key, Json::nullValue);
    return (val.isNull() ? "" : val.asString());
}

void JSON::ToStream(std::ostream & outputStream) const
{
    Json::StyledWriter writer;
    outputStream << writer.write(*JSONValues);
}

};
