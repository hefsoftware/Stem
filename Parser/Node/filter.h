#pragma once
#include "functioncall.h"

namespace QTwig
{
  namespace Node
  {
    class Filter: public FunctionCall
    {
    public:
      Filter(const QString &filter, Ptr args, Ptr expr, const Location &location=Location()): FunctionCall(filter, args, location), m_expr(expr) { }
      Type type() const override { return Type::Filter; }
      Ptr expr() { return m_expr; }
      ConstPtr expr() const { return m_expr; }
    protected:
      Ptr m_expr;
      Accessor *accessor() override;
    };
  }
}
