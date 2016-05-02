//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Apr 28, 2016
// Last revision: May 1, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"
#include "common/dict.h"
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
        : FilterBase(filterName,
                     filteringType,
                     stateMachine.GetStateMachineType(),
                     stateMachine.GetComponentName(),
                     stateMachine.GetInterfaceName(),
                     eventDetectionMode)
    {}

    FilterTest(const std::string & filterName, const Json::Value & json)
        : FilterBase(filterName, json)
    {}

    ~FilterTest(void) {}

    bool ConfigureFilter(const Json::Value & jsonNode) { return true; }
    bool InitFilter(void) { return true; }
    void RunFilter(void) {}
    void CleanupFilter(void) {}
};

TEST(FilterBase, StateMachineInfo1)
{
    FilterBase::StateMachineInfo info1(State::STATEMACHINE_INVALID,   "aComponent1", "aInterface1");
    EXPECT_TRUE(info1.GetStateMachineType() == State::STATEMACHINE_INVALID);
    EXPECT_TRUE(info1.GetComponentName().compare("aComponent1") == 0);
    EXPECT_TRUE(info1.GetInterfaceName().empty());

    FilterBase::StateMachineInfo info2(State::STATEMACHINE_FRAMEWORK, "aComponent2", "aInterface2");
    EXPECT_TRUE(info2.GetStateMachineType() == State::STATEMACHINE_FRAMEWORK);
    EXPECT_TRUE(info2.GetComponentName().compare("aComponent2") == 0);
    EXPECT_TRUE(info2.GetInterfaceName().empty());

    FilterBase::StateMachineInfo info3(State::STATEMACHINE_APP, "aComponent3", "aInterface3");
    EXPECT_TRUE(info3.GetStateMachineType() == State::STATEMACHINE_APP);
    EXPECT_TRUE(info3.GetComponentName().compare("aComponent3") == 0);
    EXPECT_TRUE(info3.GetInterfaceName().empty());

    FilterBase::StateMachineInfo info4(State::STATEMACHINE_PROVIDED,  "aComponent4", "aInterface4");
    EXPECT_TRUE(info4.GetStateMachineType() == State::STATEMACHINE_PROVIDED);
    EXPECT_TRUE(info4.GetComponentName().compare("aComponent4") == 0);
    EXPECT_TRUE(info4.GetInterfaceName().compare("aInterface4") == 0);

    FilterBase::StateMachineInfo info5(State::STATEMACHINE_REQUIRED,  "aComponent5", "aInterface5");
    EXPECT_TRUE(info5.GetStateMachineType() == State::STATEMACHINE_REQUIRED);
    EXPECT_TRUE(info5.GetComponentName().compare("aComponent5") == 0);
    EXPECT_TRUE(info5.GetInterfaceName().compare("aInterface5") == 0);
}

TEST(FilterBase, StateMachineInfo2)
{
    FilterBase::StateMachineInfo info1("invalid_statemachine_type_string", "aComponent1", "aInterface1");
    EXPECT_TRUE(info1.GetStateMachineType() == State::STATEMACHINE_INVALID);
    EXPECT_TRUE(info1.GetComponentName().compare("aComponent1") == 0);
    EXPECT_TRUE(info1.GetInterfaceName().empty());

    FilterBase::StateMachineInfo info2("s_F", "aComponent2", "aInterface2");
    EXPECT_TRUE(info2.GetStateMachineType() == State::STATEMACHINE_FRAMEWORK);
    EXPECT_TRUE(info2.GetComponentName().compare("aComponent2") == 0);
    EXPECT_TRUE(info2.GetInterfaceName().empty());

    FilterBase::StateMachineInfo info3("s_A", "aComponent3", "aInterface3");
    EXPECT_TRUE(info3.GetStateMachineType() == State::STATEMACHINE_APP);
    EXPECT_TRUE(info3.GetComponentName().compare("aComponent3") == 0);
    EXPECT_TRUE(info3.GetInterfaceName().empty());

    FilterBase::StateMachineInfo info4("s_P", "aComponent4", "aInterface4");
    EXPECT_TRUE(info4.GetStateMachineType() == State::STATEMACHINE_PROVIDED);
    EXPECT_TRUE(info4.GetComponentName().compare("aComponent4") == 0);
    EXPECT_TRUE(info4.GetInterfaceName().compare("aInterface4") == 0);

    FilterBase::StateMachineInfo info5("s_R", "aComponent5", "aInterface5");
    EXPECT_TRUE(info5.GetStateMachineType() == State::STATEMACHINE_REQUIRED);
    EXPECT_TRUE(info5.GetComponentName().compare("aComponent5") == 0);
    EXPECT_TRUE(info5.GetInterfaceName().compare("aInterface5") == 0);
}

