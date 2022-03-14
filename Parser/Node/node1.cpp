#include "node1.h"
#include "accessor.h"
using namespace QTwig::Node;

Accessor *Node1::accessor()
{
  auto access=Generic::accessor();
  access->addNamedChildren("expr", [this](){ return m_expr; });
  return access;
}
