#pragma once
#include "nodenamevalue.h"

namespace QTwig
{
  namespace Node
  {
    // This class maps a call to a global function
    class MapString: public NodeNameValue
    {
    public:
      MapString(const QString &key, Ptr expr, const Location &location=Location()): NodeNameValue(key, expr, location) { }
      Type type() const override { return Type::MapString; }
      QString key() const { return name(); }
      Ptr expr() { return m_value; }
      ConstPtr expr() const { return m_value; }
    protected:
      Accessor *accessor() override;
    };
  }
}
