#include "cannaengine.h"
#include "kimeraglobal.h"
#include "config.h"
#include "debug.h"
#include <QApplication>
#include <QEventLoop>
#include <QMessageBox>
#include <QDataStream>
#include <QTextCodec>
#include <QDateTime>
#include <QStringList>
#include <QTimerEvent>
#include <QtNetwork/QTcpSocket>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>


CannaEngine::CannaEngine() 
  : _sock(new QTcpSocket(this)), _contxt(0), _convertflag(false), _connectimer(0)
{
  CANNA_AF_UNIX_FILES << "/tmp/.iroha_unix/IROHA" << "/var/run/.iroha_unix/IROHA";
  connect(_sock, SIGNAL(connected()), this, SLOT(slotConnected()));
  connect(_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotDetectError(QAbstractSocket::SocketError)));
}


CannaEngine::~CannaEngine()
{
}


QString
CannaEngine::name() const
{
  return QString("Canna");
}


bool
CannaEngine::isTCPConnectionSupported() const
{
  return true;
}


// connect to Kanji server
bool
CannaEngine::init()
{
  DEBUG_TRACEFUNC();
  _sock->close();
  
  if ( Config::readBoolEntry("_grpremote", false) ) {
    // INET Domain
    qDebug("INET Domain socket");
    _sock->connectToHost(Config::readEntry("_edtsvrname", "localhost"), 
			 Config::readEntry("_edtport", "0").toUInt());
    _connectimer = startTimer(3000);

  } else {
    // UNIX Domain
    qDebug("UNIX Domain socket");

    int  fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
      QMessageBox::critical(0, "Kanji Server Error", 
			    "Socket Create Error",
			    QMessageBox::Ok | QMessageBox::Default, 0);
      return false;
    }

    int i;
    for (i = 0; i < (int)CANNA_AF_UNIX_FILES.count(); ++i) {
      struct sockaddr_un  addr;
      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_UNIX;
      strncpy(addr.sun_path, CANNA_AF_UNIX_FILES[i].toAscii(), sizeof(addr.sun_path) - 1);
      
      // connect
      if ( !::connect(fd, (sockaddr *)&addr, sizeof(addr)) )
	break;
    }
    
    if (i == (int)CANNA_AF_UNIX_FILES.count()) {
      QMessageBox::critical(0, "Kanji Server Error", 
			    "Socket Connection Error",
			    QMessageBox::Ok | QMessageBox::Default, 0);
      ::close(fd);
      return false;
    }
    
    _sock->setSocketDescriptor(fd);  // UNIX Domain socket set
    slotConnected();
  }

  return true;
}


void
CannaEngine::cleanup()
{
  DEBUG_TRACEFUNC();
  qDebug("SocketState before cleanup: %d", _sock->state());

  if (_sock->state() != QAbstractSocket::UnconnectedState && _sock->state() != QAbstractSocket::ClosingState) {
    _sock->close();
  }
  _contxt = 0;
  _convertflag = false;
  if (_connectimer > 0) {
    killTimer(_connectimer);
    _connectimer = 0;
 }
}


void 
CannaEngine::slotConnected() 
{
  DEBUG_TRACEFUNC();
  qDebug("socket: %d", _sock->socketDescriptor());
  sendInitialize();
}


void 
CannaEngine::slotDetectError(QAbstractSocket::SocketError error) 
{
  DEBUG_TRACEFUNC("SocketError: %d", (int)error);

  QMessageBox::critical(0, "Kanji Server Error", 
			tr("Canna との通信でエラーが発生しました。\n"
			   "かな漢字変換を停止します。"),
			QMessageBox::Ok | QMessageBox::Default, 0);
  cleanup();
  Config::writeEntry("_grpremote", false);
}


