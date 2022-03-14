#include "hash.h"
#include "accessor.h"

using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newHash(const QList<Ptr> &children, const Location &location)
{
  return Ptr(new Hash(children, location));
}
