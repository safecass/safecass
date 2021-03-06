//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 31, 2013
// Last revision: Aug 20, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#include "filterFactory.h"
#include "threshold.h"
#include "changeDetection.h"
#include "onOff.h"
#include "nop.h"

namespace SC {

FilterFactory::FilterFactory(void)
{
    // Register filters that casros defines.  Application-specific filters should
    // be registered using SC_REGISTER_FILTER_TO_FACTORY macro, e.g.,
    // SC_REGISTER_FILTER_TO_FACTORY(FilterMyApplication);

#define REGISTER_FILTER(_name)\
    RegisterFilter(_name::Name, _name::Create);
    REGISTER_FILTER(FilterThreshold);
    //REGISTER_FILTER(FilterChangeDetection);
    REGISTER_FILTER(FilterOnOff);
    REGISTER_FILTER(FilterNOP);
#undef REGISTER_FILTER
}

bool FilterFactory::RegisterFilter(const std::string & filterName, 
                                   FilterBase::CreateFilterFuncType createFunc)
{
    SCLOG_DEBUG << "FilterFactory::RegisterFilter: " << (double*) this << std::endl;
    SCLOG_DEBUG << "FilterFactory::RegisterFilter: registering filter: \"" << filterName  << "\""
                << ", " << (double*)createFunc << std::endl;

    if (FactoryMap.find(filterName) != FactoryMap.end()) {
        SCLOG_DEBUG << "FilterFactory::RegisterFilter: already registered filter: \"" << filterName  << "\"" << std::endl;
        return false;
    }

    FactoryMap.insert(std::make_pair(filterName, createFunc));

#if 1
    FactoryMapType::const_iterator it = FactoryMap.find(filterName);
    SCASSERT(it != FactoryMap.end());

    SCLOG_DEBUG << "FilterFactory::RegisterFilter: registered filter: \"" << it->first << "\""
                << ", " << (double*)it->second << std::endl;
#endif

    return true;
}

FilterBase * FilterFactory::CreateFilter(const std::string & filterName,
                                         const JsonWrapper::JsonValue & jsonNode) const
{
    SCLOG_DEBUG << "FilterFactory::CreateFilter: " << (double*) this << std::endl;
    SCLOG_DEBUG << "FilterFactory::CreateFilter: creating filter: \"" << filterName  << "\"" << std::endl;
                //<< " with JSON: " << jsonNode << std::endl;

    FactoryMapType::const_iterator it = FactoryMap.find(filterName);
    if (it == FactoryMap.end()) {
        // try again with lower case
        std::string _filterName(filterName);
        SC::to_lowercase(_filterName);
        it = FactoryMap.find(filterName);
    }

    if (it == FactoryMap.end()) {
        FactoryMapType::const_iterator it2 = FactoryMap.begin();
        SCLOG_DEBUG << "FACTORY MAP SIZE: " << FactoryMap.size() << std::endl;
        for (; it2 != FactoryMap.end(); ++it2)
            SCLOG_DEBUG << "FACTORY MAP: " << it2->first << ", " << (double*)it2->second << std::endl;

        SCLOG_ERROR << "FilterFactory::CreateFilter: no filter found: \"" << filterName  << "\"" << std::endl;
        return 0;
    }

    SCLOG_DEBUG << "FilterFactory::CreateFilter: filter found: \"" << filterName  << "\"" << std::endl;

    return (it->second)(jsonNode);
}

};
