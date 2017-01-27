#include "dicthiragana.h"
#include "inputmode.h"
#include "config.h"
#include "debug.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QRegExp>
#include <QTextCodec>
#include <QList>
#include <QKeyEvent>
#include <QEvent>
using namespace Kimera;

const QKeyEvent DictHiragana::SEPARATOR_CHAR = QKeyEvent((QEvent::Type)QEvent::KeyPress, 0, 0, 0);

DictHiragana::DictHiragana()
      : _dicthira(QHash<QString, QString>()),
        _dictkata(QHash<QString, QString>()),
        _dicthankana(QHash<QString, QString>()),
        _dictalphbt(QHash<QString, QString>()),
        _dictsymbol(QHash<QString, QString>()),
        _dictkanainp(QHash<QString, QString>()),
        _dictdakuten(QHash<QString, QString>()),
      _reversedict(QHash<QString, QString>())
{
}


DictHiragana::~DictHiragana() { }


void
DictHiragana::init()
{
  DEBUG_TRACEFUNC();
  _dicthira.clear();
  _dictkata.clear();
  _dicthankana.clear();
  _dictalphbt.clear();
  _dictsymbol.clear();
  _dictkanainp.clear();
  _dictdakuten.clear();

  // Copy dictionary files
  copyDictFile("hiragana.dic");
  copyDictFile("katakana.dic");
  copyDictFile("hankakukana.dic");
  copyDictFile("zenkakualphabet.dic");
  copyDictFile("numeralsymbols.dic");
  copyDictFile("kanainput.dic");
  copyDictFile("dakuten.dic");

  QString dictdir = Config::dictionaryDirPath();
  initDict(_dicthira, dictdir + "/hiragana.dic");          // ひらがな辞書初期化
  initDict(_dictkata, dictdir + "/katakana.dic");          // カタカナ辞書初期化
  initDict(_dicthankana, dictdir + "/hankakukana.dic");    // 半角カナ辞書初期化
  initDict(_dictalphbt, dictdir + "/zenkakualphabet.dic"); // 全角英字辞書初期化
  initDict(_dictsymbol, dictdir + "/numeralsymbols.dic");  // 全角数字記号辞書初期化
  initDict(_dictkanainp, dictdir + "/kanainput.dic");      // かな入力辞書初期化
  initDict(_dictdakuten, dictdir + "/dakuten.dic");        // かな入力濁点辞書初期化

  // 句読点・記号・括弧
  _dictsymbol.insert(",", QString(Config::readEntry("_cmbtouten", ","))); 
  _dictsymbol.insert(".", QString(Config::readEntry("_cmbkuten", ".")));
  _dictsymbol.insert("/", QString(Config::readEntry("_cmbsymbol", "/")));
  _dictsymbol.insert("[", QString(Config::readEntry("_cmbbracket", "[").left(1)));
  _dictsymbol.insert("]", QString(Config::readEntry("_cmbbracket", "]").right(1)));

  // 逆引き辞書初期化
  initDict(_reversedict, dictdir + "/hiragana.dic", TRUE);
  initDict(_reversedict, dictdir + "/zenkakualphabet.dic", TRUE);
  initDict(_reversedict, dictdir + "/numeralsymbols.dic", TRUE);
  initDict(_reversedict, dictdir + "/hankakukana.dic", TRUE);
  initDict(_reversedict, dictdir + "/katakana.dic", TRUE);

  _reversedict.take(Config::readEntry("_cmbtouten", ","));
  _reversedict.insert(Config::readEntry("_cmbtouten", ","), QString(","));
  _reversedict.take(Config::readEntry("_cmbkuten", "."));
  _reversedict.insert(Config::readEntry("_cmbkuten", "."), QString("."));
  _reversedict.take(Config::readEntry("_cmbsymbol", "/"));
  _reversedict.insert(Config::readEntry("_cmbsymbol", "/"), QString("/")); 
  _reversedict.take(Config::readEntry("_cmbbracket", "[").left(1));
  _reversedict.insert(Config::readEntry("_cmbbracket", "[").left(1), QString("["));
  _reversedict.take(Config::readEntry("_cmbbracket", "]").right(1));
  _reversedict.insert(Config::readEntry("_cmbbracket", "]").right(1), QString("]")); 
  _reversedict.take(QObject::tr("゛"));
  _reversedict.insert(QObject::tr("゛"), QString("@"));
  _reversedict.take(QObject::tr("゜"));
  _reversedict.insert(QObject::tr("゜"), QString("["));
  _reversedict.take(QObject::tr("ﾞ"));
  _reversedict.insert(QObject::tr("ﾞ"), QString("@"));
  _reversedict.take(QObject::tr("ﾟ"));
  _reversedict.insert(QObject::tr("ﾟ"), QString("["));
}


