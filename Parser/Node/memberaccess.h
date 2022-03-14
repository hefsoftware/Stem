#pragma once
#include "mapstring.h"

namespace QTwig
{
  namespace Node
  {
    class MemberAccess: public MapString
    {
    public:
      MemberAccess(Ptr expr, const QString &key, const Location &location=Location()): MapString(key, expr, location) { }
      Type type() const override { return Type::MemberAccess; }
    };
  }
}
