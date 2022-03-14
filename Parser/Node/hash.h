#pragma once
#include "list.h"

namespace QTwig
{
  namespace Node
  {
    /**
   * @brief A node containing only a list of children
   */
    class Hash: public List
    {
    public:
      Hash(const QList<Ptr> &children, const Location &location): List(children, location) { }
      Type type() const override{return Type::Hash;}
    };
  }
}