void 
CannaEngine::sendInitialize()
{
  DEBUG_TRACEFUNC();
  QByteArray  pack;
  QDataStream ds(&pack, QIODevice::WriteOnly);
  ds.setByteOrder(QDataStream::BigEndian);

  QByteArray user("3.3:");
  struct passwd* pass = getpwuid( getuid() );
  user += QString(pass->pw_name);
  ds << (int)RID_INITIALIZE << (int)user.length() + 1;
  ds.writeRawData(user.data(), user.length() + 1);
  _sock->write(pack.data(), pack.size());
  
  // wait for replay
  QTime t;
  t.start();
  while (_sock->bytesAvailable() != 4) {
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    if (t.elapsed() > 2000) {
      qWarning("No response!");
      return;
    }
  }

  recvInitializeReply();
}


void 
CannaEngine::recvInitializeReply()
{
  DEBUG_TRACEFUNC();
  uint len;
  if ((len = _sock->size()) != 4) {
    qWarning("Server response incorrect.  %s:%d", __FILE__, __LINE__);
  } 
  
  QByteArray respack(len, 0);
  QDataStream ds(respack);
  ds.setByteOrder(QDataStream::BigEndian);

  // response packet recv
  if (_sock->read(respack.data(), len) < 0) {
    qFatal("Recv error.  %s:%d", __FILE__, __LINE__);
    return;
  }
  
  int res;
  ds >> res;
  if (res == -1 || res == -2) {
    cleanup();
    QMessageBox::critical(0, "Protocol version error", 
			  "Canna protocol version error",
			  QMessageBox::Ok | QMessageBox::Default, 0);
    return;
  } 

  QDataStream dst(respack);
  dst.setByteOrder(QDataStream::BigEndian);
  quint16  minor;
  dst >> minor >> _contxt;
  qDebug("minor version: %d  context: %d", minor, _contxt);

  noticeGroupName();
}


void 
CannaEngine::noticeGroupName()
{
  DEBUG_TRACEFUNC();
  QByteArray  pack;
  QDataStream ds(&pack, QIODevice::WriteOnly);
  ds.setByteOrder(QDataStream::BigEndian);

  const char* gname = "users";
  struct passwd* pass = getpwuid( getuid() );
  if ( pass ) {
    struct group* grp = getgrgid(pass->pw_gid);
    if ( grp ) {
      gname = grp->gr_name;
    }
  }

  quint16 glen = strlen(gname) + 1;
  ds << (quint8)RID_NOTICE_GROUP_NAME << (quint8)0 << (quint16)(glen + 6)
     << (int)0 << _contxt;
  ds.writeRawData(gname, glen);
  _sock->write(pack.data(), pack.size());

  if ( !waitForReply(RID_NOTICE_GROUP_NAME) ) {
    slotDetectError();
    return;
  }

  recvNoticeGroupName();
}


void 
CannaEngine::recvNoticeGroupName()
{
  DEBUG_TRACEFUNC();
  uint datalen = _sock->size();
  QByteArray data(datalen, 0);
  if (_sock->read(data.data(), datalen) < 0) {
    qFatal("Recv error.  %s:%d", __FILE__, __LINE__);
    return;
  }

  QDataStream ds(data);
  ds.setByteOrder(QDataStream::BigEndian);

  quint16 len;
  ds >> len;
  
  if (len != datalen - 2) {
    qWarning("Server response incorrect.  %s", __func__);
  } 
  
  qint8  stat;
  ds >> stat;
  if (stat) {
    cleanup();
    QMessageBox::critical(0, "mount dictionary error",
			  "Canna mount dictionary error",
			  QMessageBox::Ok | QMessageBox::Default, 0);  
  } else {
    qDebug("notice group name successful.");
  }

  getDictionaryList();
}


void 
CannaEngine::getDictionaryList()
{
  DEBUG_TRACEFUNC();
  QByteArray  pack;
  QDataStream ds(&pack, QIODevice::ReadWrite);
  ds.setByteOrder(QDataStream::BigEndian);
  
  ds << (quint8)RID_GET_DICTIONARY_LIST << (quint8)0 << (quint16)4 << _contxt << (quint16)4096;
  _sock->write(pack.data(), pack.size());

  if ( !waitForReply(RID_GET_DICTIONARY_LIST) ) {
    slotDetectError();
    return;
  }
  
  recvGetDictionaryListReply();
}


