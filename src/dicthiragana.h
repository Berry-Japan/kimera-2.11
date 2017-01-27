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

  // 初期化
  void init();

  // 入力 → よみがな 変換
  QString convert(const QList<QKeyEvent>& kevlst, const InputMode& mode, bool tenkey_hankaku=false) const;

  // よみがな → キー入力 逆変換  
  void reverse(const QString& yomi, const InputMode& mode, bool tenkey_hankaku, QList<QKeyEvent>& kevlst) const;

  // ひらがな → よみがな(ローマ字) 変換
  QString  convertString(const QString& hira, const InputMode& mode=InputMode()) const;
  
protected:
  void     initDict(QHash<QString, QString>& dict, const QString&, bool reverse_dict=false);
  QString  convertKey(const QKeyEvent& key, const InputMode& mode) const;  // キー → よみがな 変換
  QString  convertToHira(const QString&) const;    // ひらがな変換
  QString  convertToKata(const QString&) const;    // カタカナ変換
  QString  convertToHankakuKana(const QString& src) const;   // 半角カナ変換
  QString  convertToZenkakuEisu(const QString&) const;  // 全角英数変換
  QString  reverseConvt(const QString& hira) const;     // ひらがな(カタカナ) → ローマ字 逆変換  
  QString  replaceDakutenChar(const QString& str) const;  // 濁点半濁点文字置換

  static void copyDictFile(const QString dicname);

private:
  QHash<QString, QString>   _dicthira;      // ひらがな辞書
  QHash<QString, QString>   _dictkata;      // カタカナ辞書
  QHash<QString, QString>   _dicthankana;   // 半角カナ辞書
  QHash<QString, QString>   _dictalphbt;    // 全角英字辞書
  QHash<QString, QString>   _dictsymbol;    // 全角数字記号辞書
  QHash<QString, QString>   _dictkanainp;   // かな入力辞書
  QHash<QString, QString>   _dictdakuten;   // かな入力濁点辞書
  QHash<QString, QString>   _reversedict;   // 逆引き辞書  
};

#endif // DICTHIRAGANA_H
