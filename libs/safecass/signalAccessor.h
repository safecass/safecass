//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Apr 10, 2016
// Last revision: Apr 20, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#ifndef _SignalAccessor_h
#define _SignalAccessor_h

#include <vector>
#include <boost/circular_buffer.hpp>

#include "common/common.h"

namespace SC {

//! Base class of signal accessor classes
class SignalAccessorBase
{
protected:
    //! Reference to original object associated with this signal accessor
    const ParamBase & SignalObject;
    //! Name of signal that this signal accessor is associated with
    const std::string SignalName;

public:
    SignalAccessorBase(const ParamBase & object, const std::string & name)
        : SignalObject(object), SignalName(name)
    {}
    virtual ~SignalAccessorBase() {}

    //! Returns name of signal associated with this signal accessor
    inline std::string GetSignalName(void) const { return SignalName; }

    //
    // Pure virtual methods
    //
    virtual void Push(void) = 0;
    virtual void GetValue(ParamBase & arg) const = 0;
    virtual void ToStream(std::ostream & os) const = 0;
};

inline std::ostream & operator<< (std::ostream & os, const SignalAccessorBase & accessor)
{
    accessor.ToStream(os);
    return os;
}

//! Signal accessor class
template<class T>
class SignalAccessor: public SignalAccessorBase
{
public:
    //! Typedef for base class
    typedef SignalAccessorBase BaseType;

    //! Typedefs for container access
    typedef boost::circular_buffer<T>          ContainerType;
    typedef typename ContainerType::size_type  SizeType;
    typedef typename ContainerType::value_type ValueType;
    typedef typename ContainerType::iterator   IteratorType;
    /*!
        'param_type' of boost.call_traits represents the "best" way to pass a parameter
        of type `value_type` to a method
        \sa boost/call_traits
    */
    typedef typename boost::call_traits<ValueType>::param_type ParameterType;

    //! Container maintaining snapshots of signals
    ContainerType * Container;

    //! Constructor
    SignalAccessor(const ParamBase & object, const std::string & name, size_t size)
        : SignalAccessorBase(object, name)
    {
        Container = new ContainerType(size);
    }

    //! Destructor
    ~SignalAccessor(void) {
        delete Container;
    }

    virtual void Push(void) {
        const ValueType * param = dynamic_cast<const ValueType *>(&SignalObject);
        SCASSERT(param);
        Container->push_back(*param);
    }

    void Push(ParameterType item, bool debug = false) {
        if (debug) {
            const ValueType * _obj = dynamic_cast<const ValueType*>(&item);
            SCLOG_DEBUG << "Signal accessor \"" << this->GetSignalName() << "\": push " << *_obj << std::endl;
        }
        Container->push_back(item);
        if (debug) {
            SCLOG_DEBUG << "Signal accessor \"" << this->GetSignalName() << "\": size after push = "
                        << Container->size() << std::endl;
        }
    }

    virtual void GetValue(ParamBase & arg) const {
        if (!Container) {
            arg.SetValid(false);
            return;
        }

        // circular_buffer memory layout:
        //
        // boost::circular_buffer<int> cb(3);
        // cb.push_back(1);
        // cb.push_back(2);
        // cb.push_back(3);
        // cb.push_back(4);
        //
        // assert(cb[0] == 2);
        // assert(cb[1] == 3);
        // assert(cb[2] == 4);
        //
        // assert(*cb.begin() == 2);
        // assert(cb.front() == 2);
        // assert(cb.back() == 4);

        ValueType * pArg = dynamic_cast<ValueType *>(&arg);
        SCASSERT(pArg);
        *pArg = Container->back();
    }

    //
    // Getters
    //
    inline size_t GetContainerSize(void) { return Container->size(); }

    virtual void ToStream(std::ostream & os) const {
        os << "Signal accessor \"" << this->GetSignalName() << "\": " << SignalObject
           << ", container: ";
        if (Container->empty()) {
            os << "empty";
            return;
        }
        IteratorType it = Container->begin();
        for (; it != Container->end(); ++it) {
            os << *it;
            if (it + 1 != Container->end())
                os << ", ";
        }
    }
};

};

#endif // _SignalAccessor_h
