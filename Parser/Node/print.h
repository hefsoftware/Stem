#pragma once
#include "node1.h"

namespace QTwig
{
  namespace Node
  {
    class Print: public Node1
    {
    public:
      Print(Ptr expr, const Location &location=Location()): Node1(expr, location) { }
      Type type() const override { return Type::Print; }
    };
  }
}
