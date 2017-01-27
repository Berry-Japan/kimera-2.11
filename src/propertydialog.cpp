#include "propertydialog.h"
#include "kimeraglobal.h"
#include "kanjiengine.h"
#include "keyassigner.h"
#include "config.h"
#include "debug.h"
#include <QApplication>
#include <QRegExp>
#include <QColorDialog>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QHeaderView>
#include <QSystemTrayIcon>
#include <sys/utsname.h>
using namespace Kimera;

const int NUM_ROWS = 4;
const int NUM_COLS = 3;

static QString  ColorSetting[NUM_SETTING_TYPE][NUM_ROWS][NUM_COLS] = {
  { { "black",  "white", "1" },      // MSIME
    { "black",  "lightGray", "1" }, 
    { "black",  "white", "1" },
    { "white",  "blue",  "0" } },
  
  { { "blue",   "white", "0" },      // ATOK
    { "black",  "cyan",  "0" },
    { "blue",   "white", "0" },
    { "white",  "blue",  "0" } },
  
  { { "black",  "white", "1" },      // KINPUT2
    { "white",  "darkblue", "0" }, 
    { "black",  "white", "1" },
    { "white",  "darkblue", "0" } },
  
  { { "blue",   "gray",  "0" },      // VJE
    { "yellow", "black", "0" }, 
    { "yellow", "blue",  "0" },
    { "gray",   "blue",  "1" } },

  { { QString::null } },             // Current setting
};


