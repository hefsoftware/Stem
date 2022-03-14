#pragma once
#include "iterable.h"
#include "../value.h"
#include <QList>
#include <QMap>
namespace QTwig
{
  class Value;
  class RunThread;
  class ValueDataMap: public Iterable
  {
  public:
    ValueDataMap(const QMap<QVariant, Value> &values): m_values(values) { }
    QSharedPointer<Iterator> iterator(RunThread *thread) override;
    QString type() const override { return "map"; }
  protected:
    QMap<QVariant, Value> m_values;
  };
}
