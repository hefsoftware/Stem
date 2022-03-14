#pragma once

#include <QList>
#include "token.h"
namespace QTwig
{
  class TokenStream
  {
  public:
    TokenStream(const QList<Token> &tokens=QList<Token>());
    inline bool isEmpty() { return (m_tokens.isEmpty()); }
    bool isEOF();
    const Token &get(unsigned int pos);
    const Token &currentToken();
    QString currentText();
    Token::Type currentType();
    const Token &next();
    static const Token &nullToken();
    //    const Token &expect(Token::TokenType type);
    //    const Token &expect(Token::TokenType type, const QString &text);
    const Token &nextIf(Token::Type type);
    const Token &nextIf(Token::Type type, const QString &text);
    const Token &nextIf(Token &t,Token::Type type);
    const Token &nextIf(Token &t,Token::Type type, const QString &text);
    const Token &look(int offset);
    const QList<Token> &tokens() const{return m_tokens;}
  private:
//    void pushError(const QString &value);
    static Token m_nullToken;
    int m_index;
    QList<Token> m_tokens;
  };
}
