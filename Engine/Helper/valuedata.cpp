#include "valuedata.h"
#include "../value.h"
#include "../../Util/location.h"
using namespace QTwig;

#define tr QObject::tr

ValueData::ValueData()
{

}

QVariant ValueData::value()
{
  return QVariant();
}

Value ValueData::call(RunThread *thread, const QList<Value> &, const QMap<QString, Value> &)
{
  return Value::createError(tr("Value is not callable"), Location());
}

Value ValueData::toBool()
{
  auto data=value();
  bool ok=true;
  bool ret=false;
  if(Value::isIntType(data.typeId()))
    ret=data.toInt();
  else
  {
    switch(data.typeId())
    {
      case QMetaType::UnknownType:
        ret=false; break;
      case QMetaType::Bool:
        ret=data.toBool(); break;
      case QMetaType::QString:
        ret=!data.toString().isEmpty(); break;
      default:
        ok=false;
    }
  }
  return ok?Value(ret):Value::createError(tr("Type '%1' is not castable to bool").arg(QMetaType(data.typeId()).name()), Location());
}
