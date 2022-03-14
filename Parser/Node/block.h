#pragma once
#include "nodenamevalue.h"

namespace QTwig
{
  namespace Node
  {
    // This class maps a call to a global function
    class Block: public NodeNameValue
    {
    public:
      Block(const QString &name, Ptr value, const Location &location=Location()): NodeNameValue(name, value, location) { }
      Type type() const override { return Type::Block; }
      Ptr args() { return m_value; }
      ConstPtr args() const { return m_value; }
    protected:
    };
  }
}
