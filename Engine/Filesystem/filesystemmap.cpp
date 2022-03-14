#include "filesystemmap.h"
#include <QDir>
using namespace QTwig;

FilesystemMap::FilesystemMap()
{

}

void FilesystemMap::addFile(const QString &filename, const QString &data)
{
  auto path=QDir("/").filePath(filename);
  m_data.insert(path, data);
}


bool FilesystemMap::open(const QString &filename, QString &content, Message *message)
{
  bool ret=false;
  auto path=QDir("/").filePath(filename);
  if(m_data.contains(path))
  {
    content=m_data[path];
    ret=true;
  }
  else
    missingFile(message, filename);
  return ret;
}
