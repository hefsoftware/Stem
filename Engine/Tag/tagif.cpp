#include "tag.h"
#include "../runthread.h"
#include "../value.h"
#include "../../Parser/Node/list.h"
#include "../../Parser/Node/accessor.h"
#include "../../Parser/Node/node2.h"

#include <QDebug>
namespace QTwig
{
  namespace Node
  {
    class If: public Node2
    {
    public:
      If(Ptr tests, Ptr elseBody, const Location &location=Location()): Node2(tests, elseBody, location) { }
      Type type() const override { return Type::If; }
      bool hasExecute() const override { return true; }
      bool execute(RunThread &thread) const override;

      Ptr tests() { return m_expr1; }
      ConstPtr tests() const { return m_expr1; }
      Ptr elseBody() { return m_expr1; }
      ConstPtr elseBody() const { return m_expr2; }
    protected:
      Accessor *accessor() override
      {
        auto ret=Node2::accessor();
        ret->renameChildren("expr1", "tests");
        ret->renameChildren("expr2", "else")->setOmitIfEmpty(true);
        return ret;
      }
    };

    class IfTest: public Node2
    {
    public:
      IfTest(Ptr condition, Ptr body, const Location &location=Location()): Node2(condition, body, location) { }
      Type type() const override { return Type::IfTest; }
      Ptr test() { return m_expr1; }
      ConstPtr test() const { return m_expr1; }
      Ptr body() { return m_expr2; }
      ConstPtr body() const { return m_expr2; }
    protected:
      Accessor *accessor() override
      {
        auto ret=Node2::accessor();
        ret->renameChildren("expr1", "test");
        ret->renameChildren("expr2", "body");
        return ret;
      }
    };
  }
}
using namespace QTwig;
using namespace QTwig::Node;
#define tr QObject::tr
bool If::execute(RunThread &thread) const
{
  bool ret=true;
  bool hadTrueCond=false;
  Q_ASSERT(m_expr1.dynamicCast<Node::List>());
  auto conditions=m_expr1.staticCast<Node::List>().data();
  for(int i=0;ret && i<conditions->numChildren();i++)
  {
    Q_ASSERT(conditions->children(i).dynamicCast<Node::IfTest>());
    auto cond=conditions->children(i).staticCast<Node::IfTest>().data();
    auto check=thread.evaluate(cond->test().data());
    if(check.isValid())
      check=check.toBool();
    if(!check.isValid())
    {
      thread.errorMessage(check, cond);
      ret=false;
    }
    else if(check.value().toBool())
    {
      hadTrueCond=true;
      ret=thread.executeNode(cond->body().data());
      break;
    }
  }
  if(ret && !hadTrueCond)
    ret=thread.executeNode(m_expr2.data());
  return ret;
}

Ptr TagParse::parseIf(Parser &p)
{
  Location loc=p.location();
  Ptr expr(p.parseExpression());
  if(!expr)
    return nullptr;
  if(!p.expect(Token::Type::BlockEnd, "TokenParserIf", "Expected end of block"))
    return nullptr;
  auto isEndIf=[](const QString &tag) { return (tag=="endif"); };
  auto isEndFork=[](const QString &tag) { return (tag=="elseif" || tag=="else" || tag=="endif"); };
  Ptr body(p.subparse(isEndFork));
  if(!body)
    return nullptr;
  QList<Ptr> tests;
  Ptr elseBody;
  tests.append(Ptr(new IfTest(expr, body, loc)));
  while(true)
  {
    const Token t(p.next());
    if(t.test("else"))
    {
      if(!p.expect(Token::Type::BlockEnd, "TokenParserIf", "Expected end of block"))
        return nullptr;
      elseBody=p.subparse(isEndIf);
      if(!elseBody) return nullptr;
    }
    else if(t.test("elseif"))
    {
      Location loc(t.location());
      Ptr cond=p.parseExpression();
      if(!cond) return nullptr;
      if(!p.expect(Token::Type::BlockEnd, "TokenParserIf", "Expected end of block"))
        return nullptr;
      Ptr body=p.subparse(isEndFork);
      if(!body) return nullptr;
      tests.append(Ptr(new IfTest(cond, body, loc)));
    }
    else if(t.test("endif"))
    {
      break;
    }
    else
    {
      p.failedUnexpected("TokenParserIf",QString("Expected \"else\", \"elseif\", or \"endif\" to end the if started at line %1").arg(loc.row()));
      return nullptr;
    }
  }
  if(!elseBody)
    elseBody=Node::newList({}, p.location());
  p.next();
  return Ptr(new Node::If(Node::newList(tests, loc), elseBody, loc));
}


//#include "blockparserif.h"
//#include "../expressionparser.h"
//#include "../nodelist.h"
//#include "../../Node/generic.h"
//#include "../parser.h"

//using namespace QTwig;
//using namespace QTwig::BlockParsers;

//namespace QTwig
//{
//  namespace BlockParsers
//  {
//    BlockParser *newIf()
//    {
//      return new BlockParserIf;
//    }
//  }
//}

//BlockParserIf::BlockParserIf()
//{

//}

//Generic *BlockParserIf::parse(Parser *parser)
//{

//}

//QString BlockParserIf::tag()
//{
//  return "if";
//}

//bool BlockParserIf::isEndFork(const Token &token, void *extra)
//{
//  Q_UNUSED(extra);
//  return (token.test("elseif") || token.test("else") || token.test("endif"));
//}

//bool BlockParserIf::isEndIf(const Token &token, void *extra)
//{
//  Q_UNUSED(extra);
//  return token.test("endif");
//}
