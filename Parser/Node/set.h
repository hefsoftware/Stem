#include "map.h"
namespace QTwig
{
  namespace Node
  {
    class Set: public Map
    {
    public:
      Set(Ptr key, Ptr value, const Location &location=Location()): Map(key, value, location) { }
      Type type() const override { return Type::Set; }
      bool hasExecute() const override { return true; }
      bool execute(RunThread &thread) const override;
    };
  }
}
