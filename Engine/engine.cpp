#include "engine.h"
#include "operator.h"
#include "filesystem.h"
#include "../Parser/lexerenv.h"
#include "../Parser/lexer.h"
#include "../Parser/tokenstream.h"
#include "../Parser/parser.h"
#include "../Parser/Node/binaryop.h"
#include "../Parser/Node/unaryop.h"
#include "Tag/tag.h"
#include "Helper/arguments.h"
#include "runthread.h"
#include "value.h"
#include "Helper/valuedata.h"
#define tr QObject::tr
using namespace QTwig;

Engine::Engine(): m_initializing(true), m_lexer(nullptr), m_filesystem(nullptr)
{
  m_initializing=true;
  m_numberRegExp=QString::fromLatin1("[+-]?[0-9]+(?:\\.[0-9]+)?([Ee][\\+\\-][0-9]+)?");
  m_stringRegExp=QString::fromLatin1("\'([^\'\\\\]*(?:\\\\.[^\'\\\\]*)*)\'");
}

Engine::Ptr Engine::newEngine()
{
  return Ptr(new Engine);
}

Engine::Ptr Engine::newEngineDefault()
{
  auto ret=newEngine();
  ret->initDefault();
  return ret;
}

QSharedPointer<RunThread> Engine::newThread()
{
  return RunThread::Ptr(new RunThread(this->sharedFromThis()));
}

const Operator1 *Engine::getUnaryOp(const QString &text)
{
  return m_unaryOpMap.value(text,nullptr);
}

const Operator2 *Engine::getBinaryOp(const QString &text)
{
  return m_binaryOpMap.value(text,nullptr);
}

Value Engine::getFilter(const QString &name)
{
  return (m_filters.value(name));
}

Value Engine::getFunction(const QString &name)
{
  return (m_functions.value(name));
}

bool Engine::initAddUnaryOp(Operator1 *op)
{
  bool ret=false;
  if(m_initializing && op && !m_unaryOpMap.contains(op->text()))
  {
    ret=true;
    m_unaryOps.append(op);
    m_unaryOpMap.insert(op->text(), op);
  }
  else
    delete op;
  return ret;
}

bool Engine::initAddBinaryOp(Operator2 *op)
{
  bool ret=false;
  if(m_initializing && op && !m_binaryOpMap.contains(op->text()))
  {
    ret=true;
    m_binaryOps.append(op);
    m_binaryOpMap.insert(op->text(), op);
  }
  else
    delete op;
  return ret;
}

bool Engine::initAddFilter(const QString &key, Value filter)
{
  bool ret=false;
  if(!m_filters.contains(key))
  {
    m_filters.insert(key, filter);
    ret=true;
  }
  return ret;
}

bool Engine::initAddFunction(const QString &key, Value function)
{
  bool ret=false;
  if(!m_functions.contains(key))
  {
    m_functions.insert(key, function);
    ret=true;
  }
  return ret;
}

bool Engine::initAddTag(const QString &tag, TagParserFunction parser)
{
  bool ret=false;
  if(!m_tags.contains(tag))
  {
    m_tags.insert(tag, parser);
    ret=true;
  }
  return ret;
}

bool Engine::initAddDefaultTags()
{
  bool ret=true;
  ret&=initAddTag("set", TagParse::parseSet);
  ret&=initAddTag("if", TagParse::parseIf);
  ret&=initAddTag("block", TagParse::parseBlock);
  ret&=initAddTag("macro", TagParse::parseMacro);
  ret&=initAddTag("extends", TagParse::parseExtends);
  ret&=initAddTag("import", TagParse::parseImport);
  ret&=initAddTag("from", TagParse::parseFrom);
  ret&=initAddTag("include", TagParse::parseInclude);
  ret&=initAddTag("embed", TagParse::parseEmbed);
  ret&=initAddTag("use", TagParse::parseUse);
  return ret;
}

static QTwig::Value createFunctionNoArgs(const std::function<Value(RunThread *, const QList<Value> &, const QMap<QString,Value> &)> &function)
{
  Q_ASSERT(function);
  return Value::createFunction([function](RunThread *thread, const QList<Value> &args, const QMap<QString, Value> &named) {
    Value ret;
    if((ret=Arguments::checkNoArgs(args, named)).isNullData())
      ret=function(thread, args, named);
    return ret;
  });
}

static QTwig::Value createFilterNoArgs(const std::function<Value(RunThread *, const QList<Value> &, const QMap<QString,Value> &)> &filter)
{
  return Value::createFunction([filter](RunThread *, const QList<Value> &args, const QMap<QString, Value> &named) {
    Value ret;
    if((ret=Arguments::checkNoArgs(args, named)).isNullData())
      ret=Value::createFunction(filter);
    return ret;
  });
}
static QTwig::Value filterUpper(RunThread *, const QList<Value> &args, const QMap<QString,Value> &named)
{
  Value ret;
  QString value;
  if((ret=Arguments::checkSingleStringArg(args, named, value)).isNullData())
  {
    ret=Value(value.toUpper());
  }
  return ret;
}

