//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Jul 6, 2012
// Last revision: Apr 26, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"
#include "common/jsonwrapper.h"

#include <cstdio> // remove

using namespace SC;

const std::string VALUE_ENCODING = "UTF-8";
const std::string VALUE_PLUGIN1 = "python";
const std::string VALUE_PLUGIN2 = "c++";
const std::string VALUE_PLUGIN3 = "ruby";
const unsigned int VALUE_LENGTH = 3;
const bool VALUE_USE_SPACE = true;

// Define test class derived from Jsonand testing::Test
class JsonWrapperTest: public JsonWrapper, public testing::Test
{
protected:
    std::string Sample;

    virtual void SetUp() {
        std::stringstream ss;
        // Sample JSON from JsonCpp documentation:
        ss << "// Configuration options\n"
           << "{\n"
           << "   // Default encoding for text\n"
           << "   \"encoding\" : \"" << VALUE_ENCODING << "\",\n"
           << "\n"
           << "   // Plug-ins loaded at start-up\n"
           << "   \"plug-ins\" : [\n"
           << "       \"" << VALUE_PLUGIN1 << "\",\n"
           << "       \"" << VALUE_PLUGIN2 << "\",\n"
           << "       \"" << VALUE_PLUGIN3 << "\"\n"
           << "       ],\n"
           << "       \n"
           << "   // Tab indent size\n"
           << "   \"indent\" : { \"length\" : " << VALUE_LENGTH
           << ", \"use_space\": " << (VALUE_USE_SPACE ? "true" : "false") << " }\n"
           << "}\n";

        Sample = ss.str();
    }

    virtual void TearDown() {
    };
};

TEST_F(JsonWrapperTest, Read)
{
    EXPECT_TRUE(Read(Sample));

    std::string encoding = GetJsonRoot().get("encoding", "ERROR" ).asString();
    EXPECT_TRUE(encoding.compare("UTF-8") == 0);

    int i = 0;
    const Json::Value plugins = GetJsonRoot()["plug-ins"];
    EXPECT_TRUE(plugins[i++].asString().compare(VALUE_PLUGIN1) == 0);
    EXPECT_TRUE(plugins[i++].asString().compare(VALUE_PLUGIN2) == 0);
    EXPECT_TRUE(plugins[i++].asString().compare(VALUE_PLUGIN3) == 0);

    EXPECT_TRUE(GetJsonRoot()["indent"].get("length", 0).asUInt() == VALUE_LENGTH);
    EXPECT_TRUE(GetJsonRoot()["indent"].get("use_space", false).asBool() == VALUE_USE_SPACE);
}

TEST_F(JsonWrapperTest, ReadFromFile)
{
    EXPECT_TRUE(ReadFromFile(SC_SOURCE_ROOT_DIR"/tests/sample.json"));

    std::string encoding = GetJsonRoot().get("encoding", "ERROR" ).asString();
    EXPECT_TRUE(encoding.compare("UTF-8") == 0);

    int i = 0;
    const Json::Value plugins = GetJsonRoot()["plug-ins"];
    EXPECT_TRUE(plugins[i++].asString().compare(VALUE_PLUGIN1) == 0);
    EXPECT_TRUE(plugins[i++].asString().compare(VALUE_PLUGIN2) == 0);
    EXPECT_TRUE(plugins[i++].asString().compare(VALUE_PLUGIN3) == 0);

    EXPECT_TRUE(GetJsonRoot()["indent"].get("length", 0).asUInt() == VALUE_LENGTH);
    EXPECT_TRUE(GetJsonRoot()["indent"].get("use_space", false).asBool() == VALUE_USE_SPACE);
}

TEST_F(JsonWrapperTest, Write)
{
    EXPECT_TRUE(Read(Sample));

    const std::string newEncoding = "Korean";
    const std::string newPlugin1  = "PYTHON";
    const std::string newPlugin2  = "C/C++";
    const std::string newPlugin3  = "Java";
    const int newLength           = 100;
    const bool newUseSpace        = false;

    GetJsonRoot()["encoding"] = newEncoding;
    int i = 0;
    GetJsonRoot()["plug-ins"][i++] = newPlugin1;
    GetJsonRoot()["plug-ins"][i++] = newPlugin2;
    GetJsonRoot()["plug-ins"][i++] = newPlugin3;
    GetJsonRoot()["indent"]["length"] = newLength;
    GetJsonRoot()["indent"]["use_space"] = newUseSpace;

    std::string encoding = GetJsonRoot().get("encoding", "ERROR" ).asString();
    EXPECT_TRUE(encoding.compare(newEncoding) == 0);

    i = 0;
    const Json::Value plugins = GetJsonRoot()["plug-ins"];
    EXPECT_TRUE(plugins[i++].asString().compare(newPlugin1) == 0);
    EXPECT_TRUE(plugins[i++].asString().compare(newPlugin2) == 0);
    EXPECT_TRUE(plugins[i++].asString().compare(newPlugin3) == 0);

    EXPECT_TRUE(GetJsonRoot()["indent"].get("length", 0).asUInt() == newLength);
    EXPECT_TRUE(GetJsonRoot()["indent"].get("use_space", false).asBool() == newUseSpace);
}

TEST_F(JsonWrapperTest, WriteToFile)
{
    EXPECT_TRUE(Read(Sample));

    const std::string newEncoding = "Korean";
    const std::string newPlugin1  = "PYTHON";
    const std::string newPlugin2  = "C/C++";
    const std::string newPlugin3  = "Java";
    const int newLength           = 100;
    const bool newUseSpace        = false;

    GetJsonRoot()["encoding"] = newEncoding;
    int i = 0;
    GetJsonRoot()["plug-ins"][i++] = newPlugin1;
    GetJsonRoot()["plug-ins"][i++] = newPlugin2;
    GetJsonRoot()["plug-ins"][i++] = newPlugin3;
    GetJsonRoot()["indent"]["length"] = newLength;
    GetJsonRoot()["indent"]["use_space"] = newUseSpace;

    std::string newFileName(SC_BUILD_ROOT_DIR);
    newFileName += "/output.json";
    EXPECT_TRUE(WriteToFile(newFileName));

    EXPECT_TRUE(ReadFromFile(newFileName));

    std::string encoding = GetJsonRoot().get("encoding", "ERROR" ).asString();
    EXPECT_TRUE(encoding.compare(newEncoding) == 0);

    i = 0;
    const Json::Value plugins = GetJsonRoot()["plug-ins"];
    EXPECT_TRUE(plugins[i++].asString().compare(newPlugin1) == 0);
    EXPECT_TRUE(plugins[i++].asString().compare(newPlugin2) == 0);
    EXPECT_TRUE(plugins[i++].asString().compare(newPlugin3) == 0);

    EXPECT_TRUE(GetJsonRoot()["indent"].get("length", 0).asUInt() == newLength);
    EXPECT_TRUE(GetJsonRoot()["indent"].get("use_space", false).asBool() == newUseSpace);

    // Clean up intermediate output file
    EXPECT_TRUE(remove(newFileName.c_str()) == 0);
}
