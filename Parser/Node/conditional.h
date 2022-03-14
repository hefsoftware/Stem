#pragma once
#include "node.h"

namespace QTwig
{
  namespace Node
  {
    class Conditional: public Generic
    {
    public:
      Conditional(Ptr cond, Ptr valTrue, Ptr valFalse, const Location &location=Location()): Generic(location), m_cond(cond), m_valTrue(valTrue), m_valFalse(valFalse) { }
      Type type() const override { return Type::Conditional; }
      Ptr cond() { return m_cond; }
      ConstPtr cond() const { return m_cond; }
      Ptr valTrue() { return m_valTrue; }
      ConstPtr valTrue() const { return m_valTrue; }
      Ptr valFalse() { return m_valFalse; }
      ConstPtr valFalse() const { return m_valFalse; }

    protected:
      Accessor *accessor() override;
      Ptr m_cond;
      Ptr m_valTrue;
      Ptr m_valFalse;
    };
  }
}
