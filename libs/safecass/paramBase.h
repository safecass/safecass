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

namespace SC {

class ParamBase
{
protected:
    bool Valid;

public:
    ParamBase(void): Valid(false) {}
    virtual ~ParamBase() {}

    //! Accessors for Valid property
    inline bool IsValid(void) const {
        return Valid;
    }
    void SetValid(bool isValid = true) {
        Valid = isValid;
    }

    virtual void ToStream(std::ostream & os) const {
        os << (Valid ? "[o]" : "[x]");
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
