#include <Qt>
#include "lexerenv.h"
#include "lexer.h"
#include "../Engine/engine.h"
#include <QDebug>
#define tr QObject::tr

using namespace QTwig;

LexerEnv::LexerEnv(Engine *engine): m_engine(engine)
{
  m_tagCommentStart=QString::fromLatin1("{#"); m_tagCommentEnd=QString::fromLatin1("#}");
  m_tagBlockStart=QString::fromLatin1("{%"); m_tagBlockEnd=QString::fromLatin1("%}");
  m_tagVariableStart=QString::fromLatin1("{{"); m_tagVariableEnd=QString::fromLatin1("}}");
  m_interpolationStart=QString::fromLatin1("#{"); m_interpolationEnd=QString::fromLatin1("}");

//  m_defaultTrimLeft=Trim::spacesAndFirstNewLineLeft;
  m_trimQualifiersLeft.insert("-", Trim::spacesAndNewlinesLeft);
  m_trimQualifiersRight.insert("-", Trim::spacesAndNewlinesRight);
  m_trimQualifiersLeft.insert("~", Trim::spacesLeft);
  m_trimQualifiersRight.insert("~", Trim::spacesRight);

  m_arrowOp=QString::fromLatin1("=>");
  m_newLineChars=QSet<QChar>{'\n', '\r'};
  m_punctuation=QLatin1String("?:.,|");
  m_punctuationBrackets=QLatin1String("()[]{}");
  m_DQStringDelim=QChar('\"');
  resetRegexps();
}

Lexer *LexerEnv::newInstance(const QString &body, const QString &filename, Message *output)
{
  return new Lexer(this, body, filename, output);
}

void LexerEnv::sortByLength(QStringList &stringList)
{
  std::sort(stringList.begin(),stringList.end(), [](const QString &a, const QString &b){return a.length()>b.length();});
}
QRegularExpression LexerEnv::listRe(QStringList &list)
{
  sortByLength(list);
  for(int i=list.size()-1;i>=0;i--)
    list[i]=QRegularExpression::escape(list[i]);
  return QRegularExpression(QString("(?:%1)").arg(list.join('|')));
}
void LexerEnv::resetRegexps()
{
  m_reSpaces=QRegularExpression("\\s+");
  m_reNewLine=listRe(QStringList(m_newLineChars.cbegin(), m_newLineChars.cend()));
  auto esc=static_cast<QString (*)(const QString &)>(QRegularExpression::escape);
  QString trimmingReLeft(spaceTrimSpecToRe(m_trimQualifiersLeft));
  QString trimmingReRight(spaceTrimSpecToRe(m_trimQualifiersRight));

  // A token start is either a variable, block and comment start followed by an optional right trim specification
  m_reLexTokenStart=QRegularExpression(QString::fromLatin1("(%1|%2|%3)(%4)?").arg(esc(m_tagVariableStart),esc(m_tagBlockStart),esc(m_tagCommentStart), trimmingReRight));
  m_reLexCommentStartOrEnd=QRegularExpression(QString::fromLatin1("(?:(%3)?%2)|(%1)").arg(esc(m_tagCommentStart),esc(m_tagCommentEnd), trimmingReLeft));

  m_reInteger=QRegularExpression("[0-9]+");

  // A variable end optionally preceded by a left trim specification
  m_reLexVarEnd=QRegularExpression(QString::fromLatin1("(%2)?%1").arg(esc(m_tagVariableEnd), trimmingReLeft));
  // A identifier name
  m_reName=QRegularExpression(QString::fromLatin1("[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*"));
  // A number literal
  m_reNumber=QRegularExpression(m_engine->numberRegExp());
  // A string literal
  m_reString=QRegularExpression(m_engine->stringRegExp());
  // A "rich" string literal
  m_reDqStringPart=QRegularExpression(QString::fromLatin1("([^%1%2\\\\]*(?:(\\\\.|%2(?!%3))[^%1%2\\\\]*)*)").arg(esc(QString(m_DQStringDelim)),esc(m_interpolationStart.left(1)),esc(m_interpolationStart.mid(1))));
  // Operators
  m_reOperator=getOperatorRegex();

  m_reBlockStart=QRegularExpression(QString::fromLatin1("%1(%2)?").arg(esc(m_tagBlockStart), trimmingReRight));
  m_reBlockEnd=QRegularExpression(QString::fromLatin1("(%2)?%1").arg(esc(m_tagBlockEnd), trimmingReLeft));

  m_blockRaw=QString::fromLatin1("verbatim");
  m_blockLine=QString::fromLatin1("line");
}

QString LexerEnv::spaceTrimSpecToRe(const QMap<QString, Trim::Function> &qualifiers)
{
  QStringList speclist;
  for(auto it=qualifiers.cbegin(); it!=qualifiers.cend(); it++)
    speclist.append(QRegularExpression::escape(it.key()));
  sortByLength(speclist);
  return (QString("(?:%1)").arg(speclist.join('|')));
}


