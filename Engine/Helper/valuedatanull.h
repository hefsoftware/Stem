#include "valuedata.h"
#include <QVariant>
#include "../../Util/location.h"
namespace QTwig
{
  class ValueDataNull: public ValueData
  {
  public:
    ValueDataNull(const Location &location): m_location(location) { }
    QVariant value() override{ return QVariant(); }
    constexpr const Location &location() const { return m_location; }
    virtual ValueDataNull *changeLocation(const Location &location)=0;
  protected:
    Location m_location;
  };
  class ValueDataUndefined: public ValueDataNull
  {
  public:
    ValueDataUndefined(const QString name, const Location &location): ValueDataNull(location), m_name(name) { }
    QString name() const { return m_name; }
    bool isUndefined() override { return true; }
    ValueDataNull *changeLocation(const Location &location) override { return new ValueDataUndefined(m_name, location); }
  protected:
    QString m_name;
  };
  class ValueDataError: public ValueDataNull
  {
  public:
    ValueDataError(const QString message, const Location &location): ValueDataNull(location), m_message(message) { }
    QString message() const { return m_message; }
    bool isError() override { return true; }
    ValueDataNull *changeLocation(const Location &location) override { return new ValueDataError(m_message, location); }
  protected:
    QString m_message;
  };
}
