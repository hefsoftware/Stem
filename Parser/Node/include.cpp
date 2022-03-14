#include "include.h"
#include "accessor.h"

using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newInclude(Ptr source, bool ignoreMissing, Ptr vars, bool only, const Location &location)
{
  return Ptr(new Include(source, ignoreMissing, vars, only, location));
}

Accessor *Include::accessor()
{
  auto access=Generic::accessor();
  access->addNamedChildren("source", [this](){ return m_source; });
  access->addNamedChildren("ignoreMissing", [this](){ return m_ignoreMissing?newConstBool(true, location()):Ptr(); }).setOmitIfEmpty(true);
  access->addNamedChildren("vars", [this](){ return m_vars; }).setOmitIfEmpty(true);
  access->addNamedChildren("only", [this](){ return m_only?newConstBool(true, location()):Ptr(); }).setOmitIfEmpty(true);
  return access;
}