void 
CannaEngine::recvGetDictionaryListReply()
{
  DEBUG_TRACEFUNC();
  uint datalen = _sock->size();
  QByteArray data(datalen, 0);
  if (_sock->read(data.data(), datalen) < 0) {
    qFatal("Recv error.  %s:%d", __FILE__, __LINE__);
    return;
  }

  QDataStream ds(data);
  ds.setByteOrder(QDataStream::BigEndian);

  qint16 len, ndic;
  ds >> len >> ndic;
  
  if (len != (int)datalen - 2) {
    qWarning("Server response incorrect.  %s", __func__);
  } 
  
  qDebug("number of dictionary: %d", ndic);
  QString dic;
  QStringList diclist;
  for (int i = 0; i < ndic; i++) {
    dic = QString();
    qint8 s;
    while (1) {
      ds >> s;
      if ( !s ) break;
      dic += s;
    }
    
    if (!dic.isEmpty())
      diclist << dic;
  }

  // Mounts dictionaries
  for (QStringList::Iterator it = diclist.begin(); it != diclist.end(); ++it) {
    if (*it != "pub") {    // Dosen't mount "pub" dictionary 
      if ( !mountDictionary(*it) )
	break;
    }
  }
}


bool
CannaEngine::mountDictionary(QString dic)
{
  DEBUG_TRACEFUNC();

  QByteArray  pack;
  QDataStream ds(&pack, QIODevice::ReadWrite);
  ds.setByteOrder(QDataStream::BigEndian);
    
  qDebug("request mount dictionary name: %s", qPrintable(dic));    
  ds << (quint8)RID_MOUNT_DICTIONARY << (quint8)0 << (quint16)(dic.length()+7) << (int)0x0200 << _contxt;
  ds.writeRawData(dic.toAscii(), dic.length() + 1);
  _sock->write(pack.data(), pack.size());

  if ( !waitForReply(RID_MOUNT_DICTIONARY) ) {
    slotDetectError();
    return false;
  }

  return recvMountDictionaryReply();
}


bool
CannaEngine::recvMountDictionaryReply()
{
  DEBUG_TRACEFUNC();
  uint datalen = _sock->size();
  QByteArray data(datalen, 0);
  if (_sock->read(data.data(), datalen) < 0) {
    qFatal("Recv error.  %s:%d", __FILE__, __LINE__);
    return false;
  }

  QDataStream ds(data);
  ds.setByteOrder(QDataStream::BigEndian);

  qint16 len;
  ds >> len;
  
  if (len != (int)datalen - 2) {
    qWarning("Server response incorrect.  %s", __func__);
    return false;
  } 
  
  qint8  stat;
  ds >> stat;
  if (stat) {
    cleanup();
    QMessageBox::critical(0, "mount dictionary error",
			  "Canna mount dictionary error",
			  QMessageBox::Ok | QMessageBox::Default, 0);
    return false;
  }  
  qDebug("mount dictionary successful.");
  return true;
}


