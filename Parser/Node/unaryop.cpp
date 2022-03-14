#include "unaryop.h"
#include "../../Engine/operator.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newUnaryOp(Ptr expr, const Operator1 *op, const Location &location)
{
  return Ptr(new UnaryOp(expr, op, location));
}

Accessor *UnaryOp::accessor()
{
  auto ret=Node1::accessor();
  ret->setDataString([this](){ return m_op->text(); });
  return ret;
}
