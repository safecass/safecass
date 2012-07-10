/*

  Safety Framework for Component-based Robotics

  Created on: July 5, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef _json_h
#define _json_h

#include <jsoncpp/json.h>
#include <string>

namespace SF {

class JSON {
    // for unit-tests
    friend class JSONTest;

protected:
    Json::Value  * JSONValues;
    Json::Reader * JSONReader;

    void Cleanup(void);

public:
    JSON();
    virtual ~JSON();

    virtual bool Read(const char * json);
    virtual bool ReadFromFile(const std::string & fileName);
    virtual bool WriteToFile(const std::string & fileName) const;

    virtual std::string GetJSON() const;

    Json::Value & GetRoot(void) { return *JSONValues; }

    //virtual bool Parse(void) = 0;
};

};

#endif // _json_h
