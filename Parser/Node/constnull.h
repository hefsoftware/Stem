#pragma once
#include "constant.h"

namespace QTwig
{
  namespace Node
  {
    class ConstNull: public Constant
    {
    public:
      ConstNull(const Location &location=Location());
      Type type() const override { return Type::ConstNull; }
    protected:
      qint64 m_value;
    };
  }
}
