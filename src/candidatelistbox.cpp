#include "candidatelistbox.h"
#include "inputmethod.h"
#include "ximattribute.h"
#include "inputmethod.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QDesktopWidget>
#include <QShowEvent>
#include <QHideEvent>

const int MARGIN = 6;
const int CURRENT_ITEM_NO_LABEL_HEIGHT = 20;

CandidateListBox::CandidateListBox(QWidget* parent) 
  : QFrame(parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint),
    _lbl(new QListWidget(this)),
    _lstbox(new QListWidget(this)),
    _crntitemno(new QLabel(this)),
    _predictxt(new QLabel(this)),
    _pos()
{
  setLineWidth( 1 );
  setFrameStyle( QFrame::Plain | QFrame::Box );

  _lbl->move(1, 1);
  _lbl->setSelectionMode( QListWidget::NoSelection );
  _lbl->setFrameStyle( QFrame::NoFrame );
  _lbl->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  _lbl->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  _lbl->resize(17, 10);

  QPalette palette;
  palette.setColor(QPalette::Base, QColor(228, 228, 228));
  _lbl->setPalette(palette);

  _lstbox->move(17, 1);
  _lstbox->setFrameStyle( QFrame::NoFrame );
  _lstbox->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  _lstbox->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

  _crntitemno->setFrameStyle( QFrame::Plain | QFrame::Box );
  _crntitemno->setLineWidth( 1 );
  _crntitemno->setAlignment( Qt::AlignRight );

  _predictxt->setFrameStyle( QFrame::NoFrame );
  _predictxt->setMargin( 2 );
  _predictxt->setLineWidth( 0 );
  palette.setColor(QPalette::Base, QColor(255, 255, 242));
  _predictxt->setPalette(palette);
  _predictxt->move(1, 1);

  connect(_lstbox, SIGNAL(itemClicked(QListWidgetItem*)), this, SIGNAL(clicked(QListWidgetItem*)));
  connect(_lstbox, SIGNAL(currentRowChanged(int)), this, SLOT(setLabelValue(int)));
}


void
CandidateListBox::showList(const QStringList& strs, int crntidx)
{
  Q_ASSERT( !strs.isEmpty() );
  _lstbox->clear();
  _lstbox->addItems(strs);
  _lstbox->setCurrentRow((crntidx < (int)_lstbox->count()) ? crntidx : _lstbox->count() - 1);

  // Calculates size
  int w = 0;
  QStringListIterator it(strs);
  while (it.hasNext()) {
    w = qMax(fontMetrics().size(0, it.next()).width(), w);
  }
  w += 40;  // margin

  int h = fontMetrics().height() * qMin((int)MAX_VISIBLE_ITEMS, _lstbox->count());
  _lstbox->resize(w, h);

  _lbl->clear();
  _lbl->resize(_lbl->width(), h);
  for (int i = 1; i <= qMin((int)_lstbox->count(), 9); ++i)
    _lbl->addItem( QString::number(i) );

  // Resizes current item number
  _crntitemno->move(0, h + 1);
  _crntitemno->resize(w + _lbl->width() + 2, CURRENT_ITEM_NO_LABEL_HEIGHT);

  resize(w + _lbl->width() + 2, h + CURRENT_ITEM_NO_LABEL_HEIGHT + 1);
  qDebug("CandidateListBox w:%d h:%d", w, h);

  _lbl->show();
  _lstbox->show();
  _crntitemno->show();
  _predictxt->hide();   // Hides prediction text
  show();
}


void
CandidateListBox::selectCandidate(int idx)
{
  if (isListVisible()) {
    _lstbox->setCurrentRow((idx < (int)_lstbox->count()) ? idx : _lstbox->count() - 1);
  }
}


void
CandidateListBox::selectPreviousCandidate(uint decrement)
{
  if ( isHidden() ) {
    qFatal("Assert  %s:%d", __FILE__, __LINE__);
    return;
  }

  int prev = _lstbox->currentRow() - decrement;
  if (prev < 0) {
    prev = _lstbox->count() - 1;  // Last candidate
    if (decrement > 1 && (visibleTopItem() > 0 || _lstbox->count() <= MAX_VISIBLE_ITEMS)) {
      prev = 0;  // First candidate
    }
  }

  _lstbox->setCurrentRow(prev);
}


