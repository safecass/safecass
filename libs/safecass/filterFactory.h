//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2015 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : May 31, 2013
// Last revision: May 4, 2015
// Author       : Min Yang Jung (myj@jhu.edu)
//
#ifndef _FilterFactory_h
#define _FilterFactory_h

#include "filterBase.h"

namespace SC {

class SCLIB_EXPORT FilterFactory
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

    FilterBase * CreateFilter(const std::string & filterName, const JsonWrapper::JsonValue & jsonNode) const;
};

};

#endif // _FilterFactory_h
