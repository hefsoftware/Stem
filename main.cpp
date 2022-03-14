//#include "Engine/engine.h"
//#include "Engine/runthread.h"
//#include "Engine/Message/qdebug.h"
//#include "Parser/tokenstream.h"
//#include "Parser/Node/node.h"
#include "runtest.h"
#include <QFile>
#include <QDebug>
#include <QDir>
//using namespace QTwig;
int main(int, char *[])
{
  QDir testDir("../QTemplate4/TestSuite");
//  qDebug()<<QDir::currentPath();
//  runTest(testDir.filePath("03-SimpleExpr.twig"));
  runTest(testDir.filePath("XX.twig"));
//  QFile f("../QTemplate4/TestLex/test05.twig");
//  if(f.open(QIODevice::ReadOnly | QIODevice::Text))
//  {
//    Engine::Ptr engine=Engine::newEngine();
//    engine->initDefault();

////    auto tokens=engine.lex(QString::fromUtf8(f.readAll()), f.fileName(), new Messages::MsgQDebug);
//    auto node=engine->parse(QString::fromUtf8(f.readAll()), f.fileName(), new Messages::MsgQDebug);
//    if(node)
//    {
//      qDebug()<<node->toString().toUtf8().data();
//      auto thread=engine->newThread();
//      auto f=new QFile;
//      if(f->open(stdout, QIODevice::WriteOnly))
//        thread->execute(node, f, new Messages::MsgQDebug);
//    }
//    else
//      qDebug()<<"Parse failed";
////    lexer->tokenize();
//  }
//  else
//    qDebug()<<"Failed to open"<<f.fileName();
}
