#include "parser.h"
#include "expressionparser.h"
#include "../Engine/engine.h"
#include "tokenstream.h"
#include "../Engine/message.h"
#include <QDebug>
using namespace QTwig;
#define tr QString

Parser::Parser(Engine *engine, const TokenStream &stream, Message *output): m_engine(engine), m_stream(new TokenStream(stream)), m_output(output)
{
  m_expressionParser=nullptr;
}

Parser::~Parser()
{
  delete m_expressionParser;
}

Node::Ptr Parser::parse()
{
  return subparse();
}

Node::Ptr Parser::parseExpression(int precedence, bool allowArrow)
{
  return getExpressionParser()->parseExpression(precedence, allowArrow);
}

Node::Ptr Parser::subparse(std::function<bool (const QString &)> test, bool dropNeedle)
{
  bool success=true;
  Location startLoc;
  QList<Node::Ptr> nodes;
  if(m_stream->isEmpty())
  {
    failedParsing(tr("Subparse called with empty token stream"), Location());
    success=false;
  }
  while(success && !m_stream->isEOF())
  {
    const Token &token(currentToken());
    Location loc=token.location();
    switch(token.type())
    {
      case Token::Type::Text:
        nodes.append(Node::newText(currentToken().text(), loc));
        m_stream->next();
        break;
      case Token::Type::VarStart:
        {
          Node::Ptr parsed;
          Location loc(location());
          m_stream->next();
          parsed=parseExpression();
          if(!parsed || !expect(Token::Type::VarEnd,"subParse","Expecting variable end"))
            success=false;
          else
            nodes.append(Node::newPrint(parsed,loc));
        }
        break;
      case Token::Type::BlockStart:
      {
        next();
        Token name=currentToken();
        if(!name.test(Token::Type::Name))
        {
          failedUnexpected("subParse", "Block must start with a tag name");
          success=false;
        }
        else
        {
          if(test && test(name.text()))
          {
            if(dropNeedle)
              next();
            if(nodes.size()==0)
              return Node::newList({}, name.location());
            else if(nodes.size()==1)
              return nodes.first();
            else
              return Node::newList(nodes, nodes.first()->location());
          }
          auto parser=m_engine->getTag(name.text());
          if(parser)
          {
            stream().next();
            auto parsed=parser(*this);
            if(parsed)
              nodes.append(parsed);
            else
              success=false;
          }
          else
          {
            failedParsing(tr("Unexpected tag %1").arg(name.text()), name.location());
            success=false;
          }
        }
        break;
      }
    default:
      failedUnexpected("subparse","Lexer or parser ended up in an usupported state");
      success=false;
        break;
    }
  }
//if(nodes.count()==1)
//  return nodes.take(0);
//else
//  return Nodes::newList(nodes.take(),loc);
  return success?Node::newList(nodes, m_stream->get(0).location()):nullptr;
}

void Parser::failedParsing(const QString &msg, const Location &location)
{
  if(m_output)
    m_output->error(msg, location);
  //qDebug()<<"FAILED PARSING"<<location.row()<<location.column()<<msg;
}
void Parser::failedParsing(const QString &msg)
{
  failedParsing(msg, currentToken().location());
}
void Parser::failedUnexpected(const QString &in, const QString &expected)
{
  QString text;
  if(currentToken().text().isEmpty())
    text=tr("Unexpected token %1 while parsing %2").arg(Token::typeName(currentTokenType()),in);
  else
    text=tr("Unexpected token %1 (%2) while parsing %3").arg(Token::typeName(currentTokenType()),currentToken().shortenedText(20),in);
  if(!expected.isEmpty())
    text+=tr(" (%1)").arg(expected);
  //qDebug()<<"Failed unexpected"<<currentToken().printable()<<currentToken().location().column()<<currentToken().location().row();
  failedParsing(text, currentToken().location());
}

void Parser::unimplementedWarning(const QString &what)
{
  if(m_output)
    m_output->warning("Unimplemented!", what);
}

const Token &Parser::expect(Token::Type type, const QString &in,const QString &expected)
{
  auto &ret=nextIf(type);
  if(!ret)
  {
    QString expectedStr(expected.isEmpty()?QString("Expected %1").arg(Token::typeName(type)):expected);
    failedUnexpected(in, expectedStr);
  }
  return ret;
}

const Token &Parser::expect(const QString &value, Token::Type type, const QString &in, const QString &expected)
{
  auto &ret=nextIf(type,value);
  if(!ret)
  {
    QString expectedStr(expected.isEmpty()?QString("Expected %2(\"%1\")").arg(value,Token::typeName(type)):expected);
    failedUnexpected(in, expectedStr);
  }
  return ret;
}

const Token &Parser::next()
{
  return m_stream->next();
}

const Token &Parser::nextIf(Token::Type type)
{
  return m_stream->nextIf(type);
}

const Token &Parser::nextIf(Token::Type type, const QString &text)
{
  return m_stream->nextIf(type, text);
}

const Token &Parser::currentToken()
{
  return m_stream->currentToken();
}

Location Parser::location()
{
  return currentToken().location();
}

ExpressionParser *Parser::getExpressionParser()
{
  if(!m_expressionParser)
    m_expressionParser=new ExpressionParser(this);
  return m_expressionParser;
}

Token::Type Parser::currentTokenType()
{
  return currentToken().type();
}
