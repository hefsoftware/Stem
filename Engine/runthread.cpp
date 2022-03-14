#include "runthread.h"
#include "engine.h"
#include "message.h"
#include "Helper/scopesstack.h"
#include "../Parser/Node/node.h"
#include "../Parser/Node/list.h"
#include "../Parser/Node/text.h"
#include "../Parser/Node/print.h"
#include "operator.h"
#include "Helper/valuedatanull.h"
#include "QBuffer"
#include <QIODevice>
#include <QDebug>
#define tr QString
using namespace QTwig;
RunThread::RunThread(const QSharedPointer<Engine> &engine): m_engine(engine), m_running(false)
{
  auto firstScope=ScopesStack::create(Visibility::GetOpaque);
  m_scope.append(firstScope);
}



bool RunThread::execute(const QString &filename, QIODevice *output, Message *message)
{
  auto node=m_engine->parse(filename, message);
  bool ret=false;
  if(node)
  {
    qDebug()<<"Parse tree:";
    qDebug()<<node->toString().toUtf8().data();
    ret=execute(node, output, message);
  }
  return ret;
}



void RunThread::errorMessage(const QString &error, const Location &location)
{
  message()->error(error, location);
}

void RunThread::errorMessage(const Value &value)
{
  QString ret;
  if(value.isError())
  {
    auto error=value.getDataStaticCast<ValueDataError>();
    errorMessage(error->message(), error->location());
  }
  else if(value.isUndefined())
  {
    auto undefined=value.getDataStaticCast<ValueDataUndefined>();
    errorMessage(tr("Undefined variable \'%1\'").arg(undefined->name()), undefined->location());
  }
  else
    errorMessage(tr("BUG! Error message called with a valid value"), Location());
}

void RunThread::errorMessage(Value value, const Node::Generic *node)
{
  setErrorLocation(value, node);
  errorMessage(value);
}


QIODevice *RunThread::output(const QString &text)
{
  QIODevice *out=m_output.last();
  out->write(text.toUtf8());
  return out;
}

Value RunThread::setOuterScope(const QString &key, const Value &value)
{
  return m_scope.last()->setOuterScope(key, value);
}

Value RunThread::set(const QString &key, const Value &value)
{
  Value ret;
  bool set=false;
  for(int i=m_scope.size()-1;i>=0;i--) // First tries to set in outer scope that contains the variable, stopping if any scope is a set barrier.
  {
    auto &cur=m_scope[i];
    auto visibility=cur->visibility();
    if(cur->hasVariable(key))
    {
      ret=cur->set(key, value);
      set=true;
      break;
    }
    if(visibility<=Visibility::SetOpaque)
      break;
  }
  if(!set)
  {
    for(int i=m_scope.size()-1;i>=0;i--)
    {
      auto &cur=m_scope[i];
      auto visibility=cur->visibility();
      if(visibility<Visibility::SetTransparent)
      {
        ret=cur->set(key, value); // If the variable was not found set in the outer scope
        set=true;
        break;
      }
    }
  }
  if(!set)
    ret=Value::createError("BUG! RunThread::set did not set any value", Location());
  return ret;
}

Value RunThread::get(const QString &key)
{
  Value ret;
  for(int i=m_scope.size()-1; i>=0; i--)
  {
    auto &scope=m_scope[i];
    if(scope->hasVariable(key))
    {
      ret=scope->get(key);
      break;
    }
    else if(scope->visibility()==Visibility::GetOpaque)
    {
      ret=Value::createUndefined(key, Location());
      break;
    }
  }
  return ret;
}

QMap<QString, Value> RunThread::extractCurrentScope()
{
  int i, n=m_scope.size();
  for(i=m_scope.size()-1;i>=0;i--)
    if(m_scope[i]->visibility()==Visibility::GetOpaque)
      break;
  QMap<QString, Value> ret;
  for(;i<n;i++)
  {
    ret.insert(m_scope[i]->extractCurrentScope());
  }
  return ret;
}

void RunThread::debugCurrentScope()
{
  auto scope=extractCurrentScope();
  qDebug()<<"Current thread scope:";
  for(auto it=scope.cbegin();it!=scope.cend(); it++)
  {
    qDebug()<<it.key().toUtf8().data()<<":"<<it.value().value().toString();
  }
}

