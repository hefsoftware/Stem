#pragma once
#include "include.h"

namespace QTwig
{
  namespace Node
  {
    class Embed: public Include
    {
    public:
      Embed(Ptr source, bool ignoreMissing, Ptr vars, bool only, Ptr body, const Location &location=Location()): Include(source, ignoreMissing, vars, only, location), m_body(body) { }
      Type type() const override { return Type::Embed; }
      inline Ptr body() { return m_body; }
      inline ConstPtr body() const { return m_body; }
    protected:
      Ptr m_body;
      Accessor *accessor() override;
    };
  }
}

