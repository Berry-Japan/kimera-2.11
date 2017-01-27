#ifndef PREEDITAREA_H
#define PREEDITAREA_H

#include <QWidget>

class QLabel;

class PreeditArea : public QWidget {
  Q_OBJECT

public:
  PreeditArea(QWidget* parent = 0);
  virtual ~PreeditArea() { }

  void setFont(const QFont &);
  void showInputingString(const QString& str, int caret_pos);
  void showConvertingSegments(const QStringList& strlist, int attention=0);
  void showChangingSegmentLength(const QStringList& strlist, int attention=0);
  void movePreeditPos(const QPoint& pos);
  QString text() const;
  QString backwardText() const;
  QString attentionText() const;
  QString forwardText() const;

public slots:
  void init();
  void adjustSize();
  void hide();

signals:
  void listPointChanged(const QPoint& pos);

protected:
  void showText(const QStringList& strlist, int attention, int caret_pos=0);
  void readColorSetting();

private:
  enum State {
    Input = 0,
    Attention,
    Converted,
    Changing,
    NumStates,
  };

  enum {
    FgInput = 0,      // 入力文字色
    BgInput,          // 入力背景色 
    FgAttention,      // 注目文節文字色
    BgAttention,      // 注目文節背景色
    FgConverted,      // 変換済み文節文字色
    BgConverted,      // 変換済み文節背景色
    FgChanging,       // 文節長変更文字色
    BgChanging,       // 文節長変更背景色
    NumColors,
  };

  QLabel*  _attentseg;        // 変換済み注目文節
  QLabel*  _backseg;          // 変換済み非注目文節左
  QLabel*  _forwardseg;       // 変換済み非注目文節右
  QLabel*  _caret;            // キャレット
  QColor   _colorlist[NumColors];
  QFont    _font[NumStates];
  State    _state;
};

#endif // PREEDITAREA_H
