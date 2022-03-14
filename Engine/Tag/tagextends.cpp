#include "tag.h"
#include "../../Parser/Node/list.h"
#include <QDebug>
using namespace QTwig;
#define tr QObject::tr
Node::Ptr TagParse::parseExtends(Parser &p)
{
  Location loc=p.location();
  Node::Ptr source=p.parseExpression();
  if(!source)
    return nullptr;
  if(!p.expect(Token::Type::BlockEnd, "Parse 'extends' tag"))
    return nullptr;
  return Node::newExtends(source, loc);
}
