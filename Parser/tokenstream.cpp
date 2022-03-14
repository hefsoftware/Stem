#include "tokenstream.h"
#include <QObject>
#define tr QObject::tr
using namespace QTwig;

Token TokenStream::m_nullToken(Token::Type::Null,Location());
TokenStream::TokenStream(const QList<Token> &tokens):m_tokens(tokens)
{
  m_index=0;
}

bool TokenStream::isEOF()
{
  return m_index>=m_tokens.length() || m_tokens[m_index].type()==Token::Type::Eof;
}

const Token &TokenStream::get(unsigned int pos)
{
  return (pos<m_tokens.size()?m_tokens[pos]:m_nullToken);
}

const Token &TokenStream::currentToken()
{
//  if(isEOF())
//    return m_nullToken;
//  else
  return m_tokens[m_index];
}

QString TokenStream::currentText()
{
  return currentToken().text();
}

Token::Type TokenStream::currentType()
{
  return currentToken().type();
}
const Token &TokenStream::next()
{
  const Token &ret(currentToken());
  if(!isEOF())
    m_index++;
  return ret;
}

const Token &TokenStream::nullToken()
{
  return m_nullToken;
}

//const Token & TokenStream::expect(Token::TokenType type)
//{
//  const Token &tok=currentToken();
//  if(!tok.test(type))
//  {
//    pushError(tr("Unexpected token of type %1 (expected %2)").arg(Token::typeName(tok.type()),Token::typeName(type)));
//    return m_nullToken;
//  }
//  else
//  {
//    next();
//    return tok;
//  }
//}
//const Token & TokenStream::expect(Token::TokenType type, const QString &text)
//{
//  const Token &tok=currentToken();
//  next();
//  if(!tok.test(type,text))
//  {
//    pushError(tr("Unexpected token of type %1 (expected %2)").arg(Token::typeName(currentType()),Token::typeName(type)));
//    return m_nullToken;
//  }
//  else
//  {
//    return tok;
//  }
//}
//#include <stdio.h>
//void TokenStream::pushError(const QString &value)
//{
//  printf("Error in tokenstream: %s\n",qPrintable(value));
//}

const Token &TokenStream::nextIf(Token::Type type)
{
  if(currentToken().test(type))
    return next();
  else if(currentToken().test(Token::Type::Eof))
    return currentToken();
  else
    return nullToken();
}

const Token &TokenStream::nextIf(Token::Type type, const QString &text)
{
  if(currentToken().test(type, text))
    return next();
  else if(currentToken().test(Token::Type::Eof))
    return currentToken();
  else
    return nullToken();
}

const Token &TokenStream::look(int offset)
{
  int index=m_index+offset;
  if(index<0 || index>=m_tokens.length() || m_tokens[index].type()==Token::Type::Eof)
    return m_nullToken;
  else
    return m_tokens[index];
}
