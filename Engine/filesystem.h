#pragma once
class QString;
namespace QTwig
{
  class Message;
  class Engine;
  class Filesystem
  {
    friend class Engine;
  public:
    Filesystem(): m_engine(nullptr) { }
    virtual ~Filesystem() { }
    virtual bool open(const QString &filename, QString &content, Message *message)=0;
    static void missingFile(Message *message, const QString &filename);
    constexpr Engine *engine() { return m_engine; }
  private:
    void setEngine(Engine *engine);
    Engine *m_engine;
  };
}
