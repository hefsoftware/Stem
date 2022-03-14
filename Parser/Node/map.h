#pragma once
#include "node2.h"

namespace QTwig
{
  namespace Node
  {
    class Map: public Node2
    {
    public:
      Map(Ptr key, Ptr value, const Location &location=Location()): Node2(key, value, location) { }
      Type type() const override { return Type::Map; }
      Ptr key() { return m_expr1; }
      ConstPtr key() const { return m_expr1; }
      Ptr value() { return m_expr1; }
      ConstPtr value() const { return m_expr2; }
    protected:
      Accessor *accessor() override;
    };
  }
}
