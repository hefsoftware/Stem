#pragma once
#include "../value.h"
#include "../runthread.h"
namespace QTwig
{
  class RunThread;
  class Location;
  /* Stack/scopes model for runtime
   * The stack is created by a layer of scopes. Each scope can have a different behaviour regarding gets and sets:
   * Get opaque scope: A get opaque layer stops the search for any get or set in the stack. A scope being get opaque is effectively equivalent of not having any of its outer scopes on the stack.
   * Set opaque scope: A get behaves just normally. For the set a scope being set opaque is effectively equivalent of not having any of its outer scopes on the stack.
   * Normal scope: A get searches the variable in this scope, if it is not found outer layer are searched. If no variable with the given name is found an "undefined" is returned.
   *               A set searches the variable in this scope, if it is not found outer layer are searched. If a variable with the given name is found it is set in the scope where it resides. If no variable is found a new variable is created in the outer scope.
   * Write transparent: A get behaves just like in normal scope. When a set is done a search is executed just as it is in a normal scope. If the variable is not found the new variable is created in the first outer non write transparent scope.
   *                    If a variable is found in a write transparent layer the behaviour is for now undefined (should we override the variable of write transparent layer or remove it from write transparent layer and put it in outer non write-transparent layer? For now the first is done)
   *
   *
   * The running engine by default uses a ScopesStack scope that can implement the described behaviour a more efficient way. In any case having a layer of different ScopesStack is fully equivalent to having a single ScopesStack with several Entries.
   * The execution code can if needed add custom scopes (for e.g. handling dynamic variable get).
   */
  class Scope
  {
    friend class RunThread;
  public:
    Scope() { }
    virtual ~Scope() { }
    virtual Value get(const QString &key)=0;
    virtual Value set(const QString &key, const Value &val)=0;
    virtual Value setOuterScope(const QString &key, const Value &value) { return set(key, value); }

    virtual bool hasVariable(const QString &key)=0;
    virtual RunThread::Visibility visibility()=0;
////    // Returns true if this scope shadows getting variables from scopes before this. Note that if getBarrier returns true then also setBarrier must return true
//    virtual bool isGetBarrier()=0;
////    // Returns true if a set to a variable whose name already exists will not modify a variable in a scope before this (and thus will set it only on the outer scope)
//    virtual bool isSetBarrier()=0;
    virtual QMap<QString, Value> extractCurrentScope() { return QMap<QString, Value>(); }
  protected:
    // Optimization: Returns true only if this instance is a ScopesStack (no need for dynamic_cast)
    virtual bool isStack() { return false; }
  };
}