PropertyDialog::PropertyDialog(QWidget* parent) : QDialog(parent)
{
  _ui.setupUi(this);
  
  _kassign = new KeyAssigner(this);
  _ui._colorsetting->setRowCount(NUM_ROWS);
  _ui._colorsetting->setColumnCount(NUM_COLS);
  // Sets label
  _ui._colorsetting->setHorizontalHeaderLabels(QStringList() << tr("ʸ����") << tr("�طʿ�") << tr("����"));
  _ui._colorsetting->setVerticalHeaderLabels(QStringList() << tr("����ʸ��") << tr("����ʸ��") << tr("�Ѵ��Ѥ�ʸ��") << tr("ʸ��Ĺ�ѹ�ʸ��"));

  _ui._colorsetting->verticalHeader()->setResizeMode(QHeaderView::Stretch);
  _ui._colorsetting->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

  _ui._cmbloadcolor->insertItem(ST_MSIME, tr("MS-IME��"));
  _ui._cmbloadcolor->insertItem(ST_ATOK, tr("ATOK��"));
  _ui._cmbloadcolor->insertItem(ST_KINPUT2, tr("Kinput2��"));
  _ui._cmbloadcolor->insertItem(ST_VJE, tr("VJE��"));

  QStringList strlst;
  strlst << tr("�ʤ�") << tr("����");
  for (int i = 0; i < NUM_ROWS; i++) {
    QComboBox* cmb = new QComboBox(_ui._colorsetting);
    cmb->addItems(strlst);
    _ui._colorsetting->setCellWidget(i, 2, cmb);
    connect(cmb, SIGNAL(activated(int)), this, SLOT(insertItemUserDefined()));
  }

  QString tip = tr("���ܸ����ϤΥ��󡿥��դ����ؤ��륭�������ꤷ�ޤ���\n����ε�ư����ͭ���Ǥ���");
  _ui._lblstartkey->setToolTip( tip );
  _ui._cmbstartkey->setToolTip( tip );
  tip = tr("��ư����ľ������ϥ⡼�ɤ����򤷤ޤ���");
  _ui._lblinputmode->setToolTip( tip );
  _ui._cmbinputmode->setToolTip( tip );
  tip = tr("��ư����ľ������������򡢥����޻�����\n�ޤ��Ϥ������Ϥ������򤷤ޤ���");
  _ui._lblinputstyle->setToolTip( tip );
  _ui._cmbinputstyle->setToolTip( tip );
  tip = tr("���ܸ����Ϥ����դλ��˥ġ���С���\n��ɽ���ˤ��뤫���ꤷ�ޤ���");
  _ui._chkdispbar->setToolTip( tip );

  tip = tr("���ڡ��������򲡤����������Ϥ����\n�����Ⱦ��/���Ѥ����ꤷ�ޤ���");
  _ui._lblspacekey->setToolTip( tip );
  _ui._cmbspacekey->setToolTip( tip );
  tip = tr("�ƥ󥭡��������Ϥ���������\n�����Ⱦ��/���Ѥ����ꤷ�ޤ���");
  _ui._lbltenkey->setToolTip( tip );
  _ui._cmbtenkey->setToolTip( tip );
  tip = tr("���������򤷤ޤ���");
  _ui._lbltouten->setToolTip( tip );
  _ui._cmbtouten->setToolTip( tip );
  tip = tr("���������򤷤ޤ���");
  _ui._lblkuten->setToolTip( tip );
  _ui._cmbkuten->setToolTip( tip );
  tip = tr("��̤����򤷤ޤ���");
  _ui._lblbracket->setToolTip( tip );
  _ui._cmbbracket->setToolTip( tip );
  tip = tr("��������򤷤ޤ���");

  _ui._lblsymbol->setToolTip( tip );
  _ui._cmbsymbol->setToolTip( tip );
  _ui._btnkeyassign->setToolTip(tr("�������������������ɽ�����ޤ���"));

  _ui._tab4->setToolTip(tr("�����ο������over-the-spot\n��������ξ���ͭ���Ǥ���"));
  _ui._cmbloadcolor->setToolTip(tr("�ꥹ�Ȥ������򤵤줿��������ɤ߹��ߤޤ���"));
  _ui._colorsetting->setToolTip(tr("���̤˿������ꤷ�ޤ���"));

  tip = tr("���ʴ����Ѵ����󥸥�����򤷤ޤ���");
  _ui._lblkanjisys->setToolTip( tip );
  _ui._cmbkanjieng->setToolTip( tip );
  tip = tr("����ͽ¬���󥸥�����򤷤ޤ���");
  _ui._lblpredict->setToolTip( tip );
  _ui._cmbpredict->setToolTip( tip );
  tip = tr("���ʴ����Ѵ������Ф�IP���ɥ쥹\n�ޤ��ϥۥ���̾�����ꤷ�ޤ���");
  _ui._lblsvrname->setToolTip( tip );
  _ui._edtsvrname->setToolTip( tip );
  tip = tr("��³����ݡ����ֹ�����ꤷ�ޤ���");
  _ui._lblport->setToolTip( tip );
  _ui._edtport->setToolTip( tip );
  tip = tr("����ġ���Υ��ޥ�ɤ����ꤷ�ޤ�");
  _ui._grpcmd->setToolTip( tip );
  _ui._cmbcmd->setToolTip( tip );

  // Gets system information
  QString sysname;
  struct utsname uts;
  if (uname(&uts) == 0) {
    sysname = QString("<p align=\"center\">Qt %1 / %2 %3</p>").arg(qVersion()).arg(uts.sysname).arg(uts.release);
  }

  _ui._lblaboutkimera->setText(tr("<p align=\"center\"><font size=\"+3\"><i>Kimera</i></font><br>"
                                  "Version " KIMERA_VERSION "</p>"
                                  "%1"
                                  "<p align=\"center\">Copyright (c) 2003-2009  AOYAMA Kazz<br>"
                                  "<a href=\"http://kimera.sourceforge.jp/\">http://kimera.sourceforge.jp/</a></p>"
                                  "<p align=\"center\">This program is distributed under the terms of "
                                  "the GNU GENERAL PUBLIC LICENSE Version 3.</p>").arg(sysname));
  
  // Sets Kanji engine name
  QStringList englst = KanjiEngine::kanjiEngineList();
  QStringListIterator it(englst);
  while (it.hasNext()) {
    QString str = it.next();
    KanjiEngine* eng = KanjiEngine::kanjiEngine(str);
    if ( eng ) {
      if ( eng->isKanjiConvEnabled() )
	_ui._cmbkanjieng->addItem(str);

      if ( eng->isPredictionEnabled() )
	_ui._cmbpredict->addItem(str);
    }
  }

  // Creates connections
  connect(_ui._cancelbtn, SIGNAL(clicked()), this, SLOT(reject()));
  connect(_ui._okbtn, SIGNAL(clicked()), this, SLOT(accept()));
  connect(_ui._btnkeyassign, SIGNAL(clicked()), this, SLOT(execKeyAssiner()));
  connect(_ui._cmbloadcolor, SIGNAL(activated(int)), this, SLOT(loadColorSetting(int)));
  connect(_ui._cmbkanjieng, SIGNAL(activated(QString)), this, SLOT(slotKanjiEngineActivated(QString)));
  connect(_ui._cmdselectbtn, SIGNAL(clicked()), this, SLOT(slotFileSelection()));
  connect(_ui._colorsetting, SIGNAL(cellClicked(int,int)), this, SLOT(changeColor(int,int)));
  connect(_ui._chktrayicon, SIGNAL(toggled(bool)), _ui._chkdispbar, SLOT(setDisabled(bool)));
}


