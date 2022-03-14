#include "expressionparser.h"
#include "parser.h"
#include "tokenstream.h"
#include "../Engine/engine.h"
#include "../Engine/operator.h"
#include "lexerenv.h"
#include "Node/name.h"
using namespace QTwig;
using namespace QTwig::Node;
#define tr QObject::tr
TokenStream &ExpressionParser::stream()
{
  return m_parser->stream();
}

const Token &ExpressionParser::next()
{
  return m_parser->next();
}

Engine *ExpressionParser::engine()
{
  return m_parser->engine();
}
const Token &ExpressionParser::currentToken()
{
  return m_parser->currentToken();
}
// Delta: parseSubscriptExpression is split in parseSubscriptExpression and parseMemberAccessExpression
Ptr ExpressionParser::parseMemberAccessExpression(Node::Ptr nodeBase)
{
  Ptr ret;
  if(parser()->expect(".", Token::Type::Punctuation, "parseMemberAccessExpression", "BUG! Expected dot"))
  {
    auto token=currentToken();
    if(token.type()==Token::Type::Name || (token.type()==Token::Type::Operator && engine()->lexerEnv()->reName().match(token.text()).hasMatch()))
    {
      ret=Node::newMemberAccess(nodeBase, token.text(), token.location());
      next();
    }
    else
    {
      // Delta: Twig allows also access to numbered members (e.g. foo.1)
      parser()->failedUnexpected("parseMemberAccess", "Expected a name or a number");
    }
    // Delta: Calls are handled in parsePostFixExpression
    // Delta: Twig here checks if expression is a template
  }
  return ret;
}
// Delta: parseSubscriptExpression is split in parseSubscriptExpression and parseMemberAccessExpression
Ptr ExpressionParser::parseSubscriptExpression(Node::Ptr nodeBase)
{
  Ptr ret;
  if(parser()->expect("[", Token::Type::Punctuation, "parseMemberAccessExpression", "BUG! Expected dot"))
  {
    auto token=currentToken();
    Ptr index, end, step;
    bool isNotAccess;

    if((isNotAccess=token.test(Token::Type::Punctuation, ":")))
      index=Node::newConstNull(token.location());
    else
      index=parseExpression();
    if(index && stream().nextIf(Token::Type::Punctuation, ":"))
    {
      // Slice [start?:      (end? | (end?:step?)) ]
      step=end=Node::newConstNull(currentToken().location());
      if(!currentToken().test(Token::Type::Punctuation, "]"))
      {
        // Slice [start?:      (end? | (end?:step?)) ]
        if(!currentToken().test(Token::Type::Punctuation, ":"))
          end=parseExpression();
        if(step && stream().nextIf(Token::Type::Punctuation, ":"))
        {
          if(!currentToken().test(Token::Type::Punctuation, "]"))
            step=parseExpression();
        }
      }
      if(index && end && step)
        ret=Node::newFilter("slice", Node::newList({index, end, step}, token.location()), nodeBase, token.location());
    }
    else if(index)
    {
      if(isNotAccess)
        parser()->failedParsing("An index must be specified for array access");
      else
        ret=Node::newArrayGet(index, nodeBase, token.location());
    }
    if(!stream().nextIf(Token::Type::Punctuation, "]"))
    {
      parser()->failedUnexpected("parseMemberAccessExpression", "Expected \"]\"");
      ret=nullptr;
    }
  }
  return ret;
}

Ptr ExpressionParser::parseArrow()
{
  parser()->failedParsing(tr("Arrow expression not handled (for now)"), currentToken().location());
  return nullptr;
}

