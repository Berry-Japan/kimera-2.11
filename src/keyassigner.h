#include "ui_keyassigner.h"
#include "kanjiconvert.h"
#include <QDialog>

class KeyAssigner : public QDialog {
  Q_OBJECT

public:
  KeyAssigner( QWidget* parent = 0 );
  virtual ~KeyAssigner() {}
  
  static QString functionName( FuncID id );
  static FuncID functionID( const QString & func );
  static FuncID functionID( int key, ConvStatus stat );

public slots:
  void   init();
  static void saveDefaultSetting();

protected slots:
  void    saveSetting();
  void    loadSetting( int index );
  virtual void accept();
  virtual void reject();

protected:
  void    setKeySequenceName(int row, const QKeySequence& keyseq);

private:
  Ui::KeyAssigner _ui;
};
