#pragma once
#include "node.h"
#include <Qt>
#include <QSharedPointer>
namespace QTwig
{
  namespace Node
  {
    class Generic;

    struct AccessorValue
    {
    public:
      AccessorValue(const QString &key): m_key(key) { }
      virtual ~AccessorValue() { }
      constexpr bool omitIfEmpty() const { return m_omitIfEmpty; }
      constexpr AccessorValue &setOmitIfEmpty(bool omitIfEmpty=true) { m_omitIfEmpty=omitIfEmpty; return *this; }
      inline const QString &key() const { return m_key; }
      void setKey(const QString &newKey) { m_key=newKey; }
      Ptr child() const;
      void setGetChildren(const std::function<Ptr()> &newGetValue);
    protected:
      QString m_key;
      bool m_omitIfEmpty;
      std::function<Ptr()> m_getValue;
    };

    class Accessor
    {
      friend class Generic;
    protected:
      Accessor(Generic *node);
      Accessor(const Accessor &) = delete;
      std::function <QString()> m_printableData;
      bool m_isEmpty;
      const char *m_tag;
      Generic *m_node;
      QList<AccessorValue *> m_named;
    public:
      AccessorValue &addNamedChildren(const QString &name, const std::function<QSharedPointer<Node::Generic>()> &getValue=nullptr);
      AccessorValue *renameChildren(const QString &oldName, const QString &newName);

      constexpr const char *tag() const { return m_tag; }
      constexpr void setTag(const char *newTag) { m_tag=newTag; }
      int numNamedChildrens();
      const AccessorValue *namedChildrenAccess(int i);
      Ptr namedChild(int i);

      void setDataString(const std::function<QString ()> &newDataString);

      QString printableData() { return m_printableData?m_printableData():QString(); }
      constexpr bool isEmpty() const { return m_isEmpty; }
      constexpr void setEmpty(bool empty) { m_isEmpty=empty; }
    };
  }
}
