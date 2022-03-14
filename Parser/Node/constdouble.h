#pragma once
#include "constant.h"

namespace QTwig
{
  namespace Node
  {
    class ConstDouble: public Constant
    {
    public:
      ConstDouble(double value, const Location &location=Location());
      constexpr double value() const { return m_value; }
      Type type() const override { return Type::ConstDouble; }
    protected:
      double m_value;
      Accessor *accessor() override;
    };
  }
}
