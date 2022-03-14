#pragma once
#include "Node/node.h"
namespace QTwig
{
  class Parser;
  class Engine;
  class TokenStream;

  //class QString;
  class ExpressionParser
  {
    friend class Parser;
    ExpressionParser(Parser *parser): m_parser(parser) { }
  public:
    // Deltas checked
    Node::Ptr parsePrimaryExpression();
    Node::Ptr parseExpression(int precedence=0, bool allowArrow=false);
    Node::Ptr parsePostfixExpression(Node::Ptr node);
    Node::Ptr parseArrayExpression();
    Node::Ptr parseHashExpression();
    Node::Ptr parseMemberAccessExpression(Node::Ptr nodeBase);
    Node::Ptr parseSubscriptExpression(Node::Ptr nodeBase);
    Node::Ptr parseAssignmentTargetExpression();
    // Matching twig
    Node::Ptr getPrimary();
    inline Node::Ptr parseFilterExpression(Node::Ptr node) { next(); return parseFilterExpressionRaw(node); }
    Node::Ptr parseFilterExpressionRaw(Node::Ptr node);
    Node::Ptr parseArguments(bool namedArguments = false, bool definition = false, bool allowArrow = false);
    Node::Ptr parseStringExpression();
    Node::Ptr parseConditionalExpression(Node::Ptr expr);
    Node::Ptr parseMultitargetExpression();

    // Unimplemented (for now)
    Node::Ptr parseArrow();
    // Unimplemented (and not planning to)
    // Delta: getFunctionCallNode is not implemented
    //Node::Ptr getFunctionCallNode(const QString &function, const QString location);

//      ParseEnvironment *env();
    TokenStream &stream();
    const Token &next();
//      QString getTest();
//      static Generic *parseTestExpression(ExpressionParser *p, Generic *parentNode, Operator2 *op);
//      static Generic *parseNotTestExpression(ExpressionParser *p, Generic *node, Operator2 *op);
//      inline Parser *parser() const {return m_parser;}
//      bool checkConstantExpression(Generic *node);
    Engine *engine();
    constexpr Parser *parser() { return m_parser; }
  protected:
    inline const Token &currentToken();
    Parser      *m_parser;
  };
}
