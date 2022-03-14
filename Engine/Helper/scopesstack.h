#pragma once
#include "scope.h"
#include <QMap>
#include <QString>
namespace QTwig
{
  class ScopesStack: public Scope
  {
  public:
    ScopesStack();
    static inline ScopesStack *create(RunThread::Visibility visibility) { auto ret=new ScopesStack; ret->pushNewEntry(visibility); return ret; }
    void pushNewEntry(RunThread::Visibility visibility);
    void popEntry();
    inline int numEntries() { return m_entries.size(); }
    bool hasVariable(const QString &key) override;
    Value setOuterScope(const QString &key, const Value &value) override;
    Value set(const QString &key, const Value &val) override;
    Value get(const QString &key) override;
    RunThread::Visibility visibility() override;

//    bool isGetBarrier() override { return (m_getBarrierLevel>0); }
//    // Returns true if a set to a variable whose name already exists will not modify a variable in a scope before this (and thus will set it only on the outer scope)
//    bool isSetBarrier() override { return (m_setBarrierLevel>0); }

    QMap<QString, Value> extractCurrentScope() override;
  protected:
    bool isStack() override { return true; }

    struct Entry
    {
      Entry(RunThread::Visibility visibility, int shadowLevel, int setBarrierLevel) : visibility(visibility), getBarrierLevel(shadowLevel), setBarrierLevel(setBarrierLevel) { }
      QMap<QString, Value> variables;
      RunThread::Visibility visibility;
//      bool shadow;
//      bool setBarrier;
      int  getBarrierLevel;
      int  setBarrierLevel;
    };
    inline Entry &entry(int i) { Q_ASSERT(i>=0 && i<m_entries.size()); return m_entries[i]; }
    QList<Entry> m_entries;
    QMap<QString, QList<int>> m_variables; // Variable name->list of indexes into entries
    int m_getBarrierLevel;
    int m_setBarrierLevel;
    int m_setTransparent;
  };
}
