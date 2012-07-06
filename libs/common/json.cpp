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

JSON::JSON()
{
}

JSON::~JSON()
{
    Cleanup();
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
        }
        input.close();
    } else {
        // FIXME: replace std::cout with integrated logger
        std::cout << "Failed to open file: " << fileName << std::endl;
        return false;
    }

    return Read(jsonString);
}

bool JSON::WriteToFile(const std::string & fileName) const
{
    std::ofstream output(fileName.c_str());
    if (!output.is_open()) {
        // FIXME: replace std::cout with integrated logger
        std::cout << "Failed to create file: " << fileName << std::endl;
        return false;
    }

    Json::StyledWriter writer;
    output << writer.write(JSONValues);
    output.close();

    return true;
}

bool JSON::Read(const std::string & jsonString)
{
    if (!JSONReader.parse(jsonString, JSONValues)) {
        return false;
    }

    return true;
}

bool JSON::Parse(std::string & result)
{
    Json::Value ret = JSONValues.get("test", "UNKNOWN");

    result = ret.asString();

    return true;
}

};
