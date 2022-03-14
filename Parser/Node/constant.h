#pragma once
#include "node.h"

namespace QTwig
{
  namespace Node
  {
    class Constant: public Generic
    {
    public:
      Constant(const Location &location=Location()): Generic(location) { }
    };
  }
}
