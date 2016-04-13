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

HistoryBuffer::HistoryBuffer(size_t bufferSize, bool ownership)
    : BufferSize(bufferSize), OwnsStateAccessors(ownership),
      Index(HistoryBuffer::BaseType::INVALID_SIGNAL_INDEX)
{
}

HistoryBuffer::~HistoryBuffer()
{
    if (OwnsStateAccessors) {
        SignalAccessorsType::iterator it = SignalAccessors.begin();
        SignalAccessorsType::iterator itEnd = SignalAccessors.end();
        for (; it != itEnd; ++it)
            delete *it;
    }
}

bool HistoryBuffer::GetNewValue(const BaseType::IDType & id, ParamBase & arg)
{
    HistoryBuffer::BaseType::IndexType index = GetSignalIndex(id);
    if (index == HistoryBuffer::BaseType::INVALID_SIGNAL_INDEX) {
        SCLOG_INFO << "signal \"" << id << "\" not found" << std::endl;
        return false;
    }

    SignalAccessors[index]->GetValue(arg);
    //
    // TODO: timestamp
    //

    return true;
}

bool HistoryBuffer::GetNewValue(const BaseType::IndexType & index, ParamBase & arg)
{
    // FIXME
    return false;
}

bool HistoryBuffer::PushNewValue(const IndexType & index, const ParamBase & arg)
{
    // FIXME
    return false;
}

void HistoryBuffer::ToStream(std::ostream & os) const
{
    Serialize(os);
}

void HistoryBuffer::Serialize(std::ostream & os) const
{
    os << "size (" << BufferSize << ")"
       << ", index (" << Index << ")";
}

HistoryBuffer::BaseType::IndexType
HistoryBuffer::AddSignal(const HistoryBuffer::BaseType::IDType & name, SignalAccessorBase * accessor)
{
    if (!accessor) {
        SCLOG_INFO << "Null accessor argument specified for signal \"" << name << "\"" << std::endl;
        return HistoryBuffer::BaseType::INVALID_SIGNAL_INDEX;
    }

    const HistoryBuffer::BaseType::IndexType n = (HistoryBuffer::BaseType::IndexType) SignalAccessors.size();

    SignalAccessors.push_back(accessor);
    SignalAccessorsMap.insert(std::make_pair(name, n));

    SCLOG_INFO << "Signal \"" << name << "\" was added to History Buffer" << std::endl;

    return n;
}

bool HistoryBuffer::FindSignal(const HistoryBuffer::BaseType::IDType & id) const
{
    return (GetSignalIndex(id) != HistoryBuffer::BaseType::INVALID_SIGNAL_INDEX);
}

HistoryBuffer::BaseType::IndexType HistoryBuffer::GetSignalIndex(const BaseType::IDType & id) const
{
    SignalAccessorsMapType::const_iterator it = SignalAccessorsMap.find(id);

    if (it == SignalAccessorsMap.end())
        return HistoryBuffer::BaseType::INVALID_SIGNAL_INDEX;
    else
        return it->second;
}

void HistoryBuffer::Snapshot(void)
{
    // FIXME Iterating signal accessors, copy current parameter and push to circular buffer

    // FIXME Increase index

    // FIXME timestamp handling
}
