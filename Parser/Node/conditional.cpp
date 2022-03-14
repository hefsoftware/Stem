#include "conditional.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newConditional(Ptr cond, Ptr valTrue, Ptr valFalse, const Location &location)
{
  return Ptr(new Conditional(cond, valTrue, valFalse, location));
}

Node::Accessor *Conditional::accessor()
{
  auto access=Generic::accessor();
  access->addNamedChildren("cond", [this](){ return m_cond; });
  access->addNamedChildren("true", [this](){ return m_valTrue; });
  access->addNamedChildren("false", [this](){ return m_valFalse; });
  return access;
}
