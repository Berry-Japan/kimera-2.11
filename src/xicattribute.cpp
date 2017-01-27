#include "kimeraglobal.h"
#include "xicattribute.h"
#include "debug.h"

XICAttribute::XICAttribute()
  : _xim_predtstarted(FALSE),
    _inputstyle(0),
    _clientwin(0),
    _filterevents(KeyPressMask),
    _focuswin(0),
    _fontset_predt(),
    _spot_predt(),
    _areand_predt(),
    _area_predt(),
    _fg_predt(0),
    _bg_predt(0),
    _bgpix_predt(),
    _color_predt(),
    _stdclr_predt(0),  
    _line_predt(0),
    _cur_predt(),
    _fontset_stat(),
    _spot_stat(),
    _areand_stat(), 
    _area_stat(),
    _fg_stat(0),
    _bg_stat(0),
    _bgpix_stat(),   
    _color_stat(),
    _stdclr_stat(0), 
    _line_stat(0),
    _cur_stat()
{
}


XICAttribute::~XICAttribute()
{
}


QByteArray
XICAttribute::getValue(quint16 id) const
{
  QByteArray ret(0);
  QDataStream ds(&ret, QIODevice::WriteOnly);
  ds.setByteOrder(Kimera::sysByteOrder());
  
  switch (id) {
  case 0:
    ds << (uint)_inputstyle;
    break;

  case 1:
    ds << (uint)_clientwin;
    break;

  case 4:
    ds << (uint)_filterevents;
    break;

  default:
    qWarning("%s: unsupported id:%d", __func__, id);
    Q_ASSERT(0);
    break;
  }

  return ret;
}


void
XICAttribute::setValue(quint16 id, const QByteArray& data)
{
  QDataStream ds(data);
  ds.setByteOrder(Kimera::sysByteOrder());

  switch (id) {
  case 0:
    Q_ASSERT(data.size() == sizeof(XIMStyle));
    ds >> (uint&)_inputstyle;
    qDebug("client request input styles:%lx", _inputstyle);
    Q_ASSERT(_inputstyle == OVER_THE_SPOT_STYLE || _inputstyle == ON_THE_SPOT_STYLE
    	     || _inputstyle == ROOT_WINDOW_STYLE);
    break;

  case 1:
    Q_ASSERT(data.size() == sizeof(Window));
    ds >> (uint&)_clientwin;
    break;

  case 2:
    while ( !ds.atEnd() ) {
      quint16 id, len;
      ds >> id >> len;
      QByteArray attr(len, 0);
      ds.readRawData(attr.data(), len);
      ds.skipRawData( pad(len) );    // unused
      setValuePreeditAttr(id, attr);
    }
    break;
    
  case 3:
    while ( !ds.atEnd() ) {
      quint16 id, len;
      ds >> id >> len;
      QByteArray attr(len, 0);  
      ds.readRawData(attr.data(), len);
      ds.skipRawData( pad(len) );    // unused
      setValueStatusAttr(id, attr);
    }
    break;
      
  case 4:
    Q_ASSERT(data.size() == sizeof(uint));
    ds >> _filterevents;
    break;

  case 5:
    Q_ASSERT(data.size() == sizeof(Window));
    ds >> (uint&)_focuswin;
    break;;

  default:
    qDebug("%s:unsupported id:%d", __func__, id);
    Q_ASSERT(0);
    break;
  }
}