void
DictHiragana::initDict(QHash<QString, QString>& dict, const QString& dictfile, bool reverse_dict)
{
  DEBUG_TRACEFUNC("dictfile:%s  reverse_dict:%d", qPrintable(dictfile), reverse_dict);
  QFile file(dictfile);
  if ( !file.open(QIODevice::ReadOnly) ) {
    QMessageBox::critical(0, "File open error", "Cannot open file: " + dictfile,
    			  QMessageBox::Ok | QMessageBox::Default, 0);
    qApp->quit();
    return;
  }
    
  QTextStream  stream(&file);
  stream.setCodec(QTextCodec::codecForName("eucJP"));
  while ( !stream.atEnd() ) {
    QString line = stream.readLine();
    if ( !line.isEmpty() && line.left(2) != "##" ) {
      QStringList list = line.split(QRegExp("[ \t]"), QString::SkipEmptyParts);
      if (list.count() > 1) {
        if ( !reverse_dict ) {
          if ( !dict.contains(list[0]) ) {
            dict.insert(list[0], list[1]);
          }
        } else {
          // reverse-dictionary
          if ( !dict.contains(list[1]) ) {
            dict.insert(list[1], list[0]);
          }
        }
      }
    }
  }
  
  file.close();
}


// 平仮名へ変換する (濁点を前の文字に合わせて１文字にする 例:か゛-> が )
// parameter:    Roma string (only ASCII characters) or Hiragana string
// return value: Hiragana string
QString
DictHiragana::convertToHira(const QString& src) const
{
  DEBUG_TRACEFUNC("src: %s", qPrintable(src));
  // check parameter
  if ( src.isEmpty() )
    return QString::null;

  if (src.toLocal8Bit().length() != src.length())
    return replaceDakutenChar(src);
  
  QString dest;
  int index = 0;
  while (index < src.length()) {
    for (int len = 4; len > 0; --len) {
      // ひらがな検索
      QString  str = _dicthira.value( src.mid(index, len).toLower() );  // case-insensitive
      if (!str.isEmpty()) {
	dest += str;
	index += len;
	break;
      } 

      // 全角数字記号検索
      str = _dictsymbol.value( src.mid(index, len) );
      if (!str.isEmpty()) {
	dest += str;
	index += len;
	break;
      }
      
      if (len == 1) {
	if (index + 2 < src.length() && src.at(index) == src.at(index + 1)
            && !src.mid(index, 1).toLower().contains( QRegExp("[aiueo]") )
            && src.mid(index + 2, 1).toLower().contains( QRegExp("[aiueoyhsw]") )) {
	  // 'っ'に変換
	  str = _dicthira.value( src.mid(index + 1, 2).toLower() );  // case-insensitive
	  if (!str.isEmpty()) {
	    dest += QObject::tr("っ") + str;
	    index += 3;
	  } else {
	    dest += QObject::tr("っ");
	    ++index;
	  }

	} else if (index + 1 < src.length() && src.at(index).toLower()  == 'n'
		   && !src.mid(index + 1, 1).toLower().contains( QRegExp("[aiueoy]") )) {
	  // 'ん'に変換
	  dest += QObject::tr("ん");
	  ++index;
	  
	} else {
	  // 無変換
	  dest += src.at(index);
	  ++index;
	}
	break;
      }
    }
  }

  return dest;
}


// parameter: Roma string (only ASCII characters) or Hiragana string
QString
DictHiragana::convertToKata(const QString& src) const
{
  DEBUG_TRACEFUNC("src: %s", qPrintable(src));
  Q_ASSERT(!_dictkata.isEmpty());

  QString dest;
  // check parameter
  QString hira = (src.toLocal8Bit().length() == src.length()) ? convertToHira(src) : replaceDakutenChar(src);
  for (int i = 0; i < (int)hira.length(); ++i) {
    QString str = _dictkata.value( hira.at(i) );
    dest += (!str.isEmpty()) ? str : hira.at(i);
  }
  
  return dest;
}


// parameter: Roma string (only ASCII characters) or Hiragana string
QString
DictHiragana::convertToHankakuKana(const QString& src) const
{
  DEBUG_TRACEFUNC("src: %s", qPrintable(src));
  Q_ASSERT(!_dicthankana.isEmpty());

  QString dest;
  // check parameter
  QString hira = (src.toLocal8Bit().length() == src.length()) ? convertToHira(src) : src;
  for (int i = 0; i < (int)hira.length(); ++i) {
    QString str = _dicthankana.value( hira.at(i) );
    dest += (!str.isEmpty()) ? str : hira.at(i);
  }
  return dest;
}


