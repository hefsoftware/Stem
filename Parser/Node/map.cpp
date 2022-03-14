#include "map.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newMap(Ptr key, Ptr value, const Location &location)
{
  return Ptr(new Map(key, value, location));
}

Accessor *Map::accessor()
{
  auto ret=Node2::accessor();
  ret->renameChildren("expr1", "key");
  ret->renameChildren("expr2", "value");
  return ret;
}
