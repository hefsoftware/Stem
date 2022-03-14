#pragma once
#include "list.h"

namespace QTwig
{
  namespace Node
  {
    class Concatenate: public List
    {
    public:
      Concatenate(const QList<Ptr> &children, const Location &location): List(children, location) { }
      Type type() const override { return Type::Concatenate; }

    };
  }
}
