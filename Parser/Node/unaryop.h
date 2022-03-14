#pragma once
#include "node1.h"

namespace QTwig
{
  class Operator1;
  namespace Node
  {
    class UnaryOp: public Node1
    {
    public:
      UnaryOp(Ptr expr, const Operator1 *op, const Location &location=Location()): Node1(expr, location), m_op(op) { }
      Type type() const override { return Type::UnaryOp; }
      constexpr const Operator1 *op() const { return m_op; }
    protected:
      Accessor *accessor() override;
      const Operator1 *m_op;
    };
  }
}
