//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Mar 15, 2016
// Last revision: Mar 15, 2016
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/safecass/safecass
//
#include "safecass/historyBuffer.h"

namespace SC {

HistoryBuffer::HistoryBuffer(void)
{
    // Queue = new boost::lockfree::spsc_queue<int>(10);

    // SCLOG_INFO << "info" << std::endl;
    // SCLOG_DEBUG << "debug" << std::endl;
    // SCLOG_WARNING << "warning" << std::endl;
    // SCLOG_ERROR << "error" << std::endl;
}

HistoryBuffer::~HistoryBuffer()
{
}

}; // SC

