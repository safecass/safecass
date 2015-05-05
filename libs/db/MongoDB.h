//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Aug 14, 2012
// Last revision: May 8, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
#ifndef _MongoDB_h
#define _MongoDB_h

#include "common.h"
#include "jsonSerializer.h"

namespace SC {

class SCLIB_EXPORT MongoDB {
protected:

    //! Convert Monitor topic messages from JSON to MongoDB entry format
    static const std::string ConvertTopicMessageToDBEntry_Monitor(JSONSerializer & jsonSerializer);
    //! Convert Fault topic messages from JSON to MongoDB entry format
    static const std::string ConvertTopicMessageToDBEntry_Event(JSONSerializer & jsonSerializer);

public:
    //! Constructor
    MongoDB(void);
    //! Destructor
    ~MongoDB();

    //! Convert topic messages from JSON to MongoDB entry format
    static const std::string ConvertTopicMessageToDBEntry(
        const Topic::Type topic, JSONSerializer & jsonSerializer);
};

};

#endif // _MongoDB_h
