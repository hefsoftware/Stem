#pragma once
#include "nodenamevalue.h"

namespace QTwig
{
  namespace Node
  {
    // This class maps a call to a global function
    class FunctionCall: public NodeNameValue
    {
    public:
      FunctionCall(const QString &function, Ptr args, const Location &location=Location()): NodeNameValue(function, args, location) { }
      Type type() const override { return Type::FunctionCall; }
      Ptr args() { return m_value; }
      ConstPtr args() const { return m_value; }
    protected:
      Accessor *accessor() override { return NodeNameValue::accessor("args"); }
    };
  }
}
