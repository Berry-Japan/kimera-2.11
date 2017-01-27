#ifndef TOMOEENGINE_H
#define TOMOEENGINE_H

#include "kanjiengine.h"

class QProcess;

class TomoeEngine : public KanjiEngine {
  Q_OBJECT

public:
  TomoeEngine();
  ~TomoeEngine();
  
  QString name() const;
  bool isKanjiConvEnabled() const { return false; }

  bool  init();
  void  cleanup();
  bool  beginConvert(const QString& hira, QStringList& kanji, QStringList& yomigana);
  void  endConvert(const QStringList& kanji=QStringList());
  bool  getCandidate(int idx, QStringList& candidate);
  bool  resizeSegment(int idx, int len, QStringList& kanji, QStringList& yomigana);
  bool  isConverting() { return false; }

protected slots:
  void  readFromStdout();

private:
  QProcess*    _tomoe;
};

#endif  // TOMOEENGINE_H