QRegularExpression LexerEnv::getOperatorRegex()
{
  QSet<QString> ops{"="};
  for(auto it=m_engine->unaryOps().cbegin(); it!=m_engine->unaryOps().cend(); it++)
    ops.insert(it.key());
  for(auto it=m_engine->binaryOps().cbegin(); it!=m_engine->binaryOps().cend(); it++)
    ops.insert(it.key());

  QStringList opList(ops.begin(), ops.end());
  sortByLength(opList);

  QStringList regexs;
  for(auto it=opList.begin();it!=opList.end();it++)
  {
    QString curRegEx=*it;
    QString curRegExEscaped=QRegularExpression::escape(curRegEx).replace("\\ ",m_reSpaces.pattern());
    if(curRegExEscaped.at(curRegExEscaped.size()-1).isLetter())
      curRegExEscaped+=QString("(?=%1)").arg(m_reSpaces.pattern());
    regexs.append(curRegExEscaped);
  }
  QRegularExpression ret(regexs.join("|"),QRegularExpression::CaseInsensitiveOption);
  return ret;
}

Trim::Function LexerEnv::trimLeft(const QString &spec)
{
  return m_trimQualifiersLeft.value(spec, m_defaultTrimLeft);
}

Trim::Function LexerEnv::trimRight(const QString &spec)
{
  return m_trimQualifiersRight.value(spec, m_defaultTrimRight);
}

const QRegularExpression &LexerEnv::reName() const
{
  return m_reName;
}

////Lexer::TrimFunction Lexer::getTrimFunction(Lexer::Trim trimType)
////{
////  switch(trimType)
////  {
////    default:
////    case Trim::None:
////      return [](QChar ch){Q_UNUSED(ch); return false;};
////    case Trim::Line:
////      return isSpace;
////    case Trim::Full:
////      return isSpaceOrNewLine;
////  }
////}

















//void Lexer::moveCursor(int pos)
//{
//  Q_ASSERT(pos>=m_cursor);
//  if(pos>m_cursor)
//  {
//    int numNewLine;
//    numNewLine=m_source.midRef(m_cursor,pos-m_cursor).count(m_newLineChar);
//    if(numNewLine)
//      m_lastNewLineCursor=m_cursor+m_source.midRef(m_cursor,pos-m_cursor).lastIndexOf(m_newLineChar);
//    m_lineNo+=numNewLine;
//    m_cursor=pos;
//  }
//}

//void Lexer::moveCursor(const QRegularExpressionMatch &match)
//{
//  Q_ASSERT(match.hasMatch());
//  moveCursor(match.capturedEnd());
//}
//void Lexer::moveCursor(const QString &str)
//{
//  moveCursor(m_cursor+str.size());
//}


//bool Lexer::cursorMatch(const QRegularExpression &expression, QRegularExpressionMatch &result, bool doSkipSpaces)
//{
//  if(doSkipSpaces)
//    skipAllSpaces();
//  result=expression.match(m_code, m_cursor,QRegularExpression::NormalMatch, QRegularExpression::AnchoredMatchOption);
//  if(result.hasMatch())
//    moveCursor(result);
//  return result.hasMatch();
//}

//bool Lexer::cursorMatch(const QString &string, bool doSkipSpaces)
//{
//  if(doSkipSpaces)
//    skipAllSpaces();
//  bool ret=false;
//  if(m_code.midRef(m_cursor,string.size())==string)
//  {
//     moveCursor(string);
//     ret=true;
//  }
//  return ret;
//}
//QString Lexer::normalizeString(const QString &start)
//{
//  return start;
//}

//QString Lexer::captureTextBeforeTag(int cursorStart, int cursorEnd, const QStringRef &trimType)
//{
//  Q_ASSERT(trimType.isEmpty() || m_trimQualifiersLeft.contains(trimType.toString()));
//  QString ret=m_code.mid(cursorStart, cursorEnd-cursorStart);
//  Trim::Function trim=m_trimQualifiersLeft.value(trimType.toString(), m_defaultTrimLeft);
//  Trim::rtrimInline(trim, ret);
//  return ret;
//}



//bool Lexer::skipSpaces(const QStringRef &spaceType)
//{
//  Q_ASSERT(spaceType.isEmpty() || m_trimQualifiersRight.contains(spaceType.toString()));
//  Trim::Function trim=m_trimQualifiersRight.value(spaceType.toString(), m_defaultTrimRight);
//  return skipSpaces(trim);
//}

//bool Lexer::skipAllSpaces()
//{
//  int pos=m_cursor+Trim::ltrimPos(Trim::spacesAndNewlinesLeft, m_code.midRef(m_cursor));
//  moveCursor(pos);
//  return (pos<=m_end);
//}

//bool Lexer::checkEof(const QString &reason)
//{
//  bool ret=true;
//  if(m_cursor>=m_end)
//  {
//    if(reason.isEmpty())
//      failedLexing(tr("Unexpected end of file"));
//    else
//      failedLexing(tr("Unexpected end of file (%1)").arg(reason));
//    ret=false;
//  }
//  return ret;
//}

//void Lexer::failedMatch()
//{
//  if(checkEof())
//    failedUnexpected();
//}

//void Lexer::failedUnexpected()
//{
//  const int printedLen=5;
//  failedLexing(tr("Unexpected \'%1\'").arg(m_end-m_cursor<printedLen?m_code.mid(m_cursor):(m_code.mid(m_cursor,printedLen)+QString::fromLatin1("..."))));
//}

//Location Lexer::currentLocation()
//{
//  return Location(m_fileName,m_lineNo, m_cursor-m_lastNewLineCursor);
//}




//void Lexer::outputError(const QString &error)
//{
//  outputError(error, currentLocation());
//}

//void Lexer::outputError(const QString &error, const Location &location)
//{
//  Q_ASSERT(m_output);
//  if(m_output)
//    m_output->error(error, location);
//}





//QString Lexer::unescapeC(const QString &src)
//{
//  return src;
//}






