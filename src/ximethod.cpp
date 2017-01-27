#include "ximethod.h"
#include "xicattribute.h"
#include "xicontext.h"
#include "kimeraglobal.h"
#include "debug.h"

QList<SupportAttr> XIMethod::_attr_list = QList<SupportAttr>();

XIMethod::XIMethod()
{
}


void
XIMethod::initSuppIMAttr()
{
  if ( !_attr_list.isEmpty() ) return;
  
  _attr_list << SupportAttr(0, XNQueryInputStyle, TYPE_XIMSTYLES);
  Q_ASSERT(_attr_list.count() == 1);
}


QList<SupportAttr>
XIMethod::getSuppIMAttr()
{
  if ( _attr_list.isEmpty() ) initSuppIMAttr();
  return _attr_list;
}


QList<SupportAttr>
XIMethod::getSuppICAttr() 
{ 
  return XIContext::getSuppICAttr(); 
}


void
XIMethod::connectIM(Window w)
{
  _disconnect_wins.removeAll(w);
}


QList<Window>
XIMethod::disconnectIM()
{
  QList<Window> res(_disconnect_wins);
  _disconnect_wins.clear();
  return res;
}


bool
XIMethod::exists(quint16 im) const
{
  return _imlist.contains(im);
}


quint16 
XIMethod::createIM(Window w)
{
  quint16 newim = 0;    // 1 origin
  while ( _imlist.contains(++newim) ) {}   // search new IM
  
  _imlist.insert(newim, new XIMAttribute(w));
  qDebug("create IM:%d", newim);
  return newim;
}


void
XIMethod::removeIM(quint16 im)
{
  XIMAttribute* pxim = _imlist.value(im);
  if ( pxim ) {
    _disconnect_wins << pxim->commWin();
    delete _imlist.take( im );
    qDebug("Remove IM:%d  number of IMs:%d", im, _imlist.count());
  }
}


quint16
XIMethod::createXIC(quint16 im)
{
  XIMAttribute* pxim = _imlist.value(im);
  return pxim ? pxim->_xic.createIC() : 0;
}


void
XIMethod::removeXIC(quint16 im, quint16 ic)
{
  XIMAttribute* pxim = _imlist.value(im);
  if ( pxim ) {
    pxim->_xic.removeIC(ic);
  }
}


void
XIMethod::setICValue(quint16 im, quint16 ic, quint16 id, const QByteArray& data)
{
  qDebug("%s: IM:%d IC:%d attributes-id:%d data-size:%d",
	 __func__, im, ic, id, data.size());

  XIMAttribute* pxim = _imlist.value(im);
  if ( pxim )
    pxim->_xic.setValue(ic, id, data);
}


QByteArray 
XIMethod::getICValue(quint16 im, quint16 ic, quint16 id)
{
  qDebug("%s: IM:%d IC:%d attributes-id:%d", __func__, im, ic, id);
  XIMAttribute* pxim = _imlist.value(im);
  return pxim ? pxim->_xic.getValue(ic, id) : QByteArray();
}


QString
XIMethod::fontPreedit(quint16 im, quint16 ic)
{
  XIMAttribute* pxim = _imlist.value(im);
  return pxim ? pxim->_xic.fontPreedit(ic) : QString();
}


void
XIMethod::setSpotPreedit(quint16 im, quint16 ic, const QPoint& p)
{
  XIMAttribute*  pxim = _imlist.value(im);
  if ( pxim ) {
    pxim->_xic.setSpotPreedit(ic, p);
  }
}


QPoint
XIMethod::spotPreedit(quint16 im, quint16 ic)
{
  XIMAttribute* pxim = _imlist.value(im);
  return pxim ? pxim->_xic.spotPreedit(ic) : QPoint();
}


Window
XIMethod::commWin(quint16 im) const
{
  XIMAttribute* pxim = _imlist.value(im);
  return pxim ? pxim->commWin() : 0;
}


Window
XIMethod::focusWindow(quint16 im, quint16 ic) const
{
  XIMAttribute* pxim = _imlist.value(im);
  return pxim ? pxim->_xic.focusWindow(ic) : 0;
}



XIMStyle
XIMethod::getInputStyle(quint16 im, quint16 ic) const
{
  XIMAttribute*  pxim = _imlist.value(im);
  return pxim ? pxim->_xic.inputStyle(ic) : 0;
}


QByteArray
XIMethod::queryInputStyle() const
{
  QByteArray  ret(0);
  QDataStream ds(&ret, QIODevice::WriteOnly);
  ds.setByteOrder(sysByteOrder());
  ds << (quint16)(sizeof(uint) * 3) << (quint16)0
     << (uint)OVER_THE_SPOT_STYLE << (uint)ON_THE_SPOT_STYLE << (uint)ROOT_WINDOW_STYLE;

  return ret;
}


void
XIMethod::setXIMPreeditStarted(quint16 im, quint16 ic, bool b)
{
  XIMAttribute*  pxim = _imlist.value(im);
  if ( pxim ) {
    pxim->_xic.setXIMPreeditStarted(ic, b);
  }
}


bool
XIMethod::ximPreeditStarted(quint16 im, quint16 ic) const
{
  XIMAttribute*  pxim = _imlist.value(im);
  return pxim ? pxim->_xic.ximPreeditStarted(ic) : FALSE;
}


QList<quint16>
XIMethod::getIMList() const
{
  QList<quint16> lst;
  QHashIterator<quint16, XIMAttribute*>  it(_imlist);
  while (it.hasNext()) {
    it.next();
    lst << it.key();
  }
  return lst;
}


QList<quint16>
XIMethod::getICList(quint16 im) const
{
  XIMAttribute*  pxim = _imlist.value(im);
  return pxim ? pxim->xic().icList() : QList<quint16>();
}