// parameter: Roma string (only ASCII characters)
QString
DictHiragana::convertToZenkakuEisu(const QString& src) const
{
  DEBUG_TRACEFUNC("src: %s", qPrintable(src));
  Q_ASSERT(!_dictalphbt.isEmpty());
  Q_ASSERT(!_dictsymbol.isEmpty());

  QString dest;
  for (int i = 0; i < (int)src.length(); i++) {
    QString str = _dictalphbt.value(src.at(i));
    if ( !str.isEmpty() ) {  // if the key exists
      dest += str;
    } else {
      str = _dictsymbol.value(src.at(i));
      if ( !str.isEmpty() ) {  // if the key exists 
        dest += str;
      } else {
        dest += src.at(i);
      }
    }
  }
  
  return dest;
}


QString
DictHiragana::convertString(const QString& hira, const InputMode& mode) const
{
  DEBUG_TRACEFUNC("hira: %s  mode:0x%x", qPrintable(hira), mode.id());
  if (hira.isEmpty())
    return QString::null;

  QString res;
  switch (mode.id() & Mode_ModeMask) {
  case Mode_Hiragana:
    res = convertToHira(hira);
    break;
    
  case Mode_Katakana:
    res = convertToKata(hira);
    break;
    
  case Mode_HankakuKana:
    res = convertToHankakuKana(hira);
    break;

  case Mode_ZenkakuEisu:
    if (hira.toLocal8Bit().length() != hira.length()) {
      // Multibyte chars
      res = convertToZenkakuEisu( reverseConvt(hira) );
    } else {
      res = convertToZenkakuEisu(hira);
    }
    break;
    
  case Mode_HankakuEisu:
    if (hira.toLocal8Bit().length() != hira.length()) {
      res = reverseConvt(hira);
    } else {
      res = hira;
    }
    break;

  default:
    Q_ASSERT(0);
    break;
  }
  
  return res;
}


//
// 最後の文字が ('n' + SEPARATOR_CHAR) だったら, 'ん' に変換
//
QString
DictHiragana::convert(const QList<QKeyEvent>& kevlst, const InputMode& mode, bool tenkey_hankaku) const
{
  DEBUG_TRACEFUNC("kevlst.count: %u  mode: 0x%x  tenkey_hankaku: %d", kevlst.count(), mode.id(), tenkey_hankaku);
  if ( !kevlst.count() )
    return QString::null;
  
  QString res;
  QListIterator<QKeyEvent> it(kevlst);
  const QKeyEvent* kev;

  if ((mode.id() & Mode_KanaInput) && !(mode.id() & (Mode_ZenkakuEisu | Mode_HankakuEisu))) {
    QString str;
    while ( it.hasNext() ) {
      kev = &(it.next());
      if (tenkey_hankaku && (kev->modifiers() & Qt::KeypadModifier)) {
	res += convertString(str, mode);   // 濁点変換
	str = QString::null;
	res += kev->text();
      } else {
	str += convertKey(*kev, mode);
      }
    }
    res += convertString(str, mode);

  } else if (mode.id() & (Mode_ZenkakuEisu | Mode_HankakuEisu)) {
    QString cstr;
    while ( it.hasNext() ) {
      cstr += it.next().text();
    }
    res = convertString(cstr, mode);
    
  } else {
    QString cstr;
    while ( it.hasNext() ) {
      kev = &(it.next());
      if (tenkey_hankaku && (kev->modifiers() & Qt::KeypadModifier)) {
	res += convertString(cstr, mode);
	cstr = QString::null;
	res += kev->text();
      } else if (kev->modifiers() & Qt::MetaModifier) {
	// Couples dakuten-char
	QString s = convertString(cstr, mode);
	if (s.length() > 1) {
	  res += s.left(s.length() - 1);
	}
	res += convertString(s.right(1) + convertKey(*kev, mode), mode);
	cstr = QString::null;
      } else {
	if ( !kev->text().isEmpty() ) {   // means it's not SEPARATOR_CHAR
	  cstr += kev->text();
	} else {
	  res += convertString(cstr, mode); // Check next char
	  cstr = QString::null;
	}
      }
    }
    res += convertString(cstr, mode);

    // Check last KeyEvent
    if (kevlst.last().text().isEmpty() && res.right(1).toLower() == "n") {
      res.truncate(res.length() - 1);
      res += convertString("nn", mode);
    }
  }
  
  return res;
}


