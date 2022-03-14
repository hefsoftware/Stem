#pragma once
#include "node.h"

namespace QTwig
{
  namespace Node
  {
    class Include: public Generic
    {
    public:
      Include(Ptr source, bool ignoreMissing, Ptr vars, bool only, const Location &location=Location()): Generic(location), m_source(source), m_ignoreMissing(ignoreMissing), m_vars(vars), m_only(only) { }
      Type type() const override { return Type::Include; }
      inline Ptr source() { return m_source; }
      inline ConstPtr source() const { return m_source; }
      inline bool ignoreMissing() const { return m_ignoreMissing; }
      inline Ptr vars() { return m_source; }
      inline ConstPtr vars() const { return m_source; }
      inline bool only() const { return m_only; }
    protected:
      Accessor *accessor() override;
      Ptr m_source;
      bool m_ignoreMissing;
      Ptr m_vars;
      bool m_only;
    };
  }
}
