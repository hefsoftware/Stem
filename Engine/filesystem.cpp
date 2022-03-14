#include "filesystem.h"
#include "message.h"

using namespace QTwig;
void Filesystem::missingFile(Message *message, const QString &filename)
{
  if(message)
    message->error(QString("File \"%1\" not found").arg(filename));
}

void Filesystem::setEngine(Engine *engine)
{
  m_engine=engine;
}
