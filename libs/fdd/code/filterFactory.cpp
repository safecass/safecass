/*

  Safety Framework for Component-based Robotics

  Created on: May 31, 2013

  Copyright (C) 2013 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "filterFactory.h"
#include "threshold.h"

namespace SF {

FilterFactory::FilterFactory(void)
{
    // obsolete: replaced with SF_REGISTER_FILTER_TO_FACTORY macro
    //RegisterFilter(FilterThreshold::Name, FilterThreshold::Create);
}

bool FilterFactory::RegisterFilter(const std::string & filterName, 
                                   FilterBase::CreateFilterFuncType createFunc)
{
    if (FactoryMap.find(filterName) != FactoryMap.end())
        return false;

    FactoryMap[filterName] = createFunc;

    FactoryMapType::const_iterator it = FactoryMap.begin();
    for (; it != FactoryMap.end(); ++it)
        SFLOG_DEBUG << "FACTORY MAP: " << it->first << std::endl;

    return true;
}

FilterBase * FilterFactory::CreateFilter(const std::string & filterName,
                                         const JSON::JSONVALUE & jsonNode)
{
    FactoryMapType::const_iterator it = FactoryMap.find(filterName);

    if (it != FactoryMap.end())
        return (it->second)(jsonNode);

    return 0;
}

};
