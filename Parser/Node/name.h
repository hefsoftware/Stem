#pragma once
#include "node.h"
namespace QTwig
{
  namespace Node
  {
    class Name: public Generic
    {
    public:
      Name(const QString &name, const Location &location=Location()): Generic(location), m_name(name) {}
      Type type() const override { return Type::Name; }
      inline QString name() const { return m_name; }
    protected:
      QString m_name;
      Accessor *accessor() override;
    };
  }
}
