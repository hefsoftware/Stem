#include "call.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newCall(Ptr expr, Ptr args, const Location &location)
{
  return Ptr(new Call(expr, args, location));
}

Accessor *Call::accessor()
{
  auto ret=Node2::accessor();
  ret->renameChildren("expr1", "expr");
  ret->renameChildren("expr2", "args");
  return ret;
}
