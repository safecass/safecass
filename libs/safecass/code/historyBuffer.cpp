//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Mar 15, 2016
// Last revision: Apr 20, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//

#include <iomanip>

#include "safecass/historyBuffer.h"

using namespace SC;

HistoryBuffer::HistoryBuffer(size_t bufferSize)
    : BufferSize(bufferSize),
      SnapshotIndex(HistoryBuffer::BaseType::INVALID_SIGNAL_INDEX)
{
}

HistoryBuffer::~HistoryBuffer()
{
    SignalAccessorsType::iterator it = SignalAccessors.begin();
    SignalAccessorsType::iterator itEnd = SignalAccessors.end();
    for (; it != itEnd; ++it)
        delete *it;
}

bool HistoryBuffer::GetNewValue(const BaseType::IDType & id, ParamBase & arg) const
{
    HistoryBuffer::BaseType::IndexType index = GetSignalIndex(id);
    if (index == HistoryBuffer::BaseType::INVALID_SIGNAL_INDEX) {
        SCLOG_WARNING << "Signal \"" << id << "\" not found" << std::endl;
        return false;
    }

    SignalAccessors[index]->GetValue(arg);
    //
    // TODO: timestamp
    //

    return true;
}

bool HistoryBuffer::GetNewValue(const BaseType::IndexType & index, ParamBase & arg) const
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
    os << "Snapshot idx (" << SnapshotIndex << "), "
       << "Number of signals (" << SignalAccessors.size() << "): ";

    if (SignalAccessors.empty()) {
        os << "No signal accessor" << std::endl;
        return;
    } else {
        os << std::endl;
    }

    const size_t digit = log10(BufferSize) + 1;
    const std::ios::fmtflags f(os.flags());
    for (size_t i = 0; i < SignalAccessors.size(); ++i) {
        const char prevFiller = os.fill('0');
        os << std::setw(digit) << i << ": ";
        os.flags(f);
        os.fill(prevFiller);
        os << (*SignalAccessors[i]) << std::endl;
    }

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
    // Iterating signal accessors, copy current parameter and push to circular buffer
    SignalAccessorsType::iterator it = SignalAccessors.begin();
    const SignalAccessorsType::iterator itEnd = SignalAccessors.end();
    for (; it != itEnd; ++it)
        (*it)->Push();

    // Update snapshot index
    // In case of the very first snapshot, manually set it to 1
    if (SnapshotIndex == HistoryBuffer::BaseType::INVALID_SIGNAL_INDEX)
        SnapshotIndex = 1;
    else
        ++SnapshotIndex;

    // FIXME timestamp handling
}
