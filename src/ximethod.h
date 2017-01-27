#ifndef XIMETHOD_H
#define XIMETHOD_H

#include "kimeraglobal.h"
#include "ximattribute.h"
#include "supportattr.h"
#include <QList>
#include <QHash>
using namespace Kimera;

class XIMAttribute;


class XIMethod {
public:
  XIMethod();
  ~XIMethod() { }

  void        connectIM(Window w);
  QList<Window> disconnectIM();
  quint16     createIM(Window w);
  bool        exists(quint16 im) const;
  void        removeIM(quint16 im);
  quint16     createXIC(quint16 im);
  void        removeXIC(quint16 im, quint16 ic);
  void        setICValue(quint16 im, quint16 ic, quint16 id, const QByteArray&);
  QByteArray  getICValue(quint16 im, quint16 ic, quint16 id);
  QString     fontPreedit(quint16 im, quint16 ic);
  void        setSpotPreedit(quint16 im, quint16 ic, const QPoint& p);
  QPoint      spotPreedit(quint16 im, quint16 ic);
  Window      commWin(quint16 im) const;
  Window      focusWindow(quint16 im, quint16 ic) const;
  void        setXIMPreeditStarted(quint16 im, quint16 ic, bool b);
  bool        ximPreeditStarted(quint16 im, quint16 ic) const;
  XIMStyle    getInputStyle(quint16 im, quint16 ic) const;
  QByteArray  queryInputStyle() const;
  QList<quint16> getIMList() const;
  QList<quint16> getICList(quint16 im) const;

  static QList<SupportAttr>  getSuppIMAttr();
  static QList<SupportAttr>  getSuppICAttr();

protected:
  static void  initSuppIMAttr();   // initialize IM attributes supported

private:
  QHash<quint16, XIMAttribute*>  _imlist;     // list of IM
  QList<Window>                  _disconnect_wins;
  static QList<SupportAttr>      _attr_list; // IM attributes supported
};


#endif // XIMETHOD_H

