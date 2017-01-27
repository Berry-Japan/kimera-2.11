#include "tomoeengine.h"
#include "debug.h"
#include <QProcess>
#include <QDir>

TomoeEngine::TomoeEngine()
  : _tomoe(new QProcess(0))
{
  connect(_tomoe, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
}


TomoeEngine::~TomoeEngine()
{}


QString
TomoeEngine::name() const
{
  return "Tomoe";
}


bool
TomoeEngine::init()
{
  DEBUG_TRACEFUNC();

  if (_tomoe->state() != QProcess::NotRunning)
    return true;

  _tomoe->setWorkingDirectory(QDir::homePath());
  _tomoe->start("kimera-tomoe-gtk");
  return true;
}


void
TomoeEngine::cleanup()
{
  DEBUG_TRACEFUNC();
  _tomoe->kill();
}


bool
TomoeEngine::beginConvert(const QString&, QStringList&, QStringList&)
{
  return true;
}


void
TomoeEngine::endConvert(const QStringList&)
{}


bool
TomoeEngine::getCandidate(int, QStringList&)
{
  return true;
}


bool
TomoeEngine::resizeSegment(int, int, QStringList&, QStringList&)
{
  return true;
}


void
TomoeEngine::readFromStdout()
{ 
  DEBUG_TRACEFUNC();
  const QString prefix("commit_string:\t");

  while (_tomoe->canReadLine()) {
    QByteArray str = _tomoe->readAll();
    QString line = QString::fromUtf8(str.data(), str.size()).section('\n', -2, -2);
    if ( line.startsWith(prefix, Qt::CaseInsensitive) ) {
      qDebug("String commited(%d): %s", line.length(), qPrintable(line.mid(prefix.length())));
      emit decided(line.mid(prefix.length()));
    }
  }
}


EXPORT_KANJIENGINE(TomoeEngine)
