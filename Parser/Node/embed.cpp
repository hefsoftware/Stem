#include "embed.h"
#include "accessor.h"

using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newEmbed(Ptr source, bool ignoreMissing, Ptr vars, bool only, Ptr body, const Location &location)
{
  return Ptr(new Embed(source, ignoreMissing, vars, only, body, location));
}

Accessor *Embed::accessor()
{
  auto access=Include::accessor();
  access->addNamedChildren("body", [this](){ return m_body; });
  return access;
}

