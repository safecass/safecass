/*

  Safety Framework for Component-based Robotics

  Created on: July 6, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "json.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace SF {

JSON::JSON(): JSONValues(0), JSONReader(0)
{
    JSONValues = new Json::Value;
    JSONReader = new Json::Reader;
}

JSON::~JSON()
{
}

void JSON::Cleanup(void)
{
    if (JSONValues) delete JSONValues;
    if (JSONReader) delete JSONReader;
    JSONValues = 0;
    JSONReader = 0;
}

bool JSON::Read(const char * json)
{
    if (!json) return false;

    if (!JSONReader->parse(json, *JSONValues)) {
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

std::string JSON::GetJSON() const
{
    std::stringstream ss;
    Json::StyledWriter writer;
    ss << writer.write(*JSONValues);

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

/*
bool JSON::Parse(std::string & result)
{
    Json::Value ret = JSONValues.get("test", "UNKNOWN");

    result = ret.asString();

    return true;
}
*/

};
