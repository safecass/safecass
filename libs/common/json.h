/*

  Safety Framework for Component-based Robotics

  Created on: July 5, 2012

  Copyright (C) 2012 Min Yang Jung <myj@jhu.edu>

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include <jsoncpp/json.h>
#include <string>

namespace SF {

class JSON {
protected:
    static Json::Reader JSONReader;
    Json::Value JSONValues;

public:
    JSON();
    virtual ~JSON();

    virtual bool Read(const std::string & jsonString);
    virtual bool ReadFromFile(const std::string & fileName);

    virtual bool Write(std::string & jsonString) const;
    virtual bool WriteToFile(const std::string & fileName) const;

    //virtual bool Parse(void) = 0;
    virtual bool Parse(std::string & result);

    void Cleanup();
};

};