bool
CannaEngine::beginConvert(const QString& hira, QStringList& kanji, QStringList& yomigana)
{
  DEBUG_TRACEFUNC("hira: %s", qPrintable(hira));
  if ( _sock->state() != QAbstractSocket::ConnectedState )
    return false;

  if ( _convertflag ) {
    qWarning("%s:_convertflag incorrect", __func__);
    return false;
  }

  QByteArray  pack;
  QDataStream ds(&pack, QIODevice::WriteOnly);
  ds.setByteOrder(QDataStream::BigEndian);

  qDebug("Yomigana size(euc): %d", hira.toLocal8Bit().length());
  QByteArray str = eucToUint16(QTextCodec::codecForTr()->fromUnicode(hira));
  qDebug("Yomigana size(uint16 array): %d", str.size());

  ds << (quint8)RID_BEGIN_CONVERT << (quint8)0 << (quint16)(str.size()+6) << (int)0 << _contxt;
  ds.writeRawData(str.data(), str.size());
  _sock->write(pack.data(), pack.size());

  _convertflag = true;  // start converting
  qDebug("beginConvert in progress ...");
 
  if ( !waitForReply(RID_BEGIN_CONVERT) ) {
    slotDetectError();
    return false;
  }

  yomigana.clear();
  kanji = recvBeginConvertReply();
  if (kanji.isEmpty()) {
    kanji << hira;
  }

  for (int i = 0; i < (int)kanji.count(); ++i) {
    QString str = getYomi(i);
    if (str.isEmpty())
      return false;
    yomigana << str;
  }

  Q_ASSERT(kanji.count() == yomigana.count());
  return true;
}


QStringList
CannaEngine::recvBeginConvertReply()
{
  DEBUG_TRACEFUNC();

  uint datalen = _sock->size();
  QByteArray data(datalen, 0);
  if (_sock->read(data.data(), datalen) < 0) {
    qFatal("Recv error.  %s:%d", __FILE__, __LINE__);
    return QStringList();
  }

  QDataStream ds(data);
  ds.setByteOrder(QDataStream::BigEndian);

  qint16 len;
  ds >> len;
  
  if (len != (int)datalen - 2) {
    qWarning("beginConvert: server response incorrect. (%s)", __func__);
  } 

  qint16 nbunsetu;
  ds >> nbunsetu;
  if (nbunsetu <= 0) {
    qDebug("beginConvert: convert error.  %s", __func__);
    return QStringList();
  }

  QStringList strlist;
  for (int i = 0; i < nbunsetu; i++) {
    QByteArray  ba;
    QDataStream dsba(&ba, QIODevice::WriteOnly);
    dsba.setByteOrder(QDataStream::BigEndian);
    quint16 us;

    ds >> us;
    while (us) {
      dsba << us;
      ds >> us;
    }

    QString str = (tr(uint16ToEuc(ba)));
    if (!str.isEmpty())
      strlist << str;
  }

  qDebug("beginConvert successful");
  qDebug("[%s] string: %s (%d)", __func__, qPrintable(strlist.join(" ")), strlist.count());
  return strlist;
}


void
CannaEngine::endConvert(const QStringList& kanji)
{
  DEBUG_TRACEFUNC();
  if ( _sock->state() != QAbstractSocket::ConnectedState )
    return;

  if ( !_convertflag ) {
    qDebug("%s: unnecessary to end convert", __func__);
    return;
  }

  QByteArray  pack;
  QDataStream ds(&pack, QIODevice::WriteOnly);
  ds.setByteOrder(QDataStream::BigEndian);

  // 学習機能追加
  ds << (quint8)RID_END_CONVERT << (quint8)0 << (quint16)(8 + kanji.count() * 2) << _contxt
     << (qint16)kanji.count() << (int)1;
  
  for (int i = 0; i < (int)kanji.count(); ++i) {
    QStringList cand;
    int n = getCandidate(i, cand) ? cand.indexOf(kanji[i]) : 0;
    ds << (qint16)qMax(n, 0);
  }
  _sock->write(pack.data(), pack.size());

  _convertflag = false;  // conversion done
  qDebug("%s: done", __func__);

  if ( !waitForReply(RID_END_CONVERT) ) {
    slotDetectError();
    return;
  }
  
  recvEndConvertReply();
}


