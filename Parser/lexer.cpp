#include "lexer.h"
#include "lexerenv.h"
#include "../Engine/message.h"
#include "tokenstream.h"
#define tr QString::fromLatin1
using namespace QTwig;

Lexer::Lexer(LexerEnv *lexerEnv, const QString &body, const QString &filename, Message *output): m_lexerEnv(lexerEnv), m_filename(filename), m_output(output)
{
  QString *bodyPtr=new QString(body);
  m_body=bodyPtr;
  m_bodyView=QStringView(*bodyPtr);
  m_cur.cursor=0;
  m_cur.lastNewLineCursor=0;
  m_cur.lineNo=1;
  m_end=body.length();
  m_tokens.clear();
  m_state=State::Data;
}

Lexer::~Lexer()
{
  delete ((QString *)m_body);
}

TokenStream Lexer::tokenize()
{
  bool ok=tokenizeInternal();
  return (ok?TokenStream(m_tokens):TokenStream());
}

bool Lexer::tokenizeInternal()
{
  for(m_cur.cursor=0;m_cur.cursor<m_end;)
  {
    switch(m_state)
    {
      case State::Data:
        lexData(); break;
      case State::Var:
        lexVar(); break;
      case State::String:
        lexString(); break;
      case State::Interpolation:
        lexInterpolation(); break;
      case State::Block:
        lexBlock(); break;
      default:
        failedLexing(tr("Unhandled state %1").arg((int)m_state));
    }
  }
  if(m_state!=State::Fail) // Final checks
  {
    if(m_brackets.size())
      outputError(tr("Unclosed %1").arg(m_brackets.last()), m_bracketsPos.last());
  }
  pushToken(Token::Type::Eof);
  return (m_state!=State::Fail);
}

void Lexer::lexBlock()
{
  skipAllSpaces();
  QRegularExpressionMatch m;
  if(skipIfMatch(m_lexerEnv->m_reBlockEnd,m))
  {
    skipSpaces(m_lexerEnv->trimLeft(m.captured(1)));
    pushToken(Token::Type::BlockEnd);
    popState();
  }
  else
  {
    lexExpression();
  }
}

void Lexer::lexString()
{
  QRegularExpressionMatch m;
  if(m_cur.cursor>=m_end)
  {
    failedLexing(tr("Unexpected end-of-file while parsing string"));
  }
  else if(skipIfMatch(interpolationStart()))
  {
    pushBracket(interpolationStart());
    pushToken(Token::Type::InterpolationStart);
    pushState(State::Interpolation);
  }
  else if(skipIfMatch(m_lexerEnv->m_reDqStringPart,m) && m.capturedLength()>0)
    pushToken(Token::Type::String,m.captured());
  else if(m_bodyView.at(m_cur.cursor)==dqStringDelim())
  {
    if(popBracket(dqStringDelim(),dqStringDelim()))
    {
      popState();
      m_cur.cursor++;
    }
  }
  else{
    failedLexing(tr("Unexpected character %1").arg(m_bodyView.mid(m_cur.cursor,1)));
  }
}

void Lexer::lexInterpolation()
{
  skipAllSpaces();
  if(skipIfMatch(interpolationEnd()) && m_brackets.last()==interpolationStart())
  {
    if(popBracket(interpolationStart(),interpolationEnd()))
    {
      pushToken(Token::Type::InterpolationEnd);
      popState();
    }
  }
  else
    lexExpression();
}

void Lexer::lexVar()
{
  bool lexExpr=true;
  if(m_brackets.isEmpty())
  {
    QRegularExpressionMatch m;
    if(skipIfMatch(m_lexerEnv->m_reLexVarEnd, m, true))
    {
      pushToken(Token::Type::VarEnd);
      popState();
      skipSpaces(m_lexerEnv->trimLeft(m.captured(1)));
      lexExpr=false;
    }
  }
  if(lexExpr)
    lexExpression();
}