Ptr ExpressionParser::parseHashExpression()
{
  QList<Ptr> values;
  Location loc=currentToken().location();
  if(!parser()->expect("{",Token::Type::Punctuation,"parseArrayExpression","Hashes should start with \"{\""))
    return nullptr;
  bool first=true;

  while(!currentToken().test(Token::Type::Punctuation,"}"))
  {
    Location loc=currentToken().location();
    if(!first)
    {
      if(!parser()->expect(",", Token::Type::Punctuation, "parseArrayExpression","An hash element must be followed by a comma"))
        return nullptr;
      if(currentToken().test(Token::Type::Punctuation, "}"))
        break;
    }
    else
      first=false;
    Ptr key, value;
    Token t;
    if((t=parser()->nextIf(Token::Type::String)) || (t=parser()->nextIf(Token::Type::Name)) )
      key=Node::newText(t.text(),t.location());
    else if((t=parser()->nextIf(Token::Type::Number)))
    {
      key=engine()->parseNumber(t.text(), t.location());
      if(!key)
      {
        parser()->failedParsing(tr("BUG! Can't convert number '%1' to node").arg(t.text()));
        return nullptr;
      }
    }
    else if(currentToken().test(Token::Type::Punctuation,"("))
    {
      key=parseExpression();
      if(!key)
        return nullptr;
    }
    else
    {
      parser()->failedUnexpected("parseHashExpression","A hash key must be a quoted string, a number or a name or an expression enclosed in parenthesis");
      return nullptr;
    }
    if(!parser()->expect(":",Token::Type::Punctuation,"parseHashExpression","A hash key must be followed by a colon (:)"))
      return nullptr;
    value=parseExpression();
    if(!value)
      return nullptr;
    values.append(Node::newMap(key,value,loc));
  }
  if(!parser()->expect("}",Token::Type::Punctuation, "parseArrayExpression","An opened array is not properly closed"))
    return nullptr;
  return Node::newHash(values, loc);
}


Ptr ExpressionParser::parseArrayExpression()
{
  QList<Ptr> values;
  Location loc=currentToken().location();
  if(!parser()->nextIf(Token::Type::Punctuation,"["))
  {
    m_parser->failedUnexpected("parseArrayExpression","Array should start with \"[\"");
    return nullptr;
  }
  bool first=true;
  while(!currentToken().test(Token::Type::Punctuation,"]"))
  {
    if(!first)
    {
      if(!parser()->expect(",",Token::Type::Punctuation,"parseArrayExpression","An array element must be followed by a comma"))
        return nullptr;
      if(currentToken().test(Token::Type::Punctuation, "]"))
        break;
    }
    else
      first=false;
    Ptr cur=parseExpression();
    if(!cur)
      return nullptr;
    values.append(cur);
  }
  if(!parser()->expect("]",Token::Type::Punctuation, "parseArrayExpression","An opened array is not properly closed"))
    return nullptr;
  return (Node::newArray(values, loc));
}

Ptr ExpressionParser::parseFilterExpressionRaw(Ptr node)
{
  while (true) {
    const Token &nameToken(parser()->nextIf(Token::Type::Name));
    if(!nameToken)
    {
      m_parser->failedUnexpected("parseFilterExpressionRaw", "Expected a name");
      node=nullptr;
      break;
    }
    Ptr arguments=currentToken().test(Token::Type::Punctuation, "(")?
                    parseArguments(true, false, true):
                    Node::newList({}, currentToken().location());
    if(!arguments)
    {
      node=nullptr;
      break;
    }
    node=Node::newFilter(nameToken.text(), arguments, node, nameToken.location());
    if(!parser()->nextIf(Token::Type::Punctuation, "|"))
      break;
  }
  return node;
}


Ptr ExpressionParser::parseExpression(int precedence, bool allowArrow)
{
  Ptr ret;
  if(allowArrow && (ret=parseArrow()))
  {

  }
  else
  {
    ret=getPrimary();
    if(ret)
    {
      const Operator2 *op=nullptr;
      while(currentToken().test(Token::Type::Operator) && (op=engine()->getBinaryOp(currentToken().text())) && op->precedence()>=precedence)
      {
        parser()->next();
        if(auto p=op->parser()) // Delta: Twig here explicitly specifies "is" and "is not" parsing. Here the parsing will be handled by a custom parser for the operators
        {
          ret=p(this, ret, op);
        }
        else
        {
          Token t=currentToken();
          Ptr expr2=parseExpression(op->associativity()==Operator::OPERATOR_LEFT?op->precedence()+1: op->precedence());
          if(expr2)
            ret=Node::newBinaryOp(ret,expr2,op,t.location());
          else
            ret=nullptr;
        }
      }
    }
    if(ret && precedence==0)
      ret=parseConditionalExpression(ret);
  }
  return ret;
}

