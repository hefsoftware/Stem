#pragma once
#include "constant.h"

namespace QTwig
{
  namespace Node
  {
    class Text: public Constant
    {
    public:
      Text(const QString &text, const Location &location=Location());
      QString text() const;
      QString printableText() const;
      Type type() const override { return Type::Text; }
    protected:
      QString m_text;
      Accessor *accessor() override;
    };
  }
}
