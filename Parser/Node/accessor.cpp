#include "accessor.h"
using namespace QTwig::Node;


Accessor::Accessor(Generic *node): m_node(node)
{
  m_tag=nodeTypeNamePtr(node->type());
}

AccessorValue &Accessor::addNamedChildren(const QString &name, const std::function<QSharedPointer<Generic> ()> &getValue)
{
  AccessorValue *value=new AccessorValue(name);
  value->setGetChildren(getValue);
  m_named.append(value);
  return *value;
}

AccessorValue *Accessor::renameChildren(const QString &oldName, const QString &newName)
{
  AccessorValue *ret=nullptr;
  for(int i=0;i<m_named.size();i++)
  {
    if(m_named[i]->key()==oldName)
    {
      m_named[i]->setKey(newName);
      ret=m_named[i];
      break;
    }
  }
  return ret;
}

int Accessor::numNamedChildrens()
{
  return m_named.size();
}

const AccessorValue *Accessor::namedChildrenAccess(int i)
{
  AccessorValue *ret=nullptr;
  if(i>=0 && i<m_named.size())
    ret=m_named[i];
  return ret;
}

Ptr Accessor::namedChild(int i)
{
  Ptr ret;
  if(i>=0 && i<m_named.size())
    ret=m_named[i]->child();
  return ret;
}

void Accessor::setDataString(const std::function<QString()> &newDataString)
{
  m_printableData = newDataString;
}

Ptr AccessorValue::child() const
{
  return m_getValue?m_getValue():nullptr;
}

void AccessorValue::setGetChildren(const std::function<Ptr()> &function)
{
  m_getValue = function;
}