void 
CannaEngine::recvEndConvertReply() const
{
  DEBUG_TRACEFUNC();
  uint datalen = _sock->size();
  QByteArray data(datalen, 0);
  if (_sock->read(data.data(), datalen) < 0) {
    qFatal("Recv error.  %s:%d", __FILE__, __LINE__);
    return;
  }
  
  QDataStream ds(data);
  ds.setByteOrder(QDataStream::BigEndian);
  
  qint16 len;
  ds >> len;
  
  if (len != (int)datalen - 2) {
    qWarning("Server response incorrect.  %s", __func__);
    return;
  }
  
  qint8 res;
  ds >> res;
  if ( res ) {
    qFatal("End Convert error.  %s:%d", __FILE__, __LINE__);
    return;
  } 
    
  qDebug("endConvert successful");
}


bool
CannaEngine::getCandidate(int idx, QStringList& candidate)
{
  DEBUG_TRACEFUNC("idx: %d", idx);
  if ( _sock->state() != QAbstractSocket::ConnectedState )
    return false;

  if ( !_convertflag )
    return false;
 
  QByteArray  pack;
  QDataStream ds(&pack, QIODevice::WriteOnly);
  ds.setByteOrder(QDataStream::BigEndian);

  ds << (quint8)RID_GET_CANDIDATE_LIST << (quint8)0 << (quint16)6 << _contxt 
     << (qint16)idx << (quint16)4096;
  _sock->write(pack.data(), pack.size());

  if ( !waitForReply(RID_GET_CANDIDATE_LIST) ) {
    slotDetectError();
    return false;
  }

  candidate = recvGetCandidateListReply();
  return true;
}


QStringList
CannaEngine::recvGetCandidateListReply()
{
  DEBUG_TRACEFUNC();
  uint datalen = _sock->size();
  QByteArray data(datalen, 0);
  if (_sock->read(data.data(), datalen) < 0) {
    qFatal("Recv error.  %s:%d", __FILE__, __LINE__);
    return QStringList();
  }
  
  QDataStream ds(data);
  ds.setByteOrder(QDataStream::BigEndian);
  
  qint16 len;
  ds >> len;
  
  if (len != (int)datalen - 2) {
    qWarning("Server response incorrect.  %s", __func__);
    return QStringList();
  }

  qint16 ncand;
  ds >> ncand;
  if (ncand < 0) {
    qFatal("GetCandidateList error.  %s:%d", __FILE__, __LINE__);
    return QStringList();
  }
  qDebug("Number of Candidates: %d", ncand);
  
  if ( !ncand ) {
    return QStringList();
  }
  
  QStringList  listcand;
  for (int i = 0; i < ncand; i++) {
    QByteArray  ba;
    QDataStream dsba(&ba, QIODevice::WriteOnly);
    dsba.setByteOrder(QDataStream::BigEndian);
    quint16 us;
    
    ds >> us;
    while (us) {
      dsba << us;
      ds >> us;
    }    
    
    QString str = tr(uint16ToEuc(ba));
    if (!str.isEmpty() && !listcand.contains(str)) {
      listcand << str;
    }
  }
  
  qDebug("[%s]  %s", __func__, qPrintable(listcand.join(" ")));
  return listcand;
}


QString
CannaEngine::getYomi(int idx)
{
  DEBUG_TRACEFUNC();
  if ( !_convertflag )
    return QString::null;

  QByteArray  pack;
  QDataStream ds(&pack, QIODevice::WriteOnly);
  ds.setByteOrder(QDataStream::BigEndian);
  
  ds << (quint8)RID_GET_YOMI << (quint8)0 << (quint16)6 << _contxt 
     << (qint16)idx << (quint16)4096;
  _sock->write(pack.data(), pack.size());
  
  if ( !waitForReply(RID_GET_YOMI) ) {
    slotDetectError();
    return QString::null;
  }

  return recvGetYomiReply();
}


