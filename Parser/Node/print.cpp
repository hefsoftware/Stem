#include "print.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newPrint(Ptr expr, const Location &location)
{
  return Ptr(new Print(expr, location));
}
