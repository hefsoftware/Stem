#include "tag.h"
#include "../../Parser/Node/list.h"
#include <QDebug>
using namespace QTwig;
#define tr QObject::tr
static bool parseArguments(Parser &p, bool &ignoreMissing, Node::Ptr &variables, bool &only);
Node::Ptr TagParse::parseInclude(Parser &p)
{
  Location loc=p.location();
  Node::Ptr source=p.parseExpression();
  if(!source)
    return nullptr;
  bool ignoreMissing, only;
  Node::Ptr variables;
  if(!parseArguments(p, ignoreMissing, variables, only))
    return nullptr;
  if(!p.expect(Token::Type::BlockEnd, "Parse 'include' tag"))
    return nullptr;
  return Node::newInclude(source, ignoreMissing, variables, only, loc);
}

Node::Ptr TagParse::parseEmbed(Parser &p)
{
  Location loc=p.location();
  Node::Ptr source=p.parseExpression();
  if(!source)
    return nullptr;
  bool ignoreMissing, only;
  Node::Ptr variables, body;
  if(!parseArguments(p, ignoreMissing, variables, only))
    return nullptr;
  if(!p.expect(Token::Type::BlockEnd, "Parse 'embed' tag"))
    return nullptr;
  body=p.subparse([](const QString & tag){return tag=="endembed";}, true);
  if(!p.expect(Token::Type::BlockEnd, "Parse 'endembed' tag"))
    return nullptr;
  return Node::newEmbed(source, ignoreMissing, variables, only, body, loc);
}


static bool parseArguments(Parser &p, bool &ignoreMissing, Node::Ptr &variables, bool &only)
{
  ignoreMissing=only=false;
  variables=Node::Ptr();
  if(p.nextIf(Token::Type::Name, "ignore"))
  {
    if(p.expect("missing", Token::Type::Name, "Arguments", "Expected 'missing after 'ignore'"))
      ignoreMissing=true;
    else
      return false;
  }
  if(p.nextIf(Token::Type::Name, "with"))
  {
    variables=p.parseExpression();
    if(!variables)
      return false;
    if(p.nextIf(Token::Type::Name, "only"))
    {
      only=true;
    }
  }
  return true;
}
