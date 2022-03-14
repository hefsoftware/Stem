#pragma once
#include "../filesystem.h"
#include <QMap>
namespace QTwig
{
  class Message;
  class FilesystemMap: public Filesystem
  {
  public:
    FilesystemMap();
    void addFile(const QString &filename, const QString &data);
    bool open(const QString &filename, QString &content, Message *message) override;
  protected:
    QMap<QString, QString> m_data;
  };
}
