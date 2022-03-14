#include "map.h"
namespace QTwig
{
  namespace Node
  {
    class ArrayGet: public Map
    {
    public:
      ArrayGet(Ptr key, Ptr value, const Location &location=Location()): Map(key, value, location) { }
      Type type() const override { return Type::ArrayGet; }
    };
  }
}