void RunThread::setErrorLocation(Value &value, const Location &location)
{
  if(auto err=value.getDataCast<ValueDataNull>())
  {
    if(err->location().isUndefined())
      value=Value::createData(err->changeLocation(location));
  }
}

void RunThread::setErrorLocation(Value &value, const Node::Generic *source)
{
  if(auto err=value.getDataCast<ValueDataNull>())
  {
    if(err->location().isUndefined())
      value=Value::createData(err->changeLocation(source->location()));
  }
}


#include "../Parser/Node/filter.h"
#include "../Parser/Node/mapstring.h"

Value RunThread::executeFunction(Value function, const Node::Generic *args)
{
  Value ret;
  if(!args)
    ret=Value::createError(tr("BUG! Null args."), Location());
  else if(args->type()!=Node::Type::List)
    ret=Value::createError(tr("BUG! Arguments to a function must be a list, got '%1'").arg(Node::nodeTypeName(args->type())), args->location());
  else
  {
    Q_ASSERT(dynamic_cast<const Node::List *>(args));
    auto list=static_cast<const Node::List *>(args);
    QList<Value> argVal;
    QMap<QString, Value> namedArgVal;
    bool ok=true;
    for(int i=0;i<list->numChildren();i++)
    {
      auto arg=list->children(i).data();
      if(!arg)
      {
        ok=false;
        ret=Value::createError(tr("BUG! Null children %1").arg(i), Location());
        break;
      }
      else if(arg->type()==Node::Type::MapString)
      {
        Q_ASSERT(dynamic_cast<const Node::MapString *>(arg));
        auto map=static_cast<const Node::MapString *>(arg);
        Value val=evaluate(map->expr().data());
        if(!val.isValid())
        {
          ok=false;
          ret=val;
          break;
        }
        namedArgVal.insert(map->key(), val);
      }
      else
      {
        Value val=evaluate(arg);
        if(!val.isValid())
        {
          ok=false;
          ret=val;
          break;
        }
        argVal.append(val);
      }
    }
    if(ok)
    {
      ret=function.call(this, argVal, namedArgVal);
      setErrorLocation(ret, args->location());
    }
  }
  return ret;
}



