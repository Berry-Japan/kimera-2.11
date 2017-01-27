#include "xicontext.h"
#include <QList>

QList<SupportAttr> XIContext::_attr_list = QList<SupportAttr>();

void
XIContext::initSuppICAttr()
{
  if ( !_attr_list.isEmpty() ) return;
  
  quint16 i = 0;
  _attr_list << SupportAttr(i++, XNInputStyle, TYPE_LONG);
  _attr_list << SupportAttr(i++, XNClientWindow,TYPE_WINDOW);
  _attr_list << SupportAttr(i++, XNPreeditAttributes, TYPE_NESTEDLIST);
  _attr_list << SupportAttr(i++, XNStatusAttributes, TYPE_NESTEDLIST);
  _attr_list << SupportAttr(i++, XNFilterEvents, TYPE_WORD);
  _attr_list << SupportAttr(i++, XNFocusWindow, TYPE_WINDOW);
  _attr_list << SupportAttr(i++, XNFontSet, TYPE_XFONTSET);
  _attr_list << SupportAttr(i++, XNSpotLocation, TYPE_XPOINT);
  _attr_list << SupportAttr(i++, XNAreaNeeded, TYPE_XRECTANGLE);
  _attr_list << SupportAttr(i++, XNArea, TYPE_XRECTANGLE);
  _attr_list << SupportAttr(i++, XNForeground, TYPE_LONG);
  _attr_list << SupportAttr(i++, XNBackground, TYPE_LONG);
  _attr_list << SupportAttr(i++, XNBackgroundPixmap, TYPE_LONG);
  _attr_list << SupportAttr(i++, XNColormap, TYPE_WORD);
  _attr_list << SupportAttr(i++, XNStdColormap, TYPE_WORD);
  //_attr_list << SupportAttr(i++, XNLineSpace, TYPE_WORD);
  _attr_list << SupportAttr(i++, XNLineSpace, TYPE_LONG);
  _attr_list << SupportAttr(i++, XNCursor, TYPE_WORD);

  Q_ASSERT(_attr_list.count() == 17);
}


QList<SupportAttr>
XIContext::getSuppICAttr() 
{
  if ( _attr_list.isEmpty() ) initSuppICAttr();
  return _attr_list; 
}


XIContext::XIContext()
{
}


XIContext::~XIContext()
{
}


quint16     
XIContext::createIC()
{
  quint16 newic = 0;  // 1 origin
  while ( _iclist.contains(++newic) ) {}    // search new IC

  _iclist.insert(newic, new XICAttribute());
  return newic;
}


void
XIContext::removeIC(quint16 ic) 
{ 
  delete _iclist.take(ic); 
}


void
XIContext::setValue(quint16 ic, quint16 id, const QByteArray& data)
{
  XICAttribute* pxic = _iclist.value(ic);
  if ( pxic ) pxic->setValue(id, data);
}


QByteArray
XIContext::getValue(quint16 ic, quint16 id) const
{
  XICAttribute* pxic = _iclist.value(ic);
  return pxic ? pxic->getValue(id) : QByteArray();
}


XIMStyle
XIContext::inputStyle(quint16 ic) const
{
  XICAttribute* pxic = _iclist.value(ic);
  return pxic ? pxic->inputStyle() : 0;
}


Window
XIContext::focusWindow(quint16 ic) const
{
  XICAttribute* pxic = _iclist.value(ic);
  return pxic ? pxic->focusWindow() : 0;
}


QString
XIContext::fontPreedit(quint16 ic) const
{
  XICAttribute* pxic = _iclist.value(ic);
  return pxic ? pxic->fontPreedit() : QString();
}


void
XIContext::setSpotPreedit(quint16 ic, const QPoint& p)
{
  XICAttribute* pxic = _iclist.value(ic);
  if ( pxic ) pxic->setSpotPreedit( p );
}


QPoint
XIContext::spotPreedit(quint16 ic) const
{
  XICAttribute* pxic = _iclist.value(ic);
  return pxic ? pxic->spotPreedit() : QPoint();
}


bool
XIContext::ximPreeditStarted(quint16 ic) const
{
  XICAttribute* pxic = _iclist.value(ic);
  return pxic ? pxic->ximPreeditStarted() : FALSE;
}


void
XIContext::setXIMPreeditStarted(quint16 ic, bool b)
{
  XICAttribute* pxic = _iclist.value(ic);
  if ( pxic ) pxic->setXIMPreeditStarted( b );
}


QList<quint16>
XIContext::icList() const
{
  QList<quint16> lst;
  QHashIterator<quint16, XICAttribute*>  it(_iclist);
  while (it.hasNext()) {
    it.next();
    lst << it.key();
  }
  return lst;
}
