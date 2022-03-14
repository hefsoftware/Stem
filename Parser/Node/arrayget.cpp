#include "arrayget.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newArrayGet(Ptr key, Ptr value, const Location &location)
{
  return Ptr(new ArrayGet(key, value, location));
}
