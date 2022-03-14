#pragma once
#include "Node/node.h"
#include "token.h"
namespace QTwig
{
  class Engine;
  class Message;
  class TokenStream;
  class ExpressionParser;
  class Parser
  {
  public:
    Parser(Engine *engine, const TokenStream &stream, Message *output);
    Parser(const Parser &) = delete;
    ~Parser();
    Node::Ptr parse();
    Node::Ptr subparse(std::function<bool (const QString &)> test=nullptr, bool dropNeedle=false);
    Node::Ptr parseExpression(int precedence=0, bool allowArrow=false);

    // Functions to output messages and errors
    void failedParsing(const QString &msg, const Location &location);
    void failedParsing(const QString &msg);
    void failedUnexpected(const QString &in, const QString &expected=QString());
    void unimplementedWarning(const QString &what);

    // Function that can be used by parser and sub-parsers to handle token stream
    const Token &expect(Token::Type type, const QString &in, const QString &expected=QString());
    const Token &expect(const QString &value,Token::Type type, const QString &in, const QString &expected=QString());
    QString lastTokenText();
    const Token &next();
    const Token &nextIf(Token::Type type);
    const Token &nextIf(Token::Type type, const QString &text);
    const Token &currentToken();
    Token::Type currentTokenType();
    Location location();

    ExpressionParser *getExpressionParser();
    TokenStream &stream() { return *m_stream; }
    constexpr Engine *engine() { return m_engine; }
  protected:

    Engine *m_engine;
    TokenStream *m_stream;
    Message *m_output;
    ExpressionParser *m_expressionParser;
  };
}
