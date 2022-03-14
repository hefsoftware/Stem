#pragma once
#include "functioncall.h"

namespace QTwig
{
  namespace Node
  {
    class Macro: public FunctionCall
    {
    public:
      Macro(const QString &name, Ptr args, Ptr body, const Location &location=Location()): FunctionCall(name, args, location), m_body(body) { }
      Type type() const override { return Type::Macro; }
      Ptr body() { return m_body; }
      ConstPtr body() const { return m_body; }
    protected:
      Ptr m_body;
      Accessor *accessor() override;
    };
  }
}
