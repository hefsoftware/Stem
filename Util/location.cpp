#include "location.h"
#include <QDebug>
using namespace QTwig;

Location::Location(const QString &file, int row, int column)
{
  init(file,row,column);
}
Location::Location(int row, int column)
{
  init(QString::fromLatin1("(source)"),row,column);
}

QString Location::toLocalString() const
{
  return QString::fromLatin1("%1:%2").arg(m_row).arg(m_column);
}

QString Location::toString() const
{
  return QString::fromLatin1("\"%3\" (%1:%2)").arg(m_row).arg(m_column).arg(m_file);

}

void Location::init(const QString &file, int row, int column)
{
  m_file=file;
  m_row=row;
  m_column=column;
}

QDebug operator<<(QDebug dbg, const QTwig::Location &location)
{
  dbg<<QString::fromLatin1("%3 %1:%2").arg(location.row()).arg(location.column()).arg(location.file()).toUtf8();
  return dbg;
}
