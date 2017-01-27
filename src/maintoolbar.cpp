#include "maintoolbar.h"
#include "kimeraglobal.h"
#include "kimeraapp.h"
#include "inputmethod.h"
#include "inputmode.h"
#include "propertydialog.h"
#include "keyassigner.h"
#include "config.h"
#include "kanjiengine.h"
#include "debug.h"
#include <QLayout>
#include <QToolTip>
#include <QString>
#include <QTimer>
#include <QMessageBox>
#include <QPixmap>
#include <QProcess>
#include <QDir>
#include <QMouseEvent>
#include <QEvent>
#include <QDesktopWidget>
#include <QActionGroup>
#include <QAction>
#include <QSystemTrayIcon>
using namespace Kimera;


MainToolBar::MainToolBar()
  : QFrame(0, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint),
    _leftmover(new Mover(this)),
    _toolbtn1(new QToolButton(this)),
    _toolbtn2(new QToolButton(this)),
    _toolbtn3(new QToolButton(this)),
    _toolbtn4(new QToolButton(this)),
    _pupmenu1(new Popup(this)),
    _pupmenu2(new Popup(this)),
    _pupmenu3(new Popup(this)),
    _propdlg(new PropertyDialog(this)),
    _inputmodegrp(new QActionGroup(this)),
    _inputstylegrp(new QActionGroup(this)),
    _modemap(),
    _labelmap(),
    _iconimgpathmap(),
    _handwriting(0),
    _trayicon(new QSystemTrayIcon(0)),
    _trayiconmenu(new Popup(0))
{
  setAttribute( Qt::WA_DeleteOnClose );
  setAttribute( Qt::WA_AlwaysShowToolTips );
  setFrameShape( QFrame::StyledPanel );
  setFrameShadow( QFrame::Raised );

  QFont f("gothic", 9);
  const QSize btnsize(30, 26);
  QPoint pos(15, 3);

  _leftmover->move(1, 3);
  _leftmover->setMaximumHeight( btnsize.height() );

  _toolbtn1->move( pos );
  _toolbtn1->resize( btnsize );
  pos += QPoint(_toolbtn1->width(), 0);
  _toolbtn1->setAutoRaise(true);

  _toolbtn2->move( pos );
  _toolbtn2->resize( btnsize + QSize(6, 0) );
  pos += QPoint(_toolbtn2->width(), 0);
  _toolbtn2->setAutoRaise(true);

  _toolbtn3->move( pos );
  _toolbtn3->resize( btnsize );
  pos += QPoint(_toolbtn3->width(), 0);
  _toolbtn3->setIcon( QIcon(QPixmap(Config::imageDirPath() + "/red_dictionary.png")) );
  _toolbtn3->setMaximumSize( btnsize );
  _toolbtn3->setAutoRaise(true);
  
  _toolbtn4->move( pos );
  _toolbtn4->resize( btnsize );
  pos += QPoint(_toolbtn4->width(), 0);
  _toolbtn4->setIcon( QIcon(QPixmap(Config::imageDirPath() + "/property_icon.png")) );
  _toolbtn4->setText( tr("プロパティ") );
  _toolbtn4->setMaximumSize( btnsize );
  _toolbtn4->setAutoRaise(true);

  _handwriting = KanjiEngine::kanjiEngine("Tomoe");
  Q_CHECK_PTR( _handwriting );

  QAction* actHira = _inputmodegrp->addAction( _pupmenu1->addAction(tr("ひらがな")) );
  actHira->setCheckable(true);
  QAction* actKata = _inputmodegrp->addAction( _pupmenu1->addAction(tr("カタカナ")) );
  actKata->setCheckable(true);
  QAction* actKana = _inputmodegrp->addAction( _pupmenu1->addAction(tr("半角カナ")) );
  actKana->setCheckable(true);
  QAction* actEisu = _inputmodegrp->addAction( _pupmenu1->addAction(tr("全角英数")) );
  actEisu->setCheckable(true);
  QAction* actDrct = _inputmodegrp->addAction( _pupmenu1->addAction(tr("直接入力")) );
  actDrct->setCheckable(true);
  actDrct->setChecked(true);
  _inputmodegrp->setExclusive(true);

  _inputstylegrp->addAction( _pupmenu2->addAction(tr("ローマ字入力")) )->setCheckable(true);
  _inputstylegrp->addAction( _pupmenu2->addAction(tr("かな入力")) )->setCheckable(true);
  _inputstylegrp->setExclusive( true );

  QAction* actDict = _pupmenu3->addAction(tr("辞書ツール"), this, SLOT(execDictTool()));
  QAction* actHand = _pupmenu3->addAction(tr("手書き認識"), this, SLOT(execHandWritingTool()));

  _toolbtn1->setMenu(_pupmenu1);
  _toolbtn1->setPopupMode(QToolButton::DelayedPopup);
  _toolbtn1->setFocusPolicy(Qt::NoFocus);
  _toolbtn2->setMenu(_pupmenu2);
  _toolbtn2->setPopupMode(QToolButton::DelayedPopup);
  _toolbtn2->setFocusPolicy(Qt::NoFocus);
  _toolbtn3->setMenu(_pupmenu3);
  _toolbtn3->setPopupMode(QToolButton::DelayedPopup);
  _toolbtn3->setFocusPolicy(Qt::NoFocus);

  // Sets to be the context menu for the system tray icon.
  _trayiconmenu->addAction( actHira );
  _trayiconmenu->addAction( actKata );
  _trayiconmenu->addAction( actKana );
  _trayiconmenu->addAction( actEisu );
  _trayiconmenu->addAction( actDrct );
  _trayiconmenu->addSeparator();
  _trayiconmenu->addAction( actDict );
  _trayiconmenu->addAction( actHand );
  _trayiconmenu->addSeparator();
  _trayiconmenu->addAction(tr("プロパティ"), this, SLOT(showPropertyDialog()));
  _trayicon->setContextMenu(_trayiconmenu);
 
  _modemap.insert(tr("ひらがな"),     int(Mode_Hiragana));
  _modemap.insert(tr("カタカナ"),     int(Mode_Katakana));
  _modemap.insert(tr("半角カナ"),     int(Mode_HankakuKana));
  _modemap.insert(tr("全角英数"),     int(Mode_ZenkakuEisu));
  _modemap.insert(tr("ローマ字入力"), int(Mode_RomaInput));
  _modemap.insert(tr("かな入力"),     int(Mode_KanaInput));
  _modemap.insert(tr("直接入力"),     int(Mode_DirectInput));

  _labelmap.insert(Mode_Hiragana,    QString(tr("あ")));
  _labelmap.insert(Mode_Katakana,    QString(tr("ア")));
  _labelmap.insert(Mode_HankakuKana, QString(tr("_ｱ")));
  _labelmap.insert(Mode_ZenkakuEisu, QString(tr("Ａ")));
  _labelmap.insert(Mode_RomaInput,   QString(tr("ロ-マ")));
  _labelmap.insert(Mode_KanaInput,   QString(tr("かな ")));
  _labelmap.insert(Mode_DirectInput, QString(tr("_A")));

  _iconimgpathmap.insert(Mode_Hiragana,    Config::imageDirPath() + "/trayicon_hiragana.png");
  _iconimgpathmap.insert(Mode_Katakana,    Config::imageDirPath() + "/trayicon_katakana.png");
  _iconimgpathmap.insert(Mode_HankakuKana, Config::imageDirPath() + "/trayicon_hankakukana.png");
  _iconimgpathmap.insert(Mode_ZenkakuEisu, Config::imageDirPath() + "/trayicon_zenkakueisu.png");
  _iconimgpathmap.insert(Mode_DirectInput, Config::imageDirPath() + "/trayicon_directinput.png");

  // Sets tooltip
  _toolbtn1->setToolTip(tr("入力モード"));
  _toolbtn2->setToolTip(tr("入力方式"));
  _toolbtn3->setToolTip(tr("ツール"));
  _toolbtn4->setToolTip(tr("プロパティ"));

  // Sets tray icon
  setTrayIcon(Mode_DirectInput);

  // Sets size
  resize(QSize(142, 34));
  
  // signals and slots connections
  InputMethod* im = KimeraApp::inputmethod();
  connect(im, SIGNAL(triggerNotify(bool)), this, SLOT(slotTriggerNotify(bool)));
  connect(this, SIGNAL(triggerNotify(bool)), im, SLOT(setXIMInputtingEnabled(bool)));
  connect(this, SIGNAL(selected(const InputMode&)), im->kanjiConvert(), SLOT(setInputMode(const InputMode&)));
  connect(im->kanjiConvert(), SIGNAL(inputModeChanged(const InputMode&)), this, SLOT(update(const InputMode&)));
  connect(_pupmenu1, SIGNAL(triggered(QAction*)), this, SLOT(setInputModeButtonText(QAction*)));
  connect(_pupmenu2, SIGNAL(triggered(QAction*)), this, SLOT(setInputStyleButtonText(QAction*)));
  connect(_toolbtn4, SIGNAL(clicked()), this, SLOT(showPropertyDialog()));
  connect(_leftmover,  SIGNAL(mouseMoved(const QPoint&)), this, SLOT(move(const QPoint&)));
  connect(_leftmover,  SIGNAL(mouseMoveStopped(const QPoint&)), this, SLOT(savePos()));
  connect(im->kanjiConvert(), SIGNAL(dictToolActivated()), this, SLOT(execDictTool()));
  connect(im->kanjiConvert(), SIGNAL(propertyDialogActivated()), this, SLOT(showPropertyDialog()));
  connect(_propdlg, SIGNAL(settingChanged()), this, SLOT(initIM()));
  connect(_handwriting, SIGNAL(decided(const QString&)), this, SLOT(slotDecided(const QString&)));
  connect(this, SIGNAL(decided(const QString&)), KimeraApp::inputmethod()->kanjiConvert(), SIGNAL(decideSegments(const QString&)));

  // Save Default setting
  PropertyDialog::saveDefaultSetting();
  KeyAssigner::saveDefaultSetting();

  // Sets default value
  int p = _modemap.value( Config::readEntry("_cmbinputmode", "") );
  if (p > 0) {
    setButton1Text( p );   // Default value
  }
  setButton1Text( Mode_DirectInput );
  setTrayIcon( Mode_DirectInput );

  p = _modemap.value( Config::readEntry("_cmbinputstyle", "") );
  if (p > 0) {
    setButton2Text( p );
  }

  QTimer::singleShot(0, this, SLOT(redraw()));
  // Redraw continuously
  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(redraw()));
  timer->start(5000);
}


