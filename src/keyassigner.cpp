#include "keyassigner.h"
#include "kimeraglobal.h"
#include "config.h"
#include "debug.h"
#include <QVector>
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QHeaderView>
#include <QHash>

const QString PREFIX = "ktbl";
const int  MAX_AVAILABLE_KEYS = 100;

static QVector<QString>  FuncName(FID_NumFunctions);
static QHash< int, QVector<int> >  CurrentKeyAssign;


class VariableInitializer {
  Q_DECLARE_TR_FUNCTIONS(VariableInitializer)

public:
  VariableInitializer();
};
static VariableInitializer initializer;


VariableInitializer::VariableInitializer()
{
  //
  // variable initialize
  //
}


static int AvailableKeys[] = {
  Qt::Key_Space,
  Qt::CTRL+Qt::Key_Space,
  Qt::SHIFT+Qt::Key_Space,
  Qt::CTRL+Qt::SHIFT+Qt::Key_Space,
  Qt::Key_Return,
  Qt::CTRL+Qt::Key_Return,
  Qt::SHIFT+Qt::Key_Return,
  Qt::Key_Enter,
  Qt::CTRL+Qt::Key_Enter,
  Qt::SHIFT+Qt::Key_Enter,
  Qt::Key_Backspace,
  Qt::CTRL+Qt::Key_Backspace,
  Qt::SHIFT+Qt::Key_Backspace,
  Qt::CTRL+Qt::SHIFT+Qt::Key_Backspace,
  Qt::Key_Delete,
  Qt::CTRL+Qt::Key_Delete,
  Qt::SHIFT+Qt::Key_Delete,
  Qt::Key_Escape,
  Qt::CTRL+Qt::Key_Escape,
  Qt::SHIFT+Qt::Key_Escape,
  Qt::Key_Insert,
  Qt::CTRL+Qt::Key_Insert,
  Qt::SHIFT+Qt::Key_Insert,
  Qt::Key_Home,
  Qt::CTRL+Qt::Key_Home,
  Qt::SHIFT+Qt::Key_Home,
  Qt::Key_End,
  Qt::CTRL+Qt::Key_End,
  Qt::SHIFT+Qt::Key_End,
  Qt::Key_Left,
  Qt::CTRL+Qt::Key_Left,
  Qt::SHIFT+Qt::Key_Left,
  Qt::Key_Right,
  Qt::CTRL+Qt::Key_Right,
  Qt::SHIFT+Qt::Key_Right,
  Qt::Key_Up,
  Qt::CTRL+Qt::Key_Up,
  Qt::SHIFT+Qt::Key_Up,
  Qt::Key_Down,
  Qt::CTRL+Qt::Key_Down,
  Qt::SHIFT+Qt::Key_Down,
  Qt::Key_Tab,
  Qt::CTRL+Qt::Key_Tab,
  Qt::SHIFT+Qt::Key_Tab,
  Qt::Key_PageUp,
  Qt::CTRL+Qt::Key_PageUp,
  Qt::SHIFT+Qt::Key_PageUp,
  Qt::Key_PageDown,
  Qt::SHIFT+Qt::Key_PageDown,
  Qt::CTRL+Qt::Key_PageDown,
  Qt::CTRL+Qt::Key_Comma,
  Qt::CTRL+Qt::Key_Minus,
  Qt::CTRL+Qt::Key_Period,
  Qt::CTRL+Qt::Key_Slash,
  Qt::CTRL+Qt::Key_0,
  Qt::CTRL+Qt::Key_1,
  Qt::CTRL+Qt::Key_2,
  Qt::CTRL+Qt::Key_3,
  Qt::CTRL+Qt::Key_4,
  Qt::CTRL+Qt::Key_5,
  Qt::CTRL+Qt::Key_6,
  Qt::CTRL+Qt::Key_7,
  Qt::CTRL+Qt::Key_8,
  Qt::CTRL+Qt::Key_9,
  Qt::CTRL+Qt::Key_Colon,
  Qt::CTRL+Qt::Key_Semicolon,
  Qt::CTRL+Qt::Key_A,
  Qt::CTRL+Qt::Key_B,
  Qt::CTRL+Qt::Key_C,
  Qt::CTRL+Qt::Key_D,
  Qt::CTRL+Qt::Key_E,
  Qt::CTRL+Qt::Key_F,
  Qt::CTRL+Qt::Key_G,
  Qt::CTRL+Qt::Key_H,
  Qt::CTRL+Qt::Key_I,
  Qt::CTRL+Qt::Key_J,
  Qt::CTRL+Qt::Key_K,
  Qt::CTRL+Qt::Key_L,
  Qt::CTRL+Qt::Key_M,
  Qt::CTRL+Qt::Key_N,
  Qt::CTRL+Qt::SHIFT+Qt::Key_N,
  Qt::CTRL+Qt::Key_O,
  Qt::CTRL+Qt::Key_P,
  Qt::CTRL+Qt::SHIFT+Qt::Key_P,
  Qt::CTRL+Qt::Key_Q,
  Qt::CTRL+Qt::Key_R,
  Qt::CTRL+Qt::Key_S,
  Qt::CTRL+Qt::Key_T,
  Qt::CTRL+Qt::Key_U,
  Qt::CTRL+Qt::Key_V,
  Qt::CTRL+Qt::Key_W,
  Qt::CTRL+Qt::Key_X,
  Qt::CTRL+Qt::Key_Y,
  Qt::CTRL+Qt::Key_Z,
  Qt::CTRL+Qt::Key_BracketLeft,
  Qt::CTRL+Qt::Key_Backslash,
  Qt::CTRL+Qt::Key_BracketRight,
  Qt::CTRL+Qt::Key_AsciiCircum,
  Qt::CTRL+Qt::Key_At,
  Qt::Key_F1,
  Qt::CTRL+Qt::Key_F1,
  Qt::SHIFT+Qt::Key_F1,
  Qt::Key_F2,
  Qt::CTRL+Qt::Key_F2,
  Qt::SHIFT+Qt::Key_F2,
  Qt::Key_F3,
  Qt::CTRL+Qt::Key_F3,
  Qt::SHIFT+Qt::Key_F3,
  Qt::Key_F4,
  Qt::CTRL+Qt::Key_F4,
  Qt::SHIFT+Qt::Key_F4,
  Qt::Key_F5,
  Qt::CTRL+Qt::Key_F5,
  Qt::SHIFT+Qt::Key_F5,
  Qt::Key_F6,
  Qt::CTRL+Qt::Key_F6,
  Qt::SHIFT+Qt::Key_F6,
  Qt::Key_F7,
  Qt::CTRL+Qt::Key_F7,
  Qt::SHIFT+Qt::Key_F7,
  Qt::Key_F8,
  Qt::CTRL+Qt::Key_F8,
  Qt::SHIFT+Qt::Key_F8,
  Qt::Key_F9,
  Qt::CTRL+Qt::Key_F9,
  Qt::SHIFT+Qt::Key_F9,
  Qt::Key_F10,
  Qt::CTRL+Qt::Key_F10,
  Qt::SHIFT+Qt::Key_F10,
  Qt::Key_F11,
  Qt::CTRL+Qt::Key_F11,
  Qt::SHIFT+Qt::Key_F11,
  Qt::Key_F12,
  Qt::CTRL+Qt::Key_F12,
  Qt::SHIFT+Qt::Key_F12,
  Qt::Key_Eisu_toggle,
  Qt::Key_Henkan,
  Qt::CTRL+Qt::Key_Henkan,
  Qt::SHIFT+Qt::Key_Henkan,
  Qt::Key_Muhenkan,
  Qt::CTRL+Qt::Key_Muhenkan,
  Qt::SHIFT+Qt::Key_Muhenkan,
  Qt::CTRL+Qt::SHIFT+Qt::Key_Muhenkan,
  Qt::Key_Katakana,
  Qt::CTRL+Qt::Key_Katakana,
  Qt::SHIFT+Qt::Key_Katakana,
};


