#include "set.h"
#include "../../Engine/runthread.h"
#include "../../Engine/value.h"
#include "list.h"
#include "name.h"
using namespace QTwig;
using namespace QTwig::Node;

Ptr Node::newSet(Ptr target, Ptr value, const Location &location)
{
  return Ptr(new Set(target, value, location));
}

bool QTwig::Node::Set::execute(RunThread &thread) const
{
  Q_ASSERT(m_expr1.dynamicCast<List>());
  Q_ASSERT(m_expr2.dynamicCast<List>());
  auto target=m_expr1.staticCast<List>().data();
  auto value=m_expr2.staticCast<List>().data();
  Q_ASSERT(target->numChildren()==value->numChildren());
  bool ret=true;
  for(int i=0;i<target->numChildren();i++)
  {
    Q_ASSERT(target->children(i).dynamicCast<Name>());
    QString name=target->children(i).staticCast<Name>()->name();
    auto v=thread.evaluate(value->children(i).data());
    if(v.isValid())
      v=thread.set(name, v);
    if(!v.isValid())
    {
      thread.errorMessage(v, this);
      ret=false;
      break;
    }
  }
  return ret;
}
