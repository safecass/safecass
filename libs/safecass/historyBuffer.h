//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Mar 15, 2016
// Last revision: Apr 10, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#ifndef _HistoryBuffer_h
#define _HistoryBuffer_h

#include "safecass/historyBufferBase.h"
#include "safecass/signalAccessor.h"

#include <vector>

namespace SC {

/*!

    Design considerations:
        - vector per signal or vector per time unit?
        - chose vector per signal using boost::circular_buffer

    Each column is an instance of circular buffer

    1) Initial status (default size=1024): two default columns

    index   time
    -------------
    0       0.0
    1       0.0
    2       0.0
    ...
    1023    0.0
    -------------

    2) Adding a set of signals by AddSignal()

    index   time    s(0)  s(1)
    ---------------------------
    0       0.0     0     '0'
    1       0.0     0     '0'
    2       0.0     0     '0'
    ...
    1023    0.0     0     '0'
    ---------------------------

    3) Taking a snapshot of all signals (2 snapshots in the following case)

    index   time    s(0)  s(1)
    ---------------------------
    0       1.0     1     'a'
    1       2.0     2     'b'
    2       0.0     0     '0'
    ...
    1023    0.0     0     '0'
    ---------------------------

    4) A signal can be added at run-time.  However, valid data will be available only for
    time after the signal was added.  The following table shows a case where five snapshots
    were taken.

    index   time    s(0)  s(1)  s(2)
    ----------------------------------
    0       1.0     1     'a'   x
    1       2.0     2     'b'   x
    2       3.0     2     'c'   x
    3       4.0     3     'd'   100  <= Added s(2) at t=[3.0, 4.0)
    4       5.0     3     'd'   101
    5       0.0     0     '0'   102
    ...
    1023    0.0     0     '0'   0
    ----------------------------------

*/
class SCLIB_EXPORT HistoryBuffer: public HistoryBufferBase
{
public:
    //! Typedef of base type
    typedef HistoryBufferBase BaseType;

protected:
    //! Typedef of vector containing a set of signal accessors
    typedef std::vector<SignalAccessorBase *> SignalAccessorsType;

    //! Size of each signal accessor (i.e., length of underlying circular buffer)
    const size_t BufferSize;

    //! Index of snapshot in the table. Equals to (# of snapshots) mod (

    //! Vector of signal accessors
    SignalAccessorsType SignalAccessors;

public:
    //! Constructor
    /*!
        \param bufferSize Max length of snapshots that this history buffer
                          maintains.  This argument is passed to signal accessors
                          to define the length of its underlying circular buffer.
        \sa signalAccessor()
    */
    HistoryBuffer(size_t bufferSize = 128);

    //! Destructor
    virtual ~HistoryBuffer();

    //
    // Methods required by the base class
    //
    //! Get latest value from history buffer via direct access to history buffer
    /*!
        \sa HistoryBufferBase()
    */
    virtual bool GetNewValue(const BaseType::IDType & id, ParamBase & arg);
    virtual bool GetNewValue(const BaseType::IndexType & index, ParamBase & arg);

    //! Push value to history buffer
    virtual bool PushNewValue(const IndexType & index, const ParamBase & arg);

    virtual void ToStream(std::ostream & os) const;

    //
    // Getters
    //
    inline size_t GetBufferSize(void) const { return BufferSize; }

    //
    // Methods to provide fully-functional history buffer
    //
    //! Add signal to this history buffer
    /*!
        When adding a signal, DESCRIBE WHAT HAPPENS
    */
    //bool AddSignal(const BaseType::IDType & id, ParamBase & arg)

    //! Export content of table
    //
    // TODO: serialization and deserialization of *this with support for
    // for different export format (raw, human readable, csv, ..)
    void Serialize(std::ostream & os) const;
};

};

#endif // _HistoryBufferBase_h
