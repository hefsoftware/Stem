#include "name.h"
#include "accessor.h"
using namespace QTwig::Node;
using namespace QTwig;
Ptr Node::newName(const QString &name, const Location &location)
{
  return Ptr(new Name(name, location));
}

Accessor *Name::accessor()
{
  auto access=Generic::accessor();
  access->setDataString([this](){return m_name;});
  return access;
}
