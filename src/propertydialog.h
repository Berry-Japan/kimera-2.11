#include "ui_propertydialog.h"

class KeyAssigner;

class PropertyDialog : public QDialog {
  Q_OBJECT
    
public:
  PropertyDialog(QWidget* parent = 0);
  virtual ~PropertyDialog() {}

  static void saveDefaultSetting();

public slots:
  void show();

signals:
  void settingChanged();

protected:
  void hideEvent(QHideEvent* e);

protected slots:
  void loadSetting();
  bool saveSetting();
  void changeColor(int row, int col);
  void execKeyAssiner();
  void loadColorSetting(int index);
  void saveColorSetting();
  void slotKanjiEngineActivated(const QString& string);
  void slotFileSelection();
  void accept();
  void insertItemUserDefined();
  void removeItemUserDefined();
 
private:
  Ui::PropertyDialog  _ui;
  KeyAssigner*        _kassign;
};
