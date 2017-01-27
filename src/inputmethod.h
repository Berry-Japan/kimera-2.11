//
//  There is a single InputMethod object in an application, and you can access
//  it using KimeraApp::inputmethod(). 
//

#ifndef INPUTMETHOD_H
#define INPUTMETHOD_H

#include "kimeraapp.h"
#include "xicattribute.h"
#include <QWidget>
#include <QBuffer>
#include <QStack>
#include <QCustomEvent>
#include <QKeyEvent>
#include <QEvent>

class XIMethod;
class KanjiConvert;
class InputMode;

class InputMethod : public QWidget {
  Q_OBJECT

public:
  enum { 
    // dividing size between ClientMessage and Property
    DIVIDINGSIZE = 100,
    
    // Major Protocol number  
    XIM_CONNECT = 1,
    XIM_CONNECT_REPLY = 2,
    XIM_DISCONNECT = 3,
    XIM_DISCONNECT_REPLY = 4,
    XIM_AUTH_REQUIRED = 10,
    XIM_AUTH_REPLY = 11,
    XIM_AUTH_NEXT = 12,
    XIM_AUTH_SETUP = 13,
    XIM_AUTH_NG = 14,
    XIM_ERROR = 20,
    XIM_OPEN = 30,
    XIM_OPEN_REPLY = 31,
    XIM_CLOSE = 32,
    XIM_CLOSE_REPLY = 33,
    XIM_REGISTER_TRIGGERKEYS = 34,
    XIM_TRIGGER_NOTIFY = 35,
    XIM_TRIGGER_NOTIFY_REPLY = 36,
    XIM_SET_EVENT_MASK = 37,
    XIM_ENCODING_NEGOTIATION = 38,
    XIM_ENCODING_NEGOTIATION_REPLY = 39,
    XIM_QUERY_EXTENSION = 40,
    XIM_QUERY_EXTENSION_REPLY = 41,
    XIM_SET_IM_VALUES = 42,
    XIM_SET_IM_VALUES_REPLY = 43,
    XIM_GET_IM_VALUES = 44,
    XIM_GET_IM_VALUES_REPLY = 45,
    XIM_CREATE_IC = 50,
    XIM_CREATE_IC_REPLY = 51,
    XIM_DESTROY_IC = 52,
    XIM_DESTROY_IC_REPLY = 53,
    XIM_SET_IC_VALUES = 54,
    XIM_SET_IC_VALUES_REPLY = 55,
    XIM_GET_IC_VALUES = 56,
    XIM_GET_IC_VALUES_REPLY = 57,
    XIM_SET_IC_FOCUS = 58,
    XIM_UNSET_IC_FOCUS = 59,
    XIM_FORWARD_EVENT = 60,
    XIM_SYNC = 61,
    XIM_SYNC_REPLY = 62,
    XIM_COMMIT = 63,
    XIM_RESET_IC = 64,
    XIM_RESET_IC_REPLY = 65,
    XIM_GEOMETRY = 70,
    XIM_STR_CONVERSION = 71,
    XIM_STR_CONVERSION_REPLY = 72,
    XIM_PREEDIT_START = 73,
    XIM_PREEDIT_START_REPLY = 74,
    XIM_PREEDIT_DRAW = 75,
    XIM_PREEDIT_CARET = 76,
    XIM_PREEDIT_CARET_REPLY = 77,
    XIM_PREEDIT_DONE = 78,
    XIM_STATUS_START = 79,
    XIM_STATUS_DRAW = 80,
    XIM_STATUS_DONE = 81,
    XIM_PREEDITSTATE = 82,
 
    // Extension major protocol number
    //XIM_EXT_SET_EVENT_MASK = 129,
    //XIM_EXT_FORWARD_KEYEVENT = 130,
    //XIM_EXT_MOVE = 131,
  };

  enum XIMErrorFlag {
    XIM_FLAG_INVALID_IM_IC = 0,
    XIM_FLAG_VALID_IM      = 1,
    XIM_FLAG_VALID_IC      = 2,
    XIM_FLAG_VALID_IM_IC   = 3,
  };

  enum XIMErrorCode {
    XIM_BAD_ALLOC          = 1,
    XIM_BAD_STYLE          = 2,
    XIM_BAD_CLIENT_WINDOW  = 3,
    XIM_BAD_FOCUS_WINDOW   = 4,
    XIM_BAD_AREA           = 5,
    XIM_BAD_SPOT_LOCATION  = 6,
    XIM_BAD_COLORMAP       = 7,
    XIM_BAD_ATOM           = 8,
    XIM_BAD_PIXEL          = 9,
    XIM_BAD_PIXMAP         = 10,
    XIM_BAD_NAME           = 11,
    XIM_BAD_CURSOR         = 12,
    XIM_BAD_PROTOCOL       = 13,
    XIM_BAD_FOREGROUND     = 14,
    XIM_BAD_BACKFROUND     = 15,
    XIM_BAD_LOCALE_NOT_SUPPORTED = 16,
    XIM_BAD_SOMETHING      = 999,
  };

