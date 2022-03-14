#include "memberaccess.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newMemberAccess(Ptr expr, const QString &name, const Location &location)
{
  return Ptr(new MemberAccess(expr, name, location));
}

