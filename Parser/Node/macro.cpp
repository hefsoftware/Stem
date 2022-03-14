#include "macro.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;
Ptr Node::newMacro(const QString &name, Ptr args, Ptr body, const Location &location)
{
  return Ptr(new Macro(name, args, body, location));
}

Accessor *Macro::accessor()
{
  auto access=FunctionCall::accessor();
  access->addNamedChildren("body", [this](){ return m_body; });
  return access;
}