Ptr ExpressionParser::parsePrimaryExpression()
{
  Ptr ret;
  auto token(currentToken());
  switch(token.type())
  {
    case Token::Type::Name:
      parser()->next();
      ret=engine()->getConstantExpression(token.text(), token.location());
      if(!ret)
      {
        if(currentToken().test(Token::Type::Punctuation, "("))
        {
          // Delta: Twig calls a method getFunctionCallNode that checks number of arguments and availability of function.
          // Stem always creates a FunctionCall node. It will be responsability of the called function to check the arguments.
          auto args=parseArguments(true);
          if(!args)
            ret=nullptr;
          else
            ret=Node::newFunctionCall(token.text(), args, token.location());
        }
        else
          ret=Node::newName(token.text(), token.location());
      }
      break;
    case Token::Type::Number:
      ret=m_parser->engine()->parseNumber(token.text(),token.location());
      if(!ret)
        m_parser->failedParsing(QString("Could not convert %1 to a number").arg(token.text()));
      m_parser->next();
      break;
    case Token::Type::String:
      ret=parseStringExpression();
      break;
    case Token::Type::InterpolationStart:
      ret=parseStringExpression();
      break;
    case Token::Type::Operator:
      if(currentToken().test(Token::Type::Operator, "=") && (stream().look(-1).text()=="==" || stream().look(-1).text()=="!=" ))
        m_parser->failedParsing(tr("Unexpected operator. Did you try to use \"===\" or \"!==\" for strict comparisation? Use is same as(value) instead."), currentToken().location());
      else
      {
        const Operator1 *op;
        if((op=engine()->getUnaryOp(currentToken().text()))) // Delta: Twig does not allow +/- unary operators in this context. Maybe we should add a predicate to the operator to disable this?
        {
          Location loc=currentToken().location();
          stream().next();
          ret=parsePrimaryExpression();
          if(ret)
            ret=Node::newUnaryOp(ret, op, loc);
        }
        else
        {
          if(engine()->lexerEnv()->reName().match(currentToken().text()).hasMatch())
          {
            ret=Node::newName(currentToken().text(), currentToken().location());
            stream().next();
          }
          else
            m_parser->failedParsing(tr("Unexpected non-unary operator"), currentToken().location());
        }
      }
      break;
    default:
      if(token.test(Token::Type::Punctuation,"["))
        ret=parseArrayExpression();
      else if(token.test(Token::Type::Punctuation,"{"))
        ret=parseHashExpression();
      else // Delta: Twig checks also for '===' and '!==' operators here and gives a specific error code in Qt.
        m_parser->failedUnexpected("primary expression");
      break;
  }
  return parsePostfixExpression(ret);
}

Ptr ExpressionParser::parsePostfixExpression(Ptr node)
{
  while(true)
  {
    auto token(currentToken());
    if(token.test(Token::Type::Punctuation,"."))
      node=parseMemberAccessExpression(node);
    else if(token.test(Token::Type::Punctuation,"[")) // Delta: subscript "[value]" and member access (.name) are considered different operators
      node=parseSubscriptExpression(node);
    else if(token.test(Token::Type::Punctuation,"|"))
      node=parseFilterExpression(node);
    else if(token.test(Token::Type::Punctuation, "("))
    {
      // Delta: Twig does not allow generic expressions call
      Ptr args=parseArguments(true);
      if(!args)
        return nullptr;
      node=Node::newCall(node, args, token.location());
      break;
    }
    else
      break;
    if(!node)
      break;
  }
  return node;
}



Ptr ExpressionParser::getPrimary()
{

  Ptr ret;
  const Operator1 *op=nullptr;
  if(currentToken().type()==Token::Type::Operator && (op=engine()->getUnaryOp(currentToken().text())))
  {
    Location loc=currentToken().location();
    parser()->next();
    ret=parseExpression(op->precedence());
    if(ret)
      ret=parsePostfixExpression(Node::newUnaryOp(ret, op, loc));
  }
  else if(currentToken().test(Token::Type::Punctuation,"("))
  {
    parser()->next();
    ret=parseExpression();
    if(ret && parser()->expect(")",Token::Type::Punctuation,"getPrimary","Expected closing parenthesis"))
      ret=parsePostfixExpression(ret);
  }
  else
    ret=parsePrimaryExpression();
  return ret;
}