MainToolBar::~MainToolBar()
{
  // Do nothing
}


void
MainToolBar::redraw()
{
  DEBUG_TRACEFUNC();
  if ( isVisibleSetting() ) {
    show();
    raise();
  } else {
    hide();
  }

  // Tray Icon
  if ( isTrayIconVisibleSetting() ) {
    _trayicon->show();
  } else {
    _trayicon->hide();
  }
}


void
MainToolBar::initIM()
{
  DEBUG_TRACEFUNC();
  redraw();
  KimeraApp::inputmethod()->kanjiConvert()->init();
}


void
MainToolBar::setInputModeButtonText(QAction* action)
{
  DEBUG_TRACEFUNC("action: %s", qPrintable(action->text()));
  action->setChecked(true);
  int id = _modemap.value( action->text() );
  setButton1Text( id );
  setTrayIcon( id );
}


void
MainToolBar::setInputStyleButtonText(QAction* action)
{
  DEBUG_TRACEFUNC("action: %s", qPrintable(action->text()));
  action->setChecked(true);
  int id = _modemap.value( action->text() );
  setButton2Text( id );
}


void
MainToolBar::setButton1Text(int id)
{
  DEBUG_TRACEFUNC("id: 0x%04x", id);

  id &= Mode_ModeMask | Mode_DirectInput;
  QString str = _modemap.key(id);
  if ( str.isEmpty() )
    return;

  // Sets check-flag to popup menu
  foreach (QAction* act, _pupmenu1->actions()) {
    act->setChecked( (act->text() == str) );
  }

  QString p = _labelmap.value(id);
  Q_ASSERT( !p.isEmpty() );
  _toolbtn1->setText(p);

  if ( !(id & Mode_DirectInput) )
    emit selected(InputMode(id));

  slotTriggerNotify( !(id & Mode_DirectInput) );
}