#include "../Parser/Node/constbool.h"
#include "../Parser/Node/constdouble.h"
#include "../Parser/Node/constint.h"
#include "../Parser/Node/name.h"
#include "../Parser/Node/binaryop.h"
#include "../Parser/Node/unaryop.h"
#include "../Parser/Node/concatenate.h"
#include "../Parser/Node/conditional.h"
#include "../Parser/Node/functioncall.h"
#include "../Parser/Node/array.h"
#include "Helper/valuedataarray.h"
// Unhandled for now: Hash, ArrayGet
Value RunThread::evaluate(const Node::Generic *node)
{
  Value ret;
  switch(node->type())
  {      
    case Node::Type::Array:
    {
      Q_ASSERT(dynamic_cast<const Node::Array *>(node));
      auto array=static_cast<const Node::Array *>(node);
      QList<Value> values;
      for(int i=0;i<array->numChildren();i++)
      {
        auto val=evaluate(array->children(i).data());
        if(!val.isValid())
        {
          setErrorLocation(val, array->children(i)->location());
          return val;
        }
        values.append(val);
      }
      return Value::createData(new ValueDataList(values));
    }
    case Node::Type::FunctionCall:
    {
      Q_ASSERT(dynamic_cast<const Node::FunctionCall *>(node));
      auto functionCall=static_cast<const Node::FunctionCall *>(node);
      ret=m_engine->getFunction(functionCall->name());
      if(ret.isNullData())
        ret=Value::createError(tr("Undefined function %1").arg(functionCall->name()), node->location());
      else
        ret=executeFunction(ret, functionCall->args().data());
    }
    break;
    case Node::Type::Conditional:
    {
      Q_ASSERT(dynamic_cast<const Node::Conditional *>(node));
      auto condition=static_cast<const Node::Conditional *>(node);
      Value val;
      ret=val=evaluate(condition->cond().data());
      if(ret.isValid() && (ret=ret.toBool()).isValid())
      {
        auto node=(ret.value().toBool()?condition->valTrue():condition->valFalse()).data();
        ret=node?evaluate(node):val;
      }
    }
    break;
    case Node::Type::Filter:
    {
      Q_ASSERT(dynamic_cast<const Node::Filter *>(node));
      auto filter=static_cast<const Node::Filter *>(node);
      auto filterFunc=m_engine->getFilter(filter->name());
      if(filterFunc.isNullData())
        ret=Value::createError(tr("Undefined filter '%1'").arg(filter->name()), node->location());
      else
      {
        ret=executeFunction(filterFunc, filter->args().data());
        if(ret.isValid())
        {
          auto val=evaluate(filter->expr().data());
          if(!val.isValid())
            ret=val;
          else
          {
            // Now we have a filter and the data to pass to it
            ret=ret.call(this, {val});
            setErrorLocation(ret, node->location());
          }
        }
      }
    }
    break;
    case Node::Type::ConstNull:
      ret=Value(QVariant());
      break;
    case Node::Type::ConstBool:
    {
      Q_ASSERT(dynamic_cast<const Node::ConstBool *>(node));
      auto cbool=static_cast<const Node::ConstBool *>(node);
      ret=Value(cbool->value());
    }
    break;
    case Node::Type::ConstInt:
    {
      Q_ASSERT(dynamic_cast<const Node::ConstInt *>(node));
      auto cint=static_cast<const Node::ConstInt *>(node);
      ret=Value(cint->value());
    }
    break;
    case Node::Type::ConstDouble:
    {
      Q_ASSERT(dynamic_cast<const Node::ConstDouble *>(node));
      auto cdouble=static_cast<const Node::ConstDouble *>(node);
      ret=Value(cdouble->value());
    }
    break;
    case Node::Type::Text:
    {
      Q_ASSERT(dynamic_cast<const Node::Text *>(node));
      auto ctext=static_cast<const Node::Text *>(node);
      ret=Value(ctext->text());
    }
    break;
    case Node::Type::Name:
    {
      Q_ASSERT(dynamic_cast<const Node::Name *>(node));
      auto name=static_cast<const Node::Name *>(node);
      ret=get(name->name());
      if(ret.isUndefined())
        ret=Value::createUndefined(name->name(), node->location());
    }
    break;
    case Node::Type::UnaryOp:
    {
      Q_ASSERT(dynamic_cast<const Node::UnaryOp*>(node));
      auto unaryOp=static_cast<const Node::UnaryOp *>(node);
      auto op=unaryOp->op();
      Q_ASSERT(op);
      auto ev=op->evaluator();
      if(ev)
        ret=ev(*this, unaryOp);
      else
        ret=Value::createError(tr("Operator \'%1\' does not define an evaluation function").arg(op->text()), node->location());
    }
    break;
    case Node::Type::BinaryOp:
    {
      Q_ASSERT(dynamic_cast<const Node::BinaryOp*>(node));
      auto binaryOp=static_cast<const Node::BinaryOp *>(node);
      auto op=binaryOp->op();
      Q_ASSERT(op);
      auto ev=op->evaluator();
      if(ev)
        ret=ev(*this, binaryOp);
      else
        ret=Value::createError(tr("Operator \'%1\' does not define an evaluation function").arg(op->text()), node->location());
    }
    break;
    case Node::Type::Print:
    {
      Q_ASSERT(dynamic_cast<const Node::Print*>(node));
      ret=executeExprToString(static_cast<const Node::Print*>(node)->expr().data());
    }
    break;
    case Node::Type::Concatenate:
    {
      Q_ASSERT(dynamic_cast<const Node::Concatenate*>(node));
      auto concatenate=static_cast<const Node::Concatenate *>(node);
      QString result;
      for(int i=0;i<concatenate->numChildren();i++)
      {
        auto curVal=evaluate(concatenate->children(i).data());
        if(!curVal.isValid())
        {
          ret=curVal;
          break;
        }
        else
        {
          QVariant v=curVal.value();
          if(v.typeId()==QMetaType::QString)
            result+=v.toString();
          else
          {
            ret=Value::createError(tr("Expression in interpolation returned a non-string \"%1\"").arg(QMetaType(v.typeId()).name()), concatenate->children(i)->location());
            break;
          }
        }
      }
      if(ret.isNullData())
        ret=Value(result);
    }
    break;
    default:
      ret=Value::createError(tr("Execute expression: Unexpected node %1").arg(nodeTypeName(node->type())), node->location());
  }
  return ret;
}


