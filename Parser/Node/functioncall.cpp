#include "functioncall.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;
Ptr Node::newFunctionCall(const QString &filter, Ptr args, const Location &location)
{
  return Ptr(new FunctionCall(filter, args, location));
}


