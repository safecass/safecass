/*

  Safety Framework for Component-based Robotics

  Created on: September 12, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _EventPublisherBase_h
#define _EventPublisherBase_h

#include "filterBase.h"

namespace SF {

class SFLIB_EXPORT EventPublisherBase
{
protected:
    /*! Filtering type (active or passive) */
    FilterBase::FilteringType Type;

public:
    EventPublisherBase(FilterBase::FilteringType filteringType)
        : Type(filteringType) {}
    virtual ~EventPublisherBase() {}

    /*! Publish an event directly to the Safety Framework or pass it to a 
        middleware-specific object that publishes the event.  A middleware needs 
        to provide plug-in that actually implements this method. */
    virtual bool PublishEvent(const std::string & eventDescriptionJSON) = 0;
};

};

#endif // _EventPublisherBase_h
