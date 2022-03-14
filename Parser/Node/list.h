#pragma once
#include "node.h"

namespace QTwig
{
  namespace Node
  {
    /**
   * @brief A node containing only a list of children
   */
    class List: public Generic
    {
    public:
      List(const QList<Ptr> &children, const Location &location);
      int count();
      Ptr takeChildren(int i);
      QList<Ptr> takeChildren();
      void appendNode(Generic *node);
      inline int numChildren() const { return m_children.size(); }
      Ptr children(int i);
      ConstPtr children(int i) const;
      Type type() const override{return Type::List;}
    protected:
      Accessor *accessor() override;
    };
  }
}