void PropertyDialog::loadSetting()
{
  DEBUG_TRACEFUNC();
  _ui._cmbstartkey->setCurrentIndex(_ui._cmbstartkey->findText(Config::readEntry("_cmbstartkey",
                                                                                 _ui._cmbstartkey->currentText())));
  _ui._cmbinputmode->setCurrentIndex(_ui._cmbinputmode->findText(Config::readEntry("_cmbinputmode", 
                                                                                   _ui._cmbinputmode->currentText())));
  _ui._cmbinputstyle->setCurrentIndex(_ui._cmbinputstyle->findText(Config::readEntry("_cmbinputstyle", 
                                                                                     _ui._cmbinputstyle->currentText())));
  _ui._chkdispbar->setChecked(Config::readBoolEntry("_chkdispbar", false));
  _ui._chktrayicon->setChecked(Config::readBoolEntry("_chktrayicon", false));
  
  _ui._cmbspacekey->setCurrentIndex(_ui._cmbspacekey->findText(Config::readEntry("_cmbspacekey",
                                                                                 _ui._cmbspacekey->currentText())));
  _ui._cmbtenkey->setCurrentIndex(_ui._cmbtenkey->findText(Config::readEntry("_cmbtenkey", 
                                                                             _ui._cmbtenkey->currentText())));
  _ui._cmbkuten->setCurrentIndex(_ui._cmbkuten->findText(Config::readEntry("_cmbkuten", 
                                                                           _ui._cmbkuten->currentText())));
  _ui._cmbtouten->setCurrentIndex(_ui._cmbtouten->findText(Config::readEntry("_cmbtouten", 
                                                                             _ui._cmbtouten->currentText())));
  _ui._cmbsymbol->setCurrentIndex(_ui._cmbsymbol->findText(Config::readEntry("_cmbsymbol", 
                                                                             _ui._cmbsymbol->currentText())));
  _ui._cmbbracket->setCurrentIndex(_ui._cmbbracket->findText(Config::readEntry("_cmbbracket", 
                                                                               _ui._cmbbracket->currentText())));
  
  _ui._cmbkanjieng->setCurrentIndex(_ui._cmbkanjieng->findText(Config::readEntry("_cmbkanjieng", 
                                                                                 _ui._cmbkanjieng->currentText())));
  slotKanjiEngineActivated(_ui._cmbkanjieng->currentText());
  _ui._grpremote->setChecked(Config::readBoolEntry("_grpremote", _ui._grpremote->isChecked()));
  _ui._edtsvrname->setText(Config::readEntry("_edtsvrname", _ui._edtsvrname->text()));
  _ui._edtport->setText(Config::readEntry("_edtport", _ui._edtport->text()));
  _ui._grppredict->setChecked(Config::readBoolEntry("_grppredict",
                                                    _ui._grppredict->isChecked()));
  _ui._cmbpredict->setCurrentIndex(_ui._cmbpredict->findText(Config::readEntry("_cmbpredict",
                                                                               _ui._cmbpredict->currentText())));
  
  _ui._cmbcmd->setCurrentIndex(_ui._cmbcmd->findText(Config::readEntry("_cmbcmd", "")));
  if (!_ui._cmbcmd->currentText().isEmpty()) {
    int i;
    for (i = 0; i < _ui._cmbcmd->count(); ++i)
      if (_ui._cmbcmd->itemText(i) == _ui._cmbcmd->currentText())
	break;
    
    if (i == _ui._cmbcmd->count())
      _ui._cmbcmd->insertItem(0, _ui._cmbcmd->currentText());
  }
  
  int colorset = Config::readNumEntry("_cmbloadcolor", ST_MSIME);
  if (colorset == ST_CURRENT_SETTING) {
    insertItemUserDefined();
  } else {
    removeItemUserDefined();
  }
  _ui._cmbloadcolor->setCurrentIndex(colorset);
  loadColorSetting( ST_CURRENT_SETTING );    // Loads color setting
}


