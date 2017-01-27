#ifndef KIMERAGLOBAL_H
#define KIMERAGLOBAL_H

#include <QDataStream>
#include <QString>
#include <QPair>
#include <QSysInfo>

namespace Kimera {
  typedef  QPair<quint16, quint16>  ImIcPair;
  
  const QString KIMERACONF =  "/kimera/" KIMERA_VERSION "/";
  
  enum ModeID {
    Mode_Hiragana    = 0x0001,  // �Ҥ餬��
    Mode_Katakana    = 0x0002,  // ��������
    Mode_HankakuKana = 0x0004,  // Ⱦ�ѥ���
    Mode_ZenkakuEisu = 0x0010,  // ���ѱѿ�
    Mode_HankakuEisu = 0x0020,  // Ⱦ�ѱѿ�
    Mode_ModeMask    = 0x00ff,
    Mode_RomaInput   = 0x0100,  // ���޻�����
    Mode_KanaInput   = 0x0200,  // ��������
    Mode_InputMask   = 0x0f00,
    Mode_DirectInput = 0x1000,  // ľ������
  };

  enum ConvStatus {        // Kanji conversion status definition
    NONE_YOMI = 0,         // �������Ϥ��Ƥ��ʤ�����
    INPUTTING_YOMI,        // �ɤߤ����Ϥ��Ƥ������
    CONVERTING_YOMI,       // �ɤߤ�������Ѵ����Ƥ������
    SHOWING_LIST,          // �Ѵ���θ��������ɽ�����Ƥ������
    CHANGING_LENGTH,       // ʸ���Ĺ�����ѹ����Ƥ������
    NUM_OF_CONVSTATUS = 5,
  };
  
  enum FuncID {     // Function name definition
    FID_None = 0,
    FID_InsertChar,
    FID_NextCandidate,
    FID_NextCandidateGroup,
    FID_PreviousCandidate,
    FID_PreviousCandidateGroup,
    FID_FirstCandidate,
    FID_LastCandidate,
    FID_ConvertAllSegments,
    FID_ConvertCrntSegment,
    FID_ConvertToOneSegment,
    FID_DeleteForwardChar,
    FID_DeleteBackwardChar,
    FID_DeleteAll,
    FID_DecideAllSegments,
    FID_DecideCrntSegment,
    FID_DecidePredictedCandidate,
    FID_CancelConversion,
    FID_CancelSegmentConversion,
    FID_LengthenSegment,
    FID_ShortenSegment,
    FID_InsertSpace,
    FID_InsertAsciiSpace,
    FID_InsertMultibyteSpace,
    FID_InsertOtherWidthSpace,
    FID_ConvertToHira,
    FID_ConvertToKana,
    FID_ConvertToHankakuKana,
    FID_ConvertToHankakuEisu,
    FID_ConvertToZenkakuEisu,
    FID_ForwardSegment,
    FID_BackwardSegment,
    FID_ToFirstSegment,
    FID_ToLastSegment,
    FID_CaretForward,
    FID_CaretBackward,
    FID_CaretToFirst,
    FID_CaretToLast,
    FID_SwitchZenkakuEisuMode,
    FID_SetToHiraganaMode,
    FID_SetToKatakanaMode,
    FID_SetToHankakuKanaMode,
    FID_SetToZenkakuEisuMode,
    FID_SwitchInputMethod,
    FID_ShowPropertyDialog,
    FID_ExecDictTool,
    FID_ReconvertClipboardString,
    FID_NumFunctions,
  };

  enum SettingType {
    ST_MSIME = 0,
    ST_ATOK,
    ST_KINPUT2,
    ST_VJE,
    ST_CURRENT_SETTING,
    NUM_SETTING_TYPE,
  };


  inline QDataStream::ByteOrder sysByteOrder()
  {
    return (QSysInfo::ByteOrder == QSysInfo::BigEndian) ? QDataStream::BigEndian : QDataStream::LittleEndian;
  }
};


#endif  // KIMERAGLOBAL_H
