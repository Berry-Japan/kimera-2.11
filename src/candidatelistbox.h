#ifndef CANDIDATELISTBOX_H
#define CANDIDATELISTBOX_H

#include <QFrame>
#include <QStringList>
#include <QPoint>

class QListWidget;
class QListWidgetItem;
class QLabel;
class QShowEvent;
class QHideEvent;

class CandidateListBox : public QFrame {
  Q_OBJECT

public:
  enum {
    MAX_VISIBLE_ITEMS = 9,
  };
  
  CandidateListBox(QWidget* parent = 0);

  void showList(const QStringList& strs, int crntidx = 0);
  void selectCandidate(int idx);
  void selectPreviousCandidate(uint decrement = 1);
  void selectNextCandidate(uint increment = 1);
  void setFont(const QFont&);
  QString currentText() const;
  QString text(int index) const;
  int  currentItem() const;
  int  visibleTopItem() const;
  int  visibleBottomItem() const;
  uint count() const;
  void scrollToItemAtTop(int index);

  bool isListVisible() const;
  bool isPredictionsVisible() const;
  QString candidatePredicted() const;

public slots:
  void setPos(const QPoint& p);
  void showCandidatePredicted(const QString& str);

signals:
  void clicked(QListWidgetItem* item);

protected:
  void showEvent(QShowEvent *);
  void hideEvent(QHideEvent *);

protected slots:
  void setLabelValue(int d);

private:
  QListWidget* _lbl;
  QListWidget* _lstbox;
  QLabel*      _crntitemno;
  QLabel*      _predictxt;
  QPoint       _pos;
};

#endif // CANDIDATELISTBOX_H
