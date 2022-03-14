#pragma once
#include "../message.h"
namespace QTwig
{
  namespace Messages
  {
    class MsgQDebug: public Message
    {
    public:
      MsgQDebug() { }
      void message(Severity severity, const QString &msg, const QString &location) override;
    };
  }
}