#include <QRandomGenerator>
static QTwig::Value functionRandom(RunThread *, const QList<Value> &, const QMap<QString,Value> &)
{
  return QVariant(QRandomGenerator::global()->generateDouble());
}

bool Engine::initAddDefaultFilters()
{
  bool ret=true;
  ret&=initAddFilter("upper", createFilterNoArgs(filterUpper));
  return ret;
}

bool Engine::initAddDefaultFunctions()
{
  bool ret=false;
  ret&=initAddFunction("random", createFunctionNoArgs(functionRandom));
  return ret;
}


bool Engine::initAddDefaultUnaryOps()
{
  bool ret=true;
  ret&=initAddUnaryOp(new Operator1("not",50));
  ret&=initAddUnaryOp((new Operator1("-",500))->setEvaluatorNum([](QVariant v){ return Value(-v.toLongLong()); }, [](QVariant v){ return Value(-v.toDouble()); }));
  ret&=initAddUnaryOp((new Operator1("+",500))->setEvaluatorNum([](QVariant v){ return v; }, [](QVariant v){ return v; }));
  return ret;
}

bool Engine::initAddDefaultBinaryOps()
{
  bool ret=true;
  ret&=initAddBinaryOp(new Operator2("or",10));
  ret&=initAddBinaryOp(new Operator2("and",15));
  ret&=initAddBinaryOp(new Operator2("b-or",16));
  ret&=initAddBinaryOp(new Operator2("b-xor",17));
  ret&=initAddBinaryOp(new Operator2("b-and",18));
  ret&=initAddBinaryOp(new Operator2("==",20));
  ret&=initAddBinaryOp(new Operator2("!=",20));
  ret&=initAddBinaryOp(new Operator2("<=>",20));
  ret&=initAddBinaryOp(new Operator2("<",20));
  ret&=initAddBinaryOp(new Operator2("<=",20));
  ret&=initAddBinaryOp(new Operator2(">",20));
  ret&=initAddBinaryOp(new Operator2(">=",20));
  ret&=initAddBinaryOp(new Operator2("not in",20));
  ret&=initAddBinaryOp(new Operator2("in",20));
  ret&=initAddBinaryOp(new Operator2("matches",20));
  ret&=initAddBinaryOp(new Operator2("starts with",20));
  ret&=initAddBinaryOp(new Operator2("ends with",20));
  ret&=initAddBinaryOp(new Operator2("..",25));
  ret&=initAddBinaryOp((new Operator2("+",30))->setEvaluatorNum([](QVariant a, QVariant b){ return Value(a.toLongLong()+b.toLongLong()); }, [](QVariant a, QVariant b){ return Value(a.toDouble()+b.toDouble()); }));
  ret&=initAddBinaryOp((new Operator2("-",30))->setEvaluatorNum([](QVariant a, QVariant b){ return Value(a.toLongLong()-b.toLongLong()); }, [](QVariant a, QVariant b){ return Value(a.toDouble()-b.toDouble()); }));
  ret&=initAddBinaryOp(new Operator2("~",40));
  ret&=initAddBinaryOp((new Operator2("*",60))->setEvaluatorNum([](QVariant a, QVariant b){ return Value(a.toLongLong()*b.toLongLong()); }, [](QVariant a, QVariant b){ return Value(a.toDouble()*b.toDouble()); }));
  ret&=initAddBinaryOp(new Operator2("/",60)  );
  ret&=initAddBinaryOp(new Operator2("//",60) );
  ret&=initAddBinaryOp(new Operator2("%",60)  );
//  ret&=initAddBinaryOp((new Operator2("is",100))->setExtraParser(ExpressionParser::parseTestExpression));
//  ret&=initAddBinaryOp((new Operator2("is not",100))->setExtraParser(ExpressionParser::parseNotTestExpression));
  ret&=initAddBinaryOp(new Operator2("**",200));
  ret&=initAddBinaryOp(new Operator2("??",300));
  return ret;
}

bool Engine::initDefault()
{
  bool ret=true;
  ret&=initAddDefaultUnaryOps();
  ret&=initAddDefaultBinaryOps();
  ret&=initAddDefaultFilters();
  ret&=initAddDefaultFunctions();
  ret&=initAddDefaultTags();
  return ret;
}

QSharedPointer<Node::Generic> Engine::parseNumber(const QString &expression, const Location &location)
{
  bool ok;
  Node::Ptr ret;
  if(expression.contains(QChar('.')))
    ret=Node::newConstDouble(expression.toDouble(&ok), location);
  else
    ret=Node::newConstInt(expression.toLongLong(&ok), location);
  return (ok?ret:nullptr);
}

Engine::TagParserFunction Engine::getTag(const QString &tag)
{
  return m_tags.value(tag);
}

