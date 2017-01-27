#ifndef DEBUG_H
#define DEBUG_H

#ifndef KIMERA_NO_DEBUG

#include <qglobal.h>

#define DEBUG_TRACEFUNC(fmt, ...)     TraceFunc ___tracefunc(__func__); \
                                      qDebug("%s-> Enter [%s()] " fmt, TraceFunc::prefix(), __func__, ## __VA_ARGS__)

#define CHECK_NOCHANGE(val, type)     CheckChange<type>  ___Check ## val ## type (val, __FILE__, __LINE__)

class TraceFunc {
public:
  TraceFunc(const char* funcname) : _funcname(funcname) {  ++_callcount; }
  ~TraceFunc() { qDebug("%s<- Leave [%s()]", prefix(), _funcname); --_callcount; }

  static const char* prefix();

private:
  const char*  _funcname;
  static uint  _callcount;
};



template <class T>
class CheckChange {
public:
  CheckChange(const T& t, const char* f, int l) : _ref(t), _param(t), _file(f), _line(l) { }
  ~CheckChange() { if (_ref != _param) qFatal("Changed parameter! (%s:%d)", _file, _line); }
  
private:
  const T&    _ref;
  T           _param;
  const char* _file;
  int         _line;
};

#else

#define DEBUG_TRACEFUNC(fmt, ...)
#define CHECK_NOCHANGE(val, type)
#define qDebug(fmt, ...)

#endif  // KIMERA_NO_DEBUG
#endif  // DEBUG_H
