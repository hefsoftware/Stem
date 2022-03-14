#pragma once
#include "valuedata.h"
#include <QSharedPointer>
class QVariant;
namespace QTwig
{
  class Value;
  class RunThread;
  class Iterator;
  class Iterable: public ValueData
  {
  public:
    Iterable() { }
    virtual bool isIterable() { return true; }
    virtual QSharedPointer<Iterator> iterator(RunThread *thread)=0;
  };
}
