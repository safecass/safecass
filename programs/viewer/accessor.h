//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 10, 2014
// Last revision: Jul 10, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _accessor_viewer_qt_h
#define _accessor_viewer_qt_h

#include "cisstAccessor.h"
#include "filterBase.h"

//
// Callback class for subscribers
//
class ViewerSubscriberCallback : public SF::SFCallback {
protected:
    const std::string TopicName;

public:
    ViewerSubscriberCallback(const std::string & topic) : TopicName(topic) {}
    ~ViewerSubscriberCallback() {}

    void CallbackControl(SF::Topic::Control::CategoryType category, const std::string & json);
    void CallbackData   (SF::Topic::Data::CategoryType    category, const std::string & json);
};

//
// CASROS accessors
//
class AccessorViewer : public SF::cisstAccessor {
public:
    AccessorViewer(void);
    ~AccessorViewer() {}

    // request list of all states in the Safety Coordinator
    bool RequestStateList(const std::string & safetyCoordinatorName = "*",
                          const std::string & componentName = "*") const;
};

#endif // _accessor_viewer_qt_h
