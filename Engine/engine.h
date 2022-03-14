#pragma once
#include <QSharedPointer>
#include <QEnableSharedFromThis>
#include <QMap>
namespace QTwig
{
  class LexerEnv;
  class Lexer;
  class Parser;
  class Template;
  class Message;
  class Operator;
  class Operator1;
  class Operator2;
  class TokenStream;
  class Location;
  class RunThread;
  class Filesystem;
  class Value;
  namespace Node
  {
    class Generic;
    class BinaryOp;
    class UnaryOp;
  }
  class Engine: public QEnableSharedFromThis<Engine>
  {
    friend class LexerEnv;
    explicit Engine();
  public:
    typedef QSharedPointer<Engine> Ptr;
    typedef std::function<QSharedPointer<Node::Generic>(Parser &parser)> TagParserFunction;
    static Ptr newEngine();
    static Ptr newEngineDefault();
    QSharedPointer<RunThread> newThread();
//    inline QSharedPointer<Template> load(const QString &filename, Message *output=nullptr) { return load(filename, QString(), output); }
//    QSharedPointer<Template> load(const QString &filename, const QString &reference, Message *output=nullptr);

    TokenStream lex(const QString &body, const QString &filename, Message *output);
    QSharedPointer<Node::Generic> parse(const QString &body, const QString &filename, Message *output);
    QSharedPointer<Node::Generic> parse(const TokenStream &stream, Message *output);
    QSharedPointer<Node::Generic> parse(const QString &filename, Message *output);
    /* if name is a constant expression (e.g. 'true', 'false') returns a node that creates that value. Otherwise returns NULL
    */
    QSharedPointer<Node::Generic> getConstantExpression(const QString &text, const Location &location);
//    inline QList<Operator1 *> unaryOps() { return m_unaryOps; }
//    inline QList<Operator2 *> binaryOps() { return m_binaryOps; }
    inline const QMap<QString, Operator1 *> &unaryOps() { return m_unaryOpMap; }
    inline const QMap<QString, Operator2 *> &binaryOps() { return m_binaryOpMap; }
    const Operator1 *getUnaryOp(const QString &text);
    const Operator2 *getBinaryOp(const QString &text);
    Value getFilter(const QString &name);
    Value getFunction(const QString &name);
    /* Initialization functions */
    bool initAddUnaryOp(Operator1 *op);
    bool initAddBinaryOp(Operator2 *op);
    // A filter is a function that returns a function mapping a value to a value
    bool initAddFilter(const QString &key, Value filter);
    // Add a function (function will be always available)
    bool initAddFunction(const QString &key, Value filter);
    // Add a tag parser
    bool initAddTag(const QString &tag, TagParserFunction parser);

    bool initAddDefaultTags();
    bool initAddDefaultFunctions();
    bool initAddDefaultFilters();
    bool initAddDefaultUnaryOps();
    bool initAddDefaultBinaryOps();
    bool initDefault(); /// @brief Init everything to the default

    QSharedPointer<Node::Generic> parseNumber(const QString &expression, const Location &location);

    TagParserFunction getTag(const QString &tag);
    bool setFilesystem(Filesystem *newFilesystem);

    bool printValue(RunThread *thread, Value val);
    constexpr LexerEnv *lexerEnv() { return m_lexer; }
  protected:
//    std::function<Value(RunThread *thread, const Node::UnaryOp *op)> generateNumOp1(const std::function<Value (QVariant)> &opInt, const std::function<Value (QVariant)> &opFloat);
//    std::function<Value(RunThread *thread, const Node::BinaryOp *op)> generateNumOp2(const std::function<Value (QVariant, QVariant)> &opInt, const std::function<Value (QVariant, QVariant)> &opFloat);

    Lexer *lexer(const QString &body, const QString &filename, Message *output);
    void endInitialization() { m_initializing=false; }
    // Functions used by lexer to access settings
    inline QString numberRegExp() { return m_numberRegExp; }
    inline QString stringRegExp() { return m_stringRegExp; }

    bool m_initializing;
    QList<Operator1 *> m_unaryOps;
    QList<Operator2 *> m_binaryOps;
    QMap<QString, Operator2 *> m_binaryOpMap;
    QMap<QString, Operator1 *> m_unaryOpMap;
    QMap<QString, Value> m_filters;
    QMap<QString, Value> m_functions;
    QMap<QString, TagParserFunction> m_tags;
    QString m_numberRegExp; // Reg expression for recognizing a number literal
    QString m_stringRegExp; // Reg expression for recognizing a string literal
    LexerEnv *m_lexer;
    Filesystem *m_filesystem;
  };
}
