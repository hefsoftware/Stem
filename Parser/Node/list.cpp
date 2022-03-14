#include "list.h"
#include "accessor.h"

using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newList(const QList<Ptr> &children, const Location &location)
{
  return Ptr(new List(children, location));
}

List::List(const QList<Ptr> &children, const Location &location): Generic(location)
{
#if defined(QT_DEBUG)
  Q_FOREACH(Ptr c, children)
    Q_ASSERT_X(c, "NodeMulti::NodeMulti", "null children");
#endif
  m_children=children;
}

int List::count()
{
  return m_children.count();
}

Ptr List::takeChildren(int i)
{
  return m_children.takeAt(i);
}

//void List::appendNode(Ptr node)
//{
//  m_children.append(node);
//}

QList<Ptr> List::takeChildren()
{
  QList<Ptr> ret(m_children);
  m_children.clear();
  return ret;
}

Ptr List::children(int i)
{
  return (i>=0 && i<m_children.size())?m_children[i]:nullptr;
}

ConstPtr List::children(int i) const
{
  return (i>=0 && i<m_children.size())?m_children[i]:nullptr;
}


Accessor *List::accessor()
{
  auto acc=Generic::accessor();
  acc->setEmpty(m_children.isEmpty());
  return acc;
}
