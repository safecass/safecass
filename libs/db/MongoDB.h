/*

  Safety Framework for Component-based Robotics

  Created on: August 14, 2012

  Copyright (C) 2012-3 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _MongoDB_h
#define _MongoDB_h

#include "common.h"
#include "jsonSerializer.h"

namespace SF {

class SFLIB_EXPORT MongoDB {
protected:

    //! Convert Monitor topic messages from JSON to MongoDB entry format
    static const std::string ConvertTopicMesssageToDBEntry_Monitor(JSONSerializer & jsonSerializer);
    //! Convert Fault topic messages from JSON to MongoDB entry format
    static const std::string ConvertTopicMesssageToDBEntry_Fault(JSONSerializer & jsonSerializer);

public:
    //! Constructor
    MongoDB(void);
    //! Destructor
    ~MongoDB();

    //! Convert topic messages from JSON to MongoDB entry format
    static const std::string ConvertTopicMesssageToDBEntry(
        const JSONSerializer::TopicType topic, JSONSerializer & jsonSerializer);
};

};

#endif // _MongoDB_h
