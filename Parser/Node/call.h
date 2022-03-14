#pragma once
#include "node2.h"

namespace QTwig
{
  namespace Node
  {
    // This class maps a call to a generic expression
    class Call: public Node2
    {
    public:
      Call(Ptr expr, Ptr args, const Location &location=Location()): Node2(expr, args, location) { }
      Type type() const override { return Type::Call; }
      Ptr expr() { return m_expr1; }
      ConstPtr expr() const { return m_expr1; }
      Ptr args() { return m_expr1; }
      ConstPtr args() const { return m_expr2; }
    protected:
      Accessor *accessor() override;
    };
  }
}
