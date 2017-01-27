#include "anthyengine.h"
#include "debug.h"
#include <QObject>
#include <QTextCodec>
#include <QTimerEvent>
#include <anthy/anthy.h>
#include <dlfcn.h>


AnthyEngine::AnthyEngine()
  : _anthy(0), _convertflag(FALSE), _dlanthydic(0), _dlanthy(0),
    _pf_init(0), _pf_quit(0), _pf_create_context(0), _pf_release_context(0), _pf_set_string(0), 
    _pf_get_stat(0), _pf_get_segment(0), _pf_get_segment_stat(0), _pf_resize_segment(0),
    _pf_commit_segment(0), _srcpredict(QString()), _prdctimer(0),
    _pf_set_prediction_string(0), _pf_get_prediction_stat(0), _pf_get_prediction(0)
{
}

AnthyEngine::~AnthyEngine()
{
  cleanup();
}


QString
AnthyEngine::name() const
{
  return QString("Anthy");
}


bool
AnthyEngine::init()
{
  DEBUG_TRACEFUNC();

  if ( !_dlanthy ) {
    _dlanthydic = dlopen("libanthydic.so", RTLD_LAZY | RTLD_GLOBAL);
    if ( !_dlanthydic ) {
      qDebug("Cannot open libanthydic.so, %s", dlerror());
      return FALSE;
    }
    
    _dlanthy = dlopen("libanthy.so", RTLD_LAZY);
    if (!_dlanthy) {
      qDebug("Cannot open libanthy.so, %s", dlerror());
      dlclose(_dlanthydic);
      _dlanthydic = 0;
      return FALSE;
    }

    const char* err = 0;
    dlerror();    // Clear any existing error
    _pf_init = (int(*)())dlsym(_dlanthy, "anthy_init");
    _pf_quit = (void(*)())dlsym(_dlanthy, "anthy_quit");
    _pf_create_context = (void*(*)())dlsym(_dlanthy, "anthy_create_context");
    _pf_release_context = (void(*)(void*))dlsym(_dlanthy, "anthy_release_context");
    _pf_set_string = (int(*)(void*, char*))dlsym(_dlanthy, "anthy_set_string");
    _pf_get_stat = (int(*)(void*, struct anthy_conv_stat*))dlsym(_dlanthy, "anthy_get_stat");
    _pf_get_segment = (int(*)(void*, int, int, char*, int))dlsym(_dlanthy, "anthy_get_segment");
    _pf_get_segment_stat = (int(*)(void*, int, struct anthy_segment_stat*))dlsym(_dlanthy, "anthy_get_segment_stat");
    _pf_resize_segment = (void(*)(void*, int, int))dlsym(_dlanthy, "anthy_resize_segment");
    _pf_commit_segment = (int(*)(void*, int, int))dlsym(_dlanthy, "anthy_commit_segment");
#ifdef HAS_ANTHY_PREDICTION
    const char* (*pfget_version_string)();
    pfget_version_string = (const char*(*)())dlsym(_dlanthy, "anthy_get_version_string");
    err = dlerror();
    if ( err ) {
      qDebug("dlsym error, %s", err);
      cleanup();
      return FALSE;
    }

    // Version check
    QString version = (*pfget_version_string)();
    qDebug("Anthy version: %s", qPrintable(version));
    if (version >= "7100b") {
      _pf_set_prediction_string = (int(*)(void*, const char*))dlsym(_dlanthy, "anthy_set_prediction_string");
      _pf_get_prediction_stat = (int(*)(void*, struct anthy_prediction_stat*))dlsym(_dlanthy, "anthy_get_prediction_stat");
      _pf_get_prediction = (int(*)(void*, int, char*, int))dlsym(_dlanthy, "anthy_get_prediction");
    }
#endif
    err = dlerror();
    if ( err ) {
      qDebug("dlsym error, %s", err);
      cleanup();
      return FALSE;
    }
  }
  
  if ((*_pf_init)() < 0) {
    cleanup();
    return FALSE;
  }
 
  if ( !_anthy ) {
    _anthy = (*_pf_create_context)();
    if ( !_anthy ) {
      cleanup();
      return FALSE; 
    }
  }
  return TRUE;
}


