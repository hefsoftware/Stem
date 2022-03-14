#include "node.h"
#include "accessor.h"
#include <QDebug>
#include <QMetaEnum>
#include "../../Engine/runthread.h"
#define tr QObject::tr
using namespace QTwig::Node;

const char *QTwig::Node::nodeTypeNamePtr(Type type)
{
  QMetaEnum metaEnum = QMetaEnum::fromType<Type>();
  return metaEnum.valueToKey((int)type);
}

QString QTwig::Node::nodeTypeName(Type type)
{
  return QString::fromLatin1(nodeTypeNamePtr(type));
}

Accessor *Generic::accessor()
{
  return (new Accessor(this));
}

QString Generic::toString(bool *isEmpty)
{
  const int maxRowLength=60;
  QScopedPointer<Accessor> acc(accessor());
  if(isEmpty)
    *isEmpty=acc->isEmpty();
  Q_ASSERT(acc);
  QString ret=acc->tag();
  QString printData=acc->printableData();
  if(!printData.isNull())
    ret+=QLatin1String("(%1)").arg(printData);
//  auto namedChildren=acc->namedChildrens();
  QStringList strings;
  bool hasNewLines=false;
  for(int i=0;i<acc->numNamedChildrens();i++)
  {
    auto v=acc->namedChildrenAccess(i);
    auto child=v->child();
    QString childStr=v->key();
    if(childStr.isNull())
      childStr=QString::number(i);
    childStr+=QString::fromLatin1(": ");
    bool isEmpty=true;
    if(child)
      childStr+=child->toString(&isEmpty);
    else
      childStr+="(NULL)";
    if(!v->omitIfEmpty() || !isEmpty)
    {
      if(childStr.contains('\n'))
      {
        childStr.replace("\n", "\n  ");
        hasNewLines=true;
      }
      strings.append(childStr);
    }
  }
  for(int i=0;i<m_children.size();i++)
  {
    auto child=m_children[i];
    QString childStr;
    if(child)
      childStr=child->toString();
    else
      childStr="(NULL)";
    if(childStr.contains('\n'))
    {
      auto bef=childStr;
      childStr.replace("\n", "\n  ");
      hasNewLines=true;
    }
    strings.append(childStr);
  }
  if(!strings.isEmpty())
  {
    bool hasContent=false;
    ret+=" {";
    if(!hasNewLines)
    {
      bool ok=true;
      QString joined;
      int limitLength=maxRowLength-(ret.size()+1);
      for(int i=0; i<strings.size(); i++)
      {
        joined+=QString::fromLatin1(i==0?"":", ")+strings[i];
        if(joined.size()>limitLength)
        {
          ok=false;
          break;
        }
      }
      if(ok)
      {
        ret+=joined;
        hasContent=true;
      }
    }
    if(!hasContent)
    {
      ret+=QString("\n");
      for(int i=0; i<strings.size(); i++)
        ret+=QString("  ")+strings[i]+QString("\n");
    }
    ret+="}";
  }
  return ret;
}

bool Generic::execute(RunThread &thread) const
{
  thread.errorMessage(tr("Unimplemented node execute in  %1").arg(nodeTypeName(type())), location());
  return false;
}

