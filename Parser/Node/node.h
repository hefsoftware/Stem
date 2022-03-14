#pragma once
#include "../../Util/location.h"
#include <QSharedPointer>
namespace QTwig
{
  Q_NAMESPACE
  class Token;
  class Operator1;
  class Operator2;
  class RunThread;
  namespace Node
  {
    Q_NAMESPACE
    class Accessor;
    enum class Type {
      Generic=50,
      //Empty,
      //GetAttr,
      MemberAccess,
      ArrayGet,
      Map, // name-key <- value
      MapString, // value <- value
      //Test,
      //For,
      If,
      Filter,
      Set,
      Macro,
      BinaryOp,
      UnaryOp,
      Block,
      Extends,
      Call, // Call to a generic expression
      FunctionCall, // Call to a global function
      Print,
      //Placeholder,
      //Node1,
      //Node2,
      Conditional,
      Concatenate, // Concatenation of strings in string interpolation
      ConstNull,
      ConstBool,
      ConstInt,
      ConstDouble,
      Text,
      Name,
      List,
      Use,
      //Root,
      //Args,
      Array,
      Hash,
      //BlockReference,
      Import,
      ImportAs,
      Include,
      Embed,
      IfTest,
      //Lookup,
      //Template
      //  Expression
      // Custom types (for internal use of tags/expressions only)
      Custom, // Any custom nodes used only internally
    };
    Q_ENUM_NS(Type)
    QString nodeTypeName(Type type);
    const char *nodeTypeNamePtr(Type type);

    class Generic
    {
    public:
      Generic(const Location &location): m_location(location) { }
      virtual ~Generic() { }
      virtual Type type() const { return Type::Generic; }
      QString toString(bool *isEmpty=nullptr);
      constexpr const Location &location() const { return m_location; }
      virtual bool hasExecute() const { return false; }
      virtual bool execute(RunThread &thread) const;
    protected:
      QList<QSharedPointer<Node::Generic> > m_children;
      virtual Accessor *accessor();
      Location m_location;
    };

    typedef QSharedPointer<Generic> Ptr;
    typedef QSharedPointer<const Generic> ConstPtr;

    //    Generic *nodePrint(Generic *expression, const Location &loc);
    //    Generic *newNodeConcatenateBinary(const QList<Generic *> &nodes, const Location &loc);
    //    Generic *newNodeEmpty(const Location &loc);
    //    Generic *newGetAttr(Generic *object, Generic *key, const Location &location);
    Ptr newMemberAccess(Ptr expr, const QString &name, const Location &location);
    Ptr newArrayGet(Ptr key, Ptr value, const Location &location);
    Ptr newMap(Ptr key, Ptr value, const Location &location);
    Ptr newMapString(const QString &key, Ptr expr, const Location &location);
    //    Generic *newTest(Generic *source, Generic *args, const QString &testName, const Location &location);
    //    Generic *newIf(Generic *cond, Generic *elseBody, const Location &location);
    //    Generic *newIfTest(Generic *cond, Generic *elseBody, const Location &location);
    //    Generic *newFor(Generic *keyTarget, Generic *valueTarget, Generic *seq, Generic *body, Generic *elseBody, const Location &location);
    Ptr newCall(Ptr expr, Ptr args, const Location &location); // Call to a generic expression
    Ptr newFunctionCall(const QString &filter, Ptr args, const Location &location); // Call to a global function
    Ptr newFilter(const QString &filter, Ptr args, Ptr expr, const Location &location);
    Ptr newSet(Ptr target, Ptr value, const Location &location);
    Ptr newUnaryOp(Ptr child, const Operator1 *op, const Location &location);
    Ptr newBinaryOp(Ptr childA, Ptr childB, const Operator2 *op, const Location &location);
    //    Generic *newMacroReference(Generic *module, Generic *name, const Location &location);
    Ptr newMacro(const QString &name, Ptr args, Ptr body, const Location &location);
    Ptr newBlock(const QString &name, Ptr body, const Location &location);
    Ptr newExtends(Ptr source, const Location &location);
    Ptr newPrint(Ptr child, const Location &location);
    Ptr newConditional(Ptr condition, Ptr valTrue, Ptr valFalse, const Location &location);
    //    Generic *newConditional2(Generic *condition, Generic *childFalse, const Location &location);
    //    Generic *newConstBool(bool value, const Location &location);
    Ptr newConcatenate(const QList<Ptr> &children, const Location &location);
    Ptr newConstNull(const Location &location);
    Ptr newConstInt(qint64 value, const Location &location);
    Ptr newConstDouble(double value, const Location &location);
    Ptr newConstBool(bool value, const Location &location);
    Ptr newText(const QString &text, const Location &location);
    Ptr newText(const Token &token);
    Ptr newName(const QString &name, const Location &location);
    //    //Generic *newRoot(const QList<Generic *> &children, const Location &location);
    Ptr newList(const QList<Ptr> &children, const Location &location);
    Ptr newArray(const QList<Ptr> &children, const Location &location);
    Ptr newHash(const QList<Ptr> &children, const Location &location);
    //    Generic *newBlockReference(const QString &ref, const Location &location);
    Ptr newImportAs(const QString &name, const QString &as, const Location &location);
    Ptr newImportAs(const QString &name, const Location &location);
    // Par can be a Text (import ... as Par) or a List (from ... import Par)
    Ptr newImport(Ptr source, Ptr par, const Location &location);
    Ptr newInclude(Ptr source, bool ignoreMissing, Ptr vars, bool only, const Location &location);
    Ptr newEmbed(Ptr source, bool ignoreMissing, Ptr vars, bool only, Ptr body, const Location &location);
    Ptr newUse(Ptr source, Ptr blocks, const Location &location);

    //    //Generic *newExpression(Generic *body, const Location &location);
    //    inline QList<Generic *> list(){return QList<Generic *>();}
    //    inline QList<Generic *> list(Generic *n1){return QList<Generic *>()<<n1;}
    //    inline QList<Generic *> list(Generic *n1, Generic *n2){return QList<Generic *>()<<n1<<n2;}
    //    inline QList<Generic *> list(Generic *n1, Generic *n2, Generic *n3){return QList<Generic *>()<<n1<<n2<<n3;}
  }
}