void
CandidateListBox::selectNextCandidate(uint increment)
{
  if ( isHidden() ) {
    qFatal("Assert  %s:%d", __FILE__, __LINE__);
    return;
  }

  int next = _lstbox->currentRow() + increment;
  if (next >= _lstbox->count()) {
    next = 0;  // First candidate
    if (increment > 1 && visibleBottomItem() < _lstbox->count() - 1) {
      next = _lstbox->count() - 1;  // Last candidate
    }
  }
  _lstbox->setCurrentRow(next);
}


void
CandidateListBox::showEvent(QShowEvent* e)
{
  // Adjusts pos
  const int offset = 2;
  int x = _pos.x() + 1;
  if ( !_predictxt->isVisible() ) {
    x -= _lbl->width();
  }
  x = qMin(x, QApplication::desktop()->screen(0)->width() - width() - offset);
  int y = _pos.y();
  if (y > QApplication::desktop()->screen(0)->height() - height() - offset) {
    if (KimeraApp::inputmethod()->currentXIMStyle() != ON_THE_SPOT_STYLE) {
      QFontMetrics fm( font() ); 
      y -= height() + fm.height() + 1;
    } else {
      y = qMin(y, QApplication::desktop()->height() - height() - offset);
    }
  }
  move(qMax(x, 0), qMax(y, 0));
  QFrame::showEvent(e);
}


void
CandidateListBox::hideEvent(QHideEvent* e)
{
  _lstbox->clear();
  QFrame::hideEvent(e);
}


void
CandidateListBox::setFont(const QFont& f)
{
  QFont font(f);
  if (QFontInfo(font).pixelSize() > 20)
    font.setPixelSize(20);
  
  _lbl->setFont(font);
  QFontMetrics fm(font);
  int w = fm.width("0") * 2;
  _lbl->resize(w, _lbl->height());

  _lstbox->move(w + 1, 1);
  _lstbox->setFont(font);
  
  QFrame::setFont(font);

  font.setPixelSize(12);
  _crntitemno->setFont( font );
}


bool
CandidateListBox::isListVisible() const
{
  return isVisible() && !_predictxt->isVisible();
}


bool
CandidateListBox::isPredictionsVisible() const
{
  return isVisible() && _predictxt->isVisible();
}


void
CandidateListBox::setPos(const QPoint& p)
{
  _pos = p;
}


void
CandidateListBox::showCandidatePredicted(const QString& str)
{
  if (!isVisible() || _predictxt->isVisible()) {
    _predictxt->setText(str);
    if (!str.isEmpty()) {
      _predictxt->adjustSize();
      _predictxt->show();
      _lbl->hide();
      _lstbox->hide();
      _crntitemno->hide();
      adjustSize();
      show();
    } else {
      _predictxt->hide();
      hide();
    }
  }
}


QString
CandidateListBox::candidatePredicted() const
{
  return _predictxt->isVisible() ? _predictxt->text() : QString::null;
}


QString
CandidateListBox::currentText() const
{
  Q_ASSERT( _lstbox->currentItem() );
  return _lstbox->currentItem()->text();
}

void
CandidateListBox::setLabelValue(int d)
{
  QString text = QString("%1/%2").arg(d + 1).arg(_lstbox->count());
  _crntitemno->setText(text);
}


QString
CandidateListBox::text(int index) const
{
  Q_ASSERT( _lstbox->item(index) );
  return _lstbox->item(index)->text();
}

int
CandidateListBox::currentItem() const
{
  return _lstbox->currentRow();
}

int
CandidateListBox::visibleTopItem() const
{
  return _lstbox->row( _lstbox->itemAt(0, 0) );
}

int
CandidateListBox::visibleBottomItem() const
{
  return _lstbox->row( _lstbox->itemAt(0, _lstbox->height() - 1));
}

uint
CandidateListBox::count() const
{
  return _lstbox->count();
}

void
CandidateListBox::scrollToItemAtTop(int index)
{
  QListWidgetItem* item = _lstbox->item(index);
  if ( item ) {
    _lstbox->scrollToItem(item, QAbstractItemView::PositionAtTop);
  }
}
