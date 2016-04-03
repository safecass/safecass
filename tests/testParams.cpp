//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Apr 3, 2016
// Last revision: Apr 3, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//

#include <typeinfo>

#include "gtest/gtest.h"
#include "safecass/historyBuffer.h"
#include "safecass/paramEigen.h"

using namespace SC;

// Tests for ParamBase
TEST(ParameterTypes, ParamBase)
{
    ParamEigen<bool> param;

    EXPECT_FALSE(param.IsValid());

    param.SetValid(true);
    EXPECT_TRUE(param.IsValid());

    param.SetValid(false);
    EXPECT_FALSE(param.IsValid());

    param.SetValid();
    EXPECT_TRUE(param.IsValid());
}

// Tests for ParamEigen
TEST(ParameterTypes, ParamEigen)
{
    //
    // Tests for primitive types
    //
#define TEST_PRIMITIVE_TYPES(_type) {\
    ParamEigen<_type> param;\
    EXPECT_TRUE(typeid(_type) == typeid(param.Val)); }

    TEST_PRIMITIVE_TYPES(bool);
    TEST_PRIMITIVE_TYPES(char);
    TEST_PRIMITIVE_TYPES(short);
    TEST_PRIMITIVE_TYPES(int);
    TEST_PRIMITIVE_TYPES(long);
    TEST_PRIMITIVE_TYPES(float);
    TEST_PRIMITIVE_TYPES(double);
    TEST_PRIMITIVE_TYPES(unsigned char);
    TEST_PRIMITIVE_TYPES(unsigned short);
    TEST_PRIMITIVE_TYPES(unsigned int);
    TEST_PRIMITIVE_TYPES(unsigned long);
#undef TEST_PRIMITIVE_TYPES

    //
    // Tests for std::vector. std::vectors use contiguous memory layout just like
    // plain arrays and thus setZero() methods should work.
    //
    ParamEigen<std::vector<int> > vec;
    EXPECT_TRUE(vec.Val.empty());

    vec.Val.push_back(1);
    EXPECT_FALSE(vec.Val.empty());
    EXPECT_EQ(1, vec.Val.size());

    vec.setZero();
    EXPECT_TRUE(vec.Val.empty());

    //
    // Tests for Eigen types
    //
    // TODO: Could incorporate examples from the official tutorial documentation:
    // https://eigen.tuxfamily.org/dox/group__TutorialMatrixArithmetic.html
    ParamEigen<Eigen::Vector3f> v3f(Eigen::Vector3f::Random());
    EXPECT_EQ(3, v3f.Val.rows());
    EXPECT_EQ(1, v3f.Val.cols());
    EXPECT_EQ(3, v3f.Val.asDiagonal().rows());
    EXPECT_EQ(3, v3f.Val.asDiagonal().cols());

    v3f.setZero();
    EXPECT_EQ(0, v3f.Val.sum());

    // TODO: Could incorporate examples from the official tutorial documentation:
    // https://eigen.tuxfamily.org/dox/group__TutorialArrayClass.html
    // Eigen uses typedefs of the form ArrayNNt.
#define TEST_EIGEN_ARRAY_TYPES(_type, _row, _col) {\
        ParamEigen<_type> param(_type::Random());\
        EXPECT_EQ(_row, param.Val.rows());\
        EXPECT_EQ(_col, param.Val.cols());\
    }

    TEST_EIGEN_ARRAY_TYPES(Eigen::Array2i, 2, 1);
    TEST_EIGEN_ARRAY_TYPES(Eigen::Array3f, 3, 1);
    TEST_EIGEN_ARRAY_TYPES(Eigen::Array33d, 3, 3);
#undef TEST_EIGEN_ARRAY_TYPES

    //
    // Tests for Clone
    //
    // Get clone of parameter prototype and check payload type and its value
    // match its original prototype.
    //
    // NOTE: Don't forget to deallocate memory returned by Clone()

#define TEST_PARAM_CLONE_WITHOUT_ARG(_type)\
    {\
        ParamEigen<_type> param;\
        \
        ParamBase * pBase = param.Clone();\
        EXPECT_TRUE(pBase != 0);\
        \
        ParamEigen<_type> * pClone = dynamic_cast<ParamEigen<_type> *>(pBase);\
        EXPECT_TRUE(pClone != 0);\
        EXPECT_TRUE(typeid(_type) == typeid(pClone->Val));\
        delete pClone;\
    }

#define TEST_PARAM_CLONE_WITH_ARG(_type, _arg)\
    {\
        ParamEigen<_type> param;\
        param.Val = _arg;\
        \
        ParamBase * pBase = param.Clone();\
        EXPECT_TRUE(pBase != 0);\
        \
        ParamEigen<_type> * pClone = dynamic_cast<ParamEigen<_type> *>(pBase);\
        EXPECT_TRUE(pClone != 0);\
        EXPECT_TRUE(typeid(_type) == typeid(pClone->Val));\
        EXPECT_EQ((_type)_arg, pClone->Val);\
        delete pClone;\
    }

    TEST_PARAM_CLONE_WITHOUT_ARG(bool);
    TEST_PARAM_CLONE_WITHOUT_ARG(char);
    TEST_PARAM_CLONE_WITHOUT_ARG(short);
    TEST_PARAM_CLONE_WITHOUT_ARG(int);
    TEST_PARAM_CLONE_WITHOUT_ARG(long);
    TEST_PARAM_CLONE_WITHOUT_ARG(float);
    TEST_PARAM_CLONE_WITHOUT_ARG(double);
    TEST_PARAM_CLONE_WITHOUT_ARG(unsigned char);
    TEST_PARAM_CLONE_WITHOUT_ARG(unsigned short);
    TEST_PARAM_CLONE_WITHOUT_ARG(unsigned int);
    TEST_PARAM_CLONE_WITHOUT_ARG(unsigned long);

    TEST_PARAM_CLONE_WITH_ARG(bool, true);
    TEST_PARAM_CLONE_WITH_ARG(char, 0xAB);
    TEST_PARAM_CLONE_WITH_ARG(short, 0xABCD);
    TEST_PARAM_CLONE_WITH_ARG(int, 0x12345678);
    TEST_PARAM_CLONE_WITH_ARG(long, 0XFFFFFFFF);
    TEST_PARAM_CLONE_WITH_ARG(float, 0.12345678f);
    TEST_PARAM_CLONE_WITH_ARG(double, 0.12345678);
    TEST_PARAM_CLONE_WITH_ARG(unsigned char, 0xBA);
    TEST_PARAM_CLONE_WITH_ARG(unsigned short, 0xDCBA);
    TEST_PARAM_CLONE_WITH_ARG(unsigned int, 0x87654321);
    TEST_PARAM_CLONE_WITH_ARG(unsigned long, 0XFFFFFFFF);

#undef TEST_PARAM_CLONE_WITHOUT_ARG
#undef TEST_PARAM_CLONE_WITH_ARG

    // TODO: add eigen-type tests
}
