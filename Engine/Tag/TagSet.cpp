#include "tag.h"
#include "../../Parser/Node/list.h"
using namespace QTwig;
#define tr QObject::tr
Node::Ptr TagParse::parseSet(Parser &p)
{
  Node::Ptr ret;
  ExpressionParser *expr=p.getExpressionParser();
  auto &startToken=p.currentToken();
  auto target=expr->parseAssignmentTargetExpression();
  if(target)
  {
    Q_ASSERT(target->type()==Node::Type::List && dynamic_cast<Node::List *>(target.data()));
    auto numTargets=target.staticCast<Node::List>()->numChildren();
    if(p.nextIf(Token::Type::Operator, "="))
    {
      auto values=expr->parseMultitargetExpression();
      if(values)
      {
        Q_ASSERT(values->type()==Node::Type::List && dynamic_cast<Node::List *>(values.data()));
        int numValues=values.staticCast<Node::List>()->numChildren();
        if(numValues!=numTargets)
          p.failedParsing(tr("Number of set targets and values mismatch (%1 vs %2).").arg(numTargets).arg(numValues));
        else if(p.expect(Token::Type::BlockEnd, "TagParse::set"))
          ret=Node::newSet(target, values, startToken.location());
      }
    }
    else
    {
      if(numTargets!=1)
      {
        p.failedParsing(tr("When using set with a block, you cannot have a multi-target."));
      }
      else if(p.expect(Token::Type::BlockEnd, "TagParse::set"))
      {
        auto &valueToken=p.currentToken();
        auto value=p.subparse([](const QString &t){ return t=="endset"; }, true);
        if(value && p.expect(Token::Type::BlockEnd, "TagParse::set"))
          ret=Node::newSet(target, Node::newList({value}, valueToken.location()), startToken.location());
      }
    }
  }
  return ret;
}
