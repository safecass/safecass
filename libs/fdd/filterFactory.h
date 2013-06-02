/*

  Safety Framework for Component-based Robotics

  Created on: May 31, 2013

  Copyright (C) 2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _FilterFactory_h
#define _FilterFactory_h

#include <string>
#include <map>
#include "filterBase.h"

namespace SF {

class SFLIB_EXPORT FilterFactory
{
private:
    FilterFactory(void);
    ~FilterFactory(void) {}

    typedef std::map<std::string, FilterBase::CreateFilterFuncType> FactoryMapType;
    FactoryMapType FactoryMap;

public:
    static FilterFactory * GetInstance(void) {
        static FilterFactory FactoryInstance;
        return &FactoryInstance;
    }

    bool RegisterFilter(const std::string & filterName, FilterBase::CreateFilterFuncType createFunc);

    FilterBase * CreateFilter(const std::string & filterName, const JSON::JSONVALUE & jsonNode);
};

};

#endif // _FilterFactory_h