bool PropertyDialog::saveSetting()
{
  DEBUG_TRACEFUNC();
  // Check entry
  if (_ui._grpremote->isEnabled() && _ui._grpremote->isChecked()) {
    if (_ui._edtsvrname->text().isEmpty() || _ui._edtport->text().isEmpty()) {
      QMessageBox::warning(this, "Incorrect entry", 
			   "Incorrect entry!\nInput Kanji server and port correctly.",
			   QMessageBox::Ok | QMessageBox::Default, 0);

      _ui._tabwdg->setCurrentWidget(_ui._tab3);  // show page
      return false;
      
    } else if ( !_ui._edtport->text().contains(QRegExp("^[0-9]{4,5}$")) ) {
      QMessageBox::warning(this, "Incorrect port number", 
			   "Incorrect port number!\nInput numerical string, 4 or 5 characters.",
			   QMessageBox::Ok | QMessageBox::Default, 0);
     
      _ui._tabwdg->setCurrentWidget(_ui._tab3);  // show page
      return false;
    }
  }
  
  // Saves setting
  bool restart = (_ui._cmbstartkey->currentText() != Config::readEntry("_cmbstartkey", tr("Zenkaku_Hankaku")));
  Config::writeEntry("_cmbstartkey", _ui._cmbstartkey->currentText());
  Config::writeEntry("_cmbinputmode", _ui._cmbinputmode->currentText());
  Config::writeEntry("_cmbinputstyle", _ui._cmbinputstyle->currentText());
  Config::writeEntry("_chkdispbar", _ui._chkdispbar->isChecked());
  Config::writeEntry("_chktrayicon", _ui._chktrayicon->isChecked());

  Config::writeEntry("_cmbspacekey", _ui._cmbspacekey->currentText());
  Config::writeEntry("_cmbtenkey", _ui._cmbtenkey->currentText());
  Config::writeEntry("_cmbkuten", _ui._cmbkuten->currentText());
  Config::writeEntry("_cmbtouten", _ui._cmbtouten->currentText());
  Config::writeEntry("_cmbsymbol", _ui._cmbsymbol->currentText());
  Config::writeEntry("_cmbbracket", _ui._cmbbracket->currentText());
  
  Config::writeEntry("_cmbkanjieng", _ui._cmbkanjieng->currentText());
  Config::writeEntry("_grpremote", _ui._grpremote->isChecked());
  Config::writeEntry("_edtsvrname", _ui._edtsvrname->text());
  Config::writeEntry("_edtport", _ui._edtport->text());
  Config::writeEntry("_grppredict", _ui._grppredict->isChecked());
  Config::writeEntry("_cmbpredict", _ui._cmbpredict->currentText());
  
  Config::writeEntry("_cmbcmd", _ui._cmbcmd->currentText());

  Config::writeEntry("_cmbloadcolor", _ui._cmbloadcolor->currentIndex());
  saveColorSetting();
  qDebug("Saved settings");

  if (restart) {
    QMessageBox::information(this, "Restart Kimera", 
			     tr("�����ȿ�Ǥ����뤿��� Kimera �κƵ�ư��ɬ�פǤ���\n"
				"Kimera ��Ƶ�ư���Ƥ���������"),
			     QMessageBox::Ok | QMessageBox::Default, 0);
  }
  
  emit settingChanged();
  return true;
}


void PropertyDialog::changeColor(int row, int col) 
{
  DEBUG_TRACEFUNC("row:%d col:%d", row, col);
  if (col > 1)
    return;

  QColor color( Qt::white );
  QTableWidgetItem* item = _ui._colorsetting->item(row, col);
  if ( item ) {
    QColor c(item->background().color().name());
    if ( c.isValid() ) {
      color = c;    // default color
    }
  } else {
    item = new QTableWidgetItem();
  }
  
  // Select color
  color = QColorDialog::getColor(color, this);
  if ( color.isValid() ) {
    item->setBackground( QBrush(color) );
    _ui._colorsetting->setItem(row, col, item);
    qDebug("get color name: %s", qPrintable(color.name()));
    insertItemUserDefined();
  }

  // Sets the non-selected state 
  item->setSelected( false );
}

