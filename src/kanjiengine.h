#ifndef KANJIENGINE_H
#define KANJIENGINE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>

#define EXPORT_KANJIENGINE(ENGINE)                              \
  class Static##ENGINE##Instance {                              \
  public:                                                       \
    Static##ENGINE##Instance() {                                \
      static ENGINE* instance = 0;                              \
      if (!instance) {                                          \
        instance = new ENGINE;                                  \
      }                                                         \
    }                                                           \
  };                                                            \
  static Static##ENGINE##Instance static##ENGINE##Instance;


class KanjiEngineCleanup;

class KanjiEngine : public QObject {
  Q_OBJECT

public:
  enum {  // For resizeSegment function
    LENGTHEN_1CHAR = -1,
    SHORTEN_1CHAR  = -2,
  };

  virtual QString name() const = 0;
  virtual bool  init();
  virtual void  cleanup();
  // Kanji conversion
  virtual bool  isKanjiConvEnabled() const;
  virtual bool  beginConvert(const QString& hira, QStringList& kanji, QStringList& yomigana) = 0;
  virtual void  endConvert(const QStringList& kanji=QStringList()) = 0;
  virtual bool  getCandidate(int index, QStringList& candidate) = 0;
  virtual bool  resizeSegment(int index, int length, QStringList& kanji, QStringList& yomigana) = 0;
  virtual bool  isConverting();
  virtual bool  isTCPConnectionSupported() const;
  // Prediction
  virtual bool  isPredictionEnabled() const;
  virtual void  predict(const QString& romaji, const QString& hira);   // Romaji and Hiragana
  virtual void  learn(const QStringList& kanji, const QStringList& yomigana);

  static KanjiEngine* kanjiEngine(const QString& name);
  static QStringList kanjiEngineList();

signals:
   void  predicted(const QString& candidate);   // Candidate predicted
   void  decided(const QString& string);    // String decided by external kanji convertion apps 
  
protected:
  KanjiEngine();
  virtual ~KanjiEngine() {}
  
  friend class QList<KanjiEngine*>;
  friend class KanjiEngineCleanup;
};

#endif  // KANJIENGINE_H