Lexer *Engine::lexer(const QString &body, const QString &filename, Message *output)
{
  endInitialization();
  if(!m_lexer)
    m_lexer=new LexerEnv(this);
  return m_lexer->newInstance(body, filename, output);
}

bool Engine::setFilesystem(Filesystem *newFilesystem)
{
  bool ret=false;
  if(m_initializing && (!newFilesystem || !newFilesystem->engine()))
  {
    if(m_filesystem)
      delete m_filesystem;
    m_filesystem = newFilesystem;
    if(newFilesystem)
      newFilesystem->setEngine(this);
  }
  return ret;
}

bool Engine::printValue(RunThread *thread, Value val)
{
  bool ok;
  thread->output(val.toPrintable(&ok));
  return ok;
}

//std::function<Value(RunThread *thread, const Node::UnaryOp *op)> Engine::generateNumOp1(const std::function<Value(QVariant val)> &opInt, const std::function<Value(QVariant val)> &opFloat)
//{
//  return [opInt, opFloat](RunThread *thread, const Node::UnaryOp *op){
//    Value val(thread->executeExpression(op->expr().data()));
//    if(val.isValid())
//    {
//      QVariant v=val.value();
//      if(!Value::isNumberType(v.typeId()))
//        val=Value::createError(tr("Expression is not a number but a '%1'").arg(QMetaType(v.typeId()).name()), op->expr()->location());
//      else
//      {
//        if(Value::isIntType(v.typeId()))
//          val=opInt(v);
//        else
//          val=opFloat(v);
//      }
//    }
//    return val;
//  };
//}

//std::function<Value(RunThread *thread, const Node::BinaryOp *op)> Engine::generateNumOp2(const std::function<Value(QVariant val1, QVariant val2)> &opInt, const std::function<Value(QVariant val1, QVariant val2)> &opFloat)
//{
//  return [opInt, opFloat](RunThread *thread, const Node::BinaryOp *op){
//    Value val(thread->executeExpression(op->expr1().data()));
//    if(val.isValid())
//    {
//      Value val2(thread->executeExpression(op->expr2().data()));
//      if(val2.isValid())
//      {
//        QVariant v1=val.value(), v2=val2.value();
//        if(!Value::isNumberType(v1.typeId()))
//          val=Value::createError(tr("Expression is not a number but a '%1'").arg(QMetaType(v1.typeId()).name()), op->expr1()->location());
//        else if(!Value::isNumberType(v2.typeId()))
//          val=Value::createError(tr("Expression is not a number but a '%1'").arg(QMetaType(v2.typeId()).name()), op->expr2()->location());
//        else
//        {
//          if(Value::isIntType(v1.typeId()) && Value::isIntType(v2.typeId()))
//            val=opInt(v1, v2);
//          else
//            val=opFloat(v1, v2);
//        }
//      }
//      else
//        val=val2;
//    }
//    return val;
//  };
//}
//Value Engine::evaluateNumberOp2(RunThread *thread, const Node::BinaryOp *op)
//{
//  Value val(thread->executeExpression(op->expr1().data()));
//  if(val.isValid())
//  {
//    Value val2(thread->executeExpression(op->expr2().data()));
//    if(val2.isValid())
//    {
//      QVariant v1=val.value(), v2=val2.value();
//      bool isInt=Value::isIntType(v1.typeId()) && Value::isIntType(v2.typeId());
//      bool isNumber=Value::isNumberType(v1.typeId()) && Value::isNumberType(v2.typeId());
//      // ...Todo
//    }
//    else
//      val=val2;
//  }
////  return Value::createError(tr("Unimplemented evaluate 2", expr1->location());
//  return val;
//}

TokenStream Engine::lex(const QString &body, const QString &filename, Message *output)
{
  auto l=lexer(body, filename, output);
  auto ret=l->tokenize();
  delete l;
  return ret;
}

QSharedPointer<Node::Generic> Engine::parse(const TokenStream &stream, Message *output)
{
  auto p=new Parser(this, stream, output);
  auto ret=p->parse();
  delete p;
  return ret;
}

QSharedPointer<Node::Generic> Engine::parse(const QString &filename, Message *output)
{
  QSharedPointer<Node::Generic> ret;
  Q_ASSERT(m_filesystem);
  QString content;
  if(m_filesystem->open(filename, content, output))
    ret=parse(content, filename, output);
  return ret;
}

QSharedPointer<Node::Generic> Engine::getConstantExpression(const QString &text, const Location &location)
{
  Node::Ptr ret;
  if(text=="true" || text=="TRUE")
    ret=Node::newConstBool(true,location);
  else if(text=="false" || text=="FALSE")
    ret=Node::newConstBool(false,location);
  else if(text=="none" || text=="NONE" || text=="null" || text=="NULL")
    ret=Node::newConstNull(location);
  return ret;
}


QSharedPointer<Node::Generic> Engine::parse(const QString &body, const QString &filename, Message *output)
{
  auto lexed=lex(body, filename, output);
  qDebug()<<lexed.tokens();
  return parse(lexed, output);
}
