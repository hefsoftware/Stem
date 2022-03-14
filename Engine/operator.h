#pragma once
#include <QString>
#include <QSharedPointer>
#include <functional>

namespace QTwig
{
  class Parser;
  class Engine;
  class RunThread;
  class Value;
  class ExpressionParser;
  namespace Node{
    class Generic;
    class BinaryOp;
    class UnaryOp;
  };
  class Operator
  {
  public:
    enum Associativity
    {
      OPERATOR_LEFT,
      OPERATOR_RIGHT
    };

    Operator(const QString &text, int precedence);
    QString text() const;

    int precedence() const;

  protected:
    QString m_text;
    int m_precedence;
  };
  class Operator1:public Operator
  {
  public:
    Operator1(const QString &text, int precedence): Operator(text,precedence){}
    Operator1 *setEvaluator(const std::function<Value (RunThread &, const Node::UnaryOp *)> &evaluator);
    Operator1 *setEvaluatorNum(const std::function<Value(QVariant val)> &opInt, const std::function<Value(QVariant val)> &opFloat);
    constexpr const std::function<Value (RunThread &, const Node::UnaryOp *op)> &evaluator() const { return m_evaluator; }
  protected:
    std::function<Value(RunThread &thread, const Node::UnaryOp *op)> generateNumOp1(const std::function<Value(QVariant val)> &opInt, const std::function<Value(QVariant val)> &opFloat);

    std::function<Value (RunThread &, const Node::UnaryOp *op)> m_evaluator;
  };

  class Operator2:public Operator
  {
  public:
    Operator2(const QString &text, int precedence, Operator::Associativity associativity=Operator::OPERATOR_LEFT):Operator(text, precedence),m_associativity(associativity){}
    Operator2 *setExtraParser(const std::function<QSharedPointer<Node::Generic>(ExpressionParser *, const QSharedPointer<Node::Generic> &, const Operator2 *)> &parser);
    Operator2 *setEvaluator(const std::function<Value (RunThread &, const Node::BinaryOp *)> &evaluator);
    Operator2 *setEvaluatorNum(const std::function<Value(QVariant val1, QVariant val2)> &opInt, const std::function<Value(QVariant val1, QVariant val2)> &opFloat);
    Associativity associativity() const;
    inline std::function<QSharedPointer<Node::Generic> (ExpressionParser *, const QSharedPointer<Node::Generic> &, const Operator2 *)> parser() const { return m_parser; }
    constexpr const std::function<Value (RunThread &, const Node::BinaryOp *op)> &evaluator() const { return m_evaluator; }
  protected:
    std::function<Value(RunThread &thread, const Node::BinaryOp *op)> generateNumOp2(const std::function<Value(QVariant val1, QVariant val2)> &opInt, const std::function<Value(QVariant val1, QVariant val2)> &opFloat);

    std::function<QSharedPointer<Node::Generic>(ExpressionParser *, const QSharedPointer<Node::Generic>&, const Operator2 *)> m_parser;
    std::function<Value (RunThread &, const Node::BinaryOp *op)> m_evaluator;
    Associativity m_associativity;
  };
}
