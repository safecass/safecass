//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Mar 27, 2016
// Last revision: Apr 19, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
/*!
    This file implements the base class that enables any variable type that
    Eigen supports, including primitive types and all classes derived from
    EigenBase, to be used as a parameter type.  This unifying mechanism
    heavily relies on template meta programming and benefits from marvin2k's
    code.

    References:

    - Eigen class hierarchy:
        http://eigen.tuxfamily.org/dox-devel/TopicClassHierarchy.html
    - Eigen template specialization (marvin2k):
        https://github.com/marvin2k/templateSpecialization
        http://stackoverflow.com/questions/22725867/eigen-type-deduction-in-template-specialization-of-base-class
*/

#ifndef _ParamEigen_h
#define _ParamEigen_h

#include <Eigen/Core>
#include <iostream>

#include "common/common.h"
#include "safecass/paramBase.h"

namespace SC {

namespace is_eigen_dense_detail {
    typedef char yes_type;
    typedef int no_type;
    template <typename T>
    yes_type test(const Eigen::DenseBase<T>*);
    no_type test(...);
}

template <typename T>
struct is_eigen_matrix {
    static const bool value =
        (sizeof(is_eigen_dense_detail::test(static_cast<T*>(0))) ==
         sizeof(is_eigen_dense_detail::yes_type));
};

// std::enable_if is C++11. In C++03, either substitute boost::enable_if or just define and use your own:
template <bool dummyCheck, typename T = void>
struct enable_if {};
template <typename T>
struct enable_if<true, T> {
    typedef T type;
// the "false" version is actually not implemented, so this can not be used during compile
// time and is ignored
};

// the interface and common functionality
template<typename T>
class ParamEigenBase: public ParamBase
{
protected:
    ParamEigenBase(const T& val): ParamBase(), Val(val) {}

public:
    T Val;

    // don't test self-assignment, causes hopefully no problem here
    // what about complex eigen-types like Eigen::Block<>?
    // const T& operator=(const T& other) {
        // this->Val = other.Val;
    // }

    // Reset value of this container
    inline void setZero() { Val = static_cast<T>(0); }

    // pretty printing
    // template<typename P>
    // friend std::ostream & operator<<(std::ostream& os, const ParamEigenBase<P>& other);

    virtual ParamBase * Clone(void) const {
        SCASSERT(false); // This method must not be called
        return 0;
    }
};

template<typename P>
inline std::ostream & operator<< (std::ostream & os, const ParamEigenBase<P> & paramEigenBase)
{
    os << paramEigenBase.Val;
    return os;
}

// Template structs to determine numeric types
template <typename T> struct IsNum {
    enum { Yes = 0, No = 1 };
};

#define REGISTER_PRIMITIVE_TYPE( _type )\
template <> struct IsNum<_type> {\
    enum { Yes = 1, No = 0 };\
};

REGISTER_PRIMITIVE_TYPE(bool);
REGISTER_PRIMITIVE_TYPE(char);
REGISTER_PRIMITIVE_TYPE(unsigned char);
REGISTER_PRIMITIVE_TYPE(signed char);
REGISTER_PRIMITIVE_TYPE(short);
REGISTER_PRIMITIVE_TYPE(unsigned short);
REGISTER_PRIMITIVE_TYPE(int);
REGISTER_PRIMITIVE_TYPE(unsigned int);
REGISTER_PRIMITIVE_TYPE(long);
REGISTER_PRIMITIVE_TYPE(unsigned long);
REGISTER_PRIMITIVE_TYPE(long long);
REGISTER_PRIMITIVE_TYPE(unsigned long long);
REGISTER_PRIMITIVE_TYPE(float);
REGISTER_PRIMITIVE_TYPE(double);

// Template specialization for collections and numeric types
template <typename T, bool num = false> struct Printer {
    void Print(const T & t, std::ostream & os = std::cout) {
        typename T::const_iterator it = t.begin();
        for (; it != t.end(); ++it)
            os << " " << *it;
    }
};

template <typename T> struct Printer<T, true> {
    void Print(const T & t, std::ostream & os = std::cout) {
        os << " " << t;
    }
};

// the common child-template class, using the "Enable" trick
template <typename T, typename Enable = void>
class ParamEigen: public ParamEigenBase<T>
{
public:
    typedef ParamEigenBase<T> BaseType;

