#include "config.h"
#include "kimeraglobal.h"
#include "debug.h"
#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QTimerEvent>
using namespace Kimera;

static QSettings* settings = 0;
static Config* conf = 0;

Config::Config() : QObject(qApp), _timerid(0)
{
}

QString
Config::readEntry(const QString& key, const QString& def)
{
  DEBUG_TRACEFUNC("key: %s", qPrintable(key));
  init();
  return settings->value(KIMERACONF + key, def).toString();
}


bool
Config::readBoolEntry(const QString& key, bool def)
{
  DEBUG_TRACEFUNC("key: %s", qPrintable(key));
  init();
  return settings->value(KIMERACONF + key, def).toBool(); 
}


int
Config::readNumEntry(const QString& key, int def)
{
  DEBUG_TRACEFUNC("key: %s", qPrintable(key));
  init();
  return settings->value(KIMERACONF + key, def).toInt(); 
}


void 
Config::writeEntry(const QString& key, const QString& value, bool overwrite)
{
  init();
  if (overwrite || !settings->contains(KIMERACONF + key)) {
    settings->setValue(KIMERACONF + key, value);

    startSyncTimer();
  }
}


void 
Config::writeEntry(const QString& key, bool value, bool overwrite)
{
  init();
  if (overwrite || !settings->contains(KIMERACONF + key)) {
    settings->setValue(KIMERACONF + key, value);
  
    startSyncTimer();
  }
}


void
Config::writeEntry(const QString& key, int value, bool overwrite)
{
  init();
  if (overwrite || !settings->contains(KIMERACONF + key)) {
    settings->setValue(KIMERACONF + key, value);

    startSyncTimer();
  }
}


void
Config::init()
{
  if ( !settings ) {
    // Set config path 
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, configDirPath());
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "kimera");
  }
  
  if ( !conf ) {
    conf = new Config();
  }
}


QString
Config::configDirPath()
{
  QString confpath = QDir::homePath() + "/.kimera";
  QDir confdir( confpath );
  if ( !confdir.exists() ) {
    confdir.mkdir(confdir.path());
  }
  return confpath;
}


QString
Config::dictionaryDirPath()
{
  QDir  dicdir(configDirPath() + "/dic-" KIMERA_VERSION);
  if ( !dicdir.exists() ) {
    dicdir.mkdir(dicdir.path());
  }

  return dicdir.path();
}


void
Config::startSyncTimer()
{ 
  DEBUG_TRACEFUNC();
  if (conf->_timerid > 0)
    conf->killTimer(conf->_timerid);
  conf->_timerid = conf->startTimer( 100 );
}


void
Config::timerEvent(QTimerEvent* e)
{
  DEBUG_TRACEFUNC("e->timerid:%d", e->timerId());

  if (e->timerId() == conf->_timerid) {
    settings->sync();    // Sync data
    conf->killTimer(conf->_timerid);
    conf->_timerid = 0;
  }
}


QString
Config::originalDictionaryDirPath()
{
  return QCoreApplication::applicationDirPath() + "/dic" ;
}


QString
Config::imageDirPath()
{
  return QCoreApplication::applicationDirPath() + "/images" ;
}
