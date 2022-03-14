#include "nodenamevalue.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;

Accessor *NodeNameValue::accessor()
{
  return accessor("value");
}

Accessor *NodeNameValue::accessor(const char *value)
{
  auto access=Name::accessor();
  access->addNamedChildren(value, [this](){ return m_value; }); //.setOmitIfEmpty();
  return access;
}