void
MainToolBar::setButton2Text(int id)
{
  DEBUG_TRACEFUNC("id: 0x%04x", id);

  id &= Mode_InputMask;
  QString str = _modemap.key(id);
  if ( str.isEmpty() )
    return;

  // Sets check-flag to popup menu
  foreach (QAction* act, _pupmenu2->actions()) {
    act->setChecked( (act->text() == str) );
  }

  QString p = _labelmap.value(id);
  Q_ASSERT( !p.isEmpty() );
  _toolbtn2->setText(p);

  emit selected(InputMode(id));
}


void
MainToolBar::setTrayIcon(int id)
{
  DEBUG_TRACEFUNC("id: %d", id); 
  QString imgpath = _iconimgpathmap.value( id & (Mode_ModeMask | Mode_DirectInput) );
  if (!imgpath.isEmpty()) {
    _trayicon->setIcon(QIcon(imgpath));
  }
  
  if ( isTrayIconVisibleSetting() ) {
    _trayicon->show();
  } else {
    _trayicon->hide();
  }
}


void
MainToolBar::slotTriggerNotify(bool b)
{
  DEBUG_TRACEFUNC("b: %d", b); 
  static int prev_id = 0;

  // Check on/off changed
  int id = b ? KimeraApp::inputmethod()->kanjiConvert()->inputMode().id() : Mode_DirectInput;
  if ((id & Mode_DirectInput) == (prev_id & Mode_DirectInput)) {
    return;
  } 

  prev_id = id;
  setButton1Text( id );
  setTrayIcon( id );
  emit triggerNotify( b );
  
  if ( !isVisibleSetting() ) {
    hide();
  } else {
    clearFocus();
    show();
    raise();
  }
}


