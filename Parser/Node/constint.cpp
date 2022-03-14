#include "constint.h"
#include "accessor.h"

using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newConstInt(qint64 value, const Location &location)
{
  return Ptr(new ConstInt(value, location));
}

ConstInt::ConstInt(qint64 value, const Location &location): Constant(location), m_value(value)
{

}

Accessor *ConstInt::accessor()
{
  auto access=Generic::accessor();
  access->setDataString([this](){ return QString::number(m_value); });
  return access;
}
