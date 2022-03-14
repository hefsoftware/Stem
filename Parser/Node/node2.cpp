#include "node2.h"
#include "accessor.h"
using namespace QTwig::Node;

Accessor *Node2::accessor()
{
  return accessor("expr1", "expr2");
}
Accessor *Node2::accessor(const char *key1, const char *key2)
{
  auto access=Generic::accessor();
  access->addNamedChildren(key1, [this](){ return m_expr1; });
  access->addNamedChildren(key2, [this](){ return m_expr2; });
  return access;
}
