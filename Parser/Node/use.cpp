#include "use.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newUse(Ptr source, Ptr blocks, const Location &location)
{
  return Ptr(new Use(source, blocks, location));
}

Accessor *Use::accessor()
{
  auto access=Generic::accessor();
  access->addNamedChildren("source", [this](){ return m_source; });
  access->addNamedChildren("blocks", [this](){ return m_blocks; }).setOmitIfEmpty(true);
  return access;
}
