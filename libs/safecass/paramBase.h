//-----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2016 Min Yang Jung and Peter Kazanzides
//
//-----------------------------------------------------------------------------------
//
// Created on   : Apr 2, 2016
// Last revision: Apr 19, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//

/*!
    This file implements the base class of parameter types that safecass uses.
*/

#ifndef _ParamBase_h
#define _ParamBase_h

#include "common/utils.h"

namespace SC {

class ParamBase
{
protected:
    //! If this object is valid
    bool Valid;
    //! Timestamp representing when this object is created or becomes valid
    /*!
        If Valid is false, this points to the time when this object was created.
        If Valid is true, this points to the time when this object became valid.
    */
    TimestampType Timestamp;

public:
    ParamBase(void): Valid(false) {
        Timestamp = GetCurrentTimestamp();
    }
    virtual ~ParamBase() {}

    //! Returns if this object is valid
    /*!
        \sa SetValid()
    */
    inline bool IsValid(void) const {
        return Valid;
    }

    //! Mark this object (in)valid
    /*!
        Validity is defined by the user of this object.  Typically, the object
        becomes valid when when it is updated to contain meaningful data.

        It is important to note that Timestamp is set to the current time when
        this method is called with true.

        \sa IsValid()
    */
    void SetValid(bool isValid = true) {
        Valid = isValid;
    }

    inline TimestampType GetTimestamp(void) const { return Timestamp; }

    virtual void ToStream(std::ostream & os) const {
        PrintTime(Timestamp, os);
        os << ", " << (Valid ? "[o]" : "[x]");
    }

    virtual ParamBase * Clone(void) const = 0;
};

inline std::ostream & operator << (std::ostream & os, const ParamBase & param)
{
    param.ToStream(os);
    return os;
}

}; // SC

#endif // _ParamBase_h