void PropertyDialog::execKeyAssiner()
{
  DEBUG_TRACEFUNC();
  _kassign->show();
  _kassign->raise();
}


void PropertyDialog::loadColorSetting( int index )
{
  DEBUG_TRACEFUNC("index: %d", index);

  QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
  
  int i = 0, j = 0;
  QTableWidgetItem* item = 0;
  switch ( index ) {
  case ST_CURRENT_SETTING:         // Current setting
    // Loads Color table
    for (i = 0; i < NUM_ROWS; i++) {
      for (j = 0; j < NUM_COLS - 1; j++) {
        QColor color(Config::readEntry("_colorsettingcolor" + QString::number(i) + '-' + QString::number(j), "white"));
        if ( color.isValid() ) {
          item =  _ui._colorsetting->item(i, j);
          if ( !item ) {
            item = new QTableWidgetItem();
          }
          item->setBackground( QBrush(color) );
          _ui._colorsetting->setItem(i, j, item);
          qDebug("set color row: %d  col: %d  name: %s", i, j, qPrintable(color.name()));
        }
      }

      QComboBox* cmb = (QComboBox*)_ui._colorsetting->cellWidget(i, j);
      QString line = Config::readEntry(QString("_colorsettingunderline") + QString::number(i), "0");
      cmb->setCurrentIndex(line.toInt());
    }
    break;

  case ST_ATOK:
  case ST_KINPUT2:
  case ST_MSIME:
  case ST_VJE:
    // Loads Color table
    for (i = 0; i < NUM_ROWS; i++) {
      for (j = 0; j < NUM_COLS - 1; j++) {
        QColor color( ColorSetting[index][i][j] );
        if ( color.isValid() ) {
          item =  _ui._colorsetting->item(i, j);
          if ( !item ) {
            item = new QTableWidgetItem();
          }
          item->setBackground( QBrush(color) );
          _ui._colorsetting->setItem(i, j, item);
          qDebug("set color row: %d  col: %d  name: %s", i, j, qPrintable(color.name()));
        }
      }
      
      QComboBox* cmb = (QComboBox*)_ui._colorsetting->cellWidget(i, j);
      cmb->setCurrentIndex((ColorSetting[index][i][j]).toInt());
      qDebug("set color row: %d  col: %d  name: %s", i, j, qPrintable(ColorSetting[index][i][j]));
    }

    // Remove item user-defined
    if (Config::readNumEntry("_cmbloadcolor", 0) != ST_CURRENT_SETTING) {
      removeItemUserDefined();
    }
    break;

  default:
    break;
  }
}


void PropertyDialog::saveColorSetting()
{
  DEBUG_TRACEFUNC();
  // Saves color table items
  int i, j;
  for (i = 0; i < NUM_ROWS; ++i) {
    for (j = 0; j < NUM_COLS - 1; ++j) {
      QTableWidgetItem* item = _ui._colorsetting->item(i, j);
      if ( item ) {
        Config::writeEntry(QString("_colorsettingcolor") + QString::number(i) + '-' + QString::number(j),
                           item->background().color().name());
      }
    }
      
    int underline = ((QComboBox*)_ui._colorsetting->cellWidget(i, j))->currentIndex();
    Config::writeEntry(QString("_colorsettingunderline") + QString::number(i),
                         QString::number(underline));
  }

  // Removes item, "User Definition".
  if (_ui._cmbloadcolor->currentIndex() != ST_CURRENT_SETTING)
    removeItemUserDefined();
}


