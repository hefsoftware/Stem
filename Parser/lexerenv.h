#pragma once
#include <QString>
#include <QRegularExpression>
#include <QList>
#include "trim.h"
#include "token.h"

namespace QTwig
{
    class TokenStream;
    class Engine;
    class Message;

    //#include <QDebug>
    class Lexer;
    class LexerEnv
    {
        friend class ExpressionParser;
        friend class Lexer;
    public:
        LexerEnv(Engine *engine);
        Lexer *newInstance(const QString &body, const QString &filename, Message *output);
        const QRegularExpression &reName() const;

    protected:
        void resetRegexps();
        /// @brief Inline sorts a list of strings putting first the longest one. Used to generate meaningful regexp when strings may have a common prefix
        static void sortByLength(QStringList &stringList);
        /// @brief Converts a list of trim-strings to a regular expression string matching one of them or none
        static QString spaceTrimSpecToRe(const QMap<QString, Trim::Function> &qualifiers);
        /// @brief Regular expression matching any of the string in list
        static QRegularExpression listRe(QStringList &list);
        /// @brief Regular expression matching any of the string in list
        static inline QRegularExpression listRe(QStringList &&list) { return listRe(list); }
        /// @brief Returns a regexp that matches any operator
        QRegularExpression getOperatorRegex();

        Trim::Function trimLeft(const QString &spec);
        Trim::Function trimRight(const QString &spec);

        QString m_tagCommentStart, m_tagCommentEnd; // String defining the begin and end of a comment (default {# ... #} )
        QString m_tagBlockStart, m_tagBlockEnd; // String defining the begin and end of a block (default {% ... %} )
        QString m_tagVariableStart, m_tagVariableEnd; // String defining the begin and end of a variable (default {{ ... }} )
        QString m_interpolationStart, m_interpolationEnd;  // String defining the begin and end of an interpolation (default #{ ... } )
        QString m_arrowOp;
        QString m_blockRaw;
        QString m_blockLine;

        Trim::Function m_defaultTrimLeft; // Default trim on the left
        Trim::Function m_defaultTrimRight; // Default trim on the right
        QMap<QString, Trim::Function> m_trimQualifiersLeft; // Available left trims and how to handle them ( default: "~" spaces "-" spaces and newlines )
        QMap<QString, Trim::Function> m_trimQualifiersRight;  // Available right trims and how to handle them ( default: "~" spaces "-" spaces and newlines )

        QRegularExpression m_reSpaces;
        QRegularExpression m_reNewLine;
        QRegularExpression m_reLexTokenStart;
        QRegularExpression m_reLexCommentStartOrEnd;
        QRegularExpression m_reLexVarEnd;
        QRegularExpression m_reOperator;
        QRegularExpression m_reBlockStart;
        QRegularExpression m_reBlockEnd;

        QRegularExpression m_reName;
        QRegularExpression m_reInteger;
        QRegularExpression m_reNumber;
        QRegularExpression m_reString;
        QRegularExpression m_reDqStringPart;

        QString m_whitespaceLineChars;
        QSet<QChar> m_newLineChars; // Characters that should be considered a new line
        QChar m_DQStringDelim;
        QString m_punctuation;
        QString m_punctuationBrackets;

        Engine *m_engine;
    };

}
