#ifndef XICONTEXT_H
#define XICONTEXT_H

#include "kimeraglobal.h"
#include "supportattr.h"
#include "xicattribute.h"
#include <QString>
#include <QPoint>
#include <QHash>
using namespace Kimera;

class XIContext {
public:
  XIContext();
  ~XIContext();

  quint16      createIC();
  void         removeIC(quint16 ic);
  void         setValue(quint16 ic, quint16 id, const QByteArray&); 
  QByteArray   getValue(quint16 ic, quint16 id) const;
  XIMStyle     inputStyle(quint16 ic) const;
  Window       focusWindow(quint16 ic) const;
  QString      fontPreedit(quint16 ic) const;
  void         setSpotPreedit(quint16 ic, const QPoint& p);
  QPoint       spotPreedit(quint16 ic) const;
  bool         ximPreeditStarted(quint16 ic) const;
  void         setXIMPreeditStarted(quint16 ic, bool b);
  QList<quint16>  icList() const;
  static QList<SupportAttr>  getSuppICAttr();

protected:
  static void  initSuppICAttr();  // initialize IC attributes supported

private:
  QHash<quint16, XICAttribute*>  _iclist;     // list of ICs
  static QList<SupportAttr>  _attr_list;  // IC attributes supported
};

#endif // XICONTEXT_H
