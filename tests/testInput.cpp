//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : Mar 16, 2016
// Last revision: Mar 16, 2016
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"
#include "safecass/historyBuffer.h"
#include "safecass/paramEigen.h"

using namespace SC;

// Tests basic features provided by boost::lockfree::spsc_queue
TEST(HistoryBuffer, spsc_basic)
{
    // queue size
    const size_t N = 5;

    // For testing, use int type
    boost::lockfree::spsc_queue<int> q(N);

    // Check initial conditions
    EXPECT_EQ(0, q.read_available());
    EXPECT_EQ(N, q.write_available());

    size_t nr = 0, nw = 0;

    // Push test
#define PUSH_CHECK( _x )\
    nr = q.read_available();\
    nw = q.write_available();\
    EXPECT_EQ(N, nr + nw);\
    EXPECT_TRUE(q.push(_x));\
    EXPECT_EQ(nr + 1, q.read_available());\
    EXPECT_EQ(nw - 1, q.write_available());

    PUSH_CHECK(1);
    EXPECT_EQ(1, q.front());

    PUSH_CHECK(2);
    EXPECT_EQ(1, q.front());

    PUSH_CHECK(3);
    EXPECT_EQ(1, q.front());

    PUSH_CHECK(4);
    EXPECT_EQ(1, q.front());

    PUSH_CHECK(5);
    EXPECT_EQ(1, q.front());

    EXPECT_FALSE(q.push(6));
    EXPECT_EQ(N, q.read_available());
    EXPECT_EQ(0, q.write_available());

    // Pop test
#define POP_CHECK\
    nr = q.read_available();\
    nw = q.write_available();\
    EXPECT_EQ(N, nr + nw);\
    EXPECT_TRUE(q.pop());\
    EXPECT_EQ(nr - 1, q.read_available());\
    EXPECT_EQ(nw + 1, q.write_available());

    POP_CHECK;
    EXPECT_EQ(2, q.front());

    POP_CHECK;
    EXPECT_EQ(3, q.front());

    POP_CHECK;
    EXPECT_EQ(4, q.front());

    POP_CHECK;
    EXPECT_EQ(5, q.front());

    EXPECT_TRUE(q.pop());

    EXPECT_FALSE(q.pop());
    EXPECT_EQ(0, q.read_available());
    EXPECT_EQ(N, q.write_available());

    // Reset test
    q.push(1);
    q.push(2);
    q.push(3);
    q.push(4);
    q.push(5);

    EXPECT_EQ(N, q.read_available());
    EXPECT_EQ(0, q.write_available());
    q.reset();
    EXPECT_EQ(0, q.read_available());
    EXPECT_EQ(N, q.write_available());

#undef PUSH_CHECK
#undef POP_CHECK
}

// Tests HistoryBuffer, which is essentially a set of timestamped spsc_queue
// instances
TEST(HistoryBuffer, spsc_array_basic)
{
    // TODO: integrate chrono
}

// Tests accessors that HistoryBuffer provides
TEST(HistoryBuffer, spsc_array_accessors)
{
}

// Tests for ParamEigen
//#include <cstddef>
#include <typeinfo>
TEST(HistoryBuffer, eigen_params)
{
    // Tests for primitive types
#define TEST_PRIMITIVE_TYPES(_type) {\
    ParamEigen<_type> param;\
    EXPECT_TRUE(typeid(_type) == typeid(param.val)); }

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

    // Tests for std::vector. std::vectors use contiguous memory layout just like
    // plain arrays and thus setZero() methods should work.
    ParamEigen<std::vector<int> > vec;
    EXPECT_TRUE(vec.val.empty());

    vec.val.push_back(1);
    EXPECT_FALSE(vec.val.empty());
    EXPECT_EQ(1, vec.val.size());

    vec.setZero();
    EXPECT_TRUE(vec.val.empty());

    // Tests for Eigen types
    // TODO: Could incorporate examples from the official tutorial documentation:
    // https://eigen.tuxfamily.org/dox/group__TutorialMatrixArithmetic.html
    ParamEigen<Eigen::Vector3f> v3f(Eigen::Vector3f::Random());
    EXPECT_EQ(3, v3f.val.rows());
    EXPECT_EQ(1, v3f.val.cols());
    EXPECT_EQ(3, v3f.val.asDiagonal().rows());
    EXPECT_EQ(3, v3f.val.asDiagonal().cols());

    v3f.setZero();
    EXPECT_EQ(0, v3f.val.sum());

    // TODO: Could incorporate examples from the official tutorial documentation:
    // https://eigen.tuxfamily.org/dox/group__TutorialArrayClass.html
    // Eigen uses typedefs of the form ArrayNNt.
#define TEST_EIGEN_ARRAY_TYPES(_type, _row, _col) {\
    ParamEigen<_type> param(_type::Random());\
    EXPECT_EQ(_row, param.val.rows());\
    EXPECT_EQ(_col, param.val.cols());\
    param.print(ParamEigenBase<_type>::EIGEN_IOFORMAT_COMMA, std::cout);\
    std::cout << std::endl;\
    param.print(ParamEigenBase<_type>::EIGEN_IOFORMAT_CLEAN, std::cout);\
    std::cout << std::endl;\
    param.print(ParamEigenBase<_type>::EIGEN_IOFORMAT_OCTAVE, std::cout);\
    std::cout << std::endl;\
    param.print(ParamEigenBase<_type>::EIGEN_IOFORMAT_HEAVY, std::cout);\
    std::cout << std::endl;\
}

    TEST_EIGEN_ARRAY_TYPES(Eigen::Array2i, 2, 1);
    TEST_EIGEN_ARRAY_TYPES(Eigen::Array3f, 3, 1);
    TEST_EIGEN_ARRAY_TYPES(Eigen::Array33d, 3, 3);
#undef TEST_EIGEN_ARRAY_TYPES
}
