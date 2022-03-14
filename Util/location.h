#pragma once

#include <QString>
namespace QTwig
{
  class Location
  {
  public:
    Location(const QString &file, int row, int column=0);
    Location(int row=0,int column=0);
    int     column() const {return m_column;}
    int     row()    const {return m_row;   }
    QString file()   const {return m_file;  }
    QString toLocalString() const;
    QString toString() const;
    constexpr bool isUndefined() const { return (m_row==0 && m_column==0); }
  private:
    void    init(const QString &file, int row, int column);
    int m_column;
    int m_row;
    QString m_file;
  };
}
QDebug operator<<(QDebug dbg, const QTwig::Location &location);
