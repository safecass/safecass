/*

  Safety Framework for Component-based Robotics

  Created on: August 16, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _cisstDic_h
#define _cisstDic_h

#define DEFINE_KEYWORD(_key) const std::string _key = #_key;

namespace SF {

    namespace Dict {

        namespace Json {

            namespace cisst {
                DEFINE_KEYWORD(command);
                DEFINE_KEYWORD(event_generator);
                DEFINE_KEYWORD(event_handler);
                DEFINE_KEYWORD(function);
                // MJ TODO: remove the followings
                DEFINE_KEYWORD(NAME_COMMAND);
                DEFINE_KEYWORD(NAME_FUNCTION);
                DEFINE_KEYWORD(NAME_EVENT_GENERATOR);
                DEFINE_KEYWORD(NAME_EVENT_HANDLER);
            };
        }

    }; // SF::Dict

}; // SF

#endif // _cisstDic_h
