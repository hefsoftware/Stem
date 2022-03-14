#pragma once
#include "node2.h"

namespace QTwig
{
  class Operator2;
  namespace Node
  {
    class BinaryOp: public Node2
    {
    public:
      BinaryOp(Ptr expr1, Ptr expr2, const Operator2 *op, const Location &location=Location()): Node2(expr1, expr2, location), m_op(op) { }
      Type type() const override { return Type::BinaryOp; }
      constexpr const Operator2 *op() const { return m_op; }
    protected:
      Accessor *accessor() override;
      const Operator2 *m_op;
    };
  }
}