void Lexer::lexExpression()
{
  if(!skipAllSpaces())
  {
    failedLexing(tr("Missing end of %1").arg(State::Block == m_state ? QString("block") : QString("variable")), m_statesStart.last());
    return;
  }
  QRegularExpressionMatch m;
  int i;
  if(m_bodyView.mid(m_cur.cursor).startsWith(m_lexerEnv->m_arrowOp))
  {
    pushToken(Token::Type::Arrow, m_lexerEnv->m_arrowOp);
    moveCursor(m_lexerEnv->m_arrowOp);
  }
  else if(skipIfMatch(m_lexerEnv->m_reOperator,m))
    pushToken(Token::Type::Operator, m.captured().replace(m_lexerEnv->m_reSpaces, " ")); // Operators before names (for string operators)
  else if(skipIfMatch(m_lexerEnv->m_reNumber,m)) // Number after operators (for handling expressions like "4 -3" as "number operator- number")
    pushToken(Token::Type::Number, m.captured());
  else if(skipIfMatch(m_lexerEnv->m_reName,m))
    pushToken(Token::Type::Name, m.captured());
  else if(skipIfMatch(m_lexerEnv->m_reString,m))
    pushToken(Token::Type::String,m.capturedView().mid(1,m.captured().size()-2).toString());
  else if((i=m_lexerEnv->m_punctuation.indexOf(m_bodyView.at(m_cur.cursor)))>=0)
  {
    pushToken(Token::Type::Punctuation, m_lexerEnv->m_punctuation[i]);
    m_cur.cursor++;
  }
  else if((i=m_lexerEnv->m_punctuationBrackets.indexOf(m_bodyView.at(m_cur.cursor)))>=0)
  {
    QString br(m_bodyView.at(m_cur.cursor));
    if(i%2) // Odd-numbered: closing bracket
    {
      if(!popBracket(QString(m_lexerEnv->m_punctuationBrackets[i-1]),br))
        return;
    }
    else // Even-numbered: opening bracket
      pushBracket(br);
    m_cur.cursor++;
    pushToken(Token::Type::Punctuation,br);
  }
  else if(m_bodyView.at(m_cur.cursor)==dqStringDelim())
  {
    pushBracket(QString(dqStringDelim()));
    pushState(State::String);
    m_cur.cursor++;
  }
  else
  {
    failedLexing(tr("Unexpected character in lexExpression"));
  }
}



void Lexer::lexData()
{
  // Find the first token after current one
  QRegularExpressionMatch m=m_lexerEnv->m_reLexTokenStart.match(m_bodyView, m_cur.cursor);
  if(!m.hasMatch())
  {
    if(m_cur.cursor<m_end)
      pushToken(Token::Type::Text, m_bodyView.mid(m_cur.cursor).toString());
    finishParsing();
    return;
  }
  int position=m.capturedStart();
  auto text=m_bodyView.mid(m_cur.cursor, position-m_cur.cursor);
  Trim::rtrimInline(m_lexerEnv->trimRight(m.captured(2)), text);
  if(!text.isEmpty())
    pushToken(Token::Type::Text, text.toString());
  moveCursor(m);
  if(m.capturedView(1)==m_lexerEnv->m_tagCommentStart)
    lexComment();
  else if(m.capturedView(1)==m_lexerEnv->m_tagVariableStart)
  {
    pushToken(Token::Type::VarStart);
    pushState(State::Var);
  }
  else if(m.capturedView(1)==m_lexerEnv->m_tagBlockStart)
  {
    auto p=currentPosition();
    skipAllSpaces();
    if(skipIfMatch(m_lexerEnv->m_reName, m))
    {
      QString tag=m.capturedView(0).toString().toLower();
      if(tag==m_lexerEnv->m_blockLine)
      {
        QRegularExpressionMatch m, n;
        if(skipAllSpaces() && skipIfMatch(m_lexerEnv->m_reNumber,m) && skipAllSpaces() && skipIfMatch(m_lexerEnv->m_reBlockEnd))
        {
          m_cur.lineNo=m.captured().toInt();
          m_cur.lastNewLineCursor=m_cur.cursor;
          skipSpaces(m_lexerEnv->trimLeft(n.captured(1)));
        }
      }
      else if(tag==m_lexerEnv->m_blockRaw)
      {
        skipAllSpaces();
        QRegularExpressionMatch m;
        QString verbatimEnd=QString::fromLatin1("endverbatim");
        if(skipIfMatch(m_lexerEnv->m_reName, m))
        {
          verbatimEnd=m.captured();
          skipAllSpaces();
        }
        if(skipIfMatch(m_lexerEnv->m_reBlockEnd, m))
        {
          lexRawData(verbatimEnd);
        }
        else
          failedLexing(tr("verbatim block invalid syntax. Expected \"%1\"").arg(m_lexerEnv->m_tagBlockEnd));
      }
      else
      {
        pushToken(p, Token::Type::BlockStart);
        pushToken(Token::Type::Name, tag);
        pushState(State::Block);
      }
    }
    else
      failedLexing(tr("Expected an identifier after a block start"));
  }
}