Ptr parseArrow()
{
  return nullptr;
//  if(streamInternal()->look(1).test(Token::ARROW_TYPE))
//  {
//    TODO_UNIMPLEMENTED
//    //      $line = $stream->getCurrent()->getLine();
//    //      $token = $stream->expect(/* Token::NAME_TYPE */ 5);
//    //      $names = [new AssignNameExpression($token->getValue(), $token->getLine())];
//    //      $stream->expect(/* Token::ARROW_TYPE */ 12);
//    //      return new ArrowFunctionExpression($this->parseExpression(0), new Node($names), $line);
//  }
//  $stream = $this->parser->getStream();

//  // short array syntax (one argument, no parentheses)?
//  if ($stream->look(1)->test(/* Token::ARROW_TYPE */ 12)) {
//  }

//  // first, determine if we are parsing an arrow function by finding => (long form)
//  $i = 0;
//  if (!$stream->look($i)->test(/* Token::PUNCTUATION_TYPE */ 9, '(')) {
//      return null;
//  }
//  ++$i;
//  while (true) {
//      // variable name
//      ++$i;
//      if (!$stream->look($i)->test(/* Token::PUNCTUATION_TYPE */ 9, ',')) {
//          break;
//      }
//      ++$i;
//  }
//  if (!$stream->look($i)->test(/* Token::PUNCTUATION_TYPE */ 9, ')')) {
//      return null;
//  }
//  ++$i;
//  if (!$stream->look($i)->test(/* Token::ARROW_TYPE */ 12)) {
//      return null;
//  }

//  // yes, let's parse it properly
//  $token = $stream->expect(/* Token::PUNCTUATION_TYPE */ 9, '(');
//  $line = $token->getLine();

//  $names = [];
//  while (true) {
//      $token = $stream->expect(/* Token::NAME_TYPE */ 5);
//      $names[] = new AssignNameExpression($token->getValue(), $token->getLine());

//      if (!$stream->nextIf(/* Token::PUNCTUATION_TYPE */ 9, ',')) {
//          break;
//      }
//  }
//  $stream->expect(/* Token::PUNCTUATION_TYPE */ 9, ')');
//  $stream->expect(/* Token::ARROW_TYPE */ 12);

  //  return new ArrowFunctionExpression($this->parseExpression(0), new Node($names), $line);
}

Ptr ExpressionParser::parseConditionalExpression(Node::Ptr expr)
{
  Ptr expr2, expr3;
  while(parser()->nextIf(Token::Type::Punctuation, "?"))
  {
    Location loc=currentToken().location();
    if(!parser()->nextIf(Token::Type::Punctuation, ":"))
    {
      expr2=parseExpression();
      if(!expr2)
        return nullptr;
      if (parser()->nextIf(Token::Type::Punctuation, ":"))
        expr3=parseExpression();
      else
        expr3=Node::newText(QString(),currentToken().location());
      if(!expr3)
        return nullptr;
    }
    else
    {
      expr3=parseExpression();
      if(!expr3)
        return nullptr;
    }
    expr=Node::newConditional(expr, expr2, expr3, loc);
  }
  return expr;
}

Ptr ExpressionParser::parseMultitargetExpression()
{
  QList<Ptr> values;
  Location loc=currentToken().location();
  while(true)
  {
    Ptr value(parseExpression());
    if(!value)
      return nullptr;
    values.append(value);
    if(!parser()->nextIf(Token::Type::Punctuation, ","))
      break;
  }
  return Node::newList(values,loc);
}

