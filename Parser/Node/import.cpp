#include "import.h"
#include "accessor.h"
#include "../token.h"
using namespace QTwig::Node;
using namespace QTwig;
Ptr Node::newImport(Ptr source, Ptr par, const Location &location)
{
  return Ptr(new Import(source, par, location));
}


Accessor *Import::accessor()
{
  auto ret=Node2::accessor();
  ret->renameChildren("expr1","source");
  ret->renameChildren("expr2","par");
  return ret;
}
