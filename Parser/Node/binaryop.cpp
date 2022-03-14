#include "binaryop.h"
#include "../../Engine/operator.h"
#include "accessor.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newBinaryOp(Ptr expr1, Ptr expr2, const Operator2 *op, const Location &location)
{
  return Ptr(new BinaryOp(expr1, expr2, op, location));
}

Accessor *BinaryOp::accessor()
{
  auto ret=Node2::accessor();
  ret->setDataString([this](){ return m_op->text(); });
  return ret;
}
