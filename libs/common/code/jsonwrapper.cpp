//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Jul 6, 2012
// Last revision: Apr 26, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// URL          : https://github.com/safecass/safecass
//
#include "common/jsonwrapper.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace SC;

bool JsonWrapper::Read(const std::string & json)
{
    // Collect and preserve comments in the JSON document
    // (can be disabled with third parameter of parse())
    if (JsonReader.parse(json, JsonValue))
        return true;

    SCLOG_ERROR << "Failed to read and parse json document: " << json << std::endl;
    SCLOG_ERROR << "Error message: " << JsonReader.getFormattedErrorMessages() << std::endl;

    return false;
}

bool JsonWrapper::ReadFromFile(const std::string & fileName)
{
    std::ifstream file(fileName.c_str());
    if (!file.is_open()) {
        SCLOG_ERROR << "Failed to open json file to read: " << fileName << std::endl;
        return false;
    }

    // placeholder to contain json document
    std::string json;

    std::string line;
    while (file.good()) {
        getline(file, line);
        json += line;
        json += "\n";
    }
    file.close();

    return Read(json);
}

bool JsonWrapper::WriteToFile(const std::string & fileName) const
{
    std::ofstream file(fileName.c_str());
    if (!file.is_open()) {
        SCLOG_ERROR << "Failed to open json file to write: " << fileName << std::endl;
        return false;
    }

    file << GetJsonString() << std::endl;
    file.close();

    return true;
}

std::string JsonWrapper::GetJsonString(void) const
{
    return JsonWrapper::GetJsonString(JsonValue);
}

std::string JsonWrapper::GetJsonString(const Json::Value & json)
{
    Json::StyledWriter writer;

    return writer.write(json);
}

void JsonWrapper::ToStream(std::ostream & os) const
{
    os << GetJsonString() << std::endl;
}
