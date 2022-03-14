#pragma once
#include <QList>
#include <QString>
#include "token.h"
#include "trim.h"
namespace QTwig
{
  class LexerEnv;
  class Location;
  class Message;
  class TokenStream;
  class Lexer
  {
    friend class LexerEnv;
  public:
    TokenStream tokenize();
    ~Lexer();
  protected:
    bool tokenizeInternal();
    enum class State{
      Data,
      Block,
      Var,
      String,
      Interpolation,
      Fail
    };
    // Sub-lexer
    void lexData();
    void lexComment();
    void lexVar();
    void lexBlock();
    void lexExpression();
    void lexString();
    void lexInterpolation();
    void lexRawData(const QString &expectedEnd);

    struct PositionTracking
    {
      int cursor;
      int lineNo;
      int lastNewLineCursor;
    };
    inline PositionTracking currentPosition() { return m_cur; }

    void outputError(const QString &error, const PositionTracking &pos);

    void finishParsing();
    void failedLexing(const QString &msg);
    void failedLexing(const QString &msg, const PositionTracking &pos);

    void moveCursor(int pos);

    QChar dqStringDelim();
    QString interpolationStart();
    QString interpolationEnd();

    Lexer(LexerEnv *lexerEnv, const QString &body, const QString &filename, Message *output);
    Lexer(const Lexer &lexer)=delete;
    /// @brief Moves the cursor skipping spaces after current cursor
    /// @return True if end is not reached
    inline bool skipSpaces(Trim::Function trim) { return skipSpaces(m_cur.cursor, trim); }
    /// @brief Move the cursor to a position that is after all the spaces selected by trim starting from given pos
    /// @return True if end is not reached
    bool skipSpaces(int pos, Trim::Function trim);
    /// @brief Skips all spaces after current cursor
    /// @return True if end is not reached
    bool skipAllSpaces();

    /// @brief Skips a regular expression after optionally skipping the spaces before that
    /// @return True if a match was found
    bool skipIfMatch(const QRegularExpression &expression, bool doSkipSpaces=false);
    /// @brief Skips a regular expression after optionally skipping the spaces before that
    /// @return True if a match was found
    bool skipIfMatch(const QRegularExpression &expression, QRegularExpressionMatch &match, bool doSkipSpaces=false);
    /// @brief Skips a constant string after optionally skipping the spaces before that if a match is found
    /// @return True if a match was found
    bool skipIfMatch(QStringView string, bool doSkipSpaces=false);

    void moveCursor(const QRegularExpressionMatch &match);
    void moveCursor(const QString &str);
    void pushToken(const PositionTracking &position, Token::Type type, QString value="");
    inline void pushToken(Token::Type type, QString value="") { return pushToken(currentPosition(), type, value); }
    void pushState(State state);
    void popState();

    void pushBracket(const QString &bracket);
    bool popBracket(const QString &expected, const QString &current);

    LexerEnv *m_lexerEnv;
    /**
     * @note The variable actually is a pointer to a QString, created by constructor and deleted by destructor. It is handled in this way to prevent any change to the string, which only way of access is and must be m_bodyView
     */
    void *m_body;
    QStringView m_bodyView;
    QString m_filename;
    Message *m_output;

    PositionTracking m_cur;
    int m_end; /// @brief End position (length)

    State m_state;
    QList<State> m_states;
    QList<PositionTracking> m_statesStart;

    QList<Token> m_tokens;


    QList<QString> m_brackets;
    QList<PositionTracking> m_bracketsPos;
  };
}
