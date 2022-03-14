#pragma once
#include <QSharedPointer>
#include <QVariant>
namespace QTwig
{
  class RunThread;
  class ValueData;
  class Location;
  class Iterator;
  class Value
  {
  public:
    Value();
    Value(const QVariant &value);
    QVariant value() const;
    static Value createData(ValueData *data);
    static Value createError(const QString &message, const Location &location);
    static Value createUndefined(const QString &name, const Location &location);
    static Value createUndefined();
    static Value createFunction(const std::function<Value(RunThread *thread, const QList<Value> &, const QMap<QString, Value> &)> &function);
    bool isUndefined() const ;
    bool isError() const;
    // Check if the value is valid (not an error or undefined)
    bool isValid() const;
    // Check if the data pointer is null (uninitialized value)
    bool isNullData() const { return !m_data; }
    Value call(RunThread *thread, const QList<Value> &args=QList<Value>(), const QMap<QString, Value> &named=QMap<QString, Value>());
    Value toBool();
    QString toPrintable(bool *ok=nullptr) const;
    bool isIterable(RunThread *thread);
    QSharedPointer<Iterator> iterator(RunThread *thread);

    static constexpr bool isIntType(int type) { return (type==QMetaType::Short || type==QMetaType::UShort || type==QMetaType::Int || type==QMetaType::UInt || type==QMetaType::Long || type==QMetaType::ULong || type==QMetaType::LongLong); }
    static constexpr bool isFloatType(int type) { return (type==QMetaType::Float || type==QMetaType::Double); }
    static constexpr bool isNumberType(int type) { return (isIntType(type) || isFloatType(type)); }
    QSharedPointer<const ValueData> getData() const { return m_data; }
    QSharedPointer<ValueData> getData() { return m_data; }
    template <typename T> const T *getDataStaticCast() const { return static_cast<const T*>(m_data.data()); }
    template <typename T> T *getDataStaticCast() { return static_cast<T*>(m_data.data()); }
    template <typename T> T *getDataCast() { return dynamic_cast<T*>(m_data.data()); }
    template <typename T> QSharedPointer<T> getDataShared() { return m_data.dynamicCast<T*>(m_data.data()); }
  protected:
    Value(ValueData *data);
    QSharedPointer<ValueData> m_data;
  };
  inline QDebug operator<<(QDebug debug, const Value &c)
  {
      QDebugStateSaver saver(debug);
      QString val;
      bool ok;
      val=c.toPrintable(&ok);
      if(ok)
        debug.nospace() << val.toUtf8().data();
      else
        debug.nospace() << "(Error in conversion)";
      return debug;
  }
}
