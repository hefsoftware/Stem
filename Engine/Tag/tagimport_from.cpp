#include "tag.h"
#include "../../Parser/Node/list.h"
#include <QDebug>
using namespace QTwig;
#define tr QObject::tr
Node::Ptr TagParse::parseImport(Parser &p)
{
  Location loc=p.location();
  Node::Ptr source=p.parseExpression();
  if(!p.expect("as", Token::Type::Name, "Parsing 'import' tag", "Expected 'as'"))
    return nullptr;
  if(auto &t=p.expect(Token::Type::Name, "Parsing 'import' tag"))
  {
    if(!p.expect(Token::Type::BlockEnd, "Parsing 'import' tag"))
      return nullptr;
    source=Node::newImport(source, Node::newName(t.text(), t.location()), loc);
  }
  else
    return nullptr;
  return source;
//  $var = new AssignNameExpression($this->parser->getStream()->expect(/* Token::NAME_TYPE */ 5)->getValue(), $token->getLine());
//  $this->parser->getStream()->expect(/* Token::BLOCK_END_TYPE */ 3);

//  $this->parser->addImportedSymbol('template', $var->getAttribute('name'));

//  return new ImportNode($macro, $var, $token->getLine(), $this->getTag(), $this->parser->isMainScope());

}
Node::Ptr TagParse::parseFrom(Parser &p)
{
  Location loc=p.location();
  Node::Ptr source=p.parseExpression();
  QList<Node::Ptr> list;
  if(!source)
    return nullptr;
  if(!p.expect("import", Token::Type::Name, "Parse 'from' tag", "Expected keyword 'import'"))
    return nullptr;
  do
  {
    auto loc=p.currentToken().location();
    QString alias;
    QString name;
    if(auto &t=p.expect(Token::Type::Name, "Parse 'from' tag"))
      name=t.text();
    else
      return nullptr;

    if (p.nextIf(Token::Type::Name, "as")) {
      if(auto &t=p.expect(Token::Type::Name, "Parse 'from' tag"))
        alias=t.text();
      else
        return nullptr;
    }
    else
      alias=name;
    list.append(Node::newImportAs(name, alias, loc));
    if (!p.nextIf(Token::Type::Punctuation, ","))
      break;
  } while(true);
  if(!p.expect(Token::Type::BlockEnd, "Parse 'from' tag"))
    return nullptr;
  return Node::newImport(source, Node::newList(list,loc), loc);
}
