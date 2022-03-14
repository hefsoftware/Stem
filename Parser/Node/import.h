#pragma once
#include "node2.h"

namespace QTwig
{
  namespace Node
  {
    /**
   * @brief A node containing only a list of children
   */
    class Import: public Node2
    {
    public:
      Import(Ptr &source, Ptr &what, const Location &location): Node2(source, what, location) { }
      Type type() const override{return Type::Import;}
    protected:
      Accessor *accessor() override;
    };
  }
}