  enum FeedbackType {
    FT_REVERSE             = XIMReverse,
    FT_UNDERLINE           = XIMUnderline,
    FT_HIGHLIGHT           = XIMHighlight,
    FT_PRIMARY             = XIMPrimary,
    FT_SECONDARY           = XIMSecondary,
    FT_TERTIARY            = XIMTertiary,
    FT_UNDERLINE_REVERSE   = XIMUnderline | XIMReverse,
    FT_UNDERLINE_HIGHLIGHT = XIMUnderline | XIMHighlight,
    //FT_VISIBLE_TO_FORWARD  = XIMVisibleToForward,
    //FT_VISIBLE_TO_BACKWARD = XIMVisibleToBackward,
    //FT_VISIBLE_CENTER      = XIMVisibleCenter,
  };

  XIMStyle currentXIMStyle() const;
  void  sendPreeditString(int caret=0, const QStringList& strlst=QStringList(), int attention_idx=-1);
  void  garbageCollectIM();
  KanjiConvert* kanjiConvert() const;
  bool isXIMInputtingEnabled() const;
  static void registerXIMServerKimera();

public slots:
  //  void  setInputMode(const InputMode&);
  void  setXIMInputtingEnabled(bool on);

signals:
  void  triggerNotify(bool);   // TRUE: on   FALSE: off

protected:
  void customEvent(QEvent*);
  void recvSelectionEvent(const QEvent* e);
  void recvSelectionRequest(const XEvent&) const;
  void recvSelectionClear(const XEvent&);
  void recvXTransportConnection(const XEvent&);
  void recvXIMConnect(const XEvent&);
  void recvXIMDisconnect(const XEvent&);
  void recvXIMOpen(const XEvent&);
  void recvXIMClose();
  void recvXIMQureyExtension();
  void recvXIMEncodingNegotiation() const;
  void recvXIMGetIMValues() const;
  void recvXIMCreateIC() const;
  void recvXIMDestroyIC();
  void recvXIMGetICValues() const;
  void recvXIMSetICValues() const;
  void recvXIMSetICFocus();
  void recvXIMUnsetICFocus();
  void recvXIMTriggerNotify();
  void recvXIMError();
  void recvProperty(const XEvent&);
  void recvXIMForwardEvent() const;
  void recvXIMResetIC();
  void recvXIMSync() const;
  void recvXIMSyncReply() const;
  //void sendXIMCommit(const XKeyEvent&) const;
  void sendXIMSetEventMask(quint16 im, quint16 ic, quint32 fwrd = 0, quint32 sync = 0) const;
  void sendXIMRegisterTriggerkeys(quint16 im) const;
  void sendXIMSync(quint16 im, quint16 ic) const;
  void sendXIMError(quint16 im, quint16 ic, XIMErrorFlag flag, XIMErrorCode error) const;
  void sendXIMError(Window client, XIMErrorCode error) const;

  // Callbacks
  void sendXIMPreeditStart(quint16 im, quint16 ic) const;
  void recvXIMPreeditStartReply() const;
  void sendXIMPreeditDraw(quint16 im, quint16 ic, int caret, int index, int length, const QString& str=QString::null, const QByteArray& feedback=QByteArray()) const;
  void sendXIMPreeditDone(quint16 im, quint16 ic) const;

  // Sends Client Message
  void sendClientMessageProtocol(Window w, XClientMessageEvent&) const;
  void sendClientMessageMoredata(Window w, XClientMessageEvent&) const;
  void sendClientMessage(quint16 im, const QByteArray&) const;
  void sendPropertywithCM(quint16 im, const QByteArray&) const;
 
  QPoint     calcPreeditPoint(quint16 im, quint16 ic, bool& ok) const;
  bool       checkCommWindow(quint16 im) const;
  int        pad(int n) const { return (4 - (n % 4)) % 4; }
  quint16  numElements(int n) const { return (n + pad(n)) / 4; }

  virtual void keyPressEvent(QKeyEvent*);

protected slots:
  void init();
  void slotDecideSegments(const QString&);

private:
  InputMethod();
  ~InputMethod();

  QStack<Window> _cltwinstack; // current client communication window ID
                               //   for XTransportConnection, XIMOpen, XIMClose,
                               //   XIMConnect and XIMDisconnect messages.
  quint16    _crnt_im;   // current input method ID
  quint16    _crnt_ic;   // current input context ID
  Atom       _server_atom;
  Atom       _xim_xconnect; 
  Atom       _xim_protocol; 
  Atom       _xim_moredata; 
  Atom       _locales;
  Atom       _transport;
  Atom       _compound_text;
  Atom       _kimera_atom;
  XIMethod*      _xim;
  KanjiConvert*  _kanjiconvt;
  QBuffer*       _buffer;       // Buffer to receive ClientMessage
  bool           _inputon;
  
  friend class KimeraApp;
};


inline KanjiConvert*
InputMethod::kanjiConvert() const
{
  return _kanjiconvt;
}


inline bool
InputMethod::isXIMInputtingEnabled() const
{
  return _inputon;
}


#endif // INPUTMETHOD_H