void
AnthyEngine::cleanup() 
{
  DEBUG_TRACEFUNC();
  _convertflag = FALSE;
  if ( _anthy ) {
    (*_pf_release_context)((anthy_context_t)_anthy);
    _anthy = 0;
    (*_pf_quit)();
  }

  if ( _dlanthy ) {
    dlclose(_dlanthy);
    _dlanthy = 0;
  }

  if ( _dlanthydic ) {
    dlclose( _dlanthydic );
    _dlanthydic = 0;
  }
}


bool
AnthyEngine::beginConvert(const QString& hira, QStringList& kanji, QStringList& yomigana)
{
  DEBUG_TRACEFUNC("hira: %s", qPrintable(hira));
  if ( !_anthy )
    return FALSE;

  if ((*_pf_set_string)((anthy_context_t)_anthy, QTextCodec::codecForTr()->fromUnicode(hira).data()) < 0) {
    qDebug("error anthy_set_string");
    return FALSE;

  } 

  kanji.clear();
  yomigana.clear();
  anthy_conv_stat cs;
  if ((*_pf_get_stat)((anthy_context_t)_anthy, &cs) < 0) {
    qWarning("error anthy_get_stat");
    return FALSE;
  }

  char buf[8000];
  for (int i = 0; i < cs.nr_segment; ++i) {
    int len = (*_pf_get_segment)((anthy_context_t)_anthy, i, 0, buf, sizeof(buf));
    if (len < 0)
      break;
    else 
      kanji << QObject::tr(buf);

    len = (*_pf_get_segment)((anthy_context_t)_anthy, i, NTH_UNCONVERTED_CANDIDATE, buf, sizeof(buf));
    if (len < 0)
      break;
    else
      yomigana << QObject::tr(buf);
  }
  _convertflag = TRUE;
  return TRUE;
}


void
AnthyEngine::endConvert(const QStringList& kanji)
{
  DEBUG_TRACEFUNC();
  if ( !_anthy )
    return;
  
  if (_convertflag && !kanji.isEmpty()) {
    anthy_conv_stat cs;
    int ret = (*_pf_get_stat)((anthy_context_t)_anthy, &cs);
    if (!ret && cs.nr_segment == (int)kanji.count()) {
      for (int i = 0; i < cs.nr_segment; ++i) {
	QStringList cand;
	bool ok = getCandidate(i, cand);
	if ( ok ) {
	  int n = cand.indexOf(kanji[i]);
	  (*_pf_commit_segment)((anthy_context_t)_anthy, i, qMax(n, 0));
	}
      }
    }
  }
  _convertflag = FALSE;
}


bool
AnthyEngine::getCandidate(int idx, QStringList& candidate)
{
  DEBUG_TRACEFUNC("idx: %d", idx);
  if ( !_anthy )
    return FALSE;

  Q_ASSERT(_convertflag == TRUE);
  candidate.clear();
  anthy_segment_stat seg;
  int ret = (*_pf_get_segment_stat)((anthy_context_t)_anthy, idx, &seg);
  if (ret < 0) {
    qDebug("error anthy_get_segment_stat");
    return FALSE;
  }

  char buf[8000];
  for (int i = 0; i < seg.nr_candidate; ++i) {
    int len = (*_pf_get_segment)((anthy_context_t)_anthy, idx, i, buf, sizeof(buf));
    if (len > 0) {
      QString str(QObject::tr(buf));
      if ( !str.isEmpty() && !candidate.contains(str)) {
	candidate << str;
      }
    }
  }
  return TRUE;
}