    ParamEigen(const T & val): ParamEigenBase<T>(val) {}
    ParamEigen(void): ParamEigenBase<T>(static_cast<T>(0)) {}

    //
    // Operator overloading for convenience
    //
    const ParamEigen & operator=(const T & val) {
        this->Val = val;
        return *this;
    }

    inline bool operator==(const ParamEigen & rhs) {
        return (this->Val == rhs.Val);
    }
    inline bool operator!=(const ParamEigen & rhs) {
        return !(*this == rhs);
    }

    virtual void ToStream(std::ostream & os) const {
        // TODO: maybe type information can be printed out as well
        BaseType::ToStream(os);
        // printer is instantiated depending on type
        Printer<T, IsNum<T>::Yes> printer;
        printer.Print(this->Val);
    }

    virtual ParamBase * Clone(void) const {
        return new ParamEigen<T>(this->Val);
    }
};

#if 0
// specific types are changed. like short
class ParamEigenShort : public ParamEigenBase<short>
{
    public:
        ParamEigenShort(const short& val)
            :ParamEigenBase<short>(val) {}
        ParamEigenShort()
            :ParamEigenBase<short>(0){}
        //void increase() { Val = Val*11; }
};
#endif

//  For 3x3 matrix,
//    1.11    2 3.33
//       4    5    6
//       7 8.89    9
//
//  Reference:
//    Eigen IOFormat class reference:
//    (https://eigen.tuxfamily.org/dox/structEigen_1_1IOFormat.html)
typedef enum {
    // << 1.11, 2, 3.33,  4, 5, 6,  7, 8.89, 9;
    EIGEN_IOFORMAT_COMMA,

    //   [1.111,     2, 3.333]
    //   [    4,     5,     6]
    //   [    7, 8.889,     9]
    EIGEN_IOFORMAT_CLEAN,

    //   [1.11,    2, 3.33;
    //       4,    5,    6;
    //       7, 8.89,    9]
    EIGEN_IOFORMAT_OCTAVE,

    // [[1.111111,        2,  3.33333];
    // [       4,        5,        6];
    // [       7, 8.888888,        9]]
    EIGEN_IOFORMAT_HEAVY
} EigenOutputFormatType;

/*!
    Specialization that exists only for cases where T is, or is derived from,
    an Eigen::DenseBase specialization.
*/
template <typename T>
class ParamEigen<T, typename enable_if<is_eigen_matrix<T>::value>::type>
    : public ParamEigenBase<T>
{
public:
    typedef ParamEigenBase<T> BaseType;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    ParamEigen(const T& val): ParamEigenBase<T>(val) {}
    ParamEigen(void): ParamEigenBase<T>(T::Zero()) {}

    void setZero() { ParamEigenBase<T>::Val.setZero(); }

    virtual void ToStream(std::ostream & os, EigenOutputFormatType format = EIGEN_IOFORMAT_CLEAN) const {
        BaseType::ToStream(os);
        os << ", ";
        PrintEigen(*this, format, os);
    }

    virtual ParamBase * Clone(void) const {
        return new ParamEigen<T>(this->Val);
    }
};

template<typename T>
inline std::ostream & operator<< (std::ostream & os, const ParamEigen<T> & param)
{
    param.ToStream(os);
    return os;
}

template<typename T>
void PrintEigen(const ParamEigenBase<T> & param,
                EigenOutputFormatType format = EIGEN_IOFORMAT_CLEAN,
                std::ostream & os = std::cout)
{
    static const Eigen::IOFormat CommaInitFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", " << ", ";");
    static const Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    static const Eigen::IOFormat OctaveFmt(Eigen::StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");
    static const Eigen::IOFormat HeavyFmt(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");

    os << std::endl;
    switch (format) {
    default:
    case EIGEN_IOFORMAT_COMMA:  os << param.Val.format(CommaInitFmt); break;
    case EIGEN_IOFORMAT_CLEAN:  os << param.Val.format(CleanFmt);     break;
    case EIGEN_IOFORMAT_OCTAVE: os << param.Val.format(OctaveFmt);    break;
    case EIGEN_IOFORMAT_HEAVY:  os << param.Val.format(HeavyFmt);     break;
    }
    os << std::endl;
}

}; // SC

#endif // _ParamEigen_h
