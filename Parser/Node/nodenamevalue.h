#pragma once
#include "name.h"

namespace QTwig
{
  namespace Node
  {
    // This class maps a call to a global function
    class NodeNameValue: public Name
    {
    public:
      NodeNameValue(const QString &name, Ptr value, const Location &location=Location()): Name(name, location), m_value(value) { }
      Ptr value() { return m_value; }
      ConstPtr value() const { return m_value; }
    protected:
      Ptr m_value;
      Accessor *accessor();
      Accessor *accessor(const char *value);
    };
  }
}
