#include "tag.h"
#include "../../Parser/Node/list.h"
#include <QDebug>
using namespace QTwig;
#define tr QObject::tr
Node::Ptr TagParse::parseBlock(Parser &p)
{
    Location loc=p.location();
    auto &nameToken=p.currentToken();
    if(!p.expect(Token::Type::Name, "TokenParserBlock","Expected the block name"))
        return nullptr;
    QString name(nameToken.text());
    Node::Ptr body;
    if(p.nextIf(Token::Type::BlockEnd))
    {
        body=p.subparse([](const QString &tag){return tag=="endblock";},true);
        if(!body)
            return nullptr;
        if(auto t=p.nextIf(Token::Type::Name))
        {
          if(name!=t.text())
          {
            p.failedParsing(QString("Expected end block for block %1 (%2), but %3 was given").arg(name, loc.toLocalString(), t.text()),t.location());
            return nullptr;
          }
        }
    }
    else
    {
        Location loc=p.location();
        body=p.parseExpression();
        if(!body)
            return nullptr;
        body=Node::newPrint(body,loc);
    }
    if(!p.expect(Token::Type::BlockEnd, "Parse block tag"))
      return nullptr;
    return Node::newBlock(name, body, loc);
}
