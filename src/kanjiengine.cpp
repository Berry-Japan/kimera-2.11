#include "kanjiengine.h"

static QList<KanjiEngine*>* enginelist = 0;


class KanjiEngineCleanup {
public:
  ~KanjiEngineCleanup();
};


KanjiEngineCleanup::~KanjiEngineCleanup()
{
  if (enginelist) {
    while ( !enginelist->isEmpty() )
      delete enginelist->takeFirst();

    delete enginelist;
  }

  enginelist = 0;
}
static KanjiEngineCleanup kanjienginecleanup;


KanjiEngine::KanjiEngine()
{
  if (!enginelist) {
    enginelist = new QList<KanjiEngine*>;
  }
  enginelist->append(this);
}


bool
KanjiEngine::init()
{
  return TRUE; 
}


void
KanjiEngine::cleanup()
{
}


bool
KanjiEngine::isKanjiConvEnabled() const
{
  return TRUE;
}


bool
KanjiEngine::isConverting()
{
  Q_ASSERT(0);
  return FALSE;
}


bool
KanjiEngine::isTCPConnectionSupported() const
{
  return FALSE;
}


KanjiEngine*
KanjiEngine::kanjiEngine(const QString& name)
{
  Q_ASSERT(enginelist);
  QListIterator<KanjiEngine*> it(*enginelist);
  while (it.hasNext()) {
    KanjiEngine* eng = it.next();
    if (eng->name().toLower() == name.toLower()) {
      return eng;
    }
  }
  return 0;
}


QStringList
KanjiEngine::kanjiEngineList()
{
  QStringList lst;
  QListIterator<KanjiEngine*> it(*enginelist);
  while (it.hasNext()) {
    QString str = it.next()->name();
    if ( !str.isEmpty() )
      lst << str;
  }
  return lst;
}


bool
KanjiEngine::isPredictionEnabled() const
{
  return FALSE;
}


void
KanjiEngine::predict(const QString&, const QString&)
{
}


void
KanjiEngine::learn(const QStringList&, const QStringList&)
{
}