//  かな入力変換
//  params: QKeyEvent
//  params: Input mode
//  return: Hiragana string
QString
DictHiragana::convertKey(const QKeyEvent& key, const InputMode& mode) const
{
  DEBUG_TRACEFUNC();
  if (mode.id() & Mode_ZenkakuEisu) {
    return convertToZenkakuEisu(key.text());
  }
  if (mode.id() & Mode_HankakuEisu) {
    return key.text();
  }
  if ((mode.id() & Mode_RomaInput) && !(key.modifiers() & Qt::MetaModifier)) {
    return QString::null;
  }
  
  QString keystr;
  if (key.modifiers() == Qt::NoModifier) {
    keystr = key.text();
  } else if (key.modifiers() & Qt::ShiftModifier) { 
    keystr = QString("shift+") + key.text();
  } else if (key.modifiers() & Qt::KeypadModifier) {
    keystr = QString("keypad+") + key.text();
  } else if (key.modifiers() & Qt::MetaModifier) {
    keystr = QString("meta+") + key.text();
  } else {
    return QString::null;
  }

  QString pres = _dictkanainp.value(keystr.toLower());   // case-insensitive
  if ( pres.isEmpty() )
    return QString::null;

  QString res;
  switch (mode.id() & Mode_ModeMask) {
  case Mode_Hiragana:
    res = pres;
    break;
    
  case Mode_Katakana:
    res = convertToKata(pres);
    break;

  case Mode_HankakuKana:
    res = convertToHankakuKana(pres);
    break;
    
  default:
    Q_ASSERT(0);
    break;
  }

  return res;
}


QString
DictHiragana::reverseConvt(const QString& hira) const
{
  DEBUG_TRACEFUNC("hira: %s", qPrintable(hira));
  QString res;
  for (int i = 0; i < (int)hira.length(); ++i) {
    QChar c = hira[i];
    if ( c.toLatin1() ) {   // Check ascii or multibyte-char
      // Ascii char
      res += c;
    } else {
      // Multibyte char
      QString str = _reversedict.value(hira.mid(i, 1));
      res += str.isEmpty() ? c : str;
    }
  } 
  return (res == hira) ? res : reverseConvt(res);
}


//
// Create new QKeyEventList
//   Auto-deletion of QPtrList class (kevlst) must be enabled
void
DictHiragana::reverse(const QString& yomi, const InputMode& mode, bool tenkey_hankaku, QList<QKeyEvent>& kevlst) const
{
  DEBUG_TRACEFUNC();

  kevlst.clear();
  if (yomi.isEmpty())
    return;

  if ((mode.id() & Mode_KanaInput) && (mode.id() & (Mode_Hiragana | Mode_Katakana | Mode_HankakuKana))) {
    Q_ASSERT(0);
    return;
  }

  QRegExp rx("[0-9+*/.-]");
  for (int i = 0; i < (int)yomi.length(); ++i) {
    if (tenkey_hankaku && rx.exactMatch(yomi.mid(i, 1))) {
      // Tenkey 
      kevlst.append( QKeyEvent(QEvent::KeyPress, 0, Qt::KeypadModifier, yomi.mid(i, 1)) );
    } else if ((mode.id() & Mode_HankakuKana) && (yomi.mid(i, 1) == QObject::tr("ﾞ") || yomi.mid(i, 1) == QObject::tr("ﾟ"))) {
      // Dakuten or Handakuten
      if (kevlst.count() > 1 && kevlst.last().text().isEmpty()) {  // SEPARATOR_CHAR
	kevlst.removeAt(kevlst.count() - 1);
      }
      QString cstr = convertString(yomi.mid(i, 1), Mode_HankakuEisu);
      kevlst.append( QKeyEvent(QEvent::KeyPress, 0, Qt::MetaModifier, cstr) );  // Exceptional key event
    } else {
      QString cstr = convertString(yomi.mid(i, 1), Mode_HankakuEisu);
      for (int j = 0; j < (int)cstr.length(); ++j) {
	kevlst.append( QKeyEvent(QEvent::KeyPress, 0, 0, cstr.mid(j, 1)) );
      }
    }
    
    if (i != (int)yomi.length() - 1)
      kevlst.append( QKeyEvent(DictHiragana::SEPARATOR_CHAR) );  // Insert SEPARATOR_CHAR
  }
}


void
DictHiragana::copyDictFile(const QString dicname)
{
  DEBUG_TRACEFUNC("dicname: %s", qPrintable(dicname));
  QString src = Config::originalDictionaryDirPath() + "/" + dicname;
  QString dst = Config::dictionaryDirPath() + "/" + dicname;

  if ( !QFile::exists(dst) ) {
    if ( !QFile::exists(src) ) {
      qFatal("no such dict file: %s", qPrintable(dicname));
      return;
    }
    
    // Copies the dictionary file    
    QFile::copy(src, dst);
    qDebug("file copy: %s -> %s", qPrintable(src), qPrintable(dst));
  }
}


QString
DictHiragana::replaceDakutenChar(const QString& str) const
{
  DEBUG_TRACEFUNC("str: %s", qPrintable(str));
  if (str.length() <= 1) 
    return str;
  
  QString dest;
  int i = 0;
  while (i < str.length()) {
    QString s = _dictdakuten.value( str.mid(i, 2) );
    if ( !s.isEmpty() ) {
      dest += s;
      ++i;
    } else {
      dest += str.at(i);
    }
    ++i;
  }
  
  return dest;
}
