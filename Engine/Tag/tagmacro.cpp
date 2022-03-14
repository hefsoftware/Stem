#include "tag.h"
#include "../../Parser/Node/list.h"
#include <QDebug>
using namespace QTwig;
#define tr QObject::tr
Node::Ptr TagParse::parseMacro(Parser &p)
{
  Location loc=p.location();
  auto &nameToken=p.currentToken();
  if(!p.expect(Token::Type::Name, "Parse 'macro' tag"))
    return nullptr;
  QString name(nameToken.text());
  Node::Ptr arguments(p.getExpressionParser()->parseArguments(true,true));
  if(!arguments || !p.expect(Token::Type::BlockEnd, "Parse 'macro' tag"))
    return nullptr;
  Node::Ptr body(p.subparse([](const QString &tag){ return (tag=="endmacro"); }, true));
  if(!body)
    return nullptr;
  if(auto t=p.nextIf(Token::Type::Name))
  {
    if(name!=t.text())
    {
      p.failedParsing(QString("Expected endmacro for macro %1 (%2), but %3 was given").arg(name, loc.toLocalString(), t.text()),t.location());
      return nullptr;
    }
  }
  if(!p.expect(Token::Type::BlockEnd, "Parse 'macro' tag"))
    return nullptr;
//  parser->setMacro(name,Nodes::newMacro(arguments.take(), body.take(), loc));
  return Node::newMacro(name, arguments, body, loc);
}
