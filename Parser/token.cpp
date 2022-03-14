
#include "token.h"
#include <QDebug>

using namespace QTwig;

QString Token::shortenedText(int maxLen) const
{
  QString ret;
  maxLen=qMax(maxLen,6);
  if(m_text.length()<maxLen)
    ret=m_text;
  else
  {
    int l1=(maxLen-3)/2;
    ret=m_text.left(maxLen-3-l1)+QString("...")+m_text.right(l1);
  }
  return ret;
}

QString Token::printable() const
{
  if(m_text.isEmpty())
    return typeName(type());
  else
    return QString::fromLatin1("%1(%2)").arg(typeName(type()), shortenedText(20));
}

bool Token::test(const QString &text) const
{
    return m_type==Type::Name && m_text==text;
}

bool Token::test(const QString &text1, const QString &text2) const
{
    return m_type==Type::Name && (m_text==text1 || m_text==text2);
}

QString Token::typeName(Token::Type type)
{
  switch(type)
  {
    case Type::Eof:
    return QString::fromLatin1("EOF");
    case Type::Text:
    return QString::fromLatin1("TEXT");
  case Type::BlockStart:
    return QString::fromLatin1("BLOCK_START");
  case Type::BlockEnd:
    return QString::fromLatin1("BLOCK_END");
  case Type::VarStart:
    return QString::fromLatin1("VAR_START");
  case Type::VarEnd:
    return QString::fromLatin1("VAR_END");
  case Type::InterpolationStart:
    return QString::fromLatin1("INTERPOLATION_START");
  case Type::InterpolationEnd:
    return QString::fromLatin1("INTERPOLATION_END");
  // Expression
  case Type::Name:
    return QString::fromLatin1("NAME");
  case Type::Number:
    return QString::fromLatin1("NUMBER");
  case Type::OpenBracket:
    return QString::fromLatin1("OPENBRACKET");
  case Type::ClosedBracket:
    return QString::fromLatin1("CLOSEDBRACKET");
  case Type::Punctuation:
    return QString::fromLatin1("PUNCTUATION");
  case Type::String:
    return QString::fromLatin1("STRING");
  case Type::Arrow:
    return QString::fromLatin1("ARROW");
  case Type::Operator:
    return QString::fromLatin1("OPERATOR");
  case Type::Null:
    return QString::fromLatin1("(null)");
  }
  return QString::fromLatin1("(Type %1)").arg((int)type);
}

Location Token::location() const
{
  return m_location;
}

void Token::setLocation(const Location &location)
{
  m_location = location;
}

QDebug operator<<(QDebug dbg, const Token &token)
{
  QDebugStateSaver saver(dbg);
  if(!token.text().isEmpty())
    dbg.nospace() << Token::typeName(token.type()).toUtf8().data() << "(" << token.text().replace("\\","\\\\").replace("\n","\\n").replace("\t","\\t").toUtf8().data()<< ")";
  else
    dbg.nospace() << Token::typeName(token.type()).toUtf8().data();
  return dbg;
}
