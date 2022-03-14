#pragma once
#include "node2.h"

namespace QTwig
{
  namespace Node
  {
    class Use: public Generic
    {
    public:
      Use(Ptr source, Ptr blocks, const Location &location=Location()): Generic(location), m_source(source), m_blocks(blocks) { }
      Type type() const override { return Type::Use; }
      Ptr source() { return m_source; }
      ConstPtr source() const { return m_source; }
      Ptr blocks() { return m_blocks; }
      ConstPtr blocks() const { return m_blocks; }
    protected:
      Accessor *accessor() override;
      Ptr m_source;
      Ptr m_blocks;
    };
  }
}
