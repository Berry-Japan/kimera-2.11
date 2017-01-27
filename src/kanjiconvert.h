#ifndef KANJICONVERT_H
#define KANJICONVERT_H

#include "kimeraglobal.h"
#include "dicthiragana.h"
#include "candidatelistbox.h"
#include "inputmode.h"
#include <QObject>
#include <QEvent>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QKeyEvent>
using namespace Kimera;

class KanjiEngine;
class CandidateListBox;
class PreeditArea;
class KanjiConvert;
class FuncPtr;
class SimpleConversionDict;

typedef bool  (KanjiConvert::*pmfunc)();
typedef Kimera::FuncID    FuncIDArray[NUM_OF_CONVSTATUS];


class KanjiConvert : public QObject {
  Q_OBJECT

public:
  KanjiConvert();
  ~KanjiConvert();
  bool    processKeyEvent(const QKeyEvent&);
  void    setFont(QFont);
  void    setPreeditPoint(QPoint);
  InputMode inputMode() const { return _mode; }
  
public slots:
  void    init();
  void    clear();
  void    setInputMode(const InputMode&);

signals:
  void    decideSegments(const QString&);
  void    inputModeChanged(const InputMode&);
  void    propertyDialogActivated();
  void    dictToolActivated();

protected slots:
  void    slotReplaceSegment(QListWidgetItem* item);
  void    showCandidatePredicted(const QString& cand);

protected:
  bool    convertAllSegments();
  bool    convertCrntSegment();
  bool    convertToOneSegment();
  bool    nextCandidate();
  bool    nextCandidateGroup();
  bool    previousCandidate();
  bool    previousCandidateGroup();
  bool    firstCandidate();
  bool    lastCandidate();
  bool    insertChar(const QKeyEvent&);
  bool    deleteForwardChar();
  bool    deleteBackwardChar();
  bool    deleteAll();
  bool    decideAllSegments();
  bool    decideCrntSegment();
  bool    decideCandidateList(int no);
  bool    decidePredictedCandidate();
  bool    cancelConversion();
  bool    lengthenSegment();
  bool    shortenSegment();
  bool    insertSpace();
  bool    insertAsciiSpace();
  bool    insertMultibyteSpace();
  bool    insertOtherWidthSpace();
  bool    convertToHira();
  bool    convertToKata();
  bool    convertToHankakuKana();
  bool    convertToHankakuEisu();
  bool    convertToZenkakuEisu();
  bool    forwardSegment();
  bool    backwardSegment();
  bool    toFirstSegment();
  bool    toLastSegment();
  bool    caretForward();
  bool    caretBackward();
  bool    caretToFirst();
  bool    caretToLast();
  bool    switchZenkakuEisuMode();
  bool    setToHiraganaMode();
  bool    setToKatakanaMode();
  bool    setToHankakuKanaMode();
  bool    setToZenkakuEisuMode();
  bool    switchInputMethod();
  bool    showPropertyDialog();
  bool    execDictTool();
  bool    reconvertClipboardString();

  bool    moveCrntItemCandidateList(int d);
  void    showPreeditArea();
  void    syncKeyEventListForward(const QString& str, QList<QKeyEvent>& kevlst) const;
  void    syncKeyEventListBackward(const QString& str, QList<QKeyEvent>& kevlst) const;
  void    createKeyEventList(const QString& yomi, QList<QKeyEvent>& kevlst) const;
  void    hideToolBox();
  QString convertYomigana(const InputMode& mode);
  void    replaceSegments(int idx, int num, const QStringList&);
  void    convertSegment(int segment_idx, const InputMode& mode);
  void    setYomigana(int idx, const QStringList&);
  int     findIndexKeyEventList(int caret_pos) const;
  QString currentSettingSpaceChar() const;
  bool    isTenkeyHankaku() const;

  void    predict();
  void    learn(const QStringList& kanji);
  void    learnPrediction(const QString& kanji);

  static void  init_funcptrdict();

private:
  int                   _crntsegidx;     // current segment index
  int                   _minsegidx;      // minimum segment index
  QList<QKeyEvent>      _inputkev;       // input key events
  QStringList           _yomiseglist;    // yomigana segment list
  QStringList           _convtkanjilist; // kanji string list converted
  ConvStatus            _stat;           // kanji conversion status
  InputMode             _mode;           // input mode
  DictHiragana          _dict;
  KanjiEngine*          _eng;
  KanjiEngine*          _engpdct;
  PreeditArea*          _preedit;
  CandidateListBox*     _candlist;
  int                   _caretpos;
  
  static QHash<int, FuncPtr>   funcptrdict;  // dictionary of function pointer
};


class FuncPtr {
public:
  FuncPtr() : _pmf(0) { }
  explicit FuncPtr(pmfunc p) : _pmf(p) { }
  pmfunc   ptr() const { return _pmf; }

private:
  pmfunc   _pmf;  // pointer to member function of KanjiConvert class
};

#endif // KANJICONVERT_H
