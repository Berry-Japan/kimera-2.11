#ifndef ANTHYENGINE_H
#define ANTHYENGINE_H

#include "kanjiengine.h"
//Added by qt3to4:
#include <QTimerEvent>

struct anthy_conv_stat;
struct anthy_segment_stat;
struct anthy_prediction_stat;

class AnthyEngine : public KanjiEngine {
  Q_OBJECT

public:
  AnthyEngine();
  ~AnthyEngine();

  QString name() const;
  bool  init();
  void  cleanup();
  bool  beginConvert(const QString& hira, QStringList& kanji, QStringList& yomigana);
  void  endConvert(const QStringList& kanji=QStringList());
  bool  getCandidate(int idx, QStringList& candidate);
  bool  resizeSegment(int idx, int len, QStringList& kanji, QStringList& yomigana) ;
  bool  isConverting() { return _convertflag; }
  // Prediction
  bool  isPredictionEnabled() const;
  void  predict(const QString& romaji, const QString& hira);   // Romaji and Hiragana
  void  learn(const QStringList& kanji, const QStringList& yomigana);

protected slots:
  void timerEvent(QTimerEvent *);

private:
  void*   _anthy;
  bool    _convertflag;  // TRUE:  conversion in progress
                         // FALSE: conversion done
  void*   _dlanthydic;
  void*   _dlanthy;
  int     (*_pf_init)();
  void    (*_pf_quit)();
  void*   (*_pf_create_context)();
  void    (*_pf_release_context)(void*);
  int     (*_pf_set_string)(void* ac, char* str);
  int     (*_pf_get_stat)(void* ac, struct anthy_conv_stat* cs);
  int     (*_pf_get_segment)(void* ac, int s, int n, char* buf, int len);
  int     (*_pf_get_segment_stat)(void* ac, int n, struct anthy_segment_stat* ss);
  void    (*_pf_resize_segment)(void* ac, int nth, int resize);
  int     (*_pf_commit_segment)(void* ac, int s, int n);
  // Prediction
  QString _srcpredict;
  int     _prdctimer;
  int     (*_pf_set_prediction_string)(void* ac, const char* str);
  int     (*_pf_get_prediction_stat)(void* ac, struct anthy_prediction_stat* ps);
  int     (*_pf_get_prediction)(void* ac, int index, char* buf, int len);
};

#endif  // ANTHYENGINE_H
