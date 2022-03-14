#pragma once
#include "node1.h"

namespace QTwig
{
  namespace Node
  {
    class Extends: public Node1
    {
    public:
      Extends(Ptr source, const Location &location=Location()): Node1(source, location) { }
      Type type() const override { return Type::Extends; }
      const Ptr &source() const { return m_expr; }
      Ptr &source() { return m_expr; }
    protected:
      Accessor *accessor() override;
    };
  }
}

