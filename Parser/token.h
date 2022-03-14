#pragma once
#include <QString>
#include "../Util/location.h"
namespace QTwig
{
  class Token
  {
  public:
    enum class Type
    {
      Eof,
      Text,
      BlockStart,
      BlockEnd,
      VarStart,
      VarEnd,
      InterpolationStart, // TODO unhandled and unused for now
      InterpolationEnd, // TODO unhandled and unused for now
      // Expression
      Arrow,
      Operator,
      Name,
      Number,
      OpenBracket,
      ClosedBracket,
      Punctuation,
      String,
      Null
    };
    Token():m_type(Type::Null){}
    Token(Type type, Location loc,const QString &text=""):m_type(type),m_text(text),m_location(loc){}
    Type type() const{return m_type;}
    QString text() const{return m_text;}
    QString shortenedText(int maxLen=20) const;
    QString printable() const;
    bool test(const QString &text) const;
    bool test(const QString &text1,const QString &text2) const;
    //bool test(const QString &text1,const QString &text2,const QString &text3);
    //bool test(const QString &text1,const QString &text2);
    bool test(Type type) const{return m_type==type;}
    bool test(Type type, const QString &text) const{return m_type==type && m_text==text;}
    static QString typeName(Type type);
    bool isNull() const{return m_type==Type::Null;}
    operator QString() const { return typeName(m_type)+" "+m_text; }
    operator bool() const { return m_type!=Type::Null && m_type!=Type::Eof; }

    Location location() const;
    void setLocation(const Location &location);
  private:
    Type m_type;
    QString m_text;
    Location m_location;
  };
}
QDebug operator<<(QDebug dbg, const QTwig::Token &token);
