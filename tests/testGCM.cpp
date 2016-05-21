//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : May 15, 2016
// Last revision: May 21, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"
#include "config.h"
#include "safecass/gcm.h"

using namespace SC;

TEST(GCM, Constructor)
{
    GCM gcm("coordinator_name", "component_name");
    EXPECT_STREQ("coordinator_name", gcm.GetCoordinatorName().c_str());
    EXPECT_STREQ("component_name", gcm.GetComponentName().c_str());
}

TEST(GCM, Accessors)
{
}

TEST(GCM, ToStream_HumanReadable)
{
    GCM gcm("coordinator_name", "component_name");
    std::cout << gcm << std::endl;
}

TEST(GCM, ToStream_GraphViz)
{
    GCM gcm("coordinator_name", "component_name");
    gcm.ToStream(std::cout, GCM::EXPORT_GRAPHVIZ);
}

TEST(GCM, ExportToGraphViz)
{
    GCM gcm("coordinator_name", "component_name");
    EXPECT_TRUE(gcm.ExportToGraphViz(SC_BUILD_ROOT_DIR "/graphviz-test"));
}
