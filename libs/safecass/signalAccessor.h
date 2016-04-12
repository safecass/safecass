//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Apr 10, 2016
// Last revision: Apr 10, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#ifndef _SignalAccessor_h
#define _SignalAccessor_h

#include <vector>
#include <boost/circular_buffer.hpp>

namespace SC {

//! Base class of signal accessor classes
class SignalAccessorBase
{
public:
    typedef std::vector<SignalAccessorBase *> SignalsType;

public:
    SignalAccessorBase() {}
    virtual ~SignalAccessorBase() {}

    virtual void ToStream(std::ostream & os) const = 0;
};

//! Signal accessor class
template<class T>
class SignalAccessor: public SignalAccessorBase
{
public:
    //! Typedefs
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
    SignalAccessor(size_t size) {
        Container = new ContainerType(size);
    }

    //! Destructor
    ~SignalAccessor(void) {
        delete Container;
    }

    inline void Push(ParameterType item) {
        Container->push_back(item);
    }

    virtual void ToStream(std::ostream & os) const {
        // BaseType::ToStream(os);
        // os << ", ";
        std::cout << "signal derived" << std::endl;
        IteratorType it = Container->begin();
        const IteratorType itEnd = Container->end();
        for (; it != itEnd; ++it)
            os << *it << std::endl;
    }
};

template<typename T>
inline std::ostream & operator<< (std::ostream & os, const SignalAccessor<T> & accessor)
{
    accessor.ToStream(os);
    return os;
}

};

#endif // _SignalAccessor_h
