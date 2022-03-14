#include "valuedataarray.h"
#include "iterator.h"
using namespace QTwig;

class ValueDataListIterator: public Iterator
{
public:
  ValueDataListIterator(const QSharedPointer<ValueDataList> &list): m_list(list) { }
protected:
  QSharedPointer<ValueDataList> m_list;
};

QSharedPointer<Iterator> ValueDataList::iterator(RunThread *thread)
{
  Q_UNUSED(thread);
  return QSharedPointer<Iterator>(new ValueDataListIterator(sharedFromThis().staticCast<ValueDataList>()));
}
