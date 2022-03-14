#include "constdouble.h"
#include "accessor.h"

using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newConstDouble(double value, const Location &location)
{
  return Ptr(new ConstDouble(value, location));
}

ConstDouble::ConstDouble(double value, const Location &location): Constant(location), m_value(value)
{

}

Accessor *ConstDouble::accessor()
{
  auto access=Generic::accessor();
  access->setDataString([this](){ return QString::number(m_value); });
  return access;
}
