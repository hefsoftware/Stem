#include "block.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newBlock(const QString &name, Ptr body, const Location &location)
{
  return Ptr(new Block(name, body, location));
}
