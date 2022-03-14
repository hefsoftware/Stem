#pragma once
#include "node.h"

namespace QTwig
{
  namespace Node
  {
    class Node2: public Generic
    {
    public:
      Node2(Ptr expr1, Ptr expr2, const Location &location=Location()): Generic(location), m_expr1(expr1), m_expr2(expr2) { }
      Ptr expr1() { return m_expr1; }
      ConstPtr expr1() const { return m_expr1; }
      Ptr expr2() { return m_expr1; }
      ConstPtr expr2() const { return m_expr2; }
    protected:
      Ptr m_expr1;
      Ptr m_expr2;
      Accessor *accessor() override;
      Accessor *accessor(const char *key1, const char *key2);
    };
  }
}
