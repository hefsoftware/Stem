#include "array.h"
#include "accessor.h"

using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newArray(const QList<Ptr> &children, const Location &location)
{
  return Ptr(new Array(children, location));
}
