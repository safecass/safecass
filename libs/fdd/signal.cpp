/*

  Safety Framework for Component-based Robotics

  Created on: January 7, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "signal.h"

using namespace SF;

const SignalElement::HistoryBufferIndexType SignalElement::INVALID_HISTORY_BUFFER_INDEX = -1;

SignalElement::SignalElement()
    : Name("NONAME"), Type(SCALAR)
{
    Init();
}

SignalElement::SignalElement(SignalType type, const std::string & name)
    : Name(name), Type(type)
{
    Init();
}

void SignalElement::Init(void)
{
    BufferIndex = INVALID_HISTORY_BUFFER_INDEX;

    PlaceholderScalar = 0.0;
    PlaceholderVector.clear();
}

