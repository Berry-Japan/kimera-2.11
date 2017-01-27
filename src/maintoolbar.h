#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include "kimeraapp.h"
#include "kanjiconvert.h"
#include <QFrame>
#include <QMenu>
#include <QToolButton>

class InputMethod;
class InputMode;
class PropertyDialog;
class Mover;
class Popup;
class QAction;
class KanjiEngine;
class QSystemTrayIcon;

/**
 * 
 */
class MainToolBar : public QFrame {
  Q_OBJECT
  
public:
  MainToolBar();
  ~MainToolBar();

  QPoint loadPos() const;
  bool  isVisibleSetting();
  bool  isTrayIconVisibleSetting();

public slots:
  void  slotTriggerNotify(bool);
  void  initIM();
  void  savePos();
  void  update(const InputMode&);
  void  move(const QPoint& pos);

signals:
  void  selected(const InputMode&);
  void  triggerNotify(bool);
  void  decided(const QString&);

protected slots:
  void  redraw();
  void  setButton1Text(int id);
  void  setButton2Text(int id);
  void  setTrayIcon(int id);
  void  setInputModeButtonText(QAction* action);
  void  setInputStyleButtonText(QAction* action);
  void  execDictTool();
  void  execHandWritingTool();
  void  slotDecided(const QString& string);
  void  showPropertyDialog();

private:
  Mover*              _leftmover;
  QToolButton*        _toolbtn1;
  QToolButton*        _toolbtn2;
  QToolButton*        _toolbtn3;
  QToolButton*        _toolbtn4;
  Popup*              _pupmenu1;
  Popup*              _pupmenu2;
  Popup*              _pupmenu3;
  PropertyDialog*     _propdlg;
  QActionGroup*       _inputmodegrp;
  QActionGroup*       _inputstylegrp;
  QHash<QString, int> _modemap;
  QHash<int, QString> _labelmap;
  QHash<int, QString> _iconimgpathmap;
  KanjiEngine*        _handwriting;
  QSystemTrayIcon*    _trayicon;
  Popup*              _trayiconmenu;
};


/**
 *
 */
class Mover : public QToolButton {
  Q_OBJECT
    
public:
  Mover(QWidget* parent);

signals:
  void mouseMoved(const QPoint& pos);
  void mouseMoveStopped(const QPoint& pos);

protected:
  void mousePressEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void enterEvent(QEvent* e);
  void leaveEvent(QEvent* e);

private:
  QPoint _p;
};


/**
 *
 */
class Popup : public QMenu {
  Q_OBJECT

public:
  Popup(QWidget* parent=0) : QMenu(parent) { }

public slots:
  void hide() { clearFocus(); QMenu::hide(); }
};

#endif // MAINTOOLBAR_H
