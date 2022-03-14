#include "tag.h"
#include "../../Parser/Node/list.h"
#include <QDebug>
using namespace QTwig;
#define tr QObject::tr
Node::Ptr TagParse::parseUse(Parser &p)
{
  auto &source=p.currentToken();
  if(!p.expect(Token::Type::String, "Parse 'use' tag"))
    return nullptr;
  QList<Node::Ptr> list;
  if(p.nextIf(Token::Type::Name, "with"))
  {
    do
    {
      if(auto &nameToken=p.expect(Token::Type::Name, "Parse 'use' tag"))
      {
        QString name, alias;
        name=alias=nameToken.text();
        if(p.nextIf(Token::Type::Name, "as"))
        {
          if(auto &aliasToken=p.expect(Token::Type::Name, "Parse 'use' tag"))
            alias=aliasToken.text();
          else
            return nullptr;
        }
        list.append(Node::newImportAs(name, alias, nameToken.location()));
        if (!p.nextIf(Token::Type::Punctuation, ","))
          break;
      }
      else
        return nullptr;
   }while(true);
 }
 if(!p.expect(Token::Type::BlockEnd, "Parse 'use' tag"))
   return nullptr;
 qDebug()<<"Parsed use"<<list.size();
 return Node::newUse(Node::newText(source), list.isEmpty()?Node::Ptr():Node::newList(list, source.location()), source.location());
}
