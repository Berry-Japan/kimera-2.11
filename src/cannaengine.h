#ifndef CANNAENGINE_H
#define CANNAENGINE_H

#include "kanjiengine.h"
#include <QString>
#include <QAbstractSocket>

class QTcpSocket;

class CannaEngine : public KanjiEngine {
  Q_OBJECT

public:
  enum RequestID {
    RID_INITIALIZE          = 0x01,
    RID_FINALIZE            = 0X02,
    RID_CREATE_CONTEXT      = 0x03,
    RID_CLOSE_CONTEXT       = 0x05,
    RID_GET_DICTIONARY_LIST = 0x06,
    RID_GET_DIRECTORY_LIST  = 0x07,
    RID_MOUNT_DICTIONARY    = 0x08,
    RID_BEGIN_CONVERT       = 0x0f,
    RID_END_CONVERT         = 0x10,
    RID_GET_CANDIDATE_LIST  = 0x11,
    RID_GET_YOMI            = 0x12,
    RID_RESIZE_PAUSE        = 0x1a,
    RID_NOTICE_GROUP_NAME   = 0x22,
  };

  CannaEngine();
  ~CannaEngine();
  
  virtual QString  name() const;
  virtual bool  init();
  virtual void  cleanup();
  virtual bool  beginConvert(const QString& hira, QStringList& kanji, QStringList& yomigana);
  virtual void  endConvert(const QStringList& kanji=QStringList());
  virtual bool  getCandidate(int idx, QStringList& candidate);
  virtual bool  resizeSegment(int idx, int len, QStringList& kanji, QStringList& yomigana);
  virtual bool  isConverting() { return _convertflag; }
  virtual bool  isTCPConnectionSupported() const;

protected:
  void   sendInitialize();
  void   noticeGroupName();
  bool   mountDictionary(QString dic);
  void   getDictionaryList();
  void   recvGetDictionaryListReply();
  void   recvInitializeReply();
  void   recvNoticeGroupName();
  bool   recvMountDictionaryReply(); 
  QStringList recvBeginConvertReply();
  void   recvEndConvertReply() const;
  QStringList recvGetCandidateListReply();
  QString recvGetYomiReply();
  QStringList recvResizePause();
  QString  getYomi(int idx);
  QByteArray  eucToUint16(const QByteArray&);
  QByteArray  uint16ToEuc(const QByteArray&);
  bool   waitForReply(int msgid, int msecs=5000);

protected slots:
  void slotConnected();
  void slotDetectError(QAbstractSocket::SocketError error = QAbstractSocket::UnknownSocketError);

protected:
  void timerEvent(QTimerEvent *);

  QTcpSocket*  _sock;
  qint16       _contxt;
  bool         _convertflag;  // TRUE:  conversion in progress
                              // FALSE: conversion done
  int          _connectimer;

private:
  QStringList  CANNA_AF_UNIX_FILES;  // Constant expression
};

#endif // CANNAENGINE_H