static int KeyAssignSetting[NUM_SETTING_TYPE][MAX_AVAILABLE_KEYS][NUM_OF_CONVSTATUS + 1] = {
  {  // MSIME
    { Qt::Key_Space,     FID_InsertSpace, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_Space,  FID_InsertAsciiSpace, FID_ConvertToOneSegment, FID_None, FID_None, FID_None },
    { Qt::SHIFT+Qt::Key_Space, FID_InsertOtherWidthSpace, FID_ConvertAllSegments, FID_PreviousCandidate, FID_PreviousCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::SHIFT+Qt::Key_Space,  FID_InsertMultibyteSpace, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_Return,    FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::CTRL+Qt::Key_Return, FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::SHIFT+Qt::Key_Return, FID_None, FID_DecidePredictedCandidate, FID_None, FID_None, FID_None },
    { Qt::Key_Enter,     FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::CTRL+Qt::Key_Enter,  FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::SHIFT+Qt::Key_Enter,  FID_None, FID_DecidePredictedCandidate, FID_None, FID_None, FID_None },
    { Qt::Key_Backspace, FID_None, FID_DeleteBackwardChar, FID_CancelConversion, FID_CancelConversion, FID_None },
    { Qt::CTRL+Qt::Key_Backspace, FID_None, FID_DeleteBackwardChar, FID_CancelConversion, FID_CancelConversion, FID_None },
    { Qt::SHIFT+Qt::Key_Backspace, FID_None, FID_DeleteBackwardChar, FID_CancelConversion, FID_CancelConversion, FID_None },
    { Qt::Key_Delete,    FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::Key_Escape,    FID_None, FID_DeleteAll, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::SHIFT+Qt::Key_Escape, FID_None, FID_DeleteAll, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::Key_Home,      FID_None, FID_CaretToFirst, FID_ToFirstSegment, FID_FirstCandidate, FID_None },
    { Qt::Key_End,       FID_None, FID_CaretToLast, FID_ToLastSegment, FID_LastCandidate, FID_None },
    { Qt::Key_Left,      FID_None, FID_CaretBackward, FID_BackwardSegment, FID_BackwardSegment, FID_BackwardSegment },
    { Qt::CTRL+Qt::Key_Left,   FID_None, FID_CaretToFirst, FID_ToFirstSegment, FID_FirstCandidate, FID_None },
    { Qt::SHIFT+Qt::Key_Left,  FID_None, FID_CaretBackward, FID_ShortenSegment, FID_ShortenSegment, FID_ShortenSegment },
    { Qt::Key_Right,     FID_None, FID_CaretForward, FID_ForwardSegment, FID_ForwardSegment, FID_ForwardSegment },
    { Qt::CTRL+Qt::Key_Right,  FID_None, FID_CaretToLast, FID_ToLastSegment, FID_LastCandidate, FID_None },
    { Qt::SHIFT+Qt::Key_Right, FID_None, FID_CaretForward, FID_LengthenSegment, FID_LengthenSegment, FID_LengthenSegment },
    { Qt::Key_Up,        FID_None, FID_CaretToFirst, FID_PreviousCandidate, FID_PreviousCandidate, FID_ShortenSegment },
    { Qt::CTRL+Qt::Key_Up,     FID_None, FID_CaretToFirst, FID_PreviousCandidate, FID_PreviousCandidate, FID_None },
    { Qt::SHIFT+Qt::Key_Up,    FID_None, FID_None, FID_None, FID_PreviousCandidateGroup, FID_None },
    { Qt::Key_Down,      FID_None, FID_CaretToLast, FID_NextCandidate, FID_NextCandidate, FID_LengthenSegment },
    { Qt::CTRL+Qt::Key_Down,   FID_None, FID_CaretToLast, FID_DecideCrntSegment, FID_DecideCrntSegment, FID_DecideCrntSegment },
    { Qt::SHIFT+Qt::Key_Down,  FID_None, FID_None, FID_None, FID_NextCandidateGroup, FID_None },
    { Qt::Key_PageUp,    FID_None, FID_None, FID_None, FID_PreviousCandidateGroup, FID_None },
    { Qt::Key_PageDown,  FID_None, FID_None, FID_None, FID_NextCandidateGroup, FID_None },
    { Qt::CTRL+Qt::Key_A, FID_None, FID_CaretToFirst, FID_ToFirstSegment, FID_FirstCandidate, FID_None },
    { Qt::CTRL+Qt::Key_D, FID_None, FID_CaretForward, FID_ForwardSegment, FID_ForwardSegment, FID_ForwardSegment },
    { Qt::CTRL+Qt::Key_E, FID_None, FID_CaretToFirst, FID_PreviousCandidate, FID_PreviousCandidate, FID_ShortenSegment },
    { Qt::CTRL+Qt::Key_F, FID_None, FID_CaretToLast, FID_ToLastSegment, FID_LastCandidate, FID_None },
    { Qt::CTRL+Qt::Key_G, FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::CTRL+Qt::Key_H, FID_None, FID_DeleteBackwardChar, FID_CancelConversion, FID_CancelConversion, FID_None },
    { Qt::CTRL+Qt::Key_I, FID_None, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana },
    { Qt::CTRL+Qt::Key_K, FID_None, FID_CaretBackward, FID_ShortenSegment, FID_ShortenSegment, FID_ShortenSegment },
    { Qt::CTRL+Qt::Key_L, FID_None, FID_CaretForward, FID_LengthenSegment, FID_LengthenSegment, FID_LengthenSegment },
    { Qt::CTRL+Qt::Key_M, FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::CTRL+Qt::Key_N, FID_None, FID_CaretToLast, FID_DecideCrntSegment, FID_DecideCrntSegment, FID_DecideCrntSegment },
    { Qt::CTRL+Qt::Key_O, FID_None, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana },
    { Qt::CTRL+Qt::Key_P, FID_None, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu },
    { Qt::CTRL+Qt::Key_S, FID_None, FID_CaretBackward, FID_BackwardSegment, FID_BackwardSegment, FID_BackwardSegment },
    { Qt::CTRL+Qt::Key_T, FID_None, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu },
    { Qt::CTRL+Qt::Key_U, FID_None, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira },
    { Qt::CTRL+Qt::Key_X, FID_None, FID_CaretToLast, FID_NextCandidate, FID_NextCandidate, FID_LengthenSegment },
    { Qt::CTRL+Qt::Key_Z, FID_None, FID_DeleteAll, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::Key_F6,        FID_None, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira },
    { Qt::Key_F7,        FID_None, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana },
    { Qt::Key_F8,        FID_None, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana },
    { Qt::Key_F9,        FID_None, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu },
    { Qt::Key_F10,       FID_None, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu },
    { Qt::Key_Henkan,    FID_ReconvertClipboardString, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::Key_Muhenkan,  FID_SetToKatakanaMode, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana },
  },
  
  {  // ATOK
    { Qt::Key_Space,     FID_InsertSpace, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_Space,  FID_None, FID_ConvertToOneSegment, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::SHIFT+Qt::Key_Space, FID_InsertOtherWidthSpace, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::Key_Return,    FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::CTRL+Qt::Key_Return,    FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::SHIFT+Qt::Key_Return,    FID_None, FID_DecidePredictedCandidate, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::Key_Enter,     FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::CTRL+Qt::Key_Enter,      FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::SHIFT+Qt::Key_Enter,     FID_None, FID_DecidePredictedCandidate, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::Key_Backspace, FID_None, FID_DeleteBackwardChar, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::SHIFT+Qt::Key_Backspace, FID_None, FID_DeleteBackwardChar, FID_None, FID_None, FID_None },
    { Qt::Key_Delete,    FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::CTRL+Qt::Key_Delete,     FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::SHIFT+Qt::Key_Delete,    FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::Key_Escape,    FID_None, FID_DeleteAll, FID_DeleteAll, FID_DeleteAll, FID_DeleteAll },
    { Qt::SHIFT+Qt::Key_Escape, FID_None, FID_DeleteAll, FID_DeleteAll, FID_DeleteAll, FID_DeleteAll },
    { Qt::Key_Insert,    FID_None, FID_None, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::CTRL+Qt::Key_Insert, FID_None, FID_None, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::SHIFT+Qt::Key_Insert, FID_None, FID_None, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::Key_Home,      FID_None, FID_CaretToFirst, FID_None, FID_FirstCandidate, FID_None },
    { Qt::Key_End,       FID_None, FID_CaretToLast, FID_None, FID_LastCandidate, FID_None },
    { Qt::Key_Left,      FID_None, FID_CaretBackward, FID_ShortenSegment, FID_ShortenSegment, FID_ShortenSegment },
    { Qt::CTRL+Qt::Key_Left,   FID_None, FID_CaretToFirst, FID_ToFirstSegment, FID_ToFirstSegment, FID_ShortenSegment },
    { Qt::SHIFT+Qt::Key_Left,  FID_None, FID_CaretBackward, FID_BackwardSegment, FID_BackwardSegment, FID_ShortenSegment },    
    { Qt::Key_Right,     FID_None, FID_CaretForward, FID_LengthenSegment, FID_LengthenSegment, FID_LengthenSegment },
    { Qt::CTRL+Qt::Key_Right,  FID_None, FID_CaretToLast, FID_ToLastSegment, FID_ToLastSegment, FID_LengthenSegment },
    { Qt::SHIFT+Qt::Key_Right, FID_None, FID_CaretForward, FID_ForwardSegment, FID_ForwardSegment, FID_LengthenSegment },
    { Qt::Key_Up,        FID_None, FID_None, FID_PreviousCandidate, FID_PreviousCandidate, FID_None },
    { Qt::CTRL+Qt::Key_Up,     FID_None, FID_None, FID_PreviousCandidate, FID_PreviousCandidate, FID_None },
    { Qt::SHIFT+Qt::Key_Up,    FID_None, FID_None, FID_PreviousCandidate, FID_PreviousCandidate, FID_None },
    { Qt::Key_Down,      FID_None, FID_DecideAllSegments, FID_DecideCrntSegment, FID_DecideCrntSegment, FID_DecideCrntSegment },
    { Qt::SHIFT+Qt::Key_Down,  FID_None, FID_None, FID_DecideCrntSegment, FID_DecideCrntSegment, FID_DecideCrntSegment },
    { Qt::Key_PageUp,    FID_None, FID_None, FID_PreviousCandidateGroup, FID_PreviousCandidateGroup, FID_None },
    { Qt::Key_PageDown,  FID_None, FID_None, FID_NextCandidateGroup, FID_NextCandidateGroup, FID_None }, 
    { Qt::CTRL+Qt::Key_A, FID_None, FID_CaretToFirst, FID_ToFirstSegment, FID_ToFirstSegment, FID_ShortenSegment },
    { Qt::CTRL+Qt::Key_D, FID_None, FID_CaretForward, FID_ForwardSegment, FID_ForwardSegment, FID_LengthenSegment },
    { Qt::CTRL+Qt::Key_E, FID_None, FID_None, FID_PreviousCandidate, FID_PreviousCandidate, FID_None },
    { Qt::CTRL+Qt::Key_F, FID_None, FID_CaretToLast, FID_ToLastSegment, FID_ToLastSegment, FID_LengthenSegment },
    { Qt::CTRL+Qt::Key_G, FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::CTRL+Qt::Key_H, FID_None, FID_DeleteBackwardChar, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::CTRL+Qt::Key_I, FID_None, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana },
    { Qt::CTRL+Qt::Key_K, FID_None, FID_CaretBackward, FID_ShortenSegment, FID_ShortenSegment, FID_ShortenSegment },
    { Qt::CTRL+Qt::Key_L, FID_None, FID_CaretForward, FID_LengthenSegment, FID_LengthenSegment, FID_LengthenSegment },
    { Qt::CTRL+Qt::Key_M, FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::CTRL+Qt::Key_N, FID_None, FID_DecideAllSegments, FID_DecideCrntSegment, FID_DecideCrntSegment, FID_DecideCrntSegment },
    { Qt::CTRL+Qt::Key_O, FID_None, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana },
    { Qt::CTRL+Qt::Key_P, FID_None, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu },
    { Qt::CTRL+Qt::Key_S, FID_None, FID_CaretBackward, FID_BackwardSegment, FID_BackwardSegment, FID_ShortenSegment },   
    { Qt::CTRL+Qt::Key_U, FID_None, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira },
    { Qt::Key_F6,        FID_None, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira },
    { Qt::Key_F7,        FID_None, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana },
    { Qt::CTRL+Qt::Key_F7,  FID_ExecDictTool, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_F8,        FID_None, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana },
    { Qt::Key_F9,        FID_None, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu },
    { Qt::Key_F10,       FID_None, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu },
    { Qt::CTRL+Qt::Key_F12, FID_ShowPropertyDialog, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_Henkan,    FID_ReconvertClipboardString, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::SHIFT+Qt::Key_Henkan, FID_None, FID_ConvertAllSegments, FID_PreviousCandidate, FID_PreviousCandidate, FID_ConvertCrntSegment },
  },
  
  {  // KINPUT2
    { Qt::Key_Space,      FID_InsertSpace, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_Space, FID_None, FID_ConvertToOneSegment, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::SHIFT+Qt::Key_Space, FID_InsertOtherWidthSpace, FID_ConvertAllSegments, FID_PreviousCandidate, FID_PreviousCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::SHIFT+Qt::Key_Space, FID_InsertMultibyteSpace, FID_ConvertAllSegments, FID_PreviousCandidate, FID_PreviousCandidate, FID_ConvertCrntSegment },
    { Qt::Key_Return,     FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::SHIFT+Qt::Key_Return, FID_None, FID_DecidePredictedCandidate, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::Key_Enter,      FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::SHIFT+Qt::Key_Enter,  FID_None, FID_DecidePredictedCandidate, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::Key_Backspace,  FID_None, FID_DeleteBackwardChar, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::Key_Delete,     FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::Key_Home,      FID_None, FID_None, FID_FirstCandidate, FID_FirstCandidate, FID_ConvertCrntSegment },
    { Qt::Key_End,       FID_None, FID_None, FID_LastCandidate, FID_LastCandidate, FID_ConvertCrntSegment },
    { Qt::Key_Left,      FID_None, FID_CaretBackward, FID_BackwardSegment, FID_BackwardSegment, FID_BackwardSegment },
    { Qt::CTRL+Qt::Key_Left,  FID_None, FID_CaretToFirst, FID_ToFirstSegment, FID_ToFirstSegment, FID_ToFirstSegment },
    { Qt::SHIFT+Qt::Key_Left,  FID_None, FID_None, FID_ShortenSegment, FID_ShortenSegment, FID_ShortenSegment },
    { Qt::Key_Right,     FID_None, FID_CaretForward, FID_ForwardSegment, FID_ForwardSegment, FID_ForwardSegment },
    { Qt::CTRL+Qt::Key_Right, FID_None, FID_CaretToLast, FID_ToLastSegment, FID_ToLastSegment, FID_ToLastSegment },
    { Qt::SHIFT+Qt::Key_Right, FID_None, FID_None, FID_LengthenSegment, FID_LengthenSegment, FID_LengthenSegment },
    { Qt::Key_Up,        FID_None, FID_None, FID_PreviousCandidate, FID_PreviousCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_Up,    FID_None, FID_None, FID_PreviousCandidateGroup, FID_PreviousCandidateGroup, FID_ConvertCrntSegment },
    { Qt::SHIFT+Qt::Key_Up,    FID_None, FID_None, FID_PreviousCandidate, FID_PreviousCandidate, FID_ConvertCrntSegment },
    { Qt::Key_Down,      FID_None, FID_None, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_Down,  FID_None, FID_None, FID_NextCandidateGroup, FID_NextCandidateGroup, FID_ConvertCrntSegment },
    { Qt::SHIFT+Qt::Key_Down,  FID_None, FID_None, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::Key_PageUp,    FID_None, FID_None, FID_PreviousCandidateGroup, FID_PreviousCandidateGroup, FID_ConvertCrntSegment },
    { Qt::Key_PageDown,  FID_None, FID_None, FID_NextCandidateGroup, FID_NextCandidateGroup, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_A, FID_None, FID_CaretToFirst, FID_ToFirstSegment, FID_ToFirstSegment, FID_ToFirstSegment },
    { Qt::CTRL+Qt::Key_B, FID_None, FID_CaretBackward, FID_BackwardSegment, FID_BackwardSegment, FID_BackwardSegment },
    { Qt::CTRL+Qt::Key_D, FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::CTRL+Qt::Key_E, FID_None, FID_CaretToLast, FID_ToLastSegment, FID_ToLastSegment, FID_ToLastSegment },
    { Qt::CTRL+Qt::Key_F, FID_None, FID_CaretForward, FID_ForwardSegment, FID_ForwardSegment, FID_ForwardSegment },
    { Qt::CTRL+Qt::Key_G, FID_None, FID_DeleteAll, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::CTRL+Qt::Key_H, FID_None, FID_DeleteBackwardChar, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::CTRL+Qt::Key_I, FID_None, FID_None, FID_ShortenSegment, FID_ShortenSegment, FID_ShortenSegment },
    { Qt::CTRL+Qt::Key_M, FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::CTRL+Qt::Key_N, FID_None, FID_None, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::SHIFT+Qt::Key_N, FID_None, FID_None, FID_NextCandidateGroup, FID_NextCandidateGroup, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_O, FID_None, FID_None, FID_LengthenSegment, FID_LengthenSegment, FID_LengthenSegment },
    { Qt::CTRL+Qt::Key_P, FID_None, FID_None, FID_PreviousCandidate, FID_PreviousCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::SHIFT+Qt::Key_P, FID_None, FID_None, FID_PreviousCandidateGroup, FID_PreviousCandidateGroup, FID_ConvertCrntSegment },
    { Qt::Key_F5,        FID_SwitchInputMethod, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_F6,        FID_SetToHiraganaMode, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira },
    { Qt::Key_F7,        FID_SetToKatakanaMode, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana },
    { Qt::Key_F8,        FID_SetToHankakuKanaMode, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana },
    { Qt::Key_F9,        FID_SetToZenkakuEisuMode, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu },
    { Qt::Key_F10,       FID_None, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu },
    { Qt::Key_F11,       FID_ExecDictTool, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_F12,       FID_ShowPropertyDialog, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_Henkan,    FID_ReconvertClipboardString, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
  },
  
  {  // VJE
    { Qt::Key_Space,     FID_InsertSpace, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_Space, FID_None, FID_ConvertToOneSegment, FID_ForwardSegment, FID_ForwardSegment, FID_ForwardSegment },
    { Qt::SHIFT+Qt::Key_Space, FID_InsertOtherWidthSpace, FID_ConvertAllSegments, FID_LengthenSegment, FID_PreviousCandidate, FID_LengthenSegment },
    { Qt::CTRL+Qt::SHIFT+Qt::Key_Space, FID_InsertMultibyteSpace, FID_None, FID_BackwardSegment, FID_BackwardSegment, FID_None },
    { Qt::Key_Return,    FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::CTRL+Qt::Key_Return,    FID_None, FID_DecideAllSegments, FID_None, FID_None, FID_None },
    { Qt::SHIFT+Qt::Key_Return,    FID_None, FID_DecidePredictedCandidate, FID_DecideAllSegments, FID_DecideAllSegments, FID_ConvertCrntSegment },
    { Qt::Key_Enter,     FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::CTRL+Qt::Key_Enter,     FID_None, FID_DecideAllSegments, FID_None, FID_None, FID_None },
    { Qt::SHIFT+Qt::Key_Enter,     FID_None, FID_DecidePredictedCandidate, FID_DecideAllSegments, FID_DecideAllSegments, FID_ConvertCrntSegment },
    { Qt::Key_Backspace, FID_None, FID_DeleteBackwardChar, FID_CancelConversion, FID_CancelConversion, FID_None },
    { Qt::SHIFT+Qt::Key_Backspace, FID_None,  FID_DeleteAll, FID_DeleteAll, FID_DeleteAll, FID_DeleteAll },
    { Qt::CTRL+Qt::SHIFT+Qt::Key_Backspace, FID_None, FID_DeleteBackwardChar, FID_None, FID_None, FID_None },
    { Qt::Key_Delete,    FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::CTRL+Qt::Key_Delete,    FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::SHIFT+Qt::Key_Delete,    FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::Key_Escape,    FID_None, FID_DeleteAll, FID_CancelConversion, FID_CancelConversion, FID_CancelConversion },
    { Qt::SHIFT+Qt::Key_Escape, FID_None, FID_DeleteAll, FID_DeleteAll, FID_CancelConversion, FID_DeleteAll },
    { Qt::Key_Home,      FID_None, FID_CaretToFirst, FID_ToFirstSegment, FID_FirstCandidate, FID_None },
    { Qt::Key_End,       FID_None, FID_CaretToLast, FID_ToLastSegment, FID_LastCandidate, FID_None },
    { Qt::Key_Left,      FID_None, FID_CaretBackward, FID_BackwardSegment, FID_BackwardSegment, FID_BackwardSegment },
    { Qt::SHIFT+Qt::Key_Left,  FID_None, FID_CaretBackward, FID_ShortenSegment, FID_ShortenSegment, FID_ShortenSegment },
    { Qt::Key_Right,     FID_None, FID_CaretForward, FID_ForwardSegment, FID_ForwardSegment, FID_ForwardSegment },
    { Qt::SHIFT+Qt::Key_Right, FID_None, FID_CaretForward, FID_LengthenSegment, FID_LengthenSegment, FID_LengthenSegment },    
    { Qt::Key_Up,        FID_None, FID_ConvertAllSegments, FID_PreviousCandidate, FID_PreviousCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_Up,     FID_None, FID_ConvertAllSegments, FID_ShortenSegment, FID_ShortenSegment, FID_ShortenSegment },
    { Qt::SHIFT+Qt::Key_Up,    FID_None, FID_ConvertAllSegments, FID_PreviousCandidate, FID_PreviousCandidateGroup, FID_ConvertCrntSegment },
    { Qt::Key_Down,      FID_None, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_Down,   FID_None, FID_ConvertAllSegments, FID_DecideCrntSegment, FID_DecideCrntSegment, FID_LengthenSegment },
    { Qt::SHIFT+Qt::Key_Down,  FID_None, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidateGroup, FID_ConvertCrntSegment },
    { Qt::Key_PageUp,    FID_None, FID_CaretToFirst, FID_PreviousCandidate, FID_PreviousCandidateGroup, FID_None },
    { Qt::Key_PageDown,  FID_None, FID_CaretToLast, FID_NextCandidate, FID_NextCandidateGroup, FID_None }, 
    { Qt::CTRL+Qt::Key_A, FID_None, FID_CaretToFirst, FID_ToFirstSegment, FID_None, FID_None },
    { Qt::CTRL+Qt::Key_C, FID_None, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_D, FID_None, FID_CaretForward, FID_ForwardSegment, FID_ForwardSegment, FID_ForwardSegment },
    { Qt::CTRL+Qt::Key_E, FID_None, FID_CaretToFirst, FID_PreviousCandidate, FID_PreviousCandidateGroup, FID_None },
    { Qt::CTRL+Qt::Key_F, FID_None, FID_CaretToLast, FID_ToLastSegment, FID_None, FID_None },
    { Qt::CTRL+Qt::Key_G, FID_None, FID_DeleteForwardChar, FID_None, FID_None, FID_None },
    { Qt::CTRL+Qt::Key_H, FID_None, FID_DeleteBackwardChar, FID_CancelConversion, FID_CancelConversion, FID_None },
    { Qt::CTRL+Qt::Key_I, FID_None, FID_None, FID_ForwardSegment, FID_ForwardSegment, FID_ForwardSegment },
    { Qt::CTRL+Qt::Key_J, FID_None, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira },
    { Qt::CTRL+Qt::Key_K, FID_None, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana },
    { Qt::CTRL+Qt::Key_L, FID_None, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu },
    { Qt::CTRL+Qt::Key_M, FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments, FID_DecideAllSegments },
    { Qt::CTRL+Qt::Key_O, FID_None, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana },
    { Qt::CTRL+Qt::Key_Q, FID_None, FID_CaretBackward, FID_ShortenSegment, FID_ShortenSegment, FID_ShortenSegment },
    { Qt::CTRL+Qt::Key_R, FID_None, FID_CaretToFirst, FID_ToFirstSegment, FID_PreviousCandidate, FID_BackwardSegment },
    { Qt::CTRL+Qt::Key_S, FID_None, FID_CaretBackward, FID_BackwardSegment, FID_BackwardSegment, FID_BackwardSegment },
    { Qt::CTRL+Qt::Key_W, FID_None, FID_CaretForward, FID_LengthenSegment, FID_LengthenSegment, FID_LengthenSegment },
    { Qt::CTRL+Qt::Key_X, FID_None, FID_CaretToLast, FID_NextCandidate, FID_NextCandidateGroup, FID_None },
    { Qt::CTRL+Qt::Key_Z, FID_None, FID_ConvertAllSegments, FID_PreviousCandidate, FID_PreviousCandidate, FID_ConvertCrntSegment },
    { Qt::CTRL+Qt::Key_F2,  FID_ExecDictTool, FID_None, FID_None, FID_None, FID_None },
    { Qt::CTRL+Qt::Key_F3,  FID_SwitchInputMethod, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_F6,        FID_None, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira, FID_ConvertToHira },
    { Qt::CTRL+Qt::Key_F6,  FID_SetToHiraganaMode, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_F7,        FID_None, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana, FID_ConvertToKana },
    { Qt::CTRL+Qt::Key_F7,  FID_SetToKatakanaMode, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_F8,        FID_None, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu, FID_ConvertToZenkakuEisu },
    { Qt::CTRL+Qt::Key_F8,  FID_SetToZenkakuEisuMode, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_F9,        FID_None, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana, FID_ConvertToHankakuKana },
    { Qt::CTRL+Qt::Key_F9,  FID_SetToHankakuKanaMode, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_F10,       FID_None, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu, FID_ConvertToHankakuEisu },
    { Qt::CTRL+Qt::Key_F10, FID_ShowPropertyDialog, FID_None, FID_None, FID_None, FID_None },
    { Qt::Key_Henkan,    FID_ReconvertClipboardString, FID_ConvertAllSegments, FID_NextCandidate, FID_NextCandidate, FID_ConvertCrntSegment },
    { Qt::Key_Muhenkan,  FID_None, FID_DecideAllSegments, FID_DecideAllSegments, FID_ForwardSegment, FID_ForwardSegment },
  },

  { { 0 } },     // Current setting  (dummy)
};


KeyAssigner::KeyAssigner(QWidget* parent) : QDialog(parent)
{
  _ui.setupUi(this);
  _ui._keyassigntbl->setColumnCount( NUM_OF_CONVSTATUS + 1 );
  _ui._keyassigntbl->verticalHeader()->setResizeMode(QHeaderView::Stretch);
  _ui._keyassigntbl->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  QStringList headerlabel;
  headerlabel << tr("キー") << tr("未入力") << tr("入力中") << tr("変換済み")
              << tr("候補一覧表示中") << tr("文節長変更中");
  _ui._keyassigntbl->setHorizontalHeaderLabels(headerlabel);

  _ui._cmbloadsetting->insertItem(ST_MSIME, tr("MS-IME風"));
  _ui._cmbloadsetting->insertItem(ST_ATOK, tr("ATOK風"));
  _ui._cmbloadsetting->insertItem(ST_KINPUT2, tr("Kinput2風"));
  _ui._cmbloadsetting->insertItem(ST_VJE, tr("VJE風"));
  _ui._cmbloadsetting->setToolTip( tr("リストから選択されたキー設定を読み込みます。") );

  // Creates connections
  connect(_ui._okbtn, SIGNAL(clicked()), this, SLOT(accept()));
  connect(_ui._cancelbtn, SIGNAL(clicked()), this, SLOT(reject()));
  connect(_ui._cmbloadsetting, SIGNAL(activated(int)), this, SLOT(loadSetting(int)));

  // Call init function
  QTimer::singleShot(0, this, SLOT(init()));
}


void KeyAssigner::init()
{
  DEBUG_TRACEFUNC();
  FuncName[FID_None]                     = "-";
  FuncName[FID_InsertChar]               = tr("入力");
  FuncName[FID_NextCandidate]            = tr("次候補");
  FuncName[FID_NextCandidateGroup]       = tr("次候補群");
  FuncName[FID_PreviousCandidate]        = tr("前候補");
  FuncName[FID_PreviousCandidateGroup]   = tr("前候補群");
  FuncName[FID_FirstCandidate]           = tr("最初候補");
  FuncName[FID_LastCandidate]            = tr("最終候補");
  FuncName[FID_ConvertAllSegments]       = tr("全変換");
  FuncName[FID_ConvertCrntSegment]       = tr("文節変換");
  FuncName[FID_ConvertToOneSegment]      = tr("単文節変換");
  FuncName[FID_DeleteForwardChar]        = tr("１字削除");
  FuncName[FID_DeleteBackwardChar]       = tr("前字削除");
  FuncName[FID_DeleteAll]                = tr("全消去");
  FuncName[FID_DecideAllSegments]        = tr("全確定");
  FuncName[FID_DecideCrntSegment]        = tr("文節確定");
  FuncName[FID_DecidePredictedCandidate] = tr("予測候補確定");
  FuncName[FID_CancelConversion]         = tr("全戻し");
  FuncName[FID_CancelSegmentConversion]  = tr("文節戻し");    // 未実装
  FuncName[FID_LengthenSegment]          = tr("文節長＋１");
  FuncName[FID_ShortenSegment]           = tr("文節長‐１");
  FuncName[FID_InsertSpace]              = tr("空白文字");
  FuncName[FID_InsertAsciiSpace]         = tr("半角空白");
  FuncName[FID_InsertMultibyteSpace]     = tr("全角空白");
  FuncName[FID_InsertOtherWidthSpace]    = tr("別幅空白");
  FuncName[FID_ConvertToHira]            = tr("ひらがな");
  FuncName[FID_ConvertToKana]            = tr("カタカナ");
  FuncName[FID_ConvertToHankakuKana]     = tr("半角カナ");
  FuncName[FID_ConvertToHankakuEisu]     = tr("半角英数");
  FuncName[FID_ConvertToZenkakuEisu]     = tr("全角英数");
  FuncName[FID_ForwardSegment]           = tr("文節右移動");
  FuncName[FID_BackwardSegment]          = tr("文節左移動");
  FuncName[FID_ToFirstSegment]           = tr("文節先頭");
  FuncName[FID_ToLastSegment]            = tr("文節末尾");
  FuncName[FID_CaretForward]             = tr("右移動");
  FuncName[FID_CaretBackward]            = tr("左移動");
  FuncName[FID_CaretToFirst]             = tr("先頭移動");
  FuncName[FID_CaretToLast]              = tr("末尾移動");
  FuncName[FID_SwitchZenkakuEisuMode]    = tr("全角英数切替");
  FuncName[FID_SetToHiraganaMode]        = tr("ひらがなモード");
  FuncName[FID_SetToKatakanaMode]        = tr("カタカナモード");
  FuncName[FID_SetToHankakuKanaMode]     = tr("半角カナモード");
  FuncName[FID_SetToZenkakuEisuMode]     = tr("全角英数モード");
  FuncName[FID_SwitchInputMethod]        = tr("入力方式切替");
  FuncName[FID_ShowPropertyDialog]       = tr("プロパティ");
  FuncName[FID_ExecDictTool]             = tr("辞書ツール");
  FuncName[FID_ReconvertClipboardString] = tr("再変換(実験的)");

  // Loads setting
  loadSetting(ST_CURRENT_SETTING);
}


void KeyAssigner::saveSetting()
{
  DEBUG_TRACEFUNC();
  for (int row = 0; row < MAX_AVAILABLE_KEYS; ++row) {
    if (row < _ui._keyassigntbl->rowCount()) {
      QKeySequence keyseq( _ui._keyassigntbl->item(row, 0)->data(Qt::UserRole).toInt() );
      int key = keyseq[0] & ~Qt::UNICODE_ACCEL;
      Config::writeEntry(PREFIX + QString::number(row) + "-" + QString::number(0), key);

      for (int col = 1; col <= NUM_OF_CONVSTATUS; ++col) {
	Config::writeEntry(PREFIX + QString::number(row) 
			   + "-" + QString::number(col), (int)functionID(_ui._keyassigntbl->item(row, col)->text()));
      }
    
    } else {
      // 0 clear
      for (int col = 0; col <= NUM_OF_CONVSTATUS; ++col)
	Config::writeEntry(PREFIX + QString::number(row) 
			   + "-" + QString::number(col), (int)0);
    }
  }

  Config::writeEntry("_cmbloadsetting", _ui._cmbloadsetting->currentIndex());
  CurrentKeyAssign.clear();  // implies reloading function
}


void KeyAssigner::loadSetting( int index )
{
  DEBUG_TRACEFUNC("index: %d", index);
  _ui._keyassigntbl->clearContents();
  _ui._keyassigntbl->setRowCount(MAX_AVAILABLE_KEYS);

  int row = 0;
  switch ( index ) {
  case ST_CURRENT_SETTING:
    for (row = 0; row < MAX_AVAILABLE_KEYS; ++row) {
      int key = Config::readNumEntry(PREFIX + QString::number(row) + "-" + QString::number(0), 0);
      if ( !key )
	break;

      QKeySequence keyseq(key);
      setKeySequenceName(row, keyseq);

      for (int col = 1; col <= NUM_OF_CONVSTATUS; ++col) {
	int funcid = Config::readNumEntry(PREFIX + QString::number(row) + "-" + QString::number(col), FID_None);
	_ui._keyassigntbl->setItem(row, col, new QTableWidgetItem(functionName( (FuncID)funcid )));
      }
    }
    _ui._keyassigntbl->setRowCount(row);
    _ui._cmbloadsetting->setCurrentIndex( Config::readNumEntry("_cmbloadsetting", ST_MSIME) );
    break;
    
  case ST_MSIME:
  case ST_ATOK:
  case ST_KINPUT2:
  case ST_VJE:
    for (row = 0; row < MAX_AVAILABLE_KEYS; ++row) {
      if ( ! KeyAssignSetting[index][row][0] )
	break;

      QKeySequence keyseq( KeyAssignSetting[index][row][0] );
      setKeySequenceName(row, keyseq);

      for (int col = 1; col <= NUM_OF_CONVSTATUS; ++col) {
	_ui._keyassigntbl->setItem(row, col, new QTableWidgetItem(functionName( (FuncID)KeyAssignSetting[index][row][col] )));
      }
    }
    _ui._keyassigntbl->setRowCount(row);
    break;
    
  default:
    break;
  }

  update();
}


QString KeyAssigner::functionName( FuncID id )
{
  DEBUG_TRACEFUNC("id: %d", (int)id);
  return FuncName.value(id);
}


FuncID KeyAssigner::functionID( const QString & func )
{
  DEBUG_TRACEFUNC("func: %s", qPrintable(func));
  int id;
  for (id = 0; id < FID_NumFunctions; ++id) {
    if (FuncName[id] == func) 
      break;
  }
  return (id < FID_NumFunctions) ? (FuncID)id : FID_None;
}


FuncID KeyAssigner::functionID( int key, ConvStatus stat )
{
  DEBUG_TRACEFUNC("key: 0x%x  stat: %d", key, stat);

  if (CurrentKeyAssign.empty()) {
    // Loads current-key-assign data
    for (int i = 0; i < MAX_AVAILABLE_KEYS; ++i) {
      QVector<int> v;
      int key = Config::readNumEntry(PREFIX + QString::number(i) + "-" + QString::number(0), 0);
      if ( key ) {
        for (int j = 1; j <= NUM_OF_CONVSTATUS; ++j) {
          v <<  Config::readNumEntry(PREFIX + QString::number(i) + "-" + QString::number(j), FID_None);
        }
        CurrentKeyAssign.insert(key, v);
      }
    }
  }

  if (CurrentKeyAssign.contains(key)) {
    return (FuncID)CurrentKeyAssign.value(key).value((int)stat);
  }
  
  if (key & ~(0xff | Qt::SHIFT)) {
    return FID_None;
  }
  
  return FID_InsertChar;
}


void KeyAssigner::accept()
{
  DEBUG_TRACEFUNC();
  saveSetting();
  QDialog::accept();
}


void KeyAssigner::reject()
{
  DEBUG_TRACEFUNC();
  QDialog::reject();
  loadSetting(ST_CURRENT_SETTING);
}


void KeyAssigner::saveDefaultSetting()
{
  DEBUG_TRACEFUNC();
  const int setting = ST_MSIME;
  for (int row = 0; row < MAX_AVAILABLE_KEYS; ++row) {
    if ( KeyAssignSetting[setting][row][0] ) {
      for (int col = 0; col <= NUM_OF_CONVSTATUS; ++col) {
	Config::writeEntry(PREFIX + QString::number(row) + "-" + QString::number(col), KeyAssignSetting[setting][row][col], FALSE);
      }

    } else {
      for (int col = 0; col <= NUM_OF_CONVSTATUS; ++col) {
	Config::writeEntry(PREFIX + QString::number(row) + "-" + QString::number(col), (int)0, FALSE);
      }
    }
  }

  Config::writeEntry("_cmbloadsetting", setting, FALSE);
}


void KeyAssigner::setKeySequenceName(int row, const QKeySequence& keyseq)
{
  DEBUG_TRACEFUNC();
  QTableWidgetItem* item = new QTableWidgetItem();
  item->setData(Qt::UserRole, keyseq[0]);   // Sets the key sequence element
  
  if (keyseq == QKeySequence(Qt::Key_Henkan)) {
    item->setText(tr("変換"));
  } else if (keyseq == QKeySequence(Qt::Key_Muhenkan)) {
    item->setText(tr("無変換"));
  } else if (keyseq == QKeySequence(Qt::SHIFT+Qt::Key_Henkan)) {
    item->setText(tr("Shift+変換"));
  } else {
    item->setText(keyseq.toString());
  }
  _ui._keyassigntbl->setItem(row, 0, item);
}
