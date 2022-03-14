#include "operator.h"
#include "runthread.h"
#include "../Parser/Node/unaryop.h"
#include "../Parser/Node/binaryop.h"
#include "value.h"
#define tr QString
using namespace QTwig;

Operator::Operator(const QString &text, int precedence): m_text(text),m_precedence(precedence)
{

}

QString Operator::text() const
{
  return m_text;
}

int Operator::precedence() const
{
    return m_precedence;
}

Operator::Associativity Operator2::associativity() const
{
  return m_associativity;
}



Operator2 *Operator2::setExtraParser(const std::function<QSharedPointer<Node::Generic> (ExpressionParser *, const QSharedPointer<Node::Generic> &, const Operator2 *)> &parser)
{
  m_parser=parser;
  return this;
}

Operator2 *Operator2::setEvaluator(const std::function<Value (RunThread &, const Node::BinaryOp *)> &evaluator)
{
  m_evaluator=evaluator;
  return this;
}

Operator2 *Operator2::setEvaluatorNum(const std::function<Value (QVariant, QVariant)> &opInt, const std::function<Value (QVariant, QVariant)> &opFloat)
{
  return setEvaluator(generateNumOp2(opInt, opFloat));
}


Operator1 *Operator1::setEvaluator(const std::function<Value (RunThread &, const Node::UnaryOp *)> &evaluator)
{
  m_evaluator=evaluator;
  return this;
}

Operator1 *Operator1::setEvaluatorNum(const std::function<Value (QVariant)> &opInt, const std::function<Value (QVariant)> &opFloat)
{
  return setEvaluator(generateNumOp1(opInt, opFloat));
}

std::function<Value (RunThread &, const Node::UnaryOp *)> Operator1::generateNumOp1(const std::function<Value (QVariant)> &opInt, const std::function<Value (QVariant)> &opFloat)
{
  return [opInt, opFloat](RunThread &thread, const Node::UnaryOp *op){
    Value val(thread.evaluate(op->expr().data()));
    if(val.isValid())
    {
      QVariant v=val.value();
      if(!Value::isNumberType(v.typeId()))
        val=Value::createError(tr("Expression is not a number but a '%1'").arg(QMetaType(v.typeId()).name()), op->expr()->location());
      else
      {
        if(Value::isIntType(v.typeId()))
          val=opInt(v);
        else
          val=opFloat(v);
      }
    }
    return val;
  };
}

std::function<Value (RunThread &, const Node::BinaryOp *)> Operator2::generateNumOp2(const std::function<Value (QVariant, QVariant)> &opInt, const std::function<Value (QVariant, QVariant)> &opFloat)
{
  return [opInt, opFloat](RunThread &thread, const Node::BinaryOp *op){
    Value val(thread.evaluate(op->expr1().data()));
    if(val.isValid())
    {
      Value val2(thread.evaluate(op->expr2().data()));
      if(val2.isValid())
      {
        QVariant v1=val.value(), v2=val2.value();
        if(!Value::isNumberType(v1.typeId()))
          val=Value::createError(tr("Expression is not a number but a '%1'").arg(QMetaType(v1.typeId()).name()), op->expr1()->location());
        else if(!Value::isNumberType(v2.typeId()))
          val=Value::createError(tr("Expression is not a number but a '%1'").arg(QMetaType(v2.typeId()).name()), op->expr2()->location());
        else
        {
          if(Value::isIntType(v1.typeId()) && Value::isIntType(v2.typeId()))
            val=opInt(v1, v2);
          else
            val=opFloat(v1, v2);
        }
      }
      else
        val=val2;
    }
    return val;
  };

}
