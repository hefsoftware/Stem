#include "constnull.h"
#include "accessor.h"

using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newConstNull(const Location &location)
{
  return Ptr(new ConstNull(location));
}

ConstNull::ConstNull(const Location &location): Constant(location)
{

}