// Check initial conditions
TEST(FilterBase, Constructors)
{
    const std::string name1("filterTest1");
    FilterBase::StateMachineInfo info1(State::STATEMACHINE_FRAMEWORK, "aComponent1", "aInterface1");
    FilterTest filter1(name1,
                       FilterBase::FILTERING_INTERNAL,
                       info1,
                       FilterBase::EVENT_DETECTION_EDGE);
    FilterBase::FilterIDType fid1 = filter1.GetFilterID();

    const std::string name2("filterTest2");
    FilterBase::StateMachineInfo info2(State::STATEMACHINE_PROVIDED, "aComponent2", "aInterface2");
    FilterTest filter2(name2,
                       FilterBase::FILTERING_EXTERNAL,
                       info2,
                       FilterBase::EVENT_DETECTION_LEVEL);
    FilterBase::FilterIDType fid2 = filter2.GetFilterID();

    const std::string name3("filterTest3");
    FilterBase::StateMachineInfo info3(State::STATEMACHINE_PROVIDED, "aComponent3", "aInterface3");
    Json::Value json;
    json["target"]["type"] = "s_P";
    json["target"]["component"] = "aComponent3";
    json["target"]["interface"] = "aInterface3";
    json["type"] = "internal";
    json["event_type"] = Dict::EVENT_DETECTION_LEVEL;

    FilterTest filter3(name3, json);
    FilterBase::FilterIDType fid3 = filter3.GetFilterID();

    // Filter UID
    EXPECT_EQ(fid1 + 1, fid2);
    EXPECT_EQ(fid2 + 1, fid3);

    // Filter Name
    EXPECT_TRUE(filter1.GetFilterName().compare(name1) == 0);
    EXPECT_TRUE(filter2.GetFilterName().compare(name2) == 0);
    EXPECT_TRUE(filter3.GetFilterName().compare(name3) == 0);

    // Registered statemachine
    EXPECT_TRUE(filter1.GetStateMachineInfo() == info1);
    EXPECT_TRUE(filter2.GetStateMachineInfo() == info2);
    EXPECT_TRUE(filter3.GetStateMachineInfo() == info3);

    // Filtering type
    EXPECT_TRUE(filter1.GetFilteringType() == FilterBase::FILTERING_INTERNAL);
    EXPECT_TRUE(filter2.GetFilteringType() == FilterBase::FILTERING_EXTERNAL);
    EXPECT_TRUE(filter3.GetFilteringType() == FilterBase::FILTERING_INTERNAL);

    // Filter State
    EXPECT_TRUE(filter1.GetFilterState() == FilterBase::STATE_INIT);
    EXPECT_TRUE(filter2.GetFilterState() == FilterBase::STATE_INIT);
    EXPECT_TRUE(filter3.GetFilterState() == FilterBase::STATE_INIT);

    // Event detection mode
    EXPECT_TRUE(filter1.GetEventDetectionMode() == FilterBase::EVENT_DETECTION_EDGE);
    EXPECT_TRUE(filter2.GetEventDetectionMode() == FilterBase::EVENT_DETECTION_LEVEL);
    EXPECT_TRUE(filter3.GetEventDetectionMode() == FilterBase::EVENT_DETECTION_LEVEL);

    // Shallow fault injection queue
    EXPECT_EQ(0, filter1.GetShallowFaultInjectionQueueSize());
    EXPECT_EQ(0, filter2.GetShallowFaultInjectionQueueSize());
    EXPECT_EQ(0, filter3.GetShallowFaultInjectionQueueSize());

    // Safety coordinator
    EXPECT_EQ(0, filter1.GetSafetyCoordinator());
    EXPECT_EQ(0, filter2.GetSafetyCoordinator());
    EXPECT_EQ(0, filter3.GetSafetyCoordinator());

    // Signals
    EXPECT_EQ(0, filter1.GetNumberOfInputSignal());
    EXPECT_EQ(0, filter2.GetNumberOfOutputSignal());
    EXPECT_EQ(0, filter3.GetNumberOfOutputSignal());
}

