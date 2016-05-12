//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : May 8, 2016
// Last revision: May 8, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"
#include "common/dict.h"
#include "safecass/event.h"

using namespace SC;

#if 0
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
#endif

TEST(Event, Constructor)
{
    const std::string name("event_name");
    const unsigned int severity = 30;
    const Event::TransitionType transition = Event::TRANSITION_N2W;

    Event e(name, severity, transition);
    EXPECT_STREQ(name.c_str(), e.GetName().c_str());
    EXPECT_EQ(severity, e.GetSeverity());
    EXPECT_TRUE(transition == e.GetTransition());
    EXPECT_EQ(0, e.GetTimestamp());
    EXPECT_TRUE(e.GetWhat().empty());
    EXPECT_EQ(false, e.IsActive());
    EXPECT_EQ(false, e.IsIgnored());

    EXPECT_TRUE(true);
}

TEST(Event, Accessors)
{
    Event e("name", 10, Event::TRANSITION_N2W);

    // Timestamp
    EXPECT_EQ(0, e.GetTimestamp());
    e.SetTimestamp(123456);
    EXPECT_EQ(123456, e.GetTimestamp());

    // What
    EXPECT_TRUE(e.GetWhat().empty());
    e.SetWhat("what");
    EXPECT_TRUE(e.GetWhat().compare("what") == 0);

    // Active
    EXPECT_EQ(false, e.IsActive());
    e.SetActive(true);
    EXPECT_EQ(true, e.IsActive());
    e.SetActive(false);
    EXPECT_EQ(false, e.IsActive());

    // Ignored
    EXPECT_EQ(false, e.IsIgnored());
    e.SetIgnored(true);
    EXPECT_EQ(true, e.IsIgnored());
    e.SetIgnored(false);
    EXPECT_EQ(false, e.IsIgnored());
}

TEST(Event, GetStateTransition)
{
    {
        Event e("name", 10, Event::TRANSITION_INVALID);
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::NORMAL));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::WARNING));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::ERROR));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::FAILURE));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::INVALID));
    }
    {
        Event e("name", 10, Event::TRANSITION_N2W);
        EXPECT_EQ(State::NORMAL_TO_WARNING, e.GetStateTransition(State::NORMAL));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::WARNING));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::ERROR));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::FAILURE));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::INVALID));
    }
    {
        Event e("name", 10, Event::TRANSITION_NW2E);
        EXPECT_EQ(State::NORMAL_TO_ERROR, e.GetStateTransition(State::NORMAL));
        EXPECT_EQ(State::WARNING_TO_ERROR, e.GetStateTransition(State::WARNING));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::ERROR));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::FAILURE));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::INVALID));
    }
    {
        Event e("name", 10, Event::TRANSITION_W2N);
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::NORMAL));
        EXPECT_EQ(State::WARNING_TO_NORMAL, e.GetStateTransition(State::WARNING));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::ERROR));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::FAILURE));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::INVALID));
    }
    {
        Event e("name", 10, Event::TRANSITION_EW2N);
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::NORMAL));
        EXPECT_EQ(State::WARNING_TO_NORMAL, e.GetStateTransition(State::WARNING));
        EXPECT_EQ(State::ERROR_TO_NORMAL, e.GetStateTransition(State::ERROR));
        EXPECT_EQ(State::ERROR_TO_NORMAL, e.GetStateTransition(State::FAILURE));
        EXPECT_EQ(State::INVALID_TRANSITION, e.GetStateTransition(State::INVALID));
    }
}

TEST(Event, SerializeJSON)
{
    Event e("name", 10, Event::TRANSITION_N2W);
    e.SetTimestamp(123456);
    e.SetWhat("what");
    e.SetIgnored();
    e.SetActive();

    Json::Value json = e.SerializeJSON();

    EXPECT_STREQ("name", json[Dict::EVENT_ATTR_NAME].asString().c_str());
    EXPECT_EQ(10, json[Dict::EVENT_ATTR_SEVERITY].asUInt());
    EXPECT_STREQ(Dict::EVENT_TRANSITION_N2W.c_str(), json[Dict::EVENT_ATTR_TRANSITION].asString().c_str());
    EXPECT_EQ(123456, json[Dict::EVENT_ATTR_TIMESTAMP].asUInt());
    EXPECT_STREQ("what", json[Dict::EVENT_ATTR_WHAT].asString().c_str());
    EXPECT_EQ(e.IsActive(), json[Dict::EVENT_ATTR_ACTIVE].asBool());
    EXPECT_EQ(e.IsIgnored(), json[Dict::EVENT_ATTR_IGNORED].asBool());
}

TEST(Event, ToStream)
{
    Event e("event_name", 10, Event::TRANSITION_N2W);
    e.SetTimestamp(123456);
    e.SetWhat("what_content");
    e.SetIgnored();
    e.SetActive();

    std::cout << e << std::endl;
}