QString
CannaEngine::recvGetYomiReply()
{
  DEBUG_TRACEFUNC();
  uint datalen = _sock->size();
  QByteArray data(datalen, 0);
  if (_sock->read(data.data(), datalen) < 0) {
    qFatal("Recv error.  %s:%d", __FILE__, __LINE__);
    return QString::null;
  }
  
  QDataStream ds(data);
  ds.setByteOrder(QDataStream::BigEndian);
  
  qint16 len;
  ds >> len;
  
  if (len != (int)datalen - 2) {
    qWarning("Server response incorrect.  %s", __func__);
    return QString::null;
  }
  
  qint16 n;
  ds >> n;
  
  QByteArray  ba;
  QDataStream dsba(&ba, QIODevice::WriteOnly);
  dsba.setByteOrder(QDataStream::BigEndian);
  quint16 us;
  
  ds >> us;
  while (us) {
    dsba << us;
    ds >> us;
  }    
  
  return tr(uint16ToEuc(ba));
}


bool
CannaEngine::resizeSegment(int idx, int len, QStringList& kanji, QStringList& yomigana)
{
  DEBUG_TRACEFUNC("idex: %d  len: %d", idx, len);
  if ( _sock->state() != QAbstractSocket::ConnectedState )
    return false;

  if ( !_convertflag ) {
    Q_ASSERT(0);
    return false;
  }

  if (len < 0) {
    int length = getYomi(0).length();
    len = (len == LENGTHEN_1CHAR) ? length + 1 : length - 1;
  }

  QByteArray  pack;
  QDataStream ds(&pack, QIODevice::WriteOnly);
  ds.setByteOrder(QDataStream::BigEndian);
  
  ds << (quint8)RID_RESIZE_PAUSE << (quint8)0 
     << (quint16)6 << _contxt 
     << (qint16)idx << (quint16)len;
  _sock->write(pack.data(), pack.size());

  if ( !waitForReply(RID_RESIZE_PAUSE) ) {
    slotDetectError();
    return false;
  }

  kanji = recvResizePause();
  yomigana.clear();
  for (int i = 0; i < (int)kanji.count(); ++i) {
    QString str = getYomi(idx + i);
    if (str.isEmpty())
      return false;
    yomigana << str;
  }

  Q_ASSERT(kanji.count() == yomigana.count());
  return true;
}


QStringList
CannaEngine::recvResizePause()
{
  DEBUG_TRACEFUNC();
  uint datalen = _sock->size();
  QByteArray data(datalen, 0);
  if (_sock->read(data.data(), datalen) < 0) {
    qFatal("Recv error.  %s:%d", __FILE__, __LINE__);
    return QStringList();
  }
  
  QDataStream ds(data);
  ds.setByteOrder(QDataStream::BigEndian);
  
  qint16 len;
  ds >> len;
  
  if (len != (int)datalen - 2) {
    qWarning("Server response incorrect.  %s", __func__);
    return QStringList();
  }
  
  qint16 nbunsetu;
  ds >> nbunsetu;
  if (nbunsetu <= 0) {
    qDebug("Convert error.  %s", __func__);
    return QStringList();
  }
  
  qDebug("%s Number of Bunsetu: %d", __func__, nbunsetu);

  QStringList strlist;
  for (int i = 0; i < nbunsetu; i++) {
    QByteArray  ba;
    QDataStream dsba(&ba, QIODevice::WriteOnly);
    dsba.setByteOrder(QDataStream::BigEndian);
    quint16 us;

    ds >> us;
    while (us && !ds.atEnd()) {
      dsba << us;
      ds >> us;
    }
    
    QString str = tr(uint16ToEuc(ba));
    if ( !str.isEmpty() )
      strlist << str;
  }

  qDebug("[%s]  %s", __func__, qPrintable(strlist.join(" ")));
  return strlist;
}


bool
CannaEngine::waitForReply(int msgid, int msecs)
{
  DEBUG_TRACEFUNC("msgid:%d msecs:%d", msgid, msecs);

  _sock->flush();
  if ( !_sock->waitForReadyRead( msecs ) ) {
    qDebug("Timeout or SocketError!  [waitForReadyRead()]");
    return false;
  }

  char buf[2] = "";
  qint64 size = _sock->read(buf, 2);
  if (size != 2) {
    qFatal("Recv error %d  %s:%d", (int)size, __FILE__, __LINE__);
    return false;
  }
  if (buf[0] != msgid) {
    qWarning("Bad message ID : req:0x%x  recv:0x%x", (quint8)msgid, (quint8)buf[0]);
    return false;
  }
  
  return true;
}


