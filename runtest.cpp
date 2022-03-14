#include "runtest.h"
#include "Engine/engine.h"
#include "Engine/runthread.h"
#include "Engine/value.h"
#include "Engine/Filesystem/filesystemmap.h"
#include "Engine/Message/qdebug.h"
#include <QBuffer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QFileInfo>
#include <QDir>
using namespace QTwig;
#include "Engine/Helper/scopesstack.h"
// Test for the stack scope set functions. Should add get too.
static void runStackTest(RunThread *thread)
{
  thread->set("Var", Value(3));
  thread->set("Foo", Value(4));
  {
    RunThread::PushScope scope(thread, RunThread::Visibility::Normal);
    thread->set("Foo", Value(5));
    thread->set("Bar", Value(6));
    qDebug()<<"A0"<<thread->extractCurrentScope(); // Expects Var: 3, Foo: 5, Bar: 6
  }
  qDebug()<<"A1"<<thread->extractCurrentScope(); // Expects Var: 3, Foo: 5
  {
    RunThread::PushScope scope(thread, RunThread::Visibility::SetOpaque);
    thread->set("Foo", Value(7));
    thread->set("Bar", Value(8));
    qDebug()<<"B0"<<thread->extractCurrentScope(); // Expects Var: 3, Foo: 7, Bar: 8
  }
  qDebug()<<"B1"<<thread->extractCurrentScope(); // Expects Var: 3, Foo: 5
  {
    RunThread::PushScope scope(thread, RunThread::Visibility::GetOpaque);
    thread->set("Foo", Value(9));
    thread->set("Bar", Value(10));
    qDebug()<<"C0"<<thread->extractCurrentScope(); // Expects Foo: 9, Bar: 10
  }
  qDebug()<<"C1"<<thread->extractCurrentScope(); // Expects Var: 3, Foo: 5
  {
    RunThread::PushScope scope(thread, RunThread::Visibility::Normal);
    {
      RunThread::PushScope scope(thread, RunThread::Visibility::SetTransparent);
      thread->set("Bar", Value(11));
      thread->setOuterScope("Boo", Value(12));
      qDebug()<<"D0"<<thread->extractCurrentScope(); // Expects Var: 3, Foo:5, Bar: 11, Boo: 12
    }
    qDebug()<<"D1"<<thread->extractCurrentScope(); // Expects Var: 3, Foo:5, Bar: 11
  }
  qDebug()<<"D2"<<thread->extractCurrentScope(); // Expects Var: 3, Foo:5

  {
    RunThread::PushScopePtr scope(thread, ScopesStack::create(RunThread::Visibility::Normal));
    thread->set("Foo", Value(21));
    thread->set("Bar", Value(22));
    qDebug()<<"E0"<<thread->extractCurrentScope(); // Expects Var: 3, Foo: 21, Bar: 22
  }
  qDebug()<<"E1"<<thread->extractCurrentScope(); // Expects Var: 3, Foo: 21
  {
    RunThread::PushScopePtr scope(thread, ScopesStack::create(RunThread::Visibility::SetOpaque));
    thread->set("Foo", Value(23));
    thread->set("Bar", Value(24));
    qDebug()<<"F0"<<thread->extractCurrentScope(); // Expects Var: 3, Foo: 23, Bar: 24
  }
  qDebug()<<"F1"<<thread->extractCurrentScope(); // Expects Var: 3, Foo: 21
  {
    RunThread::PushScopePtr scope(thread, ScopesStack::create(RunThread::Visibility::GetOpaque));
    thread->set("Foo", Value(25));
    thread->set("Bar", Value(26));
    qDebug()<<"G0"<<thread->extractCurrentScope(); // Expects Foo: 25, Bar: 26
  }
  qDebug()<<"G1"<<thread->extractCurrentScope(); // Expects Var: 3, Foo: 21
  {
    RunThread::PushScopePtr scope(thread, ScopesStack::create(RunThread::Visibility::Normal));
    {
      RunThread::PushScopePtr scope(thread, ScopesStack::create(RunThread::Visibility::SetTransparent));
      thread->set("Bar", Value(27));
      thread->setOuterScope("Boo", Value(28));
      qDebug()<<"H0"<<thread->extractCurrentScope(); // Expects Var: 3, Foo:21, Bar: 27, Boo: 28
    }
    qDebug()<<"H1"<<thread->extractCurrentScope(); // Expects Var: 3, Foo:21, Bar: 27
  }
  qDebug()<<"H2"<<thread->extractCurrentScope(); // Expects Var: 3, Foo:21
}
void runTest(const QString &filename)
{
  qDebug()<<filename;
  QFile f(filename);
  QString testName(QFileInfo(filename).fileName());
  if(f.open(QIODevice::ReadOnly|QIODevice::Text))
  {
     QMap<QString, QString> files;
     QString curFile=QDir("/").absoluteFilePath(testName);
     while(true)
     {
       QString line=QString::fromUtf8(f.readLine());
       if(line.isEmpty())
         break;
       if(line.startsWith("--- "))
       {
         curFile=QDir("/").absoluteFilePath(line.mid(4).trimmed());
         continue;
       }
       QString curData=files[curFile]+line;
       files.insert(curFile, curData);
     }
     auto engine=Engine::newEngineDefault();
     auto fs=new FilesystemMap;
     engine->setFilesystem(fs);
     auto thread=engine->newThread();
//     runStackTest(thread.data());
//     return;
     for(auto it=files.cbegin();it!=files.cend();it++)
     {
       QString curData=it.value();
       if(curData.right(1)=="\n")
         curData=curData.left(curData.length()-1);
       files[it.key()]=curData;
       fs->addFile(it.key(),curData.toUtf8());
     }
     QString json;
     if(fs->open("data.json", json, nullptr))
     {
       QJsonParseError error;
       auto jsonData=QJsonDocument::fromJson(json.toUtf8(), &error);
       if(jsonData.isNull())
         qFatal("Can't parse data.json for test %s: %s",qPrintable(filename), qPrintable(error.errorString()));
       if(!jsonData.isObject())
         qFatal("data.json for test %s is expected to contain an object",qPrintable(filename));
       QJsonObject obj=jsonData.object();
       Q_FOREACH(QString k, obj.keys())
       {
         thread->set(k, obj.value(k).toVariant());
//         thread->globalScope()->set(new Values::ConstValue(k), new Values::Json(obj.value(k)), thread, Location("jsonData",1));
       }
     }
     QByteArray result;
     auto outBuffer=new QBuffer(&result);
     if(outBuffer->open(QIODevice::Append))
     {
       if(thread->execute(testName, outBuffer, new Messages::MsgQDebug))
       {
         QString expected;
         if(fs->open("expectedFailure", expected, nullptr))
           qFatal("Expected a failure but got a result");
         else if(fs->open("expected", expected, nullptr))
         {
           if(expected.toUtf8()!=result)
           {
             qDebug()<<"Expected: ---------------------------";
             qDebug()<<expected.toUtf8().data();
             qDebug()<<"Returned: ----------------------------";
             qDebug()<<result.data();
             qDebug()<<"----------------------------";
           }
         }
         else
         {
           qDebug()<<"Returned: ----------------------------";
           qDebug()<<result.data();
           qDebug()<<"----------------------------";
           qDebug()<<thread->extractCurrentScope();
         }
       }
     }

//     Engine *engine=new Engine();
//     TemplateFSMap *fs=new TemplateFSMap();
//     QMapIterator<QString, QString> it(files);
//     while(it.hasNext())
//     {
//     }
//     QJsonDocument jsonData;
//     if(files.contains("/data.json"))
//     {
//     }
//     engine->setFilesystem(fs);
//     auto curTemplate=engine->load(testName);
//     if(curTemplate)
//     {
//       OutputString out;
//       RunThread *thread=engine->newThread(&out);

//       if(!jsonData.isNull() && !jsonData.isObject())
//         qFatal("data.json for test %s is expected to contain an object",qPrintable(testName));
//       QJsonObject obj=jsonData.object();
//       Q_FOREACH(QString k, obj.keys())
//       {
//         thread->globalScope()->set(new Values::ConstValue(k), new Values::Json(obj.value(k)), thread, Location("jsonData",1));
//       }
//       if(!curTemplate->run(thread))
//         qFatal("Rendering failed for %s",qPrintable(testName));
//       if(!files.contains("/expected"))
//         printf("%s\n",qPrintable(out.data()));
//       else
//       {
//         QString expected=files["/expected"];
//         if(expected!=out.data())
//         {
//           printf("%s result:\n%s\n----- differs from expected:\n%s\n-----\n",qPrintable(testName), qPrintable(out.data().replace(" ",".")), qPrintable(expected.replace(" ",".")));
//           exit(0);
//         }
//         else
//         {
//           printf("%s check passed\n",qPrintable(testName));
//         }
//       }
//     }

//     //    Template *curTemplate;
  }
  else
    qFatal("Can't open test %s",qPrintable(testName));
}
