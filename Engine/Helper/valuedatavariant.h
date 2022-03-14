#pragma once
#include "valuedata.h"
#include <QVariant>
namespace QTwig
{
  class ValueDataVariant: public ValueData
  {
  public:
    ValueDataVariant(const QVariant &value): m_value(value) { }
    QVariant value() override { return m_value; }
    QString type() const override { return "variant"; }
  protected:
    QVariant m_value;
  };
}

