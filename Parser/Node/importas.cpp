#include "importas.h"
#include "accessor.h"
#include "../token.h"
using namespace QTwig::Node;
using namespace QTwig;
Ptr Node::newImportAs(const QString &name, const QString &as, const Location &location)
{
  return Ptr(new ImportAs(name, as, location));
}

Ptr Node::newImportAs(const QString &name, const Location &location)
{
  return Ptr(new ImportAs(name, name, location));
}

ImportAs::ImportAs(const QString &name, const QString &as, const Location &location): Generic(location), m_name(name), m_as(as)
{

}

Accessor *ImportAs::accessor()
{
  auto access=Generic::accessor();
  access->setDataString([this](){return m_as;});
  access->addNamedChildren("source", [this](){ return (m_as==m_name)?Node::Ptr():Node::newText(m_name, location()); }).setOmitIfEmpty(true);
  return access;
}
