#include "filter.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;
Ptr Node::newFilter(const QString &filter, Ptr args, Ptr expr, const Location &location)
{
  return Ptr(new Filter(filter, args, expr, location));
}

Accessor *Filter::accessor()
{
  auto access=FunctionCall::accessor();
  access->addNamedChildren("expr", [this](){ return m_expr; });
  return access;
}
