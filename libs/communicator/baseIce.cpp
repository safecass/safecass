/*

  Safety Framework for Component-based Robotics

  Created on: July 31, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "baseIce.h"

namespace SF {

BaseIce::BaseIce(const std::string & propertyFileName)
    : IcePropertyFileName(propertyFileName)
{
    SFLOG_DEBUG << "BaseIce: Ice property file name: " << IcePropertyFileName << std::endl;
}

BaseIce::~BaseIce()
{
}

void BaseIce::IceInitialize(void)
{
    // Load configuration file to set the properties of ICE proxy
    IceInitData.properties = Ice::createProperties();
    IceInitData.properties->load(IcePropertyFileName);

    // Create logger
    IceInitData.logger = new SFLogger();

    // Initialize Ice communicator and Ice logger
    IceCommunicator = Ice::initialize(IceInitData);
    IceLogger = IceCommunicator->getLogger();
}

};