// 返却値はネットワークバイトオーダ変換済なので，直ちに送信可能
// 末尾に (quint16)0 を追加
QByteArray 
CannaEngine::eucToUint16(const QByteArray& src)
{
  DEBUG_TRACEFUNC("src: %s", src.data());

  QByteArray dest;
  QDataStream dsdest(&dest, QIODevice::ReadWrite);
  dsdest.setByteOrder(QDataStream::BigEndian);

  QByteArray::ConstIterator it = src.begin();
  while (it != src.end()) {
    if (!(*it & 0x80)) {
      // ASCII
      dsdest << (quint16)*it;

    } else if (it + 1 != src.end()) {
      switch ((quint8)*it) {
      case 0x8e:   // 半角カナ	
	dsdest << (quint16)(0x0080 | (*(++it) & 0x7f));
	break;

      case 0x8f:   // 外字
	if (it + 2 == src.end())
	  return 0;
	
	dsdest << (quint16)(0x8000 | ((*(++it) & 0x7f) << 8) | (*(++it) & 0x7f));
	break;

      default:  // 全角かな
	dsdest << (quint16)(0x8080 | ((*it & 0x7f) << 8) | (*(++it) & 0x7f));
	break;
      }

    } else {
      // error
      return 0;
    }

    ++it;
  }
  
  dsdest << (quint16)0;
  return dest;
}


// 引数はネットワークバイトオーダの配列を指定
QByteArray
CannaEngine::uint16ToEuc(const QByteArray& src)
{
  QDataStream dssrc(src);
  dssrc.setByteOrder(QDataStream::BigEndian);
  QByteArray dst;
  QDataStream  dsdst(&dst, QIODevice::WriteOnly);
  dsdst.setByteOrder(QDataStream::BigEndian);

  quint8 b;
  while (!dssrc.atEnd()) {
    quint16 us;
    dssrc >> us;
 
    switch (us & 0x8080) {
    case 0:  // ASCII
      dsdst << (quint8)(us & 0x007f);
      break;

    case 0x0080:  // 半角カナ
      dsdst << (0x8e80 | (us & 0x007f));
      break;

    case 0x8000:  // 外字
      dsdst << (quint8)0x8f;
      dsdst << us;
      break;

    case 0x8080:  // 全角かな
      b = (us & 0xff00) >> 8;
      if ((b >= 0xa1 && b <= 0xa8) || (b >= 0xb0 && b <= 0xf4)) {  // EUC code
	dsdst << us;
	
      } else if (us >= 0xada1 && us <= 0xadb4) {
        QString str;
	str.sprintf("(%u)", us - 0xada0);
	dst += str;

      } else {
	qWarning("No such EUC code!!!!!  us:0x%x", us);
      }
      break;

    default:
      Q_ASSERT(0);
      break;
    }
  }
  
  dsdst << (quint8)0;
  return dst;
}


void
CannaEngine::timerEvent(QTimerEvent* e)
{
  DEBUG_TRACEFUNC();
  if (_connectimer > 0 && e->timerId() == _connectimer) {
    killTimer(_connectimer);
    _connectimer = 0;
    
    if (_sock->state() != QAbstractSocket::ConnectedState) {
      // Detects error
      cleanup();
      Config::writeEntry("_grpremote", false);
      
      QMessageBox::warning(0, "Kanji Server Connection Error",
			   tr("Canna とのセッションを確立できません。\n"
			      "かな漢字変換を停止します。"),
			   QMessageBox::Ok | QMessageBox::Default, 0);
    }
  }
}


EXPORT_KANJIENGINE(CannaEngine)
