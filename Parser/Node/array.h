#pragma once
#include "list.h"

namespace QTwig
{
  namespace Node
  {
    /**
   * @brief A node containing only a list of children
   */
    class Array: public List
    {
    public:
      Array(const QList<Ptr> &children, const Location &location): List(children, location) { }
      Type type() const override{return Type::Array;}
    };
  }
}
