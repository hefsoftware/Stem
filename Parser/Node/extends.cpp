#include "extends.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newExtends(Ptr source, const Location &location)
{
  return Ptr(new Extends(source, location));
}
Accessor *Extends::accessor()
{
  auto ret=Node1::accessor();
  ret->renameChildren("expr", "source");
  return ret;
}
