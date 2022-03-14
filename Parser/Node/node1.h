#pragma once
#include "node.h"

namespace QTwig
{
  namespace Node
  {
    class Node1: public Generic
    {
    public:
      Node1(Ptr expr, const Location &location=Location()): Generic(location), m_expr(expr) { }
      inline Ptr expr() { return m_expr; }
      inline ConstPtr expr() const { return m_expr; }
    protected:
      Ptr m_expr;
      Accessor *accessor() override;
    };
  }
}