void
XICAttribute::setValuePreeditAttr(quint16 id, const QByteArray& data)
{
  QDataStream ds(data);
  ds.setByteOrder(Kimera::sysByteOrder()); 
  QByteArray fs;
  quint16 len;

  qDebug("%s id:%d data size:%d", __func__, id, data.size());
  switch (id) {
  case 6:
    ds >> len;
    fs.resize(len);
    ds.readRawData(fs.data(), len);
    _fontset_predt = fs;
    qDebug("Preedit Attributes FontSet:%d :%s", _fontset_predt.length(), qPrintable(_fontset_predt));
    break;
    
  case 7:
    Q_ASSERT(data.size() == sizeof(XPoint));
    ds >> _spot_predt.x >> _spot_predt.y;
    qDebug("Preedit Attributes  preedit spot x:%d y:%d", _spot_predt.x, _spot_predt.y);
    break;
  
  case 8:
    Q_ASSERT(data.size() == sizeof(XRectangle));
    ds >> _areand_predt.x >> _areand_predt.y
       >> _areand_predt.width >> _areand_predt.height;
    break;
  
  case 9:
    Q_ASSERT(data.size() == sizeof(XRectangle));
    ds >> _area_predt.x >> _area_predt.y
       >> _area_predt.width >> _area_predt.height;
    break;
  
  case 10:
    Q_ASSERT(data.size() == sizeof(uint));
    ds >> _fg_predt;
    break;
  
  case 11:
    Q_ASSERT(data.size() == sizeof(uint));
    ds >> _bg_predt;
    break;
  
  case 12:
    Q_ASSERT(data.size() == sizeof(Pixmap));
    ds >> (uint&)_bgpix_predt;
    break;
  
  case 13:
    Q_ASSERT(data.size() == sizeof(Colormap));
    ds >> (uint&)_color_predt;
    break;
  
  case 14:
    Q_ASSERT(data.size() == sizeof(Atom));
    ds >> (uint&)_stdclr_predt;
    break;
  
  case 15:
    Q_ASSERT(data.size() == sizeof(int));
    ds >> _line_predt;
    break;
  
  case 16:
    Q_ASSERT(data.size() == sizeof(Cursor));
    ds >> (uint&)_cur_predt;
    break;
  
  default:
    Q_ASSERT(0);
    break;
  }
}


void
XICAttribute::setValueStatusAttr(quint16 id, const QByteArray& data)
{
  QDataStream ds(data);
  ds.setByteOrder(Kimera::sysByteOrder());
  QByteArray fs;
  quint16 len;

  qDebug("%s id:%d data size:%d", __func__, id, data.size());
  switch (id) {
  case 6:
    ds >> len;
    fs.resize(len);
    ds.readRawData(fs.data(), len);
    _fontset_stat = QString(fs);
    qDebug("Status Attributes FontSet:%d:%s", _fontset_stat.length(), qPrintable(_fontset_stat));
    break;
    
  case 7:
    Q_ASSERT(data.size() == sizeof(XPoint));
    ds >> _spot_stat.x >> _spot_stat.y;
    break;
    
  case 8:
    Q_ASSERT(data.size() == sizeof(XRectangle));
    ds >> _areand_stat.x >> _areand_stat.y
       >> _areand_stat.width >> _areand_stat.height;
    break;
    
  case 9:
    Q_ASSERT(data.size() == sizeof(XRectangle));
    ds >> _area_stat.x >> _area_stat.y
       >> _area_stat.width >> _area_stat.height;
    break;
    
  case 10:
    Q_ASSERT(data.size() == sizeof(uint));
    ds >> _fg_stat;
    break;
    
  case 11:
    Q_ASSERT(data.size() == sizeof(uint));
    ds >> _bg_stat;
    break;
    
  case 12:
    Q_ASSERT(data.size() == sizeof(Pixmap));
    ds >> (uint&)_bgpix_stat;
    break;
    
  case 13:
    Q_ASSERT(data.size() == sizeof(Colormap));
    ds >> (uint&)_color_stat; 
    break;
    
  case 14:
    Q_ASSERT(data.size() == sizeof(Atom));
    ds >> (uint&)_stdclr_stat;
    break;
    
  case 15:
    Q_ASSERT(data.size() == sizeof(int));
    ds >> _line_stat;
    break;
    
  case 16:
    Q_ASSERT(data.size() == sizeof(Cursor));
    ds >> (uint&)_cur_stat;
    break;
    
  default:
    Q_ASSERT(0);
    break;
  }
}
