#pragma once
#include "valuedata.h"

class QVariant;
namespace QTwig
{
  class Value;
  class RunThread;
  class Iterator: public ValueData
  {
  public:
    Iterator() { }
  };
}
