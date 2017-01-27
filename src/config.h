#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QTimerEvent>

class Config : QObject {
public:
  static QString readEntry(const QString& key, const QString& defaultval = QString());
  static bool readBoolEntry(const QString& key, bool defaultval = false);
  static int  readNumEntry(const QString& key, int defaultval = 0);
  static void writeEntry(const QString& key, const QString& value, bool overwrite = true);
  static void writeEntry(const QString& key, bool value, bool overwrite = true);
  static void writeEntry(const QString& key, int value, bool overwrite = true);
  static QString dictionaryDirPath();
  static QString originalDictionaryDirPath();
  static QString imageDirPath();

protected:
  static QString configDirPath();
  static void init();
  static void startSyncTimer();
  void  timerEvent(QTimerEvent *);

private:
  Config();
  int  _timerid;
};

#endif
