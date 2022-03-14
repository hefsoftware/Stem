#pragma once
#include <QSharedPointer>
namespace QTwig
{
  class Engine;
  namespace Node
  {
    class Generic;
    class Print;
    class List;
  }
  class Location;
  class Message;
  class Scope;
  class Value;
  class RunThread: public QEnableSharedFromThis<RunThread>
  {
  friend class Engine;
    RunThread(const QSharedPointer<Engine> &engine);
  public:
    // See description in Scope class
    enum class Visibility: int
    {
      GetOpaque,
      SetOpaque,
      Normal,
      SetTransparent
    };
    class PushOutput
    {
    public:
      PushOutput(RunThread *thread, QIODevice *device): m_thread(thread) { Q_ASSERT(thread); Q_ASSERT(device); thread->m_output.append(device); }
      PushOutput(const PushOutput &) = delete;
      ~PushOutput();
    protected:
      RunThread *m_thread;
    };
    class PushMessage
    {
    public:
      PushMessage(RunThread *thread, Message *device): m_thread(thread) { Q_ASSERT(thread); Q_ASSERT(device); thread->m_message.append(device); }
      PushMessage(const PushOutput &) = delete;
      ~PushMessage();
    protected:
      RunThread *m_thread;
    };
    class PushScope
    {
    public:
      PushScope(RunThread *thread, Visibility visibility=Visibility::Normal);
      PushScope(const PushScope &) = delete;
      ~PushScope();
    protected:
      RunThread *m_thread;
    };
    class PushScopePtr
    {
    public:
      // Note: Get barrier to true implicitly sets also setBarrier to true
      PushScopePtr(RunThread *thread, Scope *ptr, bool own=true);
      PushScopePtr(const PushScope &) = delete;
      ~PushScopePtr();
    protected:
      RunThread *m_thread;
      bool m_own=false;
    };

    typedef QSharedPointer<RunThread> Ptr;
    // Converts an expression to a string
    Value executeExprToString(const Node::Generic *expr);
    // Prints an expression
    bool executeExprPrint(const Node::Generic *expr);
    bool executeNode(const Node::Generic *node);
    Value evaluate(const Node::Generic *node);
    // Note: args must be an instance of a List
    Value executeFunction(Value function, const Node::Generic *args);
    bool execute(const QSharedPointer<Node::Generic> &node, QIODevice *output, Message *message);
    bool execute(const QString &filename, QIODevice *output, Message *message);
    void errorMessage(const QString &error, const Location &location);
    void errorMessage(const Value &value);
    // Outputs the error message contained in value. If value does not provide a location, the one taken from node is used.
    void errorMessage(Value value, const Node::Generic *node);
    inline Message *message() { return m_message.last(); }
    inline Scope *scope() { return m_scope.last(); }
    QIODevice *output(const QString &text);
    // Forces setting a variable into the most out scope (ignoring default rules of visibility for setting). Returns value in case of succes, an error in case of fault
    Value setOuterScope(const QString &key, const Value &value);
    // Sets a variable (see ScopeStack for rules). Returns value in case of succes, an error in case of fault
    Value set(const QString &key, const Value &value);
    Value get(const QString &key);
    // Extracts all variables visible in this moment (all nested scopes stopping at a shadowing one)
    QMap<QString, Value> extractCurrentScope();
    void debugCurrentScope();
    // If value has not already a location sets it to location to the given value
    static void setErrorLocation(Value &value, const Location &location);
    // If value has not already a location sets it to location to the given value
    static void setErrorLocation(Value &value, const Node::Generic *source);
  protected:
    // If value is an error and has no location set changes it to location
    QSharedPointer<Engine> m_engine;
    QList<QIODevice *> m_output;
    QList<Message *> m_message;
    QList<Scope *> m_scope;
    bool m_running;
  };
}
