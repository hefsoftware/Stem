#include "text.h"
#include "accessor.h"
#include "../token.h"
using namespace QTwig::Node;
using namespace QTwig;
Ptr Node::newText(const QString &text, const Location &location)
{
  return Ptr(new Text(text, location));
}
Ptr Node::newText(const QTwig::Token &token)
{
  return newText(token.text(), token.location());
}

Text::Text(const QString &text, const Location &location): Constant(location), m_text(text)
{

}

QString Text::printableText() const
{
  const int maxLength=15;
   QString ret=m_text;
   ret=ret.replace("\"","\\").replace("\n","\\n").replace("\\r","\r").replace("\\0","\0").replace("\t","\\t");
   if(ret.size()>maxLength*2)
     ret=ret.left(maxLength)+"..."+ret.right(maxLength);
   return QString("\"%1\"").arg(ret);
 }

 QString Text::text() const
 {
     return m_text;

}

Accessor *Text::accessor()
{
  auto access=Generic::accessor();
  access->setDataString([this](){return printableText();});
  return access;
}
