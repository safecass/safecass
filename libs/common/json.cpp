/*

  Safety Framework for Component-based Robotics

  Created on: July 5, 2012

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "json.h"
#include <iostream>
#include <fstream>

namespace SF {

JSON::JSON(): JSONValues(0), JSONReader(0)
{
}

JSON::~JSON()
{
    Cleanup();
}

void JSON::Cleanup()
{
    if (JSONValues) {
        delete JSONValues;
        JSONValues = 0;
    }

    if (JSONReader) {
        // FIXME: deal with this later
        //delete JSONReader;
        JSONReader = 0;
    }

    JSONString.clear();
}

bool JSON::ReadFromFile(const std::string & fileName)
{
    std::string jsonString("");

    std::ifstream input(fileName.c_str());
    if (input.is_open()) {
        std::string line;
        while (input.good()) {
            getline(input, line);
            jsonString += line;
        }
        input.close();
    } else {
        // FIXME: replace std::cout with integrated logger
        std::cout << "Failed to open file: " << fileName << std::endl;
        return false;
    }

    return Read(jsonString);
}

bool JSON::WriteToFile(const std::string & fileName)
{
    std::ofstream output(fileName.c_str());
    if (!output.is_open()) {
        // FIXME: replace std::cout with integrated logger
        std::cout << "Failed to create file: " << fileName << std::endl;
        return false;
    }

    output << JSONString;
    output.close();

    return true;
}

bool JSON::Read(const std::string & jsonString)
{
    Cleanup();

    JSONValues = new Json::Value;
    JSONReader = new Json::Reader;

    JSONString = jsonString;

    if (!JSONReader->parse(JSONString, *JSONValues)) {
        Cleanup();
        return false;
    }

    return true;
}

bool JSON::Parse(std::string & result)
{
    if (!JSONValues) return false;

    Json::Value ret = JSONValues->get("test", "UNKNOWN");

    result = ret.asString();

    return true;
}

};