void Lexer::lexRawData(const QString &expectedEnd)
{
  PositionTracking startPos=currentPosition();
  while(true)
  {
    auto m=m_lexerEnv->m_reBlockStart.match(m_bodyView, m_cur.cursor);
    if(!m.hasMatch())
    {
      failedLexing(tr("Tag %1 end not found").arg(m_lexerEnv->m_blockRaw),startPos);
      return;
    }
    else
    {
      moveCursor(m);
      skipAllSpaces();
      if(skipIfMatch(expectedEnd,false))
      {
        QRegularExpressionMatch n;
        if(skipIfMatch(m_lexerEnv->m_reBlockEnd,n,true))
        {
          pushToken(Token::Type::Text, m_bodyView.mid(startPos.cursor, m.capturedStart()-startPos.cursor).toString());
          skipSpaces(m_lexerEnv->trimLeft(n.captured(1)));
          break;
        }
      }
    }
  }
}

void Lexer::lexComment()
{
  int nestLevel=1;
  PositionTracking p=currentPosition();
  QRegularExpressionMatch m;
  for(int pos=m_cur.cursor;;)
  {
    m=m_lexerEnv->m_reLexCommentStartOrEnd.match(m_bodyView, pos);
    if(!m.hasMatch())
    {
      failedLexing(tr("Unterminated comment comment"), p);
      return;
    }
    else if(m.capturedLength(2))
    {
      nestLevel++;
    }
    else
    {
      nestLevel--;
      if(nestLevel<=0)
      {
        break;
      }
    }
    pos=m.capturedEnd()+1;
  }
  skipSpaces(m.capturedEnd(), m_lexerEnv->trimLeft(m.captured(1)));
}

void Lexer::pushToken(const PositionTracking &position, Token::Type type, QString value)
{
  m_tokens.append(Token(type,Location(m_filename, position.lineNo, position.cursor-position.lastNewLineCursor),value));
}


void Lexer::finishParsing()
{
  m_cur.cursor=m_end;
}

void Lexer::failedLexing(const QString &msg)
{
  failedLexing(msg, currentPosition());
}

void Lexer::failedLexing(const QString &msg, const PositionTracking &pos)
{
  outputError(msg, pos);
  m_cur.cursor=m_end;
  m_state=State::Fail;
}

void Lexer::outputError(const QString &error, const PositionTracking &pos)
{
  m_output->error(error, Location(m_filename, pos.lineNo, pos.cursor-pos.lastNewLineCursor+1));
}

QChar Lexer::dqStringDelim()
{
  return m_lexerEnv->m_DQStringDelim;
}

QString Lexer::interpolationStart()
{
  return m_lexerEnv->m_interpolationStart;
}

QString Lexer::interpolationEnd()
{
  return m_lexerEnv->m_interpolationEnd;
}

