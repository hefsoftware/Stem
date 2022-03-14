#include "scopesstack.h"
#include "../../Util/location.h"
using namespace QTwig;
ScopesStack::ScopesStack(): m_getBarrierLevel(0), m_setBarrierLevel(0), m_setTransparent(0)
{

}

void ScopesStack::pushNewEntry(RunThread::Visibility visibility)
{
  m_getBarrierLevel+=(visibility==RunThread::Visibility::GetOpaque);
  m_setBarrierLevel+=(visibility<=RunThread::Visibility::SetOpaque);
  m_setTransparent+=(visibility==RunThread::Visibility::SetTransparent);
  m_entries.append(Entry(visibility, m_getBarrierLevel, m_setBarrierLevel));
}

void ScopesStack::popEntry()
{
  Q_ASSERT(!m_entries.isEmpty());
  auto &pop=m_entries.last();
  auto visibility=pop.visibility;
  m_getBarrierLevel-=(visibility==RunThread::Visibility::GetOpaque);
  m_setBarrierLevel-=(visibility<=RunThread::Visibility::SetOpaque);
  m_setTransparent-=(visibility==RunThread::Visibility::SetTransparent);
  for(auto it=pop.variables.cbegin(); it!=pop.variables.cend(); it++)
  {
    Q_ASSERT(m_variables.contains(it.key()) && !m_variables[it.key()].isEmpty() && &entry(m_variables[it.key()].last())==&pop); // Check variable exists and last instance refers to this entry
    auto &l=m_variables[it.key()];
    Q_ASSERT(!l.isEmpty());
    if(l.size()>1)
      l.removeLast();
    else
      m_variables.remove(it.key());
  }
  m_entries.removeLast();
}





QMap<QString, Value> ScopesStack::extractCurrentScope()
{
  QMap<QString, Value> ret;
  for(auto it=m_variables.cbegin();it!=m_variables.cend();it++)
  {
    auto &cur=entry(it.value().last());
    if(cur.getBarrierLevel==m_getBarrierLevel)
      ret.insert(it.key(), cur.variables[it.key()]);
  }
  return ret;
}


Value ScopesStack::set(const QString &key, const Value &val)
{
  bool set=false;
  if(m_variables.contains(key))
  {
    auto &varEntry=m_variables[key];
    auto &lastEntry=entry(varEntry.last());
    if(lastEntry.setBarrierLevel==m_setBarrierLevel)
    {
      lastEntry.variables.insert(key, val); // We have a visible variable with the given name and without a set barrier in between. We directly change its entry.
      set=true;
    }
    else
    {
      // We already have a variable with the given name but it's behind a set barrier. Insert a new one in last (non set-transparent) entry
      for(int i=m_entries.size()-1;i>=0;i--)
      {
        auto &cur=m_entries[i];
        if(cur.visibility==RunThread::Visibility::SetTransparent)
          continue;
        varEntry.append(i);
        cur.variables.insert(key, val);
        set=true;
        break;
      }
    }
  }
  else {
    // We do not have any variable with the given name
    for(int i=m_entries.size()-1;i>=0;i--)
    {
      auto &cur=m_entries[i];
      if(cur.visibility==RunThread::Visibility::SetTransparent)
        continue;
      m_variables.insert(key, {i});
      cur.variables.insert(key, val);
      set=true;
      break;
    }
  }
  return (set?val:Value::createError("BUG! In ScopesStack::set no variable was set", Location()));
}

bool ScopesStack::hasVariable(const QString &key)
{
  return m_variables.contains(key) && entry(m_variables[key].last()).getBarrierLevel==m_getBarrierLevel;
}

Value ScopesStack::setOuterScope(const QString &key, const Value &value)
{
  if(m_variables.contains(key))
    m_variables[key].append(m_entries.size()-1);
  else
    m_variables.insert(key, {(int)m_entries.size()-1});
  m_entries.last().variables.insert(key, value);
  return value;
}

Value ScopesStack::get(const QString &key)
{
  Value ret;
  if(m_variables.contains(key))
  {
    auto &cur=entry(m_variables[key].last());
    if(cur.getBarrierLevel==m_getBarrierLevel)
      ret=cur.variables[key];
    else
      ret=Value::createUndefined();
  }
  else
    ret=Value::createUndefined();
  return ret;
}

RunThread::Visibility ScopesStack::visibility()
{
  RunThread::Visibility ret;
  if(m_getBarrierLevel>0)
    ret=RunThread::Visibility::GetOpaque;
  else if(m_setBarrierLevel>0)
    ret=RunThread::Visibility::SetOpaque;
  else if(m_setTransparent==m_entries.size())
    ret=RunThread::Visibility::SetTransparent;
  else
    ret=RunThread::Visibility::Normal;
  return ret;
}
