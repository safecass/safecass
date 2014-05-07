//------------------------------------------------------------------------
//
// CASROS: Component-based Architecture for Safe Robotic Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : May 31, 2013
// Last revision: May 7, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
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

    FilterBase * CreateFilter(const std::string & filterName, const JSON::JSONVALUE & jsonNode) const;
};

};

#endif // _FilterFactory_h
