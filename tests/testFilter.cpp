//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Apr 28, 2016
// Last revision: Apr 28, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"
#include "safecass/filterBase.h"

using namespace SC;

// Define mock-up filter class derived from FilterBase
// (FilterBase defines four pure virtual methods)
class FilterTest: public FilterBase
{
public:
    FilterTest(const std::string &        filterName,
               BaseType::FilteringType    filteringType,
               BaseType::StateMachineInfo stateMachine,
               EventDetectionModeType     eventDetectionMode)
        : FilterBase(filterName, filteringType,
                     stateMachine.StateMachineType,
                     stateMachine.ComponentName,
                     stateMachine.InterfaceName,
                     eventDetectionMode)
    {}

    ~FilterTest(void) {}

    bool ConfigureFilter(const Json::Value & jsonNode) { return true; }
    bool InitFilter(void) { return true; }
    void RunFilter(void) {}
    void CleanupFilter(void) {}
};

TEST(FilterBase, Initialization)
{
    FilterBase::StateMachineInfo info;
    info.StateMachineType = State::STATEMACHINE_FRAMEWORK;
    info.ComponentName = "aComponent";
    info.InterfaceName = "aInterface";

    FilterTest filter("filterTest",
                      FilterBase::FILTERING_INTERNAL,
                      info,
                      FilterBase::EVENT_DETECTION_EDGE);
}
