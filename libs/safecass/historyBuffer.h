//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Mar 15, 2016
// Last revision: Mar 23, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#ifndef _HistoryBuffer_h
#define _HistoryBuffer_h

#include "historyBufferBase.h"
#include <boost/lockfree/spsc_queue.hpp>

namespace SC {

/*!

    1) Initial status (default size=1024):

    index   time
    -------------
    0       0.0
    1       0.0
    2       0.0
    ...
    1023    0.0
    -------------

    2) Adding the first signal

    index   time    s(0)
    ---------------------
    0       0.0     0
    1       0.0     0
    2       0.0     0
    ...
    1023    0.0     0
    ---------------------

    3) With Snapshot(),

    index   time    s(0)
    ---------------------
    0       1.0     10
    1       2.0     20
    2       3.0     30
    3       0.0     0  <= write_ptr (N=3)
    ...
    1023    0.0     0
    ---------------------

    4) Adding another signal

    index   time    s(0)    s(1)    s(2)...
    ----------------------------------------
    0       1.0     10
    1       2.0     20
    2       3.0     30
    3       4.0     40      'a'          <= s(1) added
    4       5.0     50      'b'
    5       6.0     60      'c'     1.0  <= s(2) added
    6       7.0     70      'd'     2.0
    7       0.0     0       '0'     0.0  <= write_ptr (N=7)
    ...
    1023    0.0     0       '0'     0.0
    ----------------------------------------
*/
class SCLIB_EXPORT HistoryBuffer: public HistoryBufferBase
{
public:
    //! Typedef of base type
    typedef HistoryBufferBase BaseType;

    //! Typedef of row.
    /*!
        One row of the history buffer contains a snapshot of all signals
        registered at the moment.  A new row is created by Snapshot(), which
        is usually called at each time step.
    */
    typedef struct {
        //! Time when Snapshot() was called
        BaseType::TimestampType Tick;
        //! Snapshot of registered parameters (NOTE: length may differ)
        std::vector<ParamBase*> ParamVector;
    } RowType;

protected:
    //! Typedef of underlying buffer structure that provides FIFO queue implementation
    typedef boost::lockfree::spsc_queue<RowType> QueueType;

    // TODO better naming???
    QueueType * Queue;

    //! Export content of queue
    //
    // TODO: Nice features to have
    // - Support for different export format (raw, human readable, csv, ..)
    // - Import (re-build) queue based on txt file => would be very useful
    void ExportQueue(std::ostream & os = std::cout) const;

public:
    //! Constructor
    HistoryBuffer(size_t bufferSize = 1024);
    //! Destructor
    virtual ~HistoryBuffer();

    //
    // Methods required by the base class
    //
    //! Get latest value from history buffer via direct access to history buffer
    /*!
        \sa HistoryBufferBase class
    */
    virtual bool GetNewValue(const BaseType::IDType & id, ParamBase & arg);
    virtual bool GetNewValue(const BaseType::IndexType & index, ParamBase & arg);

    //! Push value to history buffer
    virtual bool PushNewValue(const IndexType & index, const ParamBase & arg);

    //
    // Methods to provide fully-functional history buffer
    //
    //! Add signal to this history buffer
    /*!
        When adding a signal, DESCRIBE WHAT HAPPENS
    */
    //bool AddSignal(const BaseType::IDType & id, ParamBase & arg)
};

};

#endif // _HistoryBufferBase_h
