/*

   Safety Framework for Component-based Robotics

   Created on: August 7, 2012

   Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)

 */

#ifndef _COMMON_ICE
#define _COMMON_ICE

module SF
{

struct ProcessIdType {
    string ProcessName;
};

struct ComponentIdType {
    string ProcessName;
    string ComponentName;
};

enum InterfaceTypes { PROVIDED, REQUIRED };
struct InterfaceIdType {
    string NameProcess;
    string NameComponent;
    string NameInterface;
    InterfaceTypes InterfaceType;
};

struct ConnectionIdType {
    InterfaceIdType InterfaceProvidedId;
    InterfaceIdType InterfaceRequiredId;
};

};

#endif // _COMMON_ICE