Ptr ExpressionParser::parseAssignmentTargetExpression()
{
  Location loc=currentToken().location();
  QList<Ptr> targets;
  while (true) {
    auto &token=currentToken();
    if(!token.test(Token::Type::Name) && (!token.test(Token::Type::Operator) || !engine()->lexerEnv()->reName().match(token.text()).hasMatch()))
    {
      parser()->failedUnexpected("parseAssignmentExpression","Only variables can be assigned to");
      return nullptr;
    }
    if(engine()->getConstantExpression(token.text(), token.location()))
    {
      parser()->failedParsing(tr("Can't assign a value to constant \"%1\"").arg(currentToken().text()));
      return nullptr;
    }
    targets.append(Node::newName(token.text(), token.location()));
    parser()->next();
    if(!parser()->nextIf(Token::Type::Punctuation, ","))
      break;
  }
  return Node::newList(targets,loc);
}

Ptr ExpressionParser::parseArguments(bool namedArguments, bool definition, bool allowArrow)
{
  QList<Ptr> args;
  Location loc=currentToken().location();
  if(parser()->expect("(",Token::Type::Punctuation,"parseArguments", "Argument list must start with open parenthesis"))
  {
    QSet<QString> namedSet;
    while(!currentToken().test(Token::Type::Punctuation,")"))
    {
      Location loc=currentToken().location();
      Ptr value;
      QString name;
      if(!args.isEmpty() && !parser()->nextIf(Token::Type::Punctuation, ","))
      {
        m_parser->failedUnexpected("parseArguments","Arguments must be separated by a comma");
        return nullptr;
      }
      else if(definition)
      {
        if(auto t=parser()->nextIf(Token::Type::Name))
          name=t.text();
        else
        {
            m_parser->failedUnexpected("parseArguments","Argument in a definition is not a name");
            return nullptr;
        }
        if(namedArguments && parser()->nextIf(Token::Type::Operator, "="))
        {
          value=parsePrimaryExpression();
          if(!value)
            return nullptr;
          if(false /* TODO *//*!checkConstantExpression(value.data())*/)
          {
            parser()->failedParsing("A default value for an argument must be a constant (a boolean, a string, a number, or an array)", value->location());
            return nullptr;
          }
        }
        else
          value=Node::newConstNull(currentToken().location());
      }
      else //!definition
      {
        value=parseExpression(0, allowArrow);
        if(!value)
          return nullptr;
        if(namedArguments && parser()->nextIf(Token::Type::Operator, "="))
        {
          if(value->type()!=Node::Type::Name)
          {
            m_parser->failedParsing(tr("Expected parameter name before '='. Got '%1'").arg(Node::nodeTypeName(value->type())),value->location());
            break;
          }
          Q_ASSERT(dynamic_cast<Node::Name*>(value.data()));
          name=static_cast<const Node::Name *>(value.data())->name();
          value=parseExpression(0, allowArrow);
          if(!value)
            return nullptr;
        }
        else if(!namedSet.isEmpty())
        {
          parser()->failedParsing("Mixed positional an named arguments", currentToken().location());
          return nullptr;
        }
      }
      if(name.isNull())
        args.append(value);
      else
        args.append(Node::newMapString(name, value, loc));
    }
  }
  if(!parser()->expect(")", Token::Type::Punctuation,"parseArguments", "Argument list must be closed by a parenthesis"))
    return nullptr;
  return Node::newList(args, loc);
}

Ptr ExpressionParser::parseStringExpression()
{
  Ptr ret;
  QList<Ptr> retList;
  bool success=true;
  Location loc=currentToken().location();
  bool nextCanBeString=true;
  while(true)
  {
    if(nextCanBeString && currentToken().test(Token::Type::String))
    {
      retList.append(Node::newText(currentToken().text(), currentToken().location()));
      nextCanBeString=false;
      parser()->next();
    }
    else if(parser()->nextIf(Token::Type::InterpolationStart))
    {
      Location loc(currentToken().location());
      Ptr expr(parseExpression());
      if(expr && parser()->expect(Token::Type::InterpolationEnd, "parseStringExpression", "Expected interpolation end"))
      {
        retList.append(Node::newPrint(expr, loc));
        nextCanBeString=true;
      }
      else
      {
        success=false;
        break;
      }
    }
    else
      break;
  }

  if(!success) { }
  else if(retList.isEmpty())
    parser()->failedParsing(tr("Parse string returned an empty list"), loc);
  else if(retList.size()==1)
    ret=retList.first();
  else
    ret=Node::newConcatenate(retList, loc);
  return ret;
}
