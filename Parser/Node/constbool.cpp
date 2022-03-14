#include "constbool.h"
#include "accessor.h"

using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newConstBool(bool value, const Location &location)
{
  return Ptr(new ConstBool(value, location));
}

ConstBool::ConstBool(bool value, const Location &location): Constant(location), m_value(value)
{

}

Accessor *ConstBool::accessor()
{
  auto access=Generic::accessor();
  access->setDataString([this](){ return QString::fromLatin1(m_value?"true":"false"); });
  return access;
}