TEST(FilterBase, AddInputSignal)
{
    const std::string filterName("filterTest");
    FilterBase::StateMachineInfo info(State::STATEMACHINE_FRAMEWORK, "aComponent1", "aInterface1");
    FilterTest filter(filterName,
                      FilterBase::FILTERING_INTERNAL,
                      info,
                      FilterBase::EVENT_DETECTION_EDGE);

    ParamEigen<int> signal;
    const std::string name("signalInt");

    EXPECT_EQ(0, filter.GetNumberOfInputSignal());
    EXPECT_EQ(0, filter.GetNumberOfOutputSignal());

    EXPECT_TRUE(filter.AddInputSignal(signal, name));
    EXPECT_EQ(1, filter.GetNumberOfInputSignal());
    EXPECT_EQ(0, filter.GetNumberOfOutputSignal());

    EXPECT_FALSE(filter.AddInputSignal(signal, name));
    EXPECT_EQ(1, filter.GetNumberOfInputSignal());
    EXPECT_EQ(0, filter.GetNumberOfOutputSignal());
}

TEST(FilterBase, AddOutputSignal)
{
    const std::string filterName("filterTest");
    FilterBase::StateMachineInfo info(State::STATEMACHINE_FRAMEWORK, "aComponent", "aInterface");
    FilterTest filter(filterName,
                      FilterBase::FILTERING_INTERNAL,
                      info,
                      FilterBase::EVENT_DETECTION_EDGE);

    ParamEigen<double> signal;
    const std::string name("signalDouble");

    EXPECT_EQ(0, filter.GetNumberOfInputSignal());
    EXPECT_EQ(0, filter.GetNumberOfOutputSignal());

    EXPECT_TRUE(filter.AddOutputSignal(signal, name));
    EXPECT_EQ(0, filter.GetNumberOfInputSignal());
    EXPECT_EQ(1, filter.GetNumberOfOutputSignal());

    EXPECT_FALSE(filter.AddOutputSignal(signal, name));
    EXPECT_EQ(0, filter.GetNumberOfInputSignal());
    EXPECT_EQ(1, filter.GetNumberOfOutputSignal());
}

TEST(FilterBase, GetInputSignalElement)
{
    const std::string filterName("filterTest");
    FilterBase::StateMachineInfo info(State::STATEMACHINE_FRAMEWORK, "aComponent", "aInterface");
    FilterTest filter(filterName,
                      FilterBase::FILTERING_INTERNAL,
                      info,
                      FilterBase::EVENT_DETECTION_EDGE);

    ParamEigen<int> signalInt;
    ParamEigen<Eigen::Array33d> signalEigenArray33d;

    EXPECT_EQ(0, filter.GetNumberOfInputSignal());
    EXPECT_TRUE(filter.AddInputSignal(signalInt, "signalInt"));
    EXPECT_EQ(1, filter.GetNumberOfInputSignal());
    EXPECT_TRUE(filter.AddInputSignal(signalEigenArray33d, "signalEigenArray33d"));
    EXPECT_EQ(2, filter.GetNumberOfInputSignal());

    SignalElement * s1 = filter.GetInputSignalElement(0);
    EXPECT_TRUE(s1);
    EXPECT_TRUE(s1->GetName().compare("signalInt") == 0);
    ParamEigen<int> * signalIntCopy = dynamic_cast<ParamEigen<int>* >(s1->GetParamPrototype());
    EXPECT_TRUE(signalIntCopy);
    EXPECT_TRUE(signalInt == *signalIntCopy);
    // signalInt is not added to history buffer, and thus its signal index in
    // the history buffer should be invalid.
    EXPECT_EQ(HistoryBufferBase::INVALID_SIGNAL_INDEX, s1->GetSignalIndex());
}
