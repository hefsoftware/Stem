#pragma once

#include <functional>
class QStringView;
class QString;
namespace QTwig
{
  namespace Trim
  {
    // Function returning where str should be trimmed (left or right depending on context) to skip spaces
    typedef std::function<int (QStringView str)> Function;
    //typedef int (*Function)(const QStringRef &str);
    int spacesAndNewlinesRight(QStringView str);
    int spacesRight(QStringView str);
    int spacesAndFirstNewLineRight(QStringView str);
    int firstNewLineRight(QStringView str);

    int spacesAndNewlinesLeft(QStringView str);
    int spacesLeft(QStringView str);
    int spacesAndFirstNewLineLeft(QStringView str);
    int firstNewLineLeft(QStringView str);

    int ltrimPos(const Function &f, QStringView str);
    void rtrimInline(const Function &f, QStringView &string);
  };
}
