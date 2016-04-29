//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Sep 12, 2012
// Last revision: Apr 28, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#ifndef _EventPublisherBase_h
#define _EventPublisherBase_h

#include "filterBase.h"

namespace SC {

class SCLIB_EXPORT EventPublisherBase
{
protected:
    //! Filtering type (internal or external)
    /*!
        \sa FilterBase()
    */
    FilterBase::FilteringType Type;

public:
    EventPublisherBase(FilterBase::FilteringType filteringType)
        : Type(filteringType) {}
    virtual ~EventPublisherBase() {}

    //! Publish events
    /*!
        When events are generated outside SAFECASS, this method is called to
        notify SAFECASS of the events and/or to forward the events to
        framework-specific elements to handle them in a framework-specific
        manner.  Thus, actual implementation of this method should be provided
        by a framework in use.
    */
    virtual bool PublishEvent(const std::string & eventJson) = 0;
};

};

#endif // _EventPublisherBase_h
