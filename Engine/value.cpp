#include "value.h"
#include "Helper/valuedatavariant.h"
#include "Helper/valuedatanull.h"
#include "Helper/valuedatafunction.h"
#include "Helper/iterable.h"
#include "Helper/iterator.h"
using namespace QTwig;
static Value genericUndefined(Value::createData(new ValueDataUndefined(QString(), Location())));

Value::Value()
{

}

Value::Value(ValueData *data): m_data(data)
{

}

Value::Value(const QVariant &value): m_data(new ValueDataVariant(value))
{

}

QVariant Value::value() const
{
  return m_data?m_data->value():QVariant();
}

Value Value::createData(ValueData *data)
{
  return Value(data);
}

Value Value::createError(const QString &message, const Location &location)
{
  return Value(new ValueDataError(message, location));
}

Value Value::createUndefined(const QString &name, const Location &location)
{
  return Value(new ValueDataUndefined(name, location));
}

Value Value::createUndefined()
{
  return genericUndefined;
}

Value Value::createFunction(const std::function<Value (RunThread *, const QList<Value> &, const QMap<QString, Value> &)> &function)
{
  return Value(new ValueDataFunction(function));
}

bool Value::isUndefined() const
{
  return m_data && m_data->isUndefined();
}

bool Value::isError() const
{
  return m_data && m_data->isError();
}

bool Value::isValid() const
{
  return m_data && !m_data->isUndefined() && !m_data->isError();
}

Value Value::call(RunThread *thread, const QList<Value> &args, const QMap<QString, Value> &named)
{
  return m_data?m_data->call(thread, args, named):createError("Called a NULL value", Location());
}

Value Value::toBool()
{
  return m_data?m_data->toBool():Value(false);
}

QString Value::toPrintable(bool *ok) const
{
  bool ret=true;
  QString result;
  QVariant var=value();
  switch(var.typeId())
  {
    case QMetaType::Short:
    case QMetaType::UShort:
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::Long:
    case QMetaType::ULong:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
      result=QString::number(var.toLongLong());
      break;
    case QMetaType::Float:
    case QMetaType::Double:
      result=QString::number(var.toDouble());
      break;
    case QMetaType::QString:
      result=var.toString();
      break;
    case QMetaType::Bool:
      result=QString::fromLatin1(var.toBool()?"true":"false");
      break;
    case QMetaType::Void:
      result=QString::fromLatin1("void");
      break;
    case QMetaType::UnknownType:
      if(var.isNull())
      {
        if(getData())
          result=QString("(%1)").arg(getData()->type());
        else
          result=QString::fromLatin1("null");
      }
      else
        ret=false;
      break;
    default:
      ret=false;
  }
  if(ok)
    *ok=ret;
  return result;
}

bool Value::isIterable(RunThread *thread)
{
  Q_UNUSED(thread);
  auto data=m_data.data();
  bool ret=false;
  if(dynamic_cast<Iterator *>(data))
    ret=true;
  else if(auto iterable=dynamic_cast<Iterable *>(data))
    ret=iterable->isIterable();
  return ret;
}

QSharedPointer<Iterator> Value::iterator(RunThread *thread)
{
  Q_UNUSED(thread);
  QSharedPointer<Iterator> ret;
  if(!(ret=m_data.dynamicCast<Iterator>()))
  {
    if(auto iterable=dynamic_cast<Iterable *>(m_data.data()))
    {
      ret=iterable->iterator(thread);
    }
  }
  return ret;
}