void PropertyDialog::saveDefaultSetting()
{
  DEBUG_TRACEFUNC();

  Config::writeEntry("_cmbstartkey", tr("Kanji"), false);
  Config::writeEntry("_cmbinputmode", tr("�Ҥ餬��"), false);
  Config::writeEntry("_cmbinputstyle", tr("�����޻�����"), false);
  Config::writeEntry("_chkdispbar", true, false);
  Config::writeEntry("_chktrayicon", false, false);

  Config::writeEntry("_cmbspacekey", tr("���ϥ⡼�ɤ˽���"), false);
  Config::writeEntry("_cmbtenkey", tr("���ϥ⡼�ɤ˽���"), false);
  Config::writeEntry("_cmbtouten", tr("��"), false);
  Config::writeEntry("_cmbkuten", tr("��"), false);
  Config::writeEntry("_cmbsymbol", tr("��"), false);
  Config::writeEntry("_cmbbracket", tr("�֡�"), false);

  Config::writeEntry("_cmbkanjieng", tr( DEFAULT_KANJIENGINE ), false);
  Config::writeEntry("_grpremote", false, false);
  Config::writeEntry("_edtsvrname", QString::null, false);
  Config::writeEntry("_edtport", QString::null, false);
  Config::writeEntry("_grppredict", false, false);
  Config::writeEntry("_cmbpredict", tr("Anthy"), false);

  Config::writeEntry("_cmbcmd", QString::null, false);

  // Saves default color table items
  const int setting = ST_MSIME;
  Config::writeEntry("_cmbloadcolor", setting, false);
  int i, j;
  for (i = 0; i < NUM_ROWS; i++) {
    for (j = 0; j < NUM_COLS - 1; j++) {
      Config::writeEntry(QString("_colorsettingcolor") + QString::number(i) + '-' + QString::number(j),
			 ColorSetting[setting][i][j], false);
    }
    
    Config::writeEntry(QString("_colorsettingunderline") + QString::number(i), 
		       ColorSetting[setting][i][j], false);
  }
}


void PropertyDialog::slotKanjiEngineActivated( const QString & string )
{
  DEBUG_TRACEFUNC();

  KanjiEngine* eng = KanjiEngine::kanjiEngine(string);
  if (eng) {
    _ui._grpremote->setEnabled(eng->isTCPConnectionSupported());
  } else {
    _ui._grpremote->setEnabled(false);
  }
}


void PropertyDialog::slotFileSelection()
{
  DEBUG_TRACEFUNC();

  QFileInfo finf(_ui._cmbcmd->currentText());
  QString dir = finf.exists() ? finf.absoluteDir().path() : QDir::homePath();  
  for (;;) {
    QString s = QFileDialog::getOpenFileName(this, "Select a file to execute", dir);
    if (s.isEmpty())
      break;

    QFileInfo f(s);
    if (f.isExecutable()) {
      _ui._cmbcmd->insertItem(0, s);
      break;
    } else {
      QMessageBox::warning(this, "File selection error",
			   tr("�ե�����˼¹Ը��¤�����ޤ���\n") + s);
      dir = f.absoluteDir().path();
    }
  }
}


void PropertyDialog::accept()
{
  DEBUG_TRACEFUNC();
  if ( saveSetting() )
    QDialog::accept();
}


void PropertyDialog::insertItemUserDefined()
{
  DEBUG_TRACEFUNC();
  if (_ui._cmbloadcolor->count() == ST_CURRENT_SETTING)
    _ui._cmbloadcolor->insertItem(ST_CURRENT_SETTING, tr("�桼�����"));
  _ui._cmbloadcolor->setCurrentIndex(ST_CURRENT_SETTING);
}


void PropertyDialog::removeItemUserDefined()
{
  DEBUG_TRACEFUNC();
  if (_ui._cmbloadcolor->count() > ST_CURRENT_SETTING)
    _ui._cmbloadcolor->removeItem(ST_CURRENT_SETTING);
}


void PropertyDialog::show()
{
  DEBUG_TRACEFUNC();
  loadSetting();

  int x = Config::readNumEntry(QString("x_PropertyDialog"), (QApplication::desktop()->screen(0)->width() - width()) / 2);
  int y = Config::readNumEntry(QString("y_PropertyDialog"), (QApplication::desktop()->screen(0)->height() - height()) / 2);
  x = qMin(qMax(x, 0), QApplication::desktop()->screen(0)->width() - width());
  y = qMin(qMax(y, 0), QApplication::desktop()->screen(0)->height() - height());
  move(x, y);

  QDialog::show();
}


void PropertyDialog::hideEvent(QHideEvent* e)
{
  DEBUG_TRACEFUNC();
  Config::writeEntry(QString("x_PropertyDialog"), x());
  Config::writeEntry(QString("y_PropertyDialog"), y());
  QDialog::hideEvent( e );
}