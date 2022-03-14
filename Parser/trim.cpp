#include "trim.h"
#include <QString>
#include <QStringView>
using namespace QTwig;

int Trim::spacesAndNewlinesRight(QStringView str)
{
  int i=str.size();
  for(;i>0;i--)
  {
    QChar ch(str[i-1]);
    if(!ch.isSpace())
      break;
  }
  return i;
}

int Trim::spacesRight(QStringView str)
{
  int i=str.size();
  for(;i>0;i--)
  {
    QChar ch(str[i-1]);
    if(!(ch.isSpace() && ch!='\n' && ch!='\r'))
      break;
  }
  return i;
}

int Trim::spacesAndFirstNewLineRight(QStringView str)
{
  int i=str.size();
  for(;i>0;i--)
  {
    QChar ch(str[i-1]);
    if(ch=='\n')
    {
      i--;
      if(i>0 && str[i-1]=='\r')
        i--;
      break;
    }
    else if(ch=='\r')
    {
      i--;
      if(i>0 && str[i-1]=='\n')
        i--;
      break;
    }
    else if(!ch.isSpace())
      break;
  }
  return i;
}

int Trim::firstNewLineRight(QStringView str)
{
  int i=str.size();
  if(i>0)
  {
    QChar ch(str[i-1]);
    if(ch=='\n')
    {
      if(i>1 && str[i-2]=='\r')
        i-=2;
      else
        i--;
    }
    else if(ch=='\r')
    {
      if(i>1 && str[i-2]=='\n')
        i-=2;
      else
        i--;
    }
  }
  return i;

}

int Trim::spacesAndNewlinesLeft(QStringView str)
{
  int i=0, n=str.size();
  for(;i<n;i++)
  {
    QChar ch(str[i]);
    if(!ch.isSpace())
      break;
  }
  return i;
}

int Trim::spacesLeft(QStringView str)
{
  int i=0, n=str.size();
  for(;i<n;i++)
  {
    QChar ch(str[i]);
    if(!(ch.isSpace() && ch!='\n' && ch!='\r'))
      break;
  }
  return i;
}

int Trim::spacesAndFirstNewLineLeft(QStringView str)
{
  int i=0, n=str.size();
  for(;i<n;i++)
  {
    QChar ch(str[i]);
    if(ch=='\n')
    {
      i++;
      if(i<n && str[i]=='\r')
        i++;
      break;
    }
    else if(ch=='\r')
    {
      i++;
      if(i<n && str[i]=='\n')
        i++;
      break;
    }
    else if(!ch.isSpace())
      break;
  }
  return i;
}

int Trim::firstNewLineLeft(QStringView str)
{
  int i=0, n=str.size();
  if(n>0)
  {
    QChar ch(str[0]);
    if(ch=='\n')
    {
      if(n>1 && str[1]=='\r')
        i=2;
      else
        i=1;
    }
    else if(ch=='\r')
    {
      if(n>1 && str[1]=='\n')
        i=2;
      else
        i=1;
    }
  }
  return i;
}

void Trim::rtrimInline(const Function &f, QStringView &string)
{
  if(f)
    string=string.left(f(string));
}

int Trim::ltrimPos(const Trim::Function &f, QStringView str)
{
  return f?f(str):0;
}
