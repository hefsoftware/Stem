#pragma once
#include <QString>

namespace QTwig
{
  class MessageSink;
  class Location;

  class Message
  {
  public:
    enum class Severity
    {
      Info,
      Warning,
      Error
    };
    Message();
    virtual ~Message() { }
    void info(const QString &msg, const Location &location);
    void info(const QString &msg, const QString &location=QString());
    void warning(const QString &msg, const Location &location);
    void warning(const QString &msg, const QString &location=QString());
    void error(const QString &msg, const Location &location);
    void error(const QString &msg, const QString &location=QString());
    inline void message(Severity severity, const QString &msg, const Location &location);
    virtual void message(Severity severity, const QString &msg, const QString &location)=0;
  };
}