void
MainToolBar::execDictTool()
{
  DEBUG_TRACEFUNC();

  static QProcess* proc = 0;
  if (!proc) {
    proc = new QProcess(this);
    proc->setWorkingDirectory(QDir::homePath());

    QStringList env = QProcess::systemEnvironment();
    env << "XMODIFIERS=@im=kimera";
    proc->setEnvironment( env );
  }
  if (proc->state() != QProcess::Running) {
    QStringList arguments = Config::readEntry("_cmbcmd", "").split(' ', QString::SkipEmptyParts);
    if (!arguments.isEmpty()) {
      QString cmd = arguments.takeFirst();
      proc->start(cmd, arguments);
    } else {
      QMessageBox::warning(this, "Empty command",
                           tr("辞書ツールが設定されていません。\n"
                              "プロパティからコマンドを設定してください。"),
                           QMessageBox::Ok | QMessageBox::Default, 0);
    }
  }
}


void
MainToolBar::execHandWritingTool()
{
  DEBUG_TRACEFUNC();

  slotTriggerNotify( true );
  if ( !_handwriting->init() ) {
    QMessageBox::warning(this, "Execution failed",
                         tr("手書き認識ツール（kimera-tomoe-gtk）を起動できませんでした"),
                         QMessageBox::Ok | QMessageBox::Default, 0);
  }
}


void
MainToolBar::slotDecided(const QString& string)
{
  DEBUG_TRACEFUNC("string: %s", qPrintable(string));

  if ( !KimeraApp::isXIMInputtingEnabled() )
    slotTriggerNotify( true );

  emit decided(string);
}


void
MainToolBar::showPropertyDialog()
{
  DEBUG_TRACEFUNC();
  _propdlg->show();
  _propdlg->raise();
}


bool
MainToolBar::isVisibleSetting()
{
  DEBUG_TRACEFUNC();
  return (KimeraApp::isXIMInputtingEnabled() || !Config::readBoolEntry("_chkdispbar")) && !isTrayIconVisibleSetting();
}


bool
MainToolBar::isTrayIconVisibleSetting()
{
  DEBUG_TRACEFUNC();
  return QSystemTrayIcon::isSystemTrayAvailable() && Config::readBoolEntry("_chktrayicon", false);
}


void
MainToolBar::savePos()
{
  DEBUG_TRACEFUNC();
  Config::writeEntry("point_x", x());
  Config::writeEntry("point_y", y());
  qDebug("saved (%d, %d)", x(), y());
}


QPoint
MainToolBar::loadPos() const
{
  DEBUG_TRACEFUNC();

  int x = Config::readNumEntry("point_x", -1);
  int y = Config::readNumEntry("point_y", -1);
  if (x < 0 || y < 0) {
    x = QApplication::desktop()->screen(0)->width() - width() - 50;
    y = QApplication::desktop()->screen(0)->height() - 120;
  }
  
  x = qMin(qMax(x, 0), QApplication::desktop()->width() - width());
  y = qMin(qMax(y, 0), QApplication::desktop()->height() - height()); 
  return QPoint(x, y);
}


void
MainToolBar::update(const InputMode& mode)
{
  DEBUG_TRACEFUNC("mode: 0x%04x", mode.id());
  setButton1Text(mode.id());
  setTrayIcon(mode.id());
  setButton2Text(mode.id());
  slotTriggerNotify( !(mode.id() & Mode_DirectInput) ); 
}


void
MainToolBar::move(const QPoint& pos)
{
  DEBUG_TRACEFUNC();
  QWidget::move(pos);
}


/**
 * Mover class
 */
Mover::Mover(QWidget* parent) : QToolButton(parent)
{
  setMaximumWidth(14);
  setAutoRaise(true);
}


void
Mover::mousePressEvent(QMouseEvent* e)
{
  _p = e->globalPos();
  if ( topLevelWidget() ) {
    _p -= topLevelWidget()->pos();
  }
  
  QToolButton::mousePressEvent(e);
}


void
Mover::mouseReleaseEvent(QMouseEvent* e)
{
  emit mouseMoveStopped(e->globalPos() - _p);
  QToolButton::mouseReleaseEvent(e);
}


void
Mover::mouseMoveEvent(QMouseEvent* e) 
{
  emit mouseMoved(e->globalPos() - _p);
  QToolButton::mouseMoveEvent(e);
}


void
Mover::enterEvent(QEvent* e)
{
  QApplication::setOverrideCursor( QCursor(Qt::SizeAllCursor) );
  QToolButton::enterEvent(e);
}


void
Mover::leaveEvent(QEvent* e)
{
  QApplication::restoreOverrideCursor();
  QToolButton::leaveEvent(e);
}
