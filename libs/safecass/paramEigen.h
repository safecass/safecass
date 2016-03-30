//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Mar 27, 2016
// Last revision: Mar 29, 2016
// Author       : Min Yang Jung (myj@jhu.edu)
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
class ParamEigenBase
{
public:
    /*!
        For 3x3 matrix,

            1.11    2 3.33
            4    5    6
            7 8.89    9

        Reference:
            Eigen IOFormat class reference:
            (available at https://eigen.tuxfamily.org/dox/structEigen_1_1IOFormat.html)
    */
    typedef enum {
        /*!
            << 1.11, 2, 3.33,  4, 5, 6,  7, 8.89, 9;
        */
        EIGEN_IOFORMAT_COMMA,

        /*!
            [1.111,     2, 3.333]
            [    4,     5,     6]
            [    7, 8.889,     9]
        */
        EIGEN_IOFORMAT_CLEAN,

        /*
            [1.11,    2, 3.33;
                4,    5,    6;
                7, 8.89,    9]
        */
        EIGEN_IOFORMAT_OCTAVE,

        /*
            [[1.111111,        2,  3.33333];
            [       4,        5,        6];
            [       7, 8.888888,        9]]
        */
        EIGEN_IOFORMAT_HEAVY
    } EigenOutputFormatType;

protected:
    ParamEigenBase(const T& val): val(val) {}

public:
    T val;

    // don't test self-assignment, causes hopefully no problem here
    // what about complex eigen-types like Eigen::Block<>?
    const T& operator=(const T& other) { this->val = other.val; };

    // Reset value of this container
    inline void setZero() { val = static_cast<T>(0); }

    // pretty printing
    template<typename P>
    friend std::ostream& operator<<(std::ostream& os, const ParamEigenBase<P>& other);

    // formatted print
    void print(EigenOutputFormatType format, std::ostream & os)
    {
        Eigen::IOFormat CommaInitFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", " << ", ";");
        Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
        Eigen::IOFormat OctaveFmt(Eigen::StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");
        Eigen::IOFormat HeavyFmt(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");

        switch (format) {
        default:
        case EIGEN_IOFORMAT_COMMA:  os << val.format(CommaInitFmt); break;
        case EIGEN_IOFORMAT_CLEAN:  os << val.format(CleanFmt);     break;
        case EIGEN_IOFORMAT_OCTAVE: os << val.format(OctaveFmt);    break;
        case EIGEN_IOFORMAT_HEAVY:  os << val.format(HeavyFmt);     break;
        }
    }
};

// tooling
template<typename P>
std::ostream& operator<<(std::ostream& os, const ParamEigenBase<P>& base)
{
    os << base.val;

    return os;
}

// the common child-template class, using the "Enable" trick
template <typename T, typename Enable = void>
class ParamEigen: public ParamEigenBase<T>
{
public:
    ParamEigen(const T& val): ParamEigenBase<T>(val) {}
    ParamEigen(void): ParamEigenBase<T>(static_cast<T>(0)) {}
};

#if 0
// specific types are changed. like short
template<>
class ParamEigen<short> : public ParamEigenBase<short>
{
    public:
        ParamEigen(const short& val)
            :ParamEigenBase<short>(val) {}
        ParamEigen()
            :ParamEigenBase<short>(0){}
        void increase() { val = val*11; }
};
#endif

/*!
    Specialization that exists only for cases where T is, or is derived from,
    an Eigen::DenseBase specialization.
*/
template <typename T>
class ParamEigen<T, typename enable_if<is_eigen_matrix<T>::value>::type>
    : public ParamEigenBase<T>
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    ParamEigen(const T& val): ParamEigenBase<T>(val) {}
    ParamEigen(void): ParamEigenBase<T>(T::Zero()) {}

    void setZero() { ParamEigenBase<T>::val.setZero(); }
};


// TODO: print

}; // SC

#endif // _ParamEigen_h