void Lexer::moveCursor(int pos)
{
  Q_ASSERT(pos>=m_cur.cursor);
  if(pos>m_cur.cursor)
  {
    int numNewLine=0;
    int lastOffset=-1;
    auto text=m_bodyView.mid(m_cur.cursor,pos-m_cur.cursor);
    for(int offset=0; ;lastOffset=offset, numNewLine++)
    {
      offset=text.indexOf(m_lexerEnv->m_reNewLine, offset+1);
      if(offset<0)
        break;
    }
    if(lastOffset>=0)
      m_cur.lastNewLineCursor=m_cur.cursor+lastOffset;
    m_cur.lineNo+=numNewLine;
    m_cur.cursor=pos;
  }
}

void Lexer::moveCursor(const QRegularExpressionMatch &match)
{
  Q_ASSERT(match.hasMatch());
  moveCursor(match.capturedEnd());
}
void Lexer::moveCursor(const QString &str)
{
  moveCursor(m_cur.cursor+str.size());
}
bool Lexer::skipSpaces(int cursor, Trim::Function trim)
{
  Q_ASSERT(cursor>=m_cur.cursor);
  int pos=cursor+Trim::ltrimPos(trim, m_bodyView.mid(cursor));
  if(pos>m_cur.cursor)
    moveCursor(pos);
  return (pos<m_end);
}

bool Lexer::skipAllSpaces()
{
  return skipSpaces(m_cur.cursor, Trim::spacesAndNewlinesLeft);
}

void Lexer::pushState(Lexer::State state)
{
  m_states.append(m_state);
  m_statesStart.append(currentPosition());
  m_state=state;
}

void Lexer::popState()
{
  if(m_states.length()==0)
  {
    outputError(tr("Cannot pop state without a previous state"), currentPosition());
  }
  else
  {
    m_statesStart.takeLast();
    m_state=m_states.takeLast();
  }
}

void Lexer::pushBracket(const QString &bracket)
{
  m_brackets.append(bracket);
  m_bracketsPos.append(currentPosition());
}

bool Lexer::popBracket(const QString &expected, const QString &current)
{
  bool ret=true;
  if(m_brackets.size()<=0)
  {
    failedLexing(tr("Unexpected %1").arg(current));
    ret=false;
  }
  else if(m_brackets.last()!=expected)
  {
    failedLexing(tr("%1 does not match opened %2").arg(current, m_brackets.last()));
    ret=false;
  }
  else
  {
    m_brackets.removeLast();
    m_bracketsPos.removeLast();
  }
  return ret;
}

bool Lexer::skipIfMatch(const QRegularExpression &expression, bool doSkipSpaces)
{
  bool ret=false;
  int pos=m_cur.cursor;
  if(doSkipSpaces)
    pos=m_cur.cursor+Trim::ltrimPos(Trim::spacesAndFirstNewLineLeft, m_bodyView.mid(m_cur.cursor));
  QRegularExpressionMatch m=expression.match(m_bodyView, pos, QRegularExpression::NormalMatch, QRegularExpression::AnchorAtOffsetMatchOption);
  if(m.hasMatch())
  {
    moveCursor(m.capturedEnd());
    ret=true;
  }
  return ret;
}

bool Lexer::skipIfMatch(const QRegularExpression &expression, QRegularExpressionMatch &match, bool doSkipSpaces)
{
  bool ret=false;
  int pos=m_cur.cursor;
  if(doSkipSpaces)
    pos=m_cur.cursor+Trim::ltrimPos(Trim::spacesAndFirstNewLineLeft, m_bodyView.mid(m_cur.cursor));
  match=expression.match(m_bodyView, pos, QRegularExpression::NormalMatch, QRegularExpression::AnchorAtOffsetMatchOption);
  if(match.hasMatch())
  {
    moveCursor(match.capturedEnd());
    ret=true;
  }
  return ret;
}

bool Lexer::skipIfMatch(QStringView string, bool doSkipSpaces)
{
  bool ret=false;
  int pos=m_cur.cursor;
  if(doSkipSpaces)
    pos=m_cur.cursor+Trim::ltrimPos(Trim::spacesAndFirstNewLineLeft, m_bodyView.mid(m_cur.cursor));
  if(m_bodyView.mid(pos, string.length())==string)
  {
    moveCursor(pos+string.length());
    ret=true;
  }
  return ret;
}
