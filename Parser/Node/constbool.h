#pragma once
#include "constant.h"

namespace QTwig
{
  namespace Node
  {
    class ConstBool: public Constant
    {
    public:
      ConstBool(bool value, const Location &location=Location());
      constexpr bool value() const { return m_value; }
      Type type() const override { return Type::ConstBool; }
    protected:
      bool m_value;
      Accessor *accessor() override;
    };
  }
}
