#ifndef KIMERAAPP_H
#define KIMERAAPP_H

#include <QApplication>
#include <QEvent>

class InputMethod;
class KanjiEngine;

class KimeraApp : public QApplication {
public:  
  KimeraApp(int &argc, char **argv);
  ~KimeraApp() { }
  
  static InputMethod* inputmethod();
  static bool isXIMInputtingEnabled();

  enum Type {
    XIMClientMessage = QEvent::User + 1,
    Selection,
    InputMode,
  };

protected:
  bool x11EventFilter(XEvent *);
  static void cleanup_ptr();
};


//
// Kimera custom event class
//
class XimEvent : public QEvent {
public:
  XimEvent(Type type, void* data, int size) : QEvent(type), _d(new qint8[size]), _len(size)
  {
    memcpy(_d, data, _len);
  }
  
  ~XimEvent() { delete[] _d; }
  
  const void* data()  const { return _d; }
  
private:
  XimEvent(const XimEvent& e);
  XimEvent& operator=(const XimEvent& e);

  qint8* _d;
  int    _len;
};

#endif // KIMERAAPP_H