Value RunThread::executeExprToString(const Node::Generic *expr)
{
  bool ret=false;
  auto value=evaluate(expr);
  if(value.isValid())
  {
    QByteArray buf;
    auto outBuffer=new QBuffer(&buf);
    if(outBuffer->open(QIODevice::Append))
    {
      PushOutput output(this, outBuffer);
      ret=m_engine->printValue(this, value);
    }
    if(ret)
      value=Value(QString::fromUtf8(buf));
    else
      errorMessage(tr("Failed to print value (type=%1)").arg(value.value().metaType().name()), expr->location());
  }
  return value;
}

bool RunThread::executeExprPrint(const Node::Generic *expr)
{
  bool ret=false;
  auto value=evaluate(expr);
  if(!value.isValid())
    errorMessage(value);
  else
  {
    ret=m_engine->printValue(this, value);
    if(!ret)
    {
      errorMessage(tr("Failed to print value (type=%1)").arg(value.value().metaType().name()), expr->location());
    }
  }
  return ret;
}

bool RunThread::executeNode(const Node::Generic *node)
{
  bool ret=true;
  if(node->hasExecute())
    ret=node->execute(*this);
  else
  {
    switch(node->type())
    {
      // Template-specific nodes are not evaluated
      case Node::Type::Extends:
      case Node::Type::Block:
      case Node::Type::Macro:
      case Node::Type::Import:
      case Node::Type::Use:
        break;
      case Node::Type::Text:
      {
        Q_ASSERT(dynamic_cast<const Node::Text *>(node));
        auto text=static_cast<const Node::Text *>(node);
        output(text->text());
      }
        break;
      case Node::Type::List:
      {
        Q_ASSERT(dynamic_cast<const Node::List *>(node));
        auto list=static_cast<const Node::List *>(node);
        int n=list->numChildren();
        for(int i=0;ret && i<n;i++)
          ret&=executeNode(list->children(i).data());
      }
        break;
      case Node::Type::Print:
      {
        Q_ASSERT(dynamic_cast<const Node::Print *>(node));
        ret=executeExprPrint(static_cast<const Node::Print *>(node)->expr().data());
      }
        break;
      default:
        errorMessage(tr("Execute: Unexpected node %1").arg(nodeTypeName(node->type())), node->location());
        ret=false;
    }
  }
  return ret;
}

bool RunThread::execute(const QSharedPointer<Node::Generic> &node, QIODevice *output, Message *message)
{
  bool ret=false;
  if(message)
  {
    if(m_running)
      message->error(tr("Thread is already evaluating a template"));
    else if(!output)
      message->error(tr("Output is not defined"));
    else
    {
      PushOutput out(this, output);
      PushMessage msg(this, message);
      ret=executeNode(node.data());
    }
  }
  return ret;
}

RunThread::PushOutput::~PushOutput()
{
  delete m_thread->m_output.takeLast();
}

RunThread::PushMessage::~PushMessage()
{
  delete m_thread->m_message.takeLast();
}

RunThread::PushScope::PushScope(RunThread *thread, Visibility visibility): m_thread(thread)
{
  Q_ASSERT(thread);
  auto lastScope=thread->m_scope.last();
  ScopesStack *scope;
  if(lastScope->isStack())
  {
    Q_ASSERT(dynamic_cast<ScopesStack *>(thread->m_scope.last()));
    scope=dynamic_cast<ScopesStack *>(thread->m_scope.last());
  }
  else
    thread->m_scope.append((scope=(new ScopesStack)));
  scope->pushNewEntry(visibility);
}

RunThread::PushScope::~PushScope()
{
  Q_ASSERT(dynamic_cast<ScopesStack *>(m_thread->m_scope.last()));
  auto *scope=static_cast<ScopesStack *>(m_thread->m_scope.last());
  scope->popEntry();
  if(scope->numEntries()==0)
    delete (m_thread->m_scope.takeLast());
}

RunThread::PushScopePtr::PushScopePtr(RunThread *thread, Scope *ptr, bool own): m_thread(thread), m_own(own)
{
  Q_ASSERT(ptr);
  thread->m_scope.append(ptr);
}

RunThread::PushScopePtr::~PushScopePtr()
{
  auto scope=m_thread->m_scope.takeLast();
  if(m_own)
    delete scope;
}
