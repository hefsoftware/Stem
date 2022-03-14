#pragma once
#include <QMap>
namespace QTwig
{
  class Value;
  namespace Arguments
  {
    Value checkNoArgs(const QList<Value> &args, const QMap<QString, Value> &named);
    Value checkSingleArg(const QList<Value> &args, const QMap<QString, Value> &named);
    Value checkSingleStringArg(const QList<Value> &args, const QMap<QString, Value> &named, QString &value);
  }
}