bool
AnthyEngine::resizeSegment(int idx, int len, QStringList& kanji, QStringList& yomigana)
{
  DEBUG_TRACEFUNC("idx: %d  len: %d", idx, len);
  if ( !_anthy )
    return FALSE;

  Q_ASSERT(_convertflag == TRUE);
  kanji.clear();
  yomigana.clear();
  char buf[8000];
  int diff = 0;
  if (len > 0) {
    (*_pf_get_segment)((anthy_context_t)_anthy, idx, NTH_UNCONVERTED_CANDIDATE, buf, sizeof(buf));
    diff =  len - QObject::tr(buf).length();
  } else if (len == LENGTHEN_1CHAR) {
    diff = 1;
  } else if (len == SHORTEN_1CHAR) {
    diff = -1;
  } else {
    Q_ASSERT(0);
    return FALSE;
  }

  (*_pf_resize_segment)((anthy_context_t)_anthy, idx, diff);
  anthy_conv_stat cs;
  int ret = (*_pf_get_stat)((anthy_context_t)_anthy, &cs);
  if (ret < 0) {
    qDebug("error anthy_get_stat");
    return FALSE;
  }

  for (int i = idx; i < cs.nr_segment; ++i) {
    int len = (*_pf_get_segment)((anthy_context_t)_anthy, i, 0, buf, sizeof(buf));
    if (len < 0)
      break;
    else
      kanji << QObject::tr(buf);
    
    len = (*_pf_get_segment)((anthy_context_t)_anthy, i, NTH_UNCONVERTED_CANDIDATE, buf, sizeof(buf));
    if (len < 0)
      break;
    else
      yomigana << QObject::tr(buf);
  }

  return TRUE;
}


bool
AnthyEngine::isPredictionEnabled() const
{
#ifdef HAS_ANTHY_PREDICTION
  return TRUE;
#else
  return FALSE;
#endif
}


void
AnthyEngine::predict(const QString&, const QString& hira)
{
#ifdef HAS_ANTHY_PREDICTION
  DEBUG_TRACEFUNC("hira: %s", qPrintable(hira));

  if ( !_anthy || !_pf_set_prediction_string )
    return;

  if (_prdctimer > 0)
    killTimer(_prdctimer);

  _srcpredict = hira;
  if (_srcpredict.isEmpty())
    return;

  // Executes prediction after 200 msec
  _prdctimer = startTimer(200);
#endif
}


void
AnthyEngine::timerEvent(QTimerEvent* e)
{
#ifdef HAS_ANTHY_PREDICTION
  DEBUG_TRACEFUNC();
  if (e->timerId() != _prdctimer) {
    return;
  }
  // Stops timer
  killTimer(_prdctimer);
  _prdctimer = 0;

  if (_convertflag) {
    // Now converting...
    return;
  }
  // Removes the alphabet of the end
  if (_srcpredict.isEmpty()) {
    return;
  }

  qDebug("hiragana: %s", qPrintable(_srcpredict));
  int ret = (*_pf_set_prediction_string)((anthy_context_t)_anthy, QTextCodec::codecForTr()->fromUnicode(_srcpredict).data());
  if (ret < 0) {
    qWarning("error anthy_set_prediction_string");
    return;
  }

  struct anthy_prediction_stat ps;
  ret = (*_pf_get_prediction_stat)((anthy_context_t)_anthy, &ps);
  if (ret < 0) {
    qWarning("error anthy_get_prediction_stat");
    return;
  }

  char buf[1000];
  qDebug("Num of predicted: %d", ps.nr_prediction);
  if (ps.nr_prediction > 0) {
    ret = (*_pf_get_prediction)((anthy_context_t)_anthy, 0, buf, sizeof(buf));
    if (ret < 0) {
      qWarning("error anthy_get_prediction");
      return;
    }
    qDebug("Predicted: %s", buf);
    emit predicted(QObject::tr(buf));
  } else {
    emit predicted("");
  }
#endif
}


void
AnthyEngine::learn(const QStringList&, const QStringList&)
{
  DEBUG_TRACEFUNC();
}


EXPORT_KANJIENGINE(AnthyEngine)
