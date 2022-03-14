#include "message.h"
#include "../Util/location.h"
using namespace QTwig;

Message::Message()
{

}

void Message::info(const QString &msg, const Location &location)
{
  message(Severity::Info, msg, location);
}

void Message::info(const QString &msg, const QString &location)
{
  message(Severity::Info, msg, location);
}

void Message::warning(const QString &msg, const Location &location)
{
  message(Severity::Warning, msg, location);
}

void Message::warning(const QString &msg, const QString &location)
{
  message(Severity::Warning, msg, location);
}

void Message::error(const QString &msg, const Location &location)
{
  message(Severity::Error, msg, location);
}

void Message::error(const QString &msg, const QString &location)
{
  message(Severity::Error, msg, location);
}
void Message::message(Message::Severity severity, const QString &msg, const Location &location)
{
  message(severity, msg, QString::fromLatin1("%3 %1:%2").arg(location.row()).arg(location.column()).arg(location.file()));
}
