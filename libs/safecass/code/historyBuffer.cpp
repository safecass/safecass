//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Mar 15, 2016
// Last revision: Apr 3, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//

#include "safecass/historyBuffer.h"

using namespace SC;

HistoryBuffer::HistoryBuffer(size_t bufferSize)
    : Queue(new QueueType(bufferSize))
{
    // SCLOG_INFO << "info" << std::endl;
    // SCLOG_DEBUG << "debug" << std::endl;
    // SCLOG_WARNING << "warning" << std::endl;
    // SCLOG_ERROR << "error" << std::endl;
}

HistoryBuffer::~HistoryBuffer()
{
    delete Queue;
}

bool HistoryBuffer::GetNewValue(const BaseType::IDType & id, ParamBase & arg)
{
    return false;
}

bool HistoryBuffer::GetNewValue(const BaseType::IndexType & index, ParamBase & arg)
{
    return false;
}

bool HistoryBuffer::PushNewValue(const IndexType & index, const ParamBase & arg)
{
    return false;
}

void HistoryBuffer::ExportQueue(std::ostream & os) const
{
    // TODO: export queue
}
