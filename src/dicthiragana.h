#ifndef DICTHIRAGANA_H
#define DICTHIRAGANA_H

#include "kimeraglobal.h"
#include "inputmode.h"
#include <QString>
#include <QKeyEvent>
#include <QHash>

class DictHiragana {
public:
  static const QKeyEvent  SEPARATOR_CHAR;
  
  DictHiragana();
  ~DictHiragana();

  // �����
  void init();

  // ���� �� ��ߤ��� �Ѵ�
  QString convert(const QList<QKeyEvent>& kevlst, const InputMode& mode, bool tenkey_hankaku=false) const;

  // ��ߤ��� �� �������� ���Ѵ�  
  void reverse(const QString& yomi, const InputMode& mode, bool tenkey_hankaku, QList<QKeyEvent>& kevlst) const;

  // �Ҥ餬�� �� ��ߤ���(���޻�) �Ѵ�
  QString  convertString(const QString& hira, const InputMode& mode=InputMode()) const;
  
protected:
  void     initDict(QHash<QString, QString>& dict, const QString&, bool reverse_dict=false);
  QString  convertKey(const QKeyEvent& key, const InputMode& mode) const;  // ���� �� ��ߤ��� �Ѵ�
  QString  convertToHira(const QString&) const;    // �Ҥ餬���Ѵ�
  QString  convertToKata(const QString&) const;    // ���������Ѵ�
  QString  convertToHankakuKana(const QString& src) const;   // Ⱦ�ѥ����Ѵ�
  QString  convertToZenkakuEisu(const QString&) const;  // ���ѱѿ��Ѵ�
  QString  reverseConvt(const QString& hira) const;     // �Ҥ餬��(��������) �� ���޻� ���Ѵ�  
  QString  replaceDakutenChar(const QString& str) const;  // ����Ⱦ����ʸ���ִ�

  static void copyDictFile(const QString dicname);

private:
  QHash<QString, QString>   _dicthira;      // �Ҥ餬�ʼ���
  QHash<QString, QString>   _dictkata;      // �������ʼ���
  QHash<QString, QString>   _dicthankana;   // Ⱦ�ѥ��ʼ���
  QHash<QString, QString>   _dictalphbt;    // ���ѱѻ�����
  QHash<QString, QString>   _dictsymbol;    // ���ѿ������漭��
  QHash<QString, QString>   _dictkanainp;   // �������ϼ���
  QHash<QString, QString>   _dictdakuten;   // ����������������
  QHash<QString, QString>   _reversedict;   // �հ�������  
};

#endif // DICTHIRAGANA_H
