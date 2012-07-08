/*

  Safety Framework for Component-based Robotics

  Created on: July 6, 2012

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

  Reference:

  - CppTest macros: http://cpptest.sourceforge.net/cpptest-assert_8h.html

*/

#include "testJson.h"
#include "json.h"
#include "config.h"

using namespace SF;

class JSONTest: public JSON {
public:
    bool Parse(void) {
        return true;
    }
};

const std::string VALUE_ENCODING = "UTF-8";
const std::string VALUE_PLUGIN1 = "python";
const std::string VALUE_PLUGIN2 = "c++";
const std::string VALUE_PLUGIN3 = "ruby";
const unsigned int VALUE_LENGTH = 3;
const bool VALUE_USE_SPACE = true;

SFTests::SFTests()
{
    // Test registration
    TEST_ADD(SFTests::TestJSONRead);
    TEST_ADD(SFTests::TestJSONReadFromFile);
    TEST_ADD(SFTests::TestJSONWrite);
    TEST_ADD(SFTests::TestJSONWriteToFile);

    // Sample JSON from JsonCpp documentation:
    SampleJSON << "// Configuration options\n";
    SampleJSON << "{\n";
    SampleJSON << "   // Default encoding for text\n";
    SampleJSON << "   \"encoding\" : \"" << VALUE_ENCODING << "\",\n";
    SampleJSON << "\n";  
    SampleJSON << "   // Plug-ins loaded at start-up\n";
    SampleJSON << "   \"plug-ins\" : [\n";
    SampleJSON << "       \"" << VALUE_PLUGIN1 << "\",\n";
    SampleJSON << "       \"" << VALUE_PLUGIN2 << "\",\n";
    SampleJSON << "       \"" << VALUE_PLUGIN3 << "\"\n";
    SampleJSON << "       ],\n";
    SampleJSON << "       \n";
    SampleJSON << "   // Tab indent size\n";
    SampleJSON << "   \"indent\" : { \"length\" : " << VALUE_LENGTH 
               << ", \"use_space\": " << (VALUE_USE_SPACE ? "true" : "false") << " }\n";
    SampleJSON << "}\n";
}

void SFTests::TestJSONRead(void)
{
    JSONTest test;
    const std::string sample(SampleJSON.str());

    TEST_ASSERT(test.Read(sample));

    std::string encoding = test.GetRoot().get("encoding", "ERROR" ).asString();
    TEST_ASSERT(encoding.compare("UTF-8") == 0);

    int i = 0;
    const Json::Value plugins = test.GetRoot()["plug-ins"];
    TEST_ASSERT(plugins[i++].asString().compare(VALUE_PLUGIN1) == 0);
    TEST_ASSERT(plugins[i++].asString().compare(VALUE_PLUGIN2) == 0);
    TEST_ASSERT(plugins[i++].asString().compare(VALUE_PLUGIN3) == 0);

    TEST_ASSERT(test.GetRoot()["indent"].get("length", 0).asUInt() == VALUE_LENGTH);
    TEST_ASSERT(test.GetRoot()["indent"].get("use_space", false).asBool() == VALUE_USE_SPACE);
}

void SFTests::TestJSONReadFromFile(void)
{
    JSONTest test;
    TEST_ASSERT(test.ReadFromFile(SF_SOURCE_ROOT_DIR"/tests/sample.json"));

    std::string encoding = test.GetRoot().get("encoding", "ERROR" ).asString();
    TEST_ASSERT(encoding.compare("UTF-8") == 0);

    int i = 0;
    const Json::Value plugins = test.GetRoot()["plug-ins"];
    TEST_ASSERT(plugins[i++].asString().compare(VALUE_PLUGIN1) == 0);
    TEST_ASSERT(plugins[i++].asString().compare(VALUE_PLUGIN2) == 0);
    TEST_ASSERT(plugins[i++].asString().compare(VALUE_PLUGIN3) == 0);

    TEST_ASSERT(test.GetRoot()["indent"].get("length", 0).asUInt() == VALUE_LENGTH);
    TEST_ASSERT(test.GetRoot()["indent"].get("use_space", false).asBool() == VALUE_USE_SPACE);
}

void SFTests::TestJSONWrite(void)
{
    JSONTest test;
    TEST_ASSERT(test.Read(SampleJSON.str()));

    const std::string newEncoding = "Korean";
    const std::string newPlugin1  = "PYTHON";
    const std::string newPlugin2  = "C/C++";
    const std::string newPlugin3  = "Java";
    const int newLength           = 100;
    const bool newUseSpace        = false;

    test.GetRoot()["encoding"] = newEncoding;
    int i = 0;
    test.GetRoot()["plug-ins"][i++] = newPlugin1;
    test.GetRoot()["plug-ins"][i++] = newPlugin2;
    test.GetRoot()["plug-ins"][i++] = newPlugin3;
    test.GetRoot()["indent"]["length"] = newLength;
    test.GetRoot()["indent"]["use_space"] = newUseSpace;

    std::string encoding = test.GetRoot().get("encoding", "ERROR" ).asString();
    TEST_ASSERT(encoding.compare(newEncoding) == 0);

    i = 0;
    const Json::Value plugins = test.GetRoot()["plug-ins"];
    TEST_ASSERT(plugins[i++].asString().compare(newPlugin1) == 0);
    TEST_ASSERT(plugins[i++].asString().compare(newPlugin2) == 0);
    TEST_ASSERT(plugins[i++].asString().compare(newPlugin3) == 0);

    TEST_ASSERT(test.GetRoot()["indent"].get("length", 0).asUInt() == newLength);
    TEST_ASSERT(test.GetRoot()["indent"].get("use_space", false).asBool() == newUseSpace);
}

void SFTests::TestJSONWriteToFile(void)
{
    JSONTest test;
    TEST_ASSERT(test.Read(SampleJSON.str()));

    const std::string newEncoding = "Korean";
    const std::string newPlugin1  = "PYTHON";
    const std::string newPlugin2  = "C/C++";
    const std::string newPlugin3  = "Java";
    const int newLength           = 100;
    const bool newUseSpace        = false;

    test.GetRoot()["encoding"] = newEncoding;
    int i = 0;
    test.GetRoot()["plug-ins"][i++] = newPlugin1;
    test.GetRoot()["plug-ins"][i++] = newPlugin2;
    test.GetRoot()["plug-ins"][i++] = newPlugin3;
    test.GetRoot()["indent"]["length"] = newLength;
    test.GetRoot()["indent"]["use_space"] = newUseSpace;

    std::string newFileName(SF_BINARY_ROOT_DIR);
    newFileName += "/output.json";
    TEST_ASSERT(test.WriteToFile(newFileName));

    TEST_ASSERT(test.ReadFromFile(newFileName));

    std::string encoding = test.GetRoot().get("encoding", "ERROR" ).asString();
    TEST_ASSERT(encoding.compare(newEncoding) == 0);

    i = 0;
    const Json::Value plugins = test.GetRoot()["plug-ins"];
    TEST_ASSERT(plugins[i++].asString().compare(newPlugin1) == 0);
    TEST_ASSERT(plugins[i++].asString().compare(newPlugin2) == 0);
    TEST_ASSERT(plugins[i++].asString().compare(newPlugin3) == 0);

    TEST_ASSERT(test.GetRoot()["indent"].get("length", 0).asUInt() == newLength);
    TEST_ASSERT(test.GetRoot()["indent"].get("use_space", false).asBool() == newUseSpace);
}

