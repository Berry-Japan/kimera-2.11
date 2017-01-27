#include "preeditarea.h"
#include "kimeraapp.h"
#include "inputmethod.h"
#include "xicattribute.h"
#include "config.h"
#include <QFrame>
#include <QLabel>

const QString  KeyColor = "_colorsettingcolor";
const QString  KeyUnderline = "_colorsettingunderline";
const int NUM_CHAR_STATE_TYPE = 4;
const int NUM_COLOR_FGBG = 2;

PreeditArea::PreeditArea(QWidget* parent)
  : QWidget(parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint), _state(Input)
{
  _attentseg  = new QLabel(this);
  _backseg    = new QLabel(this);
  _forwardseg = new QLabel(this);
  _caret = new QLabel(this);

  _attentseg->setMargin( 0 );
  _attentseg->setIndent( 0 );
  _attentseg->setFrameStyle( QFrame::Plain | QFrame::Box );
  _attentseg->setLineWidth( 0 );
  _attentseg->setMidLineWidth( 0 );
  _attentseg->setAlignment( Qt::AlignJustify | Qt::AlignVCenter );
  _attentseg->setAutoFillBackground( true );

  _backseg->setMargin( 0 );
  _backseg->setIndent( 0 );
  _backseg->setFrameStyle( QFrame::Plain | QFrame::Box );
  _backseg->setLineWidth( 0 );
  _backseg->setMidLineWidth( 0 );
  _backseg->setAlignment( Qt::AlignJustify | Qt::AlignVCenter );
  _backseg->move(0, 0);
  _backseg->setAutoFillBackground( true );

  _forwardseg->setMargin( 0 );
  _forwardseg->setIndent( 0 );
  _forwardseg->setFrameStyle( QFrame::Plain | QFrame::Box );
  _forwardseg->setLineWidth( 0 );
  _forwardseg->setMidLineWidth( 0 );
  _forwardseg->setAlignment( Qt::AlignJustify | Qt::AlignVCenter );
  _forwardseg->setAutoFillBackground( true );

  QPalette palette;
  palette.setColor(QPalette::Window, QColor(0, 0, 0));
  _caret->setPalette(palette);
  _caret->setAutoFillBackground( true );

  resize(0, 0);
}


void 
PreeditArea::init()
{
  qDebug("PreeditArea init");
  // Loads colors
  for (int i = 0; i < NUM_CHAR_STATE_TYPE; ++i) {
    for (int j = 0; j < NUM_COLOR_FGBG; ++j) {
      _colorlist[i * NUM_COLOR_FGBG + j] = QColor( Config::readEntry(KeyColor + QString::number(i) + '-' + QString::number(j), "black") );
    }
  }
}


void 
PreeditArea::setFont(const QFont & f)
{
  for (int i = 0; i < NumStates; i++) {
      _font[i] = f;
      _font[i].setUnderline(Config::readEntry(KeyUnderline + QString::number(i), "0") == QString("1"));
  }

  _attentseg->setFont( f );
  _backseg->setFont( f );
  _forwardseg->setFont( f );
  _caret->resize(2, QFontMetrics( f ).height());
}


void
PreeditArea::showText(const QStringList& strlist, int attention, int caret_pos)
{
  if (attention >= strlist.count()) {
    qFatal("Bad parameter");
    return;
  }
  if (strlist.isEmpty()) {
    hide();
    return;
  }
  
  // Sets segments
  QString str;
  for (int i = 0; i < (int)attention; ++i)
    str += strlist[i];
  _backseg->setText(str);
  _attentseg->setText(strlist[attention]);
  str = QString::null;
  for (int i = attention + 1; i < (int)strlist.count(); ++i)
    str += strlist[i];
  _forwardseg->setText(str);

  // Show
  switch ( KimeraApp::inputmethod()->currentXIMStyle() ) {
  case OVER_THE_SPOT_STYLE:
  case ROOT_WINDOW_STYLE:
    _backseg->text().isEmpty() ? _backseg->hide() : _backseg->show();
    _attentseg->show();
    _forwardseg->text().isEmpty() ? _forwardseg->hide() : _forwardseg->show();
    if (_state != Input) {
      _caret->hide();
    } else {
      QFontMetrics fm( _attentseg->font() );
      int pos = fm.width(_attentseg->text(), caret_pos);
      if (caret_pos == _attentseg->text().length())
	pos += _attentseg->margin() + 1;
      _caret->move(QPoint(pos, 1));
      _caret->raise();
      _caret->show();
    }
    show();
    adjustSize();
    break;

  case ON_THE_SPOT_STYLE:
    if (_state == Input) {
      KimeraApp::inputmethod()->sendPreeditString(caret_pos, QStringList(_attentseg->text()));
    } else {
      KimeraApp::inputmethod()->sendPreeditString(strlist.join("").length(), strlist, attention);
    }
    break;
    
  default:
    qWarning("Bad XIMStyle: %lu", KimeraApp::inputmethod()->currentXIMStyle());
    Q_ASSERT(0);
    return;
    break;
  }
  
  qDebug("showText  back: %s  attention: %s  forward: %s", qPrintable(_backseg->text()),
	 qPrintable(_attentseg->text()), qPrintable(_forwardseg->text()));
}


