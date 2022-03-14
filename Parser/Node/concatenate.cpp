#include "concatenate.h"

using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newConcatenate(const QList<Ptr> &children, const Location &location)
{
  return Ptr(new Concatenate(children, location));
}
