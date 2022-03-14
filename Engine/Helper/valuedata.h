#pragma once
#include <QEnableSharedFromThis>
#include <QList>
#include <QMap>
class QVariant;
namespace QTwig
{
  class Value;
  class RunThread;
  class ValueData: public QEnableSharedFromThis<ValueData>
  {
  public:
    ValueData();
    virtual QVariant value();
    virtual Value call(RunThread *thread, const QList<Value> &, const QMap<QString, Value> &);
    virtual ~ValueData() { }
    virtual bool isUndefined() { return false; }
    virtual bool isError() { return false; }
    virtual Value toBool();
    virtual QString type() const { return "data";}
  };
}
