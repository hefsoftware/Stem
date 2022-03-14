#include "valuedatafunction.h"
#include "../value.h"
#include "../../Util/location.h"
using namespace QTwig;
#define tr QObject::tr
Value ValueDataFunction::call(RunThread *thread, const QList<Value> &args, const QMap<QString, Value> &namedArgs)
{
  return m_function?m_function(thread, args, namedArgs):
                    Value::createError(tr("Call of null function pointer"), Location());
}
