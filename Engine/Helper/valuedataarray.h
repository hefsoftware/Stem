#pragma once
#include "iterable.h"
#include "../value.h"
#include <QList>
#include <QMap>
namespace QTwig
{
  class Value;
  class RunThread;
  class ValueDataList: public Iterable
  {
  public:
    ValueDataList(const QList<Value> &values): m_values(values) { }
    QSharedPointer<Iterator> iterator(RunThread *thread) override;
    QString type() const  override { return "list"; }

  protected:
    QList<Value> m_values;
  };
}
