#pragma once
#include "constant.h"

namespace QTwig
{
  namespace Node
  {
    class ImportAs: public Generic
    {
    public:
      ImportAs(const QString &name, const QString &as, const Location &location=Location());
      QString name() const { return m_name; }
      QString as() const { return m_as; }
      Type type() const override { return Type::ImportAs; }
    protected:
      QString m_name;
      QString m_as;
      Accessor *accessor() override;
    };
  }
}
