#include "arguments.h"
#include "../value.h"
#include "../../Util/location.h"
using namespace QTwig;
Value Arguments::checkNoArgs(const QList<Value> &args, const QMap<QString, Value> &named)
{
  Value ret;
  if(!args.isEmpty() || !named.isEmpty())
    ret=Value::createError(QObject::tr("The function accepts no argument"), Location());
  return ret;
}

Value Arguments::checkSingleArg(const QList<Value> &args, const QMap<QString, Value> &named)
{
  Value ret;
  if(args.size()!=1 || !named.isEmpty())
    ret=Value::createError(QObject::tr("The function accepts a single non-named argument"), Location());
  return ret;
}

Value Arguments::checkSingleStringArg(const QList<Value> &args, const QMap<QString, Value> &named, QString &value)
{
  Value ret;
  if(args.size()!=1 || !named.isEmpty())
    ret=Value::createError(QObject::tr("The function accepts a single non-named argument"), Location());
  else
  {
    auto val=args[0].value();
    if(val.typeId()!=QMetaType::Type::QString)
      ret=Value::createError(QObject::tr("Functions expects a string parameter. Got '%1'").arg(QMetaType(val.typeId()).name()), Location());
    else
      value=val.toString();
  }
  return ret;
}
