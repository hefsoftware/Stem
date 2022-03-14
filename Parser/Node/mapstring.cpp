#include "mapstring.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newMapString(const QString &key, Ptr expr, const Location &location)
{
  return Ptr(new MapString(key,expr, location));
}

Accessor *MapString::accessor()
{
  return NodeNameValue::accessor("expr");
}