void 
PreeditArea::showInputingString(const QString& str, int caret_pos)
{
  Q_ASSERT( !str.isEmpty() );
  _state = Input;
  _attentseg->setFont( _font[Input] );

  QPalette palette;
  palette.setColor(QPalette::WindowText, _colorlist[FgInput]);
  palette.setColor(QPalette::Window, _colorlist[BgInput]);
  _attentseg->setPalette(palette);
  showText(QStringList(str), 0, caret_pos);
}


void 
PreeditArea::showConvertingSegments(const QStringList& strlist, int attention)
{
  Q_ASSERT( !strlist.empty() );
  _state = Attention;
  _attentseg->setFont( _font[Attention] );

  QPalette palette;
  palette.setColor(QPalette::WindowText, _colorlist[FgAttention]);
  palette.setColor(QPalette::Window, _colorlist[BgAttention]);
  _attentseg->setPalette(palette);

  _backseg->setFont( _font[Converted] );
  palette.setColor(QPalette::WindowText, _colorlist[FgConverted]);
  palette.setColor(QPalette::Window, _colorlist[BgConverted]);
  _backseg->setPalette(palette);

  _forwardseg->setFont( _font[Converted] );
  palette.setColor(QPalette::WindowText, _colorlist[FgConverted]);
  palette.setColor(QPalette::Window, _colorlist[BgConverted]);
  _forwardseg->setPalette(palette);

  showText(strlist, attention);
}


void 
PreeditArea::showChangingSegmentLength(const QStringList& strlist, int attention)
{
  Q_ASSERT( !strlist.empty() );
  _state = Changing;
  _attentseg->setFont( _font[Changing] );

  QPalette palette;
  palette.setColor(QPalette::WindowText, _colorlist[FgChanging]);
  palette.setColor(QPalette::Window, _colorlist[BgChanging]);
  _attentseg->setPalette(palette);

  _backseg->setFont( _font[Converted] );
  palette.setColor(QPalette::WindowText, _colorlist[FgConverted]);
  palette.setColor(QPalette::Window, _colorlist[BgConverted]);
  _backseg->setPalette(palette);

  _forwardseg->setFont( _font[Converted] );
  palette.setColor(QPalette::WindowText, _colorlist[FgConverted]);
  palette.setColor(QPalette::Window, _colorlist[BgConverted]);
  _forwardseg->setPalette(palette);

  showText(strlist, attention);
}


void
PreeditArea::adjustSize()
{
  QWidget::adjustSize();

  const QSize hmargin(0, 2);
  if ( !_attentseg->text().isEmpty() ) {
    _backseg->resize( _backseg->sizeHint() + hmargin );

    QPoint p(0, 0);
    if ( !_backseg->text().isEmpty() )
      p += QPoint(_backseg->size().width(), 0);
  
    _attentseg->move( p );
    _attentseg->resize( _attentseg->sizeHint() + hmargin );
    emit listPointChanged( mapToGlobal(p + QPoint(0, _attentseg->height()) + QPoint(-4, 1)) );   // Current segment pos
    
    p += QPoint(_attentseg->size().width(), 0);
    _forwardseg->move( p );
    _forwardseg->resize( _forwardseg->sizeHint() + hmargin );

    if ( !_forwardseg->text().isEmpty() )
      p += QPoint(_forwardseg->size().width(), 0);
  
    resize( QSize(p.x(), _attentseg->size().height()) );
    qDebug("back: %s  attention: %s  forward: %s", qPrintable(_backseg->text()),
	   qPrintable(_attentseg->text()), qPrintable(_forwardseg->text()));
  }
}


void
PreeditArea::movePreeditPos(const QPoint& pos)
{
  int offset = QFontMetrics(_attentseg->font()).height() - 1;
  move(pos - QPoint(0, offset));

  if (KimeraApp::inputmethod()->currentXIMStyle() == ON_THE_SPOT_STYLE)
    emit listPointChanged(pos + QPoint(12, 1));   // Current segment pos
}


QString
PreeditArea::text() const
{
  return _backseg->text() + _attentseg->text() + _forwardseg->text();
}


QString
PreeditArea::backwardText() const
{
  return _backseg->text();
}


QString
PreeditArea::attentionText() const
{
  return _attentseg->text();
}


QString
PreeditArea::forwardText() const
{
  return _forwardseg->text();
}


void
PreeditArea::hide()
{
  if (KimeraApp::inputmethod()->currentXIMStyle() == ON_THE_SPOT_STYLE) {
    KimeraApp::inputmethod()->sendPreeditString();  // Clear string
  }
  _attentseg->setText(QString::null);
  _backseg->setText(QString::null);
  _forwardseg->setText(QString::null);
  QWidget::hide();
}
