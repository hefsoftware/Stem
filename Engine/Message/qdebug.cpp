#include "qdebug.h"
#include <QDebug>
using namespace QTwig;
using namespace Messages;
void MsgQDebug::message(Severity severity, const QString &msg, const QString &location)
{
  switch(severity)
  {
  case Severity::Info:
    QDebug(QtInfoMsg)<<location<<msg; break;
  case Severity::Warning:
    QDebug(QtWarningMsg)<<location<<msg; break;
  case Severity::Error:
    QDebug(QtCriticalMsg)<<location<<msg; break;
  };
}
