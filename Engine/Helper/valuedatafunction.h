#pragma once
#include "valuedata.h"

namespace QTwig
{
  class Value;
  class RunThread;
  class ValueDataFunction: public ValueData
  {
  public:
    ValueDataFunction(const std::function<Value(RunThread *thread, const QList<Value> &, const QMap<QString, Value> &)> &function): m_function(function) { }
    Value call(RunThread *thread, const QList<Value> &args, const QMap<QString, Value> &namedArgs) override;
    QString type() const override { return "function"; }
  protected:
    std::function<Value(RunThread *thread, const QList<Value> &, const QMap<QString, Value> &)> m_function;
  };
}

