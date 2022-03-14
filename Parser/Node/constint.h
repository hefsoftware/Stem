#pragma once
#include "constant.h"

namespace QTwig
{
  namespace Node
  {
    class ConstInt: public Constant
    {
    public:
      ConstInt(qint64 value, const Location &location=Location());
      constexpr qint64 value() const { return m_value; }
      Type type() const override { return Type::ConstInt; }
    protected:
      qint64 m_value;
      Accessor *accessor() override;
    };
  }
}
