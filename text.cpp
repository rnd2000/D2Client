#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "WINMAIN.h"
#include "CoreArchive.h"
#include "ui.h"
#include "text.h"

//  TEXT_DrawPopup calls timeGetTime, which is winmm's. DLLmain.h defines WIN32_LEAN_AND_MEAN, so mmsystem.h is included explicitly, and the
//  project links no extra libraries, so the import library is named here.
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

//  text.cpp -- the text popups and message lists of D2Client.dll 1.10: the NPC speech popup and its message handlers, the hover text above units,
//  the chat, quick-message and message-log lists, the handlers of server packets 0x5A (system lines), 0x27 (an NPC's lines) and 0x26 (chat),
//  the quest scroll panel (the Inifuss scroll and the Cairn Stones pentagon) and its dismissal.
//  The original source file is UI\text.cpp (the asserts in this unit name it). Address range 0x6FB1A7C0..0x6FB1F36E; the compiler's static
//  initialiser CRT_StaticInit17 (0x6FB1F350) ends the unit and is not rewritten.

//  Layout and constant checks for the code below.
static_assert(UI_INISCROLL == 16,
              "the ini-scroll panel's id, as UI_SetUIToggle is given it");
static_assert(offsetof(SMSGHANDLER_PARAMS, uMessage) == 0x04
                  && offsetof(SMSGHANDLER_PARAMS, dwArg) == 0x14,
              "the two fields of the message record this unit reads that "
              "the panel files do not -- the rest of the record is pinned "
              "in inv.cpp, trade.cpp, waypoint.cpp and help.cpp");


//1.10f: D2Client.0x6FB1E8F0
//  The Inifuss scroll (UI_INISCROLL) dismisses itself and frees its three images. Both mouse buttons, WM_LBUTTONDOWN and WM_RBUTTONDOWN, call
//  this handler and it does not look at which one it was (uMessage is never read). The hit test is two half-open bounds with no lower bound on
//  either axis: nYpos < gnScreenHeight - 48 and nXpos < gnScreenWidth / 2, so a click anywhere in the upper-left region closes the scroll.
//  The coordinates are the 16-bit fields of the message, zero-extended and compared signed (uint16_t promoted to int).
//  The images are freed in the order gIniScrollImage2, gIniScrollImage3, gIniScrollImage; each is null-checked, unloaded and cleared.
void __stdcall TEXT_IniScrollOnMouseDown(SMSGHANDLER_PARAMS *pMsg)
{
    if (pMsg->nYpos < gnScreenHeight - 48 && pMsg->nXpos < gnScreenWidth / 2)
    {
        UI_SetUIToggle(UI_INISCROLL, UI_TOGGLE, 0);

        if (gIniScrollImage2)
        {
            ARCHIVE_CellFileFree(gIniScrollImage2);
            gIniScrollImage2 = NULL;
        }
        if (gIniScrollImage3)
        {
            ARCHIVE_CellFileFree(gIniScrollImage3);
            gIniScrollImage3 = NULL;
        }
        if (gIniScrollImage)
        {
            ARCHIVE_CellFileFree(gIniScrollImage);
            gIniScrollImage = NULL;
        }

        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        GAME_ResetClickState();
    }
}


//  Data shapes used throughout the file:
//      D2ClientTextLineStrc   8 bytes   { pszText, pNext }; one allocation per node
//      D2ClientTextNodeStrc  38 bytes   { pLines[6], wLineCount, nColor, dwExpireTick, pNext }; six allocations per node
//      D2TextDisplayListStrc  6 bytes   { pHead, wCount }; the list header
//  The message node is packed: pLines[6] fills 0x00..0x17, wLineCount is at 0x18, nColor at 0x1A, dwExpireTick at 0x1E and the link pNext at 0x22.

//  The original's assert and registry strings.
static const char szTextCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\text.cpp";
static const char szAssertMsgCount[] = "wCountDeleted == pGlobalMessageList->wCount";
static const char szRegKeyDiabloII[] = "Diablo II";
static const char szRegTextDisplayBeta[] = "Text Display Beta";
static const char szAssertTextNode[] = "pTextNode != NULL";
static const char szAssertGlobalCount[] = "wCount == sgTextState.GlobalMessageDisplayList.wCount - 1";
static const char szAssertQuickCount[] = "wCount == sgTextState.QuickMessageDisplayList.wCount - 1";
static_assert(sizeof(szTextCpp) == 55 + 1 && sizeof(szAssertMsgCount) == 43 + 1
                  && sizeof(szRegKeyDiabloII) == 9 + 1
                  && sizeof(szRegTextDisplayBeta) == 17 + 1
                  && sizeof(szAssertTextNode) == 17 + 1
                  && sizeof(szAssertGlobalCount) == 57 + 1
                  && sizeof(szAssertQuickCount) == 56 + 1,
              "the seven strings at 0x6FB8E3E4, 0x6FB8E430, 0x6FB74150, 0x6FB8E41C, "
              "0x6FB8E49C, 0x6FB8E4B0 and 0x6FB8E4EC, measured in the DLL");

//  The quest scroll's strings. aDataGlobal is one object in the original's image that every unit pushes the address of; each rewritten unit
//  keeps its own copy.
static const char aDataGlobal[] = "DATA\\GLOBAL";
static const char szFmtScroin[] = "%s\\UI\\MENU\\scroin";
static const char szFmtPathJoin[] = "%s\\%s";
static const char szUiMenuScroin2[] = "UI\\menu\\scroin2";
static const char szUiMenuScroin3[] = "UI\\menu\\scroin3";
static const char szAssertRunePos[] = "w < arrsize(sgRunePos)";
static_assert(sizeof(aDataGlobal) == 11 + 1 && sizeof(szFmtScroin) == 17 + 1
                  && sizeof(szFmtPathJoin) == 5 + 1
                  && sizeof(szUiMenuScroin2) == 15 + 1
                  && sizeof(szUiMenuScroin3) == 15 + 1
                  && sizeof(szAssertRunePos) == 22 + 1,
              "the six strings at 0x6FB743EC, 0x6FB8E868, 0x6FB8E8A4, 0x6FB8E8AC, "
              "0x6FB8E894 and 0x6FB8E87C, measured in the DLL");

//  Member offsets the file reads at fixed positions.
static_assert(offsetof(D2ClientTextNodeStrc, dwExpireTick) == 0x1E
                  && offsetof(D2ClientTextNodeStrc, pNext) == 0x22,
              "the packed message node: `mov eax, [eax+22h]` at 0x6FB1F1B5 walks the "
              "list and TEXT_ExpireAllChatMessages zeroes +0x1E");
static_assert(offsetof(D2TextDisplayListStrc, wCount) == 4,
              "the count the `wCountDeleted == pGlobalMessageList->wCount` assert "
              "compares against, at +4 of the list header");
static_assert(offsetof(D2TextStateStrc, hPopupSpeechSound) == 155,
              "TEXT_Init's memset of 0x9C plus a word and a byte covers 0..158, which "
              "is sizeof(D2TextStateStrc) exactly -- the last member starts at 155");
static_assert(offsetof(D2ClientPopupStrc, wWidth) == 0x10
                  && offsetof(D2ClientPopupStrc, wHeight) == 0x12,
              "the two fields TEXT_MeasureMessageBox writes and TEXT_DrawHoverBox "
              "reads back");
static_assert(offsetof(D2ClientTextLineStrc, pNext) == 4,
              "the line node's link, the other node shape in this file");

//  Strides and named constants the code encodes (a shift, a scaled index, a memset count).
static_assert(sizeof(D2QuestMenuStringStrc) == 4,
              "TEXT_QuestGetQuestMenuString indexes with [eax*4] and reaches record "
              "100 as +0x190 == 100 * 4");
static_assert(sizeof(D2CoordStrc) == 8,
              "TEXT_DrawQuestScrollFrame turns a stone index into a byte offset "
              "with `shl esi, 3` at 0x6FB1ED3E");
static_assert(sizeof(D2GfxDataStrc) == 0x48,
              "both draw records are cleared by `rep stosd` with ecx == 0x12, "
              "which is the only thing tying the local to the type");
static_assert(D2FONT_FONTFORMAL12 == 5,
              "the font TEXT_DrawQuestScrollFrame sets as the literal 5 at "
              "0x6FB1EC59");
static_assert(UI_TOGGLE == 2,
              "TEXT_IniScrollOpen's `mov edx, 2` at 0x6FB1EE2B -- the panel is "
              "TOGGLED, not turned on");
static_assert(STR_IDX_3724_qstsxxx == 3724,
              "the `qstsxxx` miss string, `mov ecx, 0E8Ch` at 0x6FB1E6F0");

//  The message log's strings and the subtitle formats.
static const char szNew[] = "new";
static const char szFmtSubtitlePlayer[] = "%c%c%d";
static const char szFmtSubtitleMonster[] = "%d";
static_assert(sizeof(szNew) == 3 + 1 && sizeof(szFmtSubtitlePlayer) == 6 + 1
                  && sizeof(szFmtSubtitleMonster) == 2 + 1,
              "the three strings at 0x6FB740E8, 0x6FB8E8BC and 0x6FB7707C, "
              "measured in the DLL");

//  Constants of the message log.
static_assert(UI_MSGLOG == 24,
              "`mov ecx, 18h` in all three UI_SetUIToggle call sites of this "
              "group");
static_assert(UI_TURNOFF == 1 && UI_TURNON == 0,
              "TEXT_MsgLogReopen is an OFF then an ON -- `mov edx, 1` then "
              "`xor edx, edx` -- and not a toggle");
static_assert(LANGUAGE_JAPANESE == 6 && LANGUAGE_KOREAN == 7
                  && LANGUAGE_CHINESETWN == 9,
              "TEXT_ChatShowSpeechSubtitle's gate is `cmp eax, 6 / jl`, "
              "`cmp eax, 7 / jle`, `cmp eax, 9 / jnz` -- the three literals "
              "spelled as names here");
static_assert(sizeof(D2ClientSpeechSubtitleStrc) == 4,
              "both subtitle tables are walked with [reg + idx*4] and the "
              "string id read at +2");
static_assert(offsetof(D2UnitStrc, pHoverText) == 0xA4,
              "the subtitle is stored and freed at [esi+0A4h], the only unit "
              "field this unit writes");

//  Strings of the system and chat packet handlers: seven assert texts (the same sentence with a different tbl name in each, hence seven
//  objects), one trace format and two more traces.
static const char szAssertLenDropped1[] = "Unicode::strlen( StrGetStringByIndex(strPlayerDropped1) ) < MAX_GLOBAL_MESSAGE_LEN-MAX_PLR_NAME_LEN-30";
static const char szAssertLenDropped2[] = "Unicode::strlen( StrGetStringByIndex(strPlayerDropped2) ) < MAX_GLOBAL_MESSAGE_LEN-MAX_PLR_NAME_LEN-30";
static const char szAssertLenJoined[] = "Unicode::strlen( StrGetStringByIndex(strPlayerJoined) ) < MAX_GLOBAL_MESSAGE_LEN-MAX_PLR_NAME_LEN-30";
static const char szAssertLenQuit[] = "Unicode::strlen( StrGetStringByIndex(strPlayerQuit) ) < MAX_GLOBAL_MESSAGE_LEN-MAX_PLR_NAME_LEN-30";
static const char szAssertLenNotInGame[] = "Unicode::strlen( StrGetStringByIndex(strPlayerNotInGame) ) < MAX_GLOBAL_MESSAGE_LEN-MAX_PLR_NAME_LEN-30";
static const char szAssertLenNotListening[] = "Unicode::strlen( StrGetStringByIndex(strPlayerNotListening) ) < MAX_GLOBAL_MESSAGE_LEN-MAX_PLR_NAME_LEN-30";
static const char szAssertLenNotInBattlenet[] = "Unicode::strlen( StrGetStringByIndex(strPlayerNotInBattlenet) ) < MAX_GLOBAL_MESSAGE_LEN-MAX_PLR_NAME_LEN-30";
static const char szTraceSlainLens[] = "text.cpp %d %d %d %d\n";
static const char szTraceInvalidMsgLen[] = "invalid msg len at %d of %s\n";
static const char szTraceInvalidMsgParam[] = "invalid msg param at %d of %s\n";
static_assert(sizeof(szAssertLenDropped1) == 103 && sizeof(szAssertLenDropped2) == 103
                  && sizeof(szAssertLenJoined) == 101
                  && sizeof(szAssertLenQuit) == 99
                  && sizeof(szAssertLenNotInGame) == 104
                  && sizeof(szAssertLenNotListening) == 107
                  && sizeof(szAssertLenNotInBattlenet) == 109
                  && sizeof(szTraceSlainLens) == 22
                  && sizeof(szTraceInvalidMsgLen) == 29
                  && sizeof(szTraceInvalidMsgParam) == 31,
              "the ten strings of batch 8, each measured in the DLL -- the "
              "seven assert texts differ only in the tbl name they quote");

//  Constants of the system and chat handlers. The tbl-string guard is MAX_GLOBAL_MESSAGE_LEN (306) - MAX_PLR_NAME_LEN (16) - 30 = 260
//  (`cmp eax, 104h`), because the player's name is pasted in afterwards.
static_assert(offsetof(D2GSPacketSrv5A, nType) == 1
                  && offsetof(D2GSPacketSrv5A, nColor) == 2
                  && offsetof(D2GSPacketSrv5A, dwParam) == 3
                  && offsetof(D2GSPacketSrv5A, nParam) == 7
                  && offsetof(D2GSPacketSrv5A, szText) == 8,
              "packet 0x5A's arm 0, read at [ebp+1], [ebp+2], [ebp+3], "
              "[ebp+7] and [ebp+8]; the second name is szText + 16");
static_assert(sizeof(((D2GSPacketSrv5A *)nullptr)->szText) == 32,
              "two 16-byte name slots, which is why the truncation is at 15");
static_assert(offsetof(D2GSPacketSrv26, nMessageType) == 1
                  && offsetof(D2GSPacketSrv26, nLang) == 2
                  && offsetof(D2GSPacketSrv26, nUnitType) == 3
                  && offsetof(D2GSPacketSrv26, dwUnitGUID) == 4
                  && offsetof(D2GSPacketSrv26, nMessageColor) == 8
                  && offsetof(D2GSPacketSrv26, szName) == 0x0A
                  && offsetof(D2GSPacketSrv26, szMessage) == 0x1A,
              "packet 0x26, every offset the body reads");
static_assert(offsetof(D2GSPacketSrv27, nUnitType) == 1
                  && offsetof(D2GSPacketSrv27, dwUnitGUID) == 2
                  && offsetof(D2GSPacketSrv27, pMessageList) == 6,
              "packet 0x27: `add esi, 6` is what hands the message list on");
static_assert(sizeof(D2MessageStrc) == 4
                  && offsetof(D2MessageStrc, nStringId) == 2,
              "the list's record -- [esi+8] is pMessages[0].nMenu and "
              "[esi+0Ah] its nStringId");
static_assert(offsetof(D2ClientPopupStrc, field_16) == 0x16,
              "TEXT_OnMessageListPacket writes a DWORD 1 across +0x16, which "
              "spans field_16 and field_18");


//1.10f: D2Client.0x6FB1B0B0
//  Frees a parsed-text line list: per node the text and the node itself, following the link at +4. (A message node owns six allocations and
//  links at +0x22; see TEXT_FreeMessageList.)
void __fastcall TEXT_FreeParsedLines(D2ClientTextLineStrc *pFirstLine)
{
    D2ClientTextLineStrc *pLine = pFirstLine;

    if (pLine)
    {
        D2ClientTextLineStrc *pNext;
        do
        {
            pNext = pLine->pNext;
            FOG_Free(pLine->pszText, szTextCpp, 1547, 0);
            FOG_Free(pLine, szTextCpp, 1548, 0);
            pLine = pNext;
        } while (pNext);
    }
}


//1.10f: D2Client.0x6FB1A7C0
//  TEXT_FreeParsedLines plus the header: the line list hanging off *ppParsedText, then the header itself.
//  The original inlines TEXT_FreeParsedLines here.
void __stdcall TEXT_FreeParsedText(void **ppParsedText)
{
    if (ppParsedText)
    {
        D2ClientTextLineStrc *pLine = (D2ClientTextLineStrc *)*ppParsedText;
        TEXT_FreeParsedLines(pLine);
        FOG_Free(ppParsedText, szTextCpp, 1674, 0);
    }
}


//1.10f: D2Client.0x6FB1AAF0
//  Frees one message list: per node the six line buffers and the node, following the link at +0x22. It counts as it goes and asserts the
//  count against the header's wCount at +4, then NULLs the head and zeroes the count.
void __fastcall TEXT_FreeMessageList(D2TextDisplayListStrc *pList)
{
    D2ClientTextNodeStrc *pNode = pList->pHead;
    int16_t wCountDeleted = 0;

    if (pNode)
    {
        do
        {
            D2ClientTextNodeStrc *pNodeToFree = pNode;
            pNode = pNode->pNext;
            for (int32_t i = 0; i < 6; ++i)
            {
                Unicode *pLine = pNodeToFree->pLines[i];
                if (pLine)
                    FOG_Free(pLine, szTextCpp, 1711, 0);
            }
            FOG_Free(pNodeToFree, szTextCpp, 1714, 0);
            ++wCountDeleted;
        } while (pNode);
        pList->pHead = nullptr;
    }

    if (wCountDeleted != (int16_t)pList->wCount)
    {
        FOG_DisplayAssert(szAssertMsgCount, szTextCpp, 1737);
        exit(-1);
    }
    pList->wCount = 0;
}


//1.10f: D2Client.0x6FB1A820
//  One-time init, from UI_InitUiSubsystems. Clears the whole 159-byte state block (one clear, not three statements: the original zeroes 156
//  bytes, then a word and a byte) and reads the display delay out of the registry, defaulting to 1 when the value is absent.
void __cdecl TEXT_Init()
{
    uint32_t dwValue;

    memset(&sgTextState, 0, sizeof(D2TextStateStrc));

    if (STORM_SRegLoadValue(szRegKeyDiabloII, szRegTextDisplayBeta, 0, &dwValue))
        sgTextState.nTextDisplayDelay = dwValue;
    else
        sgTextState.nTextDisplayDelay = 1;
}


//1.10f: D2Client.0x6FB1AB80
//  Sets the display delay and persists it. Reached only by tail jump, from ESCMENU_NpcSpeechOnChange and ESCMENU_NpcSpeechRefresh, once per
//  setting value.
void __fastcall TEXT_SetDisplayDelayAndSave(int32_t nDelay)
{
    sgTextState.nTextDisplayDelay = nDelay;
    STORM_SRegSaveValue(szRegKeyDiabloII, szRegTextDisplayBeta, 0, nDelay);
}


//1.10f: D2Client.0x6FB1CBD0
//  Stores a popup callback and its context. A plain setter with no validation: it does not look at what was there, so the last caller wins.
void __fastcall TEXT_SetCallback(void *pfnCallback, int32_t nContext)
{
    gpfnPopupCallback = (decltype(gpfnPopupCallback))pfnCallback;
    gpPopupCallbackArg = nContext;
}


//1.10f: D2Client.0x6FB1CBE0
//  Stops the speech sound a popup was playing: the instance is looked up by sound id and faded out with mode 4. hPopupSpeechSound is cleared
//  unconditionally, outside the `if`, so a popup that never played a sound writes zero over zero.
void __cdecl TEXT_StopPopupSpeechSound()
{
    if (gnPopupSpeechSoundId)
    {
        int32_t hInstance = SOUNDINST_GetHandleForSoundId(gnPopupSpeechSoundId);
        if (hInstance)
            SOUNDINST_UpdateForUnit(hInstance, 0, 0, 4u);
        gnPopupSpeechSoundId = SOUND_NONE;
    }
    sgTextState.hPopupSpeechSound = 0;
}


//1.10f: D2Client.0x6FB1D380
//  The unit a popup came from, resolved fresh on each call from a GUID rather than held as a pointer: an NPC talking (a MONSTER) or an object
//  the player activated. Two flag/GUID pairs, never one.
D2UnitStrc *__cdecl TEXT_GetPopupSourceUnit()
{
    if (sgTextState.bPopupIsNpcSpeech)
        return CUNIT_FindUnitByGuidAndType(sgTextState.dwPopupNpcGUID, UNIT_MONSTER);
    if (sgTextState.bPopupActive)
        return CUNIT_FindUnitByGuidAndType(sgTextState.dwPopupObjectGUID, UNIT_OBJECT);
    return nullptr;
}


//1.10f: D2Client.0x6FB1E060
//  The on-screen box of a multi-line message: the widest line plus 20, and 15 pixels per line plus 6. TEXT_DrawHoverBox reads both results
//  back. Takes one argument (ecx); edx is not read.
void __fastcall TEXT_MeasureMessageBox(D2ClientPopupStrc *pPopup)
{
    int32_t nMaxWidth = 0;
    D2ClientTextLineStrc *pLine = pPopup->pLines;
    const uint16_t wScrollRate = pPopup->wScrollRate;

    pPopup->nScrollPixels = 0;
    pPopup->nScrollStepPixels = wScrollRate;

    if (pLine)
    {
        do
        {
            const int32_t nTextWidth = D2WIN_10121_GetTextWidth(pLine->pszText);
            if (nMaxWidth <= nTextWidth)
                nMaxWidth = nTextWidth;
            pLine = pLine->pNext;
        } while (pLine);
    }

    //  A 16-bit imul on a field that is an int (the compiler's choice, not a second field).
    const int16_t nTextHeight = (int16_t)(15 * (uint16_t)pPopup->nLineCount);
    pPopup->wWidth = (uint16_t)(nMaxWidth + 20);
    pPopup->wHeight = (uint16_t)(nTextHeight + 6);
}


//1.10f: D2Client.0x6FB1E0B0
//  A unit going away takes its hover text with it (called from CUNIT_DestroyUnit): the parsed line list and its header are freed, and
//  CHAT_FreeHoverMsg is called unconditionally, outside the `if`. The original inlines TEXT_FreeParsedLines here.
//  Between the header free and CHAT_SetUsedInHoverMsg the field holds a dangling pointer; nothing runs in between.
void __fastcall TEXT_FreeUnitHoverMsg(D2HoverTextStrc *pHoverMsg)
{
    D2ClientTextLineStrc **ppParsedText = (D2ClientTextLineStrc **)D2COMMON_CHAT_GetUsedFromHoverMsg(pHoverMsg);

    if (ppParsedText)
    {
        D2ClientTextLineStrc *pLine = *ppParsedText;
        TEXT_FreeParsedLines(pLine);
        FOG_Free(ppParsedText, szTextCpp, 1674, 0);
        D2COMMON_CHAT_SetUsedInHoverMsg(pHoverMsg, 0);
    }

    D2COMMON_CHAT_FreeHoverMsg(nullptr, pHoverMsg);
}


//1.10f: D2Client.0x6FB1E6B0
//  `return pTextHeader;` -- the whole function.
D2TextHeaderStrc *__cdecl TEXT_GetMessageListHead()
{
    return gpMessageListHead;
}


//1.10f: D2Client.0x6FB1EE90
//  `return gPopupTextList != NULL;` -- the whole function. Its callers use it to keep unit labels and selection out of the way while a popup is up.
BOOL __cdecl TEXT_IsPopupActive()
{
    return gPopupTextList != nullptr;
}


//1.10f: D2Client.0x6FB1F1A0
//  Zeroes every chat node's deadline at +0x1E. That field is the `GetTickCount() + 10000` TEXT_AddQuickMessage sets, so zeroing it puts every
//  message in the past and the next draw clears the lot.
void __cdecl TEXT_ExpireAllChatMessages()
{
    for (D2ClientTextNodeStrc *pNode = sgTextState.GlobalMessageDisplayList.pHead; pNode; pNode = pNode->pNext)
    {
        pNode->dwExpireTick = 0;
    }
}


//1.10f: D2Client.0x6FB1EEA0
//  Frees two of the three message lists, the chat list and the message log, and deliberately not the quick-message list. Called from
//  UI_ResetUiSubsystems when the local player unit goes away; TEXT_Shutdown is the game-exit counterpart and frees more.
//  The original inlines TEXT_FreeMessageList for both lists.
void __cdecl TEXT_FreeChatAndMessageLogLists()
{
    TEXT_FreeMessageList(&sgTextState.GlobalMessageDisplayList);

    TEXT_FreeMessageList(&sgTextState.MessageLogDisplayList);
}


//1.10f: D2Client.0x6FB1E710
//  Clears the popup and everything around it: the popup's line list and record, the speech sound, the popup's message handlers, the
//  unacknowledged-popup reply, and the message list head. The original inlines TEXT_FreeParsedLines here.
//  gPopupTextList is read twice, once for ->pLines to start the walk and once to free the record, because the global is nulled only after the
//  record goes. No arguments, so __cdecl and __stdcall are the same code.
void __cdecl TEXT_ClearMessages()
{
    SELECT_SetClickRegion(nullptr);

    if (gPopupTextList)
    {
        D2ClientTextLineStrc *pLines = gPopupTextList->pLines;
        D2ClientPopupStrc *pPopup = gPopupTextList;
        TEXT_FreeParsedLines(pLines);
        FOG_Free(pPopup, szTextCpp, 1674, 0);
        gPopupTextList = nullptr;
    }

    if (gnPopupSpeechSoundId)
    {
        int32_t hInstance = SOUNDINST_GetHandleForSoundId(gnPopupSpeechSoundId);
        if (hInstance)
            SOUNDINST_UpdateForUnit(hInstance, 0, 0, 4u);
        gnPopupSpeechSoundId = SOUND_NONE;
    }
    sgTextState.hPopupSpeechSound = 0;

    if (gbPopupHandlersRegistered == 1)
    {
        gbPopupHandlersRegistered = 0;
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPopupMessageHandlers, 7u);
    }

    if (sgTextState.bPopupNeedsAck)
    {
        MSG_SendCmdUnit(49, -1, sgTextState.nPopupStringId);
        sgTextState.bPopupNeedsAck = 0;
    }
    sgTextState.bForceDrawPopupText = 0;

    if (gpMessageListHead)
    {
        D2COMMON_TEXT_FreeTextHeader(gpMessageListHead);
        gpMessageListHead = NULL;
    }
}


//1.10f: D2Client.0x6FB1A870
//  Frees everything text.cpp owns, on the way out of a game; called from UI_ShutdownUiSubsystems. Five list walks: the popup's line list twice,
//  the chat list, the quick-message list and the message log. The original inlines the walks of the popup's line list (TEXT_FreeParsedLines,
//  twice), the chat list and the quick-message list (TEXT_FreeMessageList); only the message-log walk is a call.
//  sgPopup.pParsedText is freed twice, once as the header the line walk ends on and once as the record that holds it: two allocations, and the
//  second free is the outer one. Reproduced as written.
void __cdecl TEXT_Shutdown()
{
    if (gpMessageListHead)
    {
        D2COMMON_TEXT_FreeTextHeader(gpMessageListHead);
        gpMessageListHead = NULL;
    }

    if (gPopupTextList)
    {
        D2ClientTextLineStrc *pLines = gPopupTextList->pLines;
        D2ClientPopupStrc *pPopup = gPopupTextList;
        TEXT_FreeParsedLines(pLines);
        FOG_Free(pPopup, szTextCpp, 1674, 0);
        gPopupTextList = nullptr;
    }

    if (sgPopup.pParsedText)
    {
        D2ClientTextLineStrc *pLine = sgPopup.pParsedText->pLines;
        D2ClientPopupStrc *pActiveTextPopup = sgPopup.pParsedText;
        TEXT_FreeParsedLines(pLine);
        FOG_Free(pActiveTextPopup, szTextCpp, 1674, 0);
        FOG_Free(sgPopup.pParsedText, szTextCpp, 1760, 0);
        sgPopup.pParsedText = nullptr;
    }

    if (sgTextState.pMessageLogDialog)
    {
        DIALOG_Close((D2ClientDialogStrc *)sgTextState.pMessageLogDialog);
        sgTextState.pMessageLogDialog = nullptr;
    }
    if (sgTextState.pMessageLogListBox)
    {
        //  The list box's own vector deleting destructor, through its vtable.
        (**(void(__thiscall ***)(void *, int32_t))sgTextState.pMessageLogListBox)(sgTextState.pMessageLogListBox, 1);
        sgTextState.pMessageLogListBox = nullptr;
    }

    //  Freed 2, 3, 1 -- the order in the instructions, and not address order.
    if (gIniScrollImage2)
    {
        ARCHIVE_CellFileFree(gIniScrollImage2);
        gIniScrollImage2 = nullptr;
    }
    if (gIniScrollImage3)
    {
        ARCHIVE_CellFileFree(gIniScrollImage3);
        gIniScrollImage3 = nullptr;
    }
    if (gIniScrollImage)
    {
        ARCHIVE_CellFileFree(gIniScrollImage);
        gIniScrollImage = nullptr;
    }

    SELECT_SetClickRegion(nullptr);

    TEXT_FreeMessageList(&sgTextState.GlobalMessageDisplayList);

    TEXT_FreeMessageList(&sgTextState.QuickMessageDisplayList);

    TEXT_FreeMessageList(&sgTextState.MessageLogDisplayList);
    sgTextState.hPopupSpeechSound = 0;
}


//  ---- the speech popup ----
//  Its four message handlers (registered through gPopupMessageHandlers, seven records over the four), its dismissal and its teardown.
//
//  All four handlers refuse to dismiss within 100 ms of sgTextState.dwPopupStartTick, which TEXT_PopupSetMessageHandlersActive stamps when it
//  registers them, so the keypress or click that opened a popup cannot also close it. Inside the floor the message is still consumed (bReturn,
//  lResult and the broken chain) and bDismissedByMouse is cleared; only the teardown is skipped. The four differ only in what they let through
//  before that:
//      TEXT_PopupOnSysKeyDown   ignores key 115 outright, and the key bound to HOTKEY_AUTOMAP when its code is below 0xE0
//      TEXT_PopupOnChar         the same minus the key-115 case
//      TEXT_PopupOnDismissKey   nothing at all (VK_ESCAPE and VK_SPACE, two records on one function)
//      TEXT_PopupOnMouseDown    two records; refuses when the click is on the NPC that is talking or on the popup's own owner unit
static const char szAssertPopupParsed[] = "sgPopup.pParsedText != NULL";
static_assert(sizeof(szAssertPopupParsed) == 27 + 1,
              "the assert text at 0x6FB8E45C, measured in the DLL");

//  Constants of the popup code.
static_assert(D2FONT_FONTINGAMECHAT == 13,
              "TEXT_DrawHoverBox draws its lines in font 13, `push 0Dh` into "
              "D2Win #10127");
static_assert(HOTKEY_AUTOMAP == 7,
              "the action TEXT_PopupOnSysKeyDown and TEXT_PopupOnChar will not dismiss "
              "on, asked of CMDTBL_ControlsIsActionBound as a literal 7");
static_assert(offsetof(D2TextStateStrc, dwPopupStartTick) == 0x7C,
              "the stamp TEXT_PopupSetMessageHandlersActive writes and all four "
              "handlers measure the 100 ms floor against -- 0x7C, measured off "
              "the packed layout against two controls, NOT 0x74, which is "
              "nHoverMsgCount two members earlier");


//1.10f: D2Client.0x6FB1B200
//  Registers or unregisters the popup's seven message records, guarded by gbPopupHandlersRegistered so the pairing stays balanced. The
//  register stamps dwPopupStartTick, which makes the 100 ms floor "since the popup opened" rather than "since the last message".
void __fastcall TEXT_PopupSetMessageHandlersActive(int32_t bActive)
{
    if (bActive)
    {
        if (bActive == 1 && !gbPopupHandlersRegistered)
        {
            gbPopupHandlersRegistered = 1;
            sgTextState.dwPopupStartTick = GetTickCount();
            WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPopupMessageHandlers, 7u);
        }
    }
    else if (gbPopupHandlersRegistered == 1)
    {
        gbPopupHandlersRegistered = 0;
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPopupMessageHandlers, 7u);
    }
}


//1.10f: D2Client.0x6FB1B1D0
//  Records which unit the popup belongs to. A null argument clears the flag instead of storing. Its only reader is TEXT_PopupOnMouseDown,
//  which uses it to refuse dismissing a popup when the player clicks the unit the popup is about.
void __fastcall TEXT_PopupSetOwnerUnit(D2UnitStrc *pUnit)
{
    if (pUnit)
    {
        const uint32_t dwUnitId = pUnit->dwUnitId;
        sgTextState.bPopupHasOwnerUnit = 1;
        sgTextState.dwPopupOwnerUnitGUID = dwUnitId;
    }
    else
    {
        sgTextState.bPopupHasOwnerUnit = 0;
    }
}


//1.10f: D2Client.0x6FB1B1F0
//  Counterpart of TEXT_PopupSetOwnerUnit: clears the popup's owner unit.
void __cdecl TEXT_PopupClearOwnerUnit()
{
    sgTextState.bPopupHasOwnerUnit = 0;
}


//1.10f: D2Client.0x6FB1E810
//  The plain close: click region, the parsed line list and its header, the voice line, the handlers, and the acknowledgement (client command 49
//  carrying nPopupStringId, sent only when bPopupNeedsAck is set). TEXT_OnPopupDismissed does the same teardown and then decides what happens
//  next. The original inlines TEXT_FreeParsedLines here.
//  void: eax at the exit is either the stale handler flag or MSG_SendCmdUnit's answer, and the one caller does not read it.
void __cdecl TEXT_ClosePopup()
{
    SELECT_SetClickRegion(nullptr);

    if (gPopupTextList)
    {
        D2ClientTextLineStrc *pLines = gPopupTextList->pLines;
        D2ClientPopupStrc *pPopup = gPopupTextList;
        TEXT_FreeParsedLines(pLines);
        FOG_Free(pPopup, szTextCpp, 1674, 0);
        gPopupTextList = nullptr;
    }

    if (gnPopupSpeechSoundId)
    {
        int32_t hInstance = SOUNDINST_GetHandleForSoundId(gnPopupSpeechSoundId);
        if (hInstance)
            SOUNDINST_UpdateForUnit(hInstance, 0, 0, 4u);
        gnPopupSpeechSoundId = SOUND_NONE;
    }

    sgTextState.hPopupSpeechSound = 0;

    if (gbPopupHandlersRegistered == 1)
    {
        gbPopupHandlersRegistered = 0;
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPopupMessageHandlers, 7u);
    }

    if (sgTextState.bPopupNeedsAck)
    {
        MSG_SendCmdUnit(49, -1, sgTextState.nPopupStringId);
        sgTextState.bPopupNeedsAck = 0;
    }
}


//1.10f: D2Client.0x6FB1AE00
//  What happens when the player dismisses a speech popup. The return is a contract: 1 when something was torn down, 0 when there was nothing
//  to dismiss; all four message handlers test it on the next instruction.
//  The popup teardown (click region, popup list, speech sound, command 49) is written twice, in the callback arm and in the else arm; the arms
//  differ only in that the callback arm calls TEXT_PopupSetMessageHandlersActive(0) where the else arm writes the same unregister inline.
//  TEXT_ClosePopup and TEXT_ClearMessages carry the same teardown.
//  The callback arm is the hook a scripted conversation hangs on. When gpfnPopupCallback(gpPopupCallbackArg, 1, bDismissedByMouse) answers
//  non-zero it has taken over: the tail (clearing bPopupIsNpcSpeech and the final unregister) is skipped, as a flag here rather than a jump, and
//  the conversation is not ended through NPCMENU_EndInteraction. That is how a scripted sequence chains one line to the next.
//  The original inlines TEXT_FreeParsedLines and the popup header free here.
int32_t __cdecl TEXT_OnPopupDismissed()
{
    sgTextState.bPopupDismissed = 1;

    if (sgTextState.bPopupIsNpcSpeech || sgTextState.bPopupActive == 1)
    {
        bool bCallbackTookOver = false;

        if (gpfnPopupCallback)
        {
            SELECT_SetClickRegion(nullptr);
            if (gPopupTextList)
            {
                D2ClientPopupStrc *pTextList = gPopupTextList;
                TEXT_FreeParsedLines(gPopupTextList->pLines);
                FOG_Free(pTextList, szTextCpp, 1674, 0);
                gPopupTextList = nullptr;
            }
            if (gnPopupSpeechSoundId)
            {
                int32_t hSpeechSound = SOUNDINST_GetHandleForSoundId(gnPopupSpeechSoundId);
                if (hSpeechSound)
                    SOUNDINST_UpdateForUnit(hSpeechSound, 0, 0, 4u);
                gnPopupSpeechSoundId = SOUND_NONE;
            }
            sgTextState.hPopupSpeechSound = 0;

            TEXT_PopupSetMessageHandlersActive(0);

            if (sgTextState.bPopupNeedsAck)
            {
                MSG_SendCmdUnit(49, -1, sgTextState.nPopupStringId);
                sgTextState.bPopupNeedsAck = 0;
            }
            UI_ArmPanelToggleDelay();

            if ((gpfnPopupCallback)(gpPopupCallbackArg, 1, sgTextState.bDismissedByMouse))
            {
                bCallbackTookOver = true;
            }
            else if (sgTextState.bPopupIsNpcSpeech)
            {
                NPCMENU_EndInteraction(sgTextState.dwPopupNpcGUID);
            }
        }
        else
        {
            if (sgTextState.bPopupIsNpcSpeech)
            {
                NPCMENU_EndInteraction(sgTextState.dwPopupNpcGUID);
            }
            else
            {
                SELECT_SetClickRegion(nullptr);
                if (gPopupTextList)
                {
                    D2ClientPopupStrc *pTextList2 = gPopupTextList;
                    TEXT_FreeParsedLines(gPopupTextList->pLines);
                    FOG_Free(pTextList2, szTextCpp, 1674, 0);
                    gPopupTextList = nullptr;
                }
                if (gnPopupSpeechSoundId)
                {
                    int32_t hSpeechSound2 = SOUNDINST_GetHandleForSoundId(gnPopupSpeechSoundId);
                    if (hSpeechSound2)
                        SOUNDINST_UpdateForUnit(hSpeechSound2, 0, 0, 4u);
                    gnPopupSpeechSoundId = SOUND_NONE;
                }
                sgTextState.hPopupSpeechSound = 0;

                //  The else arm writes the unregister out where the callback arm calls TEXT_PopupSetMessageHandlersActive(0) for it.
                if (gbPopupHandlersRegistered == 1)
                {
                    gbPopupHandlersRegistered = 0;
                    WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPopupMessageHandlers, 7u);
                }
                if (sgTextState.bPopupNeedsAck)
                {
                    MSG_SendCmdUnit(49, -1, sgTextState.nPopupStringId);
                    sgTextState.bPopupNeedsAck = 0;
                }
            }
            UI_ArmPanelToggleDelay();
        }

        if (!bCallbackTookOver)
        {
            sgTextState.bPopupIsNpcSpeech = 0;
            if (gbPopupHandlersRegistered == 1)
            {
                gbPopupHandlersRegistered = 0;
                WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPopupMessageHandlers, 7u);
            }
        }

        sgTextState.bDismissedByMouse = 0;
        return 1;
    }

    if (sgTextState.bKeepNpcInteraction != 1)
    {
        sgTextState.bDismissedByMouse = 0;
        return 0;
    }

    if (sgTextState.bDisplayingStaticInfo == 1)
    {
        sgTextState.bDismissedByMouse = 0;
        return 0;
    }

    if (!sgPopup.pParsedText)
    {
        FOG_DisplayAssert(szAssertPopupParsed, szTextCpp, 2371);
        exit(-1);
    }

    {
        D2ClientTextLineStrc *pLines = sgPopup.pParsedText->pLines;
        D2ClientPopupStrc *pActiveTextPopup = sgPopup.pParsedText;
        TEXT_FreeParsedLines(pLines);
        FOG_Free(pActiveTextPopup, szTextCpp, 1674, 0);
        FOG_Free(sgPopup.pParsedText, szTextCpp, 2373, 0);
        sgPopup.pParsedText = nullptr;
    }

    sgTextState.bKeepNpcInteraction = 0;
    sgTextState.bDisplayingStaticInfo = 0;
    sgTextState.bDismissedByMouse = 0;
    return 1;
}


//1.10f: D2Client.0x6FB1F310
//  Five bytes: `jmp TEXT_OnPopupDismissed`. Written as a forwarding call; C++ has no way to spell a bare tail jump.
void __cdecl j_text_1AE00()
{
    TEXT_OnPopupDismissed();
}


//1.10f: D2Client.0x6FB1AC60
//  WM_SYSKEYDOWN. Ignores key 115 outright and the key bound to the automap, then the 100 ms floor. Registered in gPopupMessageHandlers.
void __stdcall TEXT_PopupOnSysKeyDown(SMSGHANDLER_PARAMS *pMsg)
{
    if (pMsg->wParam == 115)
        return;

    if (CMDTBL_ControlsIsActionBound(HOTKEY_AUTOMAP, 1))
    {
        const uint16_t wBoundKey = (uint16_t)CMDTBL_ControlsFindBoundKey(HOTKEY_AUTOMAP, 1);
        //  The 0xFFFF test is implied by the < 0xE0 test and is written because the original makes both (`cmp ax,0FFFFh / je`, then
        //  `cmp ax,0DFh / ja` to the same target).
        if (wBoundKey != 0xFFFF && wBoundKey < 0xE0u && pMsg->wParam == wBoundKey)
            return;
    }

    sgTextState.bForceDrawPopupText = 0;
    if (GetTickCount() - sgTextState.dwPopupStartTick >= 0x64)
    {
        if (TEXT_OnPopupDismissed())
        {
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
        }
    }
    else
    {
        //  Inside the floor the message is still consumed.
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        sgTextState.bDismissedByMouse = 0;
    }
}


//1.10f: D2Client.0x6FB1AD00
//  WM_CHAR. TEXT_PopupOnSysKeyDown's body without the key-115 case.
void __stdcall TEXT_PopupOnChar(SMSGHANDLER_PARAMS *pMsg)
{
    if (CMDTBL_ControlsIsActionBound(HOTKEY_AUTOMAP, 1))
    {
        const uint16_t wBoundKey = (uint16_t)CMDTBL_ControlsFindBoundKey(HOTKEY_AUTOMAP, 1);
        //  The same two tests as in TEXT_PopupOnSysKeyDown.
        if (wBoundKey != 0xFFFF && wBoundKey < 0xE0u && pMsg->wParam == wBoundKey)
            return;
    }

    sgTextState.bForceDrawPopupText = 0;
    if (GetTickCount() - sgTextState.dwPopupStartTick >= 0x64)
    {
        if (TEXT_OnPopupDismissed())
        {
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
        }
    }
    else
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        sgTextState.bDismissedByMouse = 0;
    }
}


//1.10f: D2Client.0x6FB1ADA0
//  VK_ESCAPE and VK_SPACE: two records on one function. The plainest of the four: no key filtering, just the floor.
void __stdcall TEXT_PopupOnDismissKey(SMSGHANDLER_PARAMS *pMsg)
{
    sgTextState.bForceDrawPopupText = 0;
    if (GetTickCount() - sgTextState.dwPopupStartTick >= 0x64)
    {
        if (TEXT_OnPopupDismissed())
        {
            //  lResult is stored before bReturn here, the other way round from its three siblings; nothing reads them in between.
            pMsg->lResult = 0;
            pMsg->bReturn = 1;
            STORM_SMsgBreakHandlerChain(pMsg);
        }
    }
    else
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        sgTextState.bDismissedByMouse = 0;
    }
}


//1.10f: D2Client.0x6FB1B0F0
//  WM_LBUTTONDOWN and WM_RBUTTONDOWN: two records.
//  It refuses to dismiss in two cases: the popup is NPC speech and the unit under the cursor is the speaker, or the popup has an owner unit and
//  the click is on it. Otherwise the usual floor.
void __stdcall TEXT_PopupOnMouseDown(SMSGHANDLER_PARAMS *pMsg)
{
    sgTextState.bDismissedByMouse = 1;

    if (SELECT_GetSelectedUnit())
    {
        if (sgTextState.bPopupIsNpcSpeech)
        {
            D2UnitStrc *pSpeaker = CUNIT_FindUnitByGuidAndType(sgTextState.dwPopupNpcGUID, UNIT_MONSTER);
            //  The selection is asked for a second time, as the bytes do.
            if (pSpeaker == SELECT_GetSelectedUnit())
                return;
        }

        sgTextState.bForceDrawPopupText = 0;
        if (GetTickCount() - sgTextState.dwPopupStartTick < 0x64)
        {
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
            sgTextState.bDismissedByMouse = 0;
            return;
        }
    }
    else
    {
        if (sgTextState.bPopupHasOwnerUnit)
        {
            D2UnitStrc *pOwner = CUNIT_FindUnitByGuidAndType(sgTextState.dwPopupOwnerUnitGUID, UNIT_MONSTER);
            //  The owner arm compares against the speaker, looked up again, so it consumes the click only when the two are the same unit.
            if (pOwner && pOwner == CUNIT_FindUnitByGuidAndType(sgTextState.dwPopupNpcGUID, UNIT_MONSTER))
            {
                pMsg->bReturn = 1;
                pMsg->lResult = 0;
                STORM_SMsgBreakHandlerChain(pMsg);
            }
            return;
        }

        sgTextState.bForceDrawPopupText = 0;
        if (GetTickCount() - sgTextState.dwPopupStartTick < 0x64)
        {
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
            sgTextState.bDismissedByMouse = 0;
            return;
        }
    }

    if (TEXT_OnPopupDismissed())
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}


//1.10f: D2Client.0x6FB1ABA0
//  Draws one hover message: a filled box with up to ten lines in font 13. The line count is clamped to 10 rather than asserted, so a longer
//  message is silently truncated, and the box is skipped entirely when it would run off the bottom.
//  The row step is written `3 * n + 12 * n`, the 15 pixels per line that TEXT_MeasureMessageBox multiplies the line count by.
void __fastcall TEXT_DrawHoverBox(D2ClientPopupStrc *pPopup, int32_t nX, int32_t nY)
{
    int32_t nLineCount = pPopup->nLineCount;
    if ((uint32_t)nLineCount > 0xA)
        nLineCount = 10;

    D2ClientTextLineStrc *pLine = pPopup->pLines;
    if (!nLineCount || !pLine || nY < 0)
        return;

    const int32_t nBoxHeight = pPopup->wHeight;
    if ((uint16_t)nBoxHeight + nY >= gnScreenHeight)
        return;

    GFX_DrawFilledRect(nX, nY + 4, pPopup->wWidth, nBoxHeight - 5, 0, 1);

    int32_t iLine = 0;
    const enum Font eSavedFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);
    do
    {
        if (!pLine)
            break;
        const int32_t nRow = pPopup->nLineCount + iLine - nLineCount;
        D2WIN_10117_DrawText(pLine->pszText, nX + 10, 3 * nRow + nY + 12 * nRow + 18, 0, 0);
        pLine = pLine->pNext;
        ++iLine;
    } while (iLine < nLineCount);
    D2WIN_10127_SetFont(eSavedFont);
}


//  The popup's parser, the two functions that put a popup up (TEXT_ShowSpeechPopup, TEXT_ShowQuestScrollingText) and the per-frame tick.
static const char szAssertStaticInfo[] = "sgTextState.bDisplayingStaticInfo";
static const char szWarnNoScrollRate[] = "line 0 of string does not contain scroll rate";
static const char szWarnLineTooLong[] = "line %d of string is too long";
static_assert(sizeof(szAssertStaticInfo) == 33 + 1 && sizeof(szWarnNoScrollRate) == 45 + 1
                  && sizeof(szWarnLineTooLong) == 29 + 1,
              "the three strings at 0x6FB8E478, 0x6FB8E3B4 and 0x6FB8E394, measured "
              "in the DLL");

//  Font 8 in the two openers is D2FONT_FONTFORMAL11 (the value 8); D2FONT_FONT8 is 0.
static_assert(D2FONT_FONTFORMAL11 == 8,
              "both openers pass the VALUE 8 to D2Win #10127 -- which is "
              "FONTFORMAL11. D2FONT_FONT8 is 0, and the name reads like the "
              "number");
static_assert(offsetof(D2ClientPopupStrc, field_16) == 0x16
                  && offsetof(D2ClientPopupStrc, field_18) == 0x18,
              "the dword these three functions read and write at +0x16 spans "
              "BOTH of these members");
static_assert(offsetof(D2TextStateStrc, nStaticInfoTimeout) == 0x5E,
              "the counter TEXT_TickStaticInfoPanel increments first thing "
              "every frame");


//  DIFFERENCE: the original reads and writes one dword over pPopup->field_16 and field_18 (two uint16_t members); a `*(uint32_t *)&` across
//  two members is undefined in C++, so ours reads and writes the two halves, the same bytes on this little-endian target. The read is
//  written whole because the original compares the full dword. These two helpers are not functions of the original: each is one expression,
//  named so the call sites read as the one access they are.
//  The state values are 0, 1, 2 and 3: 0 is promoted to 1 on the first tick, 2 means dismissed, 3 skips the draw.
static uint32_t TEXT_PopupState(const D2ClientPopupStrc *pPopup)
{
    return (uint32_t)pPopup->field_16 | ((uint32_t)pPopup->field_18 << 16);
}

static void TEXT_SetPopupState(D2ClientPopupStrc *pPopup, uint32_t nState)
{
    pPopup->field_16 = (uint16_t)nState;
    pPopup->field_18 = (uint16_t)(nState >> 16);
}


//1.10f: D2Client.0x6FB1CF10
//  Splits a tbl string into the popup's line list and reports its scroll rate. Three arguments (`retn 4`): the text in ecx, the line count
//  in edx and the scroll rate on the stack.
//  Line 0 is not text: it is the scroll rate, read as ASCII through Unicode::toUtf and atoi, defaulting to 8 when any character is above 0x7F.
//  Every later line is capped at 100 characters (the copy loop breaks out, so a longer line is truncated) and warned about past 308 pixels,
//  so 308 is the popup's usable width; the warning only formats a diagnostic into an 80-byte stack buffer.
//  The stack buffers are wszText (100 Unicode), String (100 bytes) and szWarnTooLong1 and szWarnTooLong2 (80 bytes each).
//  The line node is { pszText, pNext }, 8 bytes; its text takes 2 * len + 2 bytes.
struct Unicode **__fastcall TEXT_ParseLinesAndScrollRate(Unicode *pwszText, int32_t *pnLineCount, int16_t *pnScrollRate)
{
    struct Unicode **pNoLines = nullptr;
    struct Unicode **pHead = nullptr;
    struct Unicode **pTail = nullptr;
    int32_t bFirstLine = 1;

    *pnLineCount = 0;
    if (!pwszText->ch)
        return pNoLines;

    Unicode *pCursor;
    do
    {
        pCursor = pwszText;

        //  The declaration emits the 99-element constructor loop the original runs from &wszText[1].
        //  DIFFERENCE: the original's extra leading memset of wszText is not reproduced; the constructors and the second memset zero the same
        //  200 bytes.
        Unicode wszText[100];
        char String[100];
        CHAR szWarnTooLong1[80];
        CHAR szWarnTooLong2[80];

        int32_t nLineLen = 0;
        memset(wszText, 0, sizeof(wszText));
        while (pCursor->ch != 10)
        {
            wszText[nLineLen++] = (Unicode)pCursor->ch;
            ++pCursor;
            if (nLineLen >= 100)
                goto LINE_READY;
        }
        ++pCursor;

    LINE_READY:
        {
            struct Unicode **pNode = (struct Unicode **)FOG_Alloc(8, szTextCpp, 1460, 0);

            if (bFirstLine == 1)
            {
                //  Line 0 carries the scroll rate. Any character above 0x7F means it is not a number, and the default is 8.
                const int32_t nFirstLineLen = Unicode::strlen(wszText);
                int32_t bAllAscii = 1;
                for (int32_t iChar = 0; iChar < nFirstLineLen; ++iChar)
                {
                    if (wszText[iChar].ch >= 0x80u)
                        bAllAscii = 0;
                }
                if (bAllAscii)
                {
                    Unicode::toUtf(String, wszText, 100);
                    *pnScrollRate = (int16_t)atoi(String);
                }
                else
                {
                    wsprintfA(szWarnTooLong1, szWarnNoScrollRate);
                    *pnScrollRate = 8;
                }
                bFirstLine = 0;

                //  ...and then the first real line is read into the same buffer, in the same pass.
                int32_t nLineLen2 = 0;
                memset(wszText, 0, sizeof(wszText));
                while (pCursor->ch != 10)
                {
                    wszText[nLineLen2++] = (Unicode)pCursor->ch;
                    ++pCursor;
                    if (nLineLen2 >= 100)
                        goto LINE_COPY;
                }
                ++pCursor;
            }

        LINE_COPY:
            {
                const int32_t nTextLen = Unicode::strlen(wszText);
                if (D2WIN_10121_GetTextWidth(wszText) >= 308)
                    wsprintfA(szWarnTooLong2, szWarnLineTooLong, *pnLineCount);

                struct Unicode *pDest = (struct Unicode *)FOG_Alloc(2 * nTextLen + 2, szTextCpp, 1511, 0);
                *pNode = pDest;
                Unicode::strcpy(pDest, wszText);
                pNode[1] = nullptr;
                if (pTail)
                    pTail[1] = (struct Unicode *)pNode;
                else
                    pHead = pNode;
                pTail = pNode;
                ++*pnLineCount;
                pwszText = pCursor;
            }
        }
    } while (pCursor->ch);

    return pHead;
}


//1.10f: D2Client.0x6FB1CC20
//  Puts an NPC's words on screen and starts the voice line. Called with pNpc = NULL it tears the static-info text down instead.
//  The whole body runs under `if (!gPopupTextList)`, so a second popup while one is up is dropped; the click region and the sound are reset
//  first regardless.
//  The popup is a 44-byte record from FOG_Alloc. It is given 325 x 112 and then re-measured from the parsed lines (the widest line plus 20,
//  and 15 pixels per line plus 6), so 325 x 112 is a placeholder. The click region around it is not re-measured: gnPopupY to gnPopupY + 182
//  clamped to gnScreenHeight - 53, and gnPopupX - 10 to gnPopupX + 335. The third parameter, nUnused, is never read.
//  The original inlines TEXT_FreeParsedLines here.
void __fastcall TEXT_ShowSpeechPopup(D2UnitStrc *pNpc, int16_t nTblStringId, int32_t nUnused)
{
    D2C_StringIndices wTblStringId = (D2C_StringIndices)nTblStringId;

    sgTextState.bPopupIsQuestText = 0;
    gnPopupY = 12;
    gnPopupX = (gnScreenWidth - 325) / 2;
    sgTextState.bPopupNeedsAck = 0;
    GAME_ResetClickState();

    if (gnPopupSpeechSoundId)
    {
        int32_t hInstance = SOUNDINST_GetHandleForSoundId(gnPopupSpeechSoundId);
        if (hInstance)
            SOUNDINST_UpdateForUnit(hInstance, 0, 0, 4u);
        gnPopupSpeechSoundId = SOUND_NONE;
    }
    sgTextState.hPopupSpeechSound = 0;

    if (pNpc)
    {
        if (sgTextState.bPopupIsNpcSpeech)
        {
            D2UnitStrc *pPrev = CUNIT_FindUnitByGuidAndType(sgTextState.dwPopupNpcGUID, UNIT_MONSTER);
            //  A different NPC was talking: end that conversation first.
            if (pPrev && pPrev != pNpc)
            {
                MSG_SendCmdUnit(48, 1, sgTextState.dwPopupNpcGUID);
                NPCMENU_NpcResyncVendorInventory(sgTextState.dwPopupNpcGUID);
            }
        }
        sgTextState.bPopupIsNpcSpeech = 1;
        sgTextState.dwPopupNpcGUID = pNpc->dwUnitId;
    }
    else
    {
        if (sgTextState.bKeepNpcInteraction)
        {
            if (sgPopup.pParsedText)
            {
                D2ClientTextLineStrc *pLines = sgPopup.pParsedText->pLines;
                D2ClientPopupStrc *pActiveTextPopup = sgPopup.pParsedText;
                TEXT_FreeParsedLines(pLines);
                FOG_Free(pActiveTextPopup, szTextCpp, 1674, 0);
                sgPopup.pParsedText = nullptr;
            }
            sgTextState.bKeepNpcInteraction = 0;
        }
        sgTextState.bPopupIsNpcSpeech = 0;
        sgTextState.dwPopupNpcGUID = 0;
    }

    //  Four dwords (left, top, right, bottom): SELECT_SetClickRegion reads all four.
    int32_t pRect[4];
    pRect[1] = gnPopupY;
    pRect[3] = gnPopupY + 182;
    if (pRect[3] >= gnScreenHeight - 53)
        pRect[3] = gnScreenHeight - 53;
    pRect[0] = gnPopupX - 10;
    pRect[2] = gnPopupX + 335;
    SELECT_SetClickRegion(pRect);
    SELECT_ClearSelectedUnit();
    sgTextState.nPopupStringId = (uint16_t)wTblStringId;

    //  A popup already up is not replaced, but the click region and the sound above were reset regardless.
    if (gPopupTextList)
        return;

    sgTextState.bPopupDismissed = 0;
    const enum Font eFontOuter = D2WIN_10127_SetFont(D2FONT_FONTFORMAL11);
    Unicode *pwszText = (Unicode *)D2LANG_GetStringFromTblIndex(wTblStringId);
    D2ClientPopupStrc *pPopup = (D2ClientPopupStrc *)FOG_Alloc(44, szTextCpp, 1625, 0);
    const enum Font eFontInner = D2WIN_10127_SetFont(D2FONT_FONTFORMAL11);

    pPopup->pLines = (struct D2ClientTextLineStrc *)TEXT_ParseLinesAndScrollRate(pwszText, &pPopup->nLineCount, (int16_t *)&pPopup->wScrollRate);
    pPopup->wWidth = 325;
    pPopup->wHeight = 112;
    pPopup->dwLastTick = 0;
    pPopup->dwStartTick = 0;
    pPopup->nStepTicks = 0;
    pPopup->nAccumTicks = 0;
    pPopup->wSameTickDraws = 0;
    D2WIN_10127_SetFont(eFontInner);

    int32_t nMaxTextWidth = 0;
    const uint16_t wScrollRate = pPopup->wScrollRate;
    D2ClientTextLineStrc *pLine = pPopup->pLines;
    pPopup->nScrollPixels = 0;
    pPopup->nScrollStepPixels = wScrollRate;
    if (pLine)
    {
        do
        {
            const int32_t nTextWidth = D2WIN_10121_GetTextWidth(pLine->pszText);
            if (nMaxTextWidth <= nTextWidth)
                nMaxTextWidth = nTextWidth;
            pLine = pLine->pNext;
        } while (pLine);
    }
    const uint16_t nMeasuredHeight = (uint16_t)(15 * (uint16_t)pPopup->nLineCount + 6);
    pPopup->wWidth = (uint16_t)(nMaxTextWidth + 20);
    pPopup->wHeight = nMeasuredHeight;
    gPopupTextList = pPopup;
    D2WIN_10127_SetFont(eFontOuter);

    SOUNDINST_StopSpeechSounds();
    sgTextState.hPopupSpeechSound = 0;
    const D2C_Sounds nSoundId = OBJECT_FindSpeechSoundByTblIndex(wTblStringId);
    if (nSoundId)
    {
        UNITSND_StopMonsterSkillSounds(pNpc);
        D2UnitStrc *pPlayer = CUNIT_GetPlayer();
        sgTextState.hPopupSpeechSound = SOUNDINST_PlaySound(nSoundId, pPlayer, 5, 0, 0);
        gnPopupSpeechSoundId = nSoundId;
    }

    //  The original clears one dword over field_16 and field_18.
    gPopupTextList->field_16 = 0;
    gPopupTextList->field_18 = 0;
    sgTextState.bPopupActive = 1;

    if (!gbPopupHandlersRegistered)
    {
        gbPopupHandlersRegistered = 1;
        sgTextState.dwPopupStartTick = GetTickCount();
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPopupMessageHandlers, 7u);
    }
}


//1.10f: D2Client.0x6FB1D110
//  The quest log's scrolling description, and TEXT_ShowSpeechPopup's twin: the same 44-byte record, the same parser, the same re-measure and
//  the same click region, and likewise the whole body under `if (!gPopupTextList)`.
//  What differs is where it goes. With bIsQuestText it anchors to the quest panel at (gnBorderXDifference, 261 - gnBorderYDifference) and
//  TEXT_DrawPopup skips the frame, because the quest log draws its own; without it, the centred speech position. nTblStringId is the first
//  parameter.
//  D2ExpRes patches one site in this function, QuestScrollingText1 (RVA 0x7D14E), redoing the position from D2ExpRes.ini for 1024x768 and
//  1280x768. The stock numbers are written here and the detour lands on the position block below.
void __fastcall TEXT_ShowQuestScrollingText(int32_t nTblStringId, int16_t bIsQuestText, int32_t bNeedsAck)
{
    const D2C_StringIndices wTblStringId = (D2C_StringIndices)(int16_t)nTblStringId;
    const int16_t nTblStringIdChecked = (int16_t)nTblStringId;

    //  D2ExpRes: QuestScrollingText1 detours here to redo the position.
    if (bIsQuestText)
    {
        sgTextState.bPopupIsQuestText = 1;
        gnPopupX = gnBorderXDifference;
        gnPopupY = 261 - gnBorderYDifference;
    }
    else
    {
        sgTextState.bPopupIsQuestText = 0;
        gnPopupY = 12;
        gnPopupX = (gnScreenWidth - 325) / 2;
    }

    GAME_ResetClickState();
    if (gnPopupSpeechSoundId)
    {
        int32_t hInstance = SOUNDINST_GetHandleForSoundId(gnPopupSpeechSoundId);
        if (hInstance)
            SOUNDINST_UpdateForUnit(hInstance, 0, 0, 4u);
        gnPopupSpeechSoundId = SOUND_NONE;
    }

    //  Four dwords, as in TEXT_ShowSpeechPopup.
    int32_t pRect[4];
    pRect[1] = gnPopupY;
    sgTextState.hPopupSpeechSound = 0;
    sgTextState.bPopupIsNpcSpeech = 0;
    pRect[3] = gnPopupY + 182;
    if (pRect[3] >= gnScreenHeight - 53)
        pRect[3] = gnScreenHeight - 53;
    pRect[0] = gnPopupX - 10;
    pRect[2] = gnPopupX + 335;
    SELECT_SetClickRegion(pRect);
    SELECT_ClearSelectedUnit();
    sgTextState.nPopupStringId = (uint16_t)wTblStringId;

    if (gPopupTextList)
        return;

    sgTextState.bPopupDismissed = 0;
    //  The two font saves are nested the other way round from the speech popup's: inner set first, restored last. Both restore what they saved.
    const enum Font eFontInner = D2WIN_10127_SetFont(D2FONT_FONTFORMAL11);
    Unicode *pwszText = (Unicode *)D2LANG_GetStringFromTblIndex(wTblStringId);
    D2ClientPopupStrc *pPopup = (D2ClientPopupStrc *)FOG_Alloc(44, szTextCpp, 1625, 0);
    const enum Font eFontOuter = D2WIN_10127_SetFont(D2FONT_FONTFORMAL11);

    pPopup->pLines = (struct D2ClientTextLineStrc *)TEXT_ParseLinesAndScrollRate(pwszText, &pPopup->nLineCount, (int16_t *)&pPopup->wScrollRate);
    pPopup->wWidth = 325;
    pPopup->wHeight = 112;
    pPopup->dwLastTick = 0;
    pPopup->dwStartTick = 0;
    pPopup->nStepTicks = 0;
    pPopup->nAccumTicks = 0;
    pPopup->wSameTickDraws = 0;
    D2WIN_10127_SetFont(eFontOuter);

    int32_t nMaxTextWidth = 0;
    const uint16_t wScrollRate = pPopup->wScrollRate;
    D2ClientTextLineStrc *pLines = pPopup->pLines;
    pPopup->nScrollPixels = 0;
    pPopup->nScrollStepPixels = wScrollRate;
    if (pLines)
    {
        do
        {
            const int32_t nTextWidth = D2WIN_10121_GetTextWidth(pLines->pszText);
            if (nMaxTextWidth <= nTextWidth)
                nMaxTextWidth = nTextWidth;
            pLines = pLines->pNext;
        } while (pLines);
    }
    const uint16_t nMeasuredHeight = (uint16_t)(15 * (uint16_t)pPopup->nLineCount + 6);
    pPopup->wWidth = (uint16_t)(nMaxTextWidth + 20);
    pPopup->wHeight = nMeasuredHeight;
    gPopupTextList = pPopup;
    D2WIN_10127_SetFont(eFontInner);

    gPopupTextList->field_16 = 0;
    gPopupTextList->field_18 = 0;
    sgTextState.bPopupActive = 1;
    sgTextState.bForceDrawPopupText = 1;

    if (!gbPopupHandlersRegistered)
    {
        gbPopupHandlersRegistered = 1;
        sgTextState.dwPopupStartTick = GetTickCount();
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPopupMessageHandlers, 7u);
    }

    const D2C_Sounds nSoundId = OBJECT_FindSpeechSoundByTblIndex(wTblStringId);
    if (nSoundId)
    {
        D2UnitStrc *pPlayer = CUNIT_GetPlayer();
        sgTextState.hPopupSpeechSound = SOUNDINST_PlaySound(nSoundId, pPlayer, 5, 0, 0);
        gnPopupSpeechSoundId = nSoundId;
    }

    //  Only these five string ids can owe an acknowledgement.
    if (nTblStringIdChecked == 127 || nTblStringIdChecked == 20002 || nTblStringIdChecked == 20169 || nTblStringIdChecked == 20131 || nTblStringIdChecked == 396)
    {
        if (bNeedsAck)
            sgTextState.bPopupNeedsAck = 1;
    }
}


//1.10f: D2Client.0x6FB1BA00
//  The per-frame tick for the static-info / NPC text panel, called from UIPANELS_DrawInGamePanels. It resolves the panel rectangle from the
//  view, draws every unit's hover text, drives whichever popup is up, and, when neither half still wants the panel, takes it down.
//  nStillShowing is the whole shutdown: the popup arm assigns into it and the static-info arm adds to it, and the tail clears bPopupActive
//  and unregisters the seven handlers only when the sum is zero.
//  The original inlines the popup header free here.
void __fastcall TEXT_TickStaticInfoPanel(D2ClientViewStrc *pView)
{
    int32_t pnLeft, pnTop, pnRight, pnBottom;
    int32_t xLeft, yTop;

    ++sgTextState.nStaticInfoTimeout;
    int32_t nStillShowing = 0;

    VIEW_GetRect(pView, &pnLeft, &pnTop, &pnRight, &pnBottom);
    VIEW_GetOrigin(pView, &xLeft, &yTop);
    SetRect((LPRECT)&sgTextState.rcTextInfoPanel, xLeft, yTop, xLeft + pnRight - pnLeft, pnBottom + yTop - pnTop);

    if (sgTextState.bPopupActive == 1)
    {
        sgTextState.nHoverMsgCount = 1;
        gHoverMsgRect[0].left = gnPopupX;
        gHoverMsgRect[0].right = gnPopupX + 325;
        gHoverMsgRect[0].top = gnPopupY;
        gHoverMsgRect[0].bottom = gnPopupY + 112;
    }
    else
    {
        sgTextState.nHoverMsgCount = 0;
    }

    CUNIT_ForEachPlayerUnit(TEXT_DrawUnitHoverTextHigh);
    CUNIT_ForEachMonsterUnit(TEXT_DrawUnitHoverTextLow);
    CUNIT_ForEachObjectUnit(TEXT_DrawUnitHoverTextLow);

    if (sgTextState.bPopupActive == 1 && gPopupTextList && TEXT_PopupState(gPopupTextList) <= 1u)
    {
        D2ClientPopupStrc *pPopupNode = gPopupTextList;
        int32_t bPopupStillShowing;

        if (sgTextState.bPopupDismissed)
        {
            TEXT_SetPopupState(gPopupTextList, 2);
            bPopupStillShowing = 0;
        }
        else
        {
            const int32_t nMasterVolume = D2SOUND_10046_GetMasterVolumeSetting();
            if ((sgTextState.hPopupSpeechSound && !SOUNDINST_IsInstanceStillPlaying(sgTextState.hPopupSpeechSound) && nMasterVolume) || (sgTextState.hPopupSpeechSound = 0, !TEXT_DrawPopup(pPopupNode)))
            {
                bPopupStillShowing = 1;
            }
            else
            {
                sgTextState.bPopupDismissed = 1;
                if (!sgTextState.bPopupIsNpcSpeech && gPopupTextList)
                {
                    D2ClientPopupStrc *pPopupToFree = gPopupTextList;
                    TEXT_FreeParsedLines(gPopupTextList->pLines);
                    FOG_Free(pPopupToFree, szTextCpp, 1674, 0);
                    gPopupTextList = nullptr;
                    SELECT_SetClickRegion(nullptr);
                }
                TEXT_ClosePopup();
                if (gpfnPopupCallback)
                {
                    bPopupStillShowing = (gpfnPopupCallback)(gpPopupCallbackArg, 0, 0);
                }
                else
                {
                    if (sgTextState.bKeepNpcInteraction == 1)
                        sgTextState.bKeepNpcInteraction = 0;
                    else if (sgTextState.bPopupIsNpcSpeech)
                        NPCMENU_EndInteraction(sgTextState.dwPopupNpcGUID);

                    UI_ArmPanelToggleDelay();
                    sgTextState.bPopupIsNpcSpeech = 0;
                    SELECT_SetClickRegion(nullptr);
                    TEXT_PopupSetMessageHandlersActive(0);
                    bPopupStillShowing = 0;
                }
            }
        }
        nStillShowing = bPopupStillShowing;

        D2UnitStrc *pSpeaker = nullptr;
        if (sgTextState.bPopupIsNpcSpeech)
            pSpeaker = CUNIT_FindUnitByGuidAndType(sgTextState.dwPopupNpcGUID, UNIT_MONSTER);

        //  The player died, or the speaker went into anim mode 12: the conversation cannot continue either way.
        if (CUNIT_IsPlayerDeadOrMissing() || (sgTextState.bPopupIsNpcSpeech && pSpeaker && pSpeaker->dwAnimMode == 12))
        {
            if (sgTextState.bPopupIsNpcSpeech)
            {
                MSG_SendCmdUnit(48, 1, sgTextState.dwPopupNpcGUID);
                NPCMENU_NpcResyncVendorInventory(sgTextState.dwPopupNpcGUID);
                //  The flag is re-tested after those two calls, as the bytes test it; neither of them clears it.
                if (sgTextState.bPopupIsNpcSpeech)
                    NPCMENU_EndInteraction(sgTextState.dwPopupNpcGUID);
            }
            else
            {
                SELECT_SetClickRegion(nullptr);
                if (gPopupTextList)
                {
                    TEXT_FreeParsedText((void **)&gPopupTextList->pLines);
                    gPopupTextList = nullptr;
                }
                TEXT_StopPopupSpeechSound();
                TEXT_PopupSetMessageHandlersActive(0);
                if (sgTextState.bPopupNeedsAck)
                {
                    MSG_SendCmdUnit(49, -1, sgTextState.nPopupStringId);
                    sgTextState.bPopupNeedsAck = 0;
                }
            }
            UI_ArmPanelToggleDelay();
            sgTextState.bPopupIsNpcSpeech = 0;
            if (gbPopupHandlersRegistered == 1)
            {
                gbPopupHandlersRegistered = 0;
                WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPopupMessageHandlers, 7u);
            }
        }
    }

    if (sgTextState.bKeepNpcInteraction)
    {
        if (!sgTextState.bDisplayingStaticInfo)
        {
            FOG_DisplayAssert(szAssertStaticInfo, szTextCpp, 3326);
            exit(-1);
        }

        int32_t bStaticInfoShowing;
        if (gnStaticInfoDeadline < GetTickCount())
        {
            bStaticInfoShowing = 0;
            if (sgPopup.pParsedText)
            {
                D2ClientPopupStrc *pActiveTextPopup = sgPopup.pParsedText;
                TEXT_FreeParsedLines(sgPopup.pParsedText->pLines);
                FOG_Free(pActiveTextPopup, szTextCpp, 1674, 0);
                sgPopup.pParsedText = nullptr;
            }
            sgTextState.bKeepNpcInteraction = 0;
            sgTextState.bPopupIsNpcSpeech = 0;
        }
        else
        {
            bStaticInfoShowing = 1;
            D2ClientPopupStrc *pMsg = sgPopup.pParsedText;
            const uint32_t nPopupState = TEXT_PopupState(sgPopup.pParsedText);
            bool bDraw = true;

            if (nPopupState == 0)
                TEXT_SetPopupState(sgPopup.pParsedText, 1);
            else if (nPopupState == 3)
                bDraw = false;

            if (bDraw)
            {
                //  The UI shift decides whether the hover box is drawn at all: shift 1 only left of centre, shift 2 only right of it, shift 3
                //  never, anything else unconditionally.
                const int32_t nUiShift = SCROLL_GetUiShiftScreen();
                bool bDrawBox;
                if (nUiShift == 1)
                    bDrawBox = sgTextState.nStaticInfoPanelX <= gnScreenWidth / 2 - 10;
                else if (nUiShift == 2)
                    bDrawBox = sgTextState.nStaticInfoPanelX >= gnScreenWidth / 2 + 10;
                else if (nUiShift == 3)
                    bDrawBox = false;
                else
                    bDrawBox = true;

                if (bDrawBox)
                    TEXT_DrawHoverBox(pMsg, sgTextState.nStaticInfoPanelX, sgTextState.nStaticInfoPanelY);
            }
        }
        nStillShowing += bStaticInfoShowing;
    }

    //  Neither half wants the panel any more.
    if (!nStillShowing && sgTextState.bPopupActive == 1)
    {
        sgTextState.bForceDrawPopupText = 0;
        sgTextState.bPopupIsNpcSpeech = 0;
        sgTextState.bPopupDismissed = 1;
        sgTextState.bPopupActive = 0;
        if (gbPopupHandlersRegistered == 1)
        {
            gbPopupHandlersRegistered = 0;
            WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPopupMessageHandlers, 7u);
        }
    }

    sgTextState.bPopupDismissed = 0;
    if (!UI_GetUiWindowState(UI_MSGLOG))
    {
        TEXT_DrawChatMessages();
        TEXT_DrawQuickMessages();
    }
    if (sgTextState.pMessageLogDialog)
        DIALOG_Draw((D2ClientDialogStrc *)sgTextState.pMessageLogDialog);
}


//  What draws: the hover text above units and the speech popup.
static_assert(offsetof(D2ClientTextLineStrc, pszText) == 0,
              "TEXT_DrawPopup's `if (&pLine->pNext->pszText)` is a null test on "
              "pNext ONLY because pszText is the first member");
static_assert(offsetof(D2TextStateStrc, nHoverMsgCount) == 0x74,
              "the frame counter TEXT_PlaceHoverMsg bounds at 16 and indexes "
              "gHoverMsgRect by -- 0x74, two members before dwPopupStartTick's 0x7C");
static_assert(offsetof(RECT, left) == 0 && offsetof(RECT, top) == 4,
              "TEXT_PlaceHoverMsg passes &rcBox.left where the callee takes an "
              "int32_t * and reads [0] and [1], so the RECT's first two members "
              "are the coordinates in that order");
static_assert(LANGUAGE_JAPANESE == 6 && LANGUAGE_KOREAN == 7
                  && LANGUAGE_CHINESETWN == 9 && LANGUAGE_ENGLISHKOR == 12
                  && LANGUAGE_CHINESESIN == 8,
              "the four hover-text languages the original tests as the literals "
              "6, 7, 9 and 12 -- and the fifth, Chinese SIMPLIFIED, which is 8 "
              "and is deliberately NOT one of them");


//1.10f: D2Client.0x6FB1B260
//  The player path of a unit's hover text, drawn 30 pixels above the unit's client position. Unlike TEXT_DrawUnitHoverTextLow it has a marker
//  test (a message that begins with the bytes FF FF is followed by a decimal tbl index; the bytes are compared through uint8_t casts, so the
//  reading does not depend on char's signedness), a four-range tbl whitelist (1069..1295, 21895..22037, 10916, 21894) and a raw-text fallback
//  with a language check. A player's hover text is typed chat; a monster's is always an index.
//  The fallback refuses text it cannot render: the double-byte languages (6 JAPANESE, 7 KOREAN, 9 CHINESETWN, 12 ENGLISHKOR) are converted
//  with MultiByteToWideChar, everything else with Unicode::win2Unicode. If the client's own language disagrees with the sender's (Korean
//  accepts ENGLISHKOR and ENGLISHKOR accepts Korean) the string is scanned and drawn only if every character is below 0x80, so a Japanese line
//  reaching an English client is dropped.
//  The scan's exit that draws nothing is the function falling off its own end.
void __fastcall TEXT_DrawUnitHoverTextHigh(D2UnitStrc *pUnit)
{
    if (!pUnit)
        return;

    D2HoverTextStrc *pHoverText = pUnit->pHoverText;
    if (!pHoverText)
        return;

    D2CoordStrc tClientCoords;
    D2COMMON_UNITS_GetClientCoords(pUnit, &tClientCoords);
    tClientCoords.nX -= sgTextState.rcTextInfoPanel.left;
    tClientCoords.nY += -30 - sgTextState.rcTextInfoPanel.top;

    //  The panel ladder: 1 shifts left by a quarter screen, 2 right by one, 3 draws nothing, anything else leaves the anchor alone.
    const int32_t nUiShift = SCROLL_GetUiShiftScreen();
    if (nUiShift == 1)
        tClientCoords.nX += gnScreenWidth / -4;
    else if (nUiShift == 2)
        tClientCoords.nX += gnScreenWidth / 4;
    else if (nUiShift == 3)
        return;

    //  Y strictly inside the screen; X with 100 pixels of slop on each side.
    if (tClientCoords.nY <= 0 || tClientCoords.nY >= gnScreenHeight || tClientCoords.nX <= -100 || tClientCoords.nX >= gnScreenWidth + 100)
        return;

    //  DIFFERENCE: one 260-byte buffer where the frame has two named slots (a 2-byte marker and a 258-byte text): two C arrays would make
    //  CHAT_CopyHoverMsgToBuffer write past a two-byte object, which is undefined. The text after the marker is &szMessage[2].
    char szMessage[260];
    D2COMMON_CHAT_CopyHoverMsgToBuffer(pHoverText, szMessage);

    if ((uint8_t)szMessage[0] == 0xFF && (uint8_t)szMessage[1] == 0xFF)
    {
        const int32_t nTblId = atoi(&szMessage[2]);
        if ((nTblId >= 1069 && nTblId <= 1295) || (nTblId >= 21895 && nTblId <= 22037) || nTblId == 10916 || nTblId == 21894)
        {
            const Unicode *pwszFromTbl = D2LANG_GetStringFromTblIndex((D2C_StringIndices)nTblId);
            if (pwszFromTbl)
            {
                //  The declaration is the 200-element construction loop.
                Unicode wszTblText[200];
                memset(wszTblText, 0, sizeof(wszTblText));
                int32_t nMax = Unicode::strlen(pwszFromTbl);
                if (nMax >= 200)
                    nMax = 199;
                Unicode::strncpy(wszTblText, pwszFromTbl, nMax);
                TEXT_PlaceHoverMsg(pUnit, pHoverText, wszTblText, &tClientCoords.nX);
                return;
            }
        }
    }

    //  256 elements: the loop constructs from index 0 and wWideCharLast sits 510 bytes in, which is element 255, the array's last.
    Unicode wszWide[256];
    const uint8_t nLangId = D2COMMON_CHAT_GetLangIdFromHoverMsg(pHoverText);

    //  The four the original spells as 7, 12, 6 and 9 are the double-byte builds: Korean, EnglishKor, Japanese and Chinese-Taiwan. Chinese
    //  simplified is 8 and is not among them, so it takes the win2Unicode path with the single-byte languages.
    bool bDraw = false;
    if (nLangId != LANGUAGE_KOREAN && nLangId != LANGUAGE_ENGLISHKOR && nLangId != LANGUAGE_JAPANESE && nLangId != LANGUAGE_CHINESETWN)
    {
        Unicode::win2Unicode(wszWide, szMessage, 256);
        bDraw = true;
    }
    else
    {
        MultiByteToWideChar(0, 1u, szMessage, 256, (LPWSTR)wszWide, 256);
        const D2C_Language eLanguage = D2LANG_STRTABLE_GetLanguage();
        if (eLanguage == (D2C_Language)nLangId)
            bDraw = true;
        else if (eLanguage == LANGUAGE_KOREAN)
            bDraw = (nLangId == LANGUAGE_ENGLISHKOR);
        else if (eLanguage == LANGUAGE_ENGLISHKOR && nLangId == LANGUAGE_KOREAN)
            bDraw = true;

        if (!bDraw)
        {
            if (!wszWide[0].ch)
            {
                bDraw = true;
            }
            else
            {
                //  Draw it only if it is entirely ASCII; the first character at or above 0x80 drops the message.
                const Unicode *pWideCursor = wszWide;
                uint16_t wch = wszWide[0].ch;
                while (wch < 0x80u)
                {
                    wch = pWideCursor[1].ch;
                    ++pWideCursor;
                    if (!wch)
                    {
                        bDraw = true;
                        break;
                    }
                }
            }
        }
    }

    if (!bDraw)
        return;

    wszWide[255].ch = 0;
    TEXT_PlaceHoverMsg(pUnit, pHoverText, wszWide, &tClientCoords.nX);
}


//1.10f: D2Client.0x6FB1B4F0
//  Works out where a unit's speech bubble goes, and expires it when its time is up. Four arguments: ecx, edx and two on the stack (`retn 8`).
//  The node it allocates is measured and then freed (it exists to be sized, not shown); the caller draws from the position written back
//  through pnPos, which is a D2CoordStrc's nX, so it is `int *` and not `LONG *`.
//  The keep-out probe is a fixed 400 x 280 rectangle, much larger than the box it places, so two hover texts never sit close together even
//  when both would fit.
//  The expiry frees the message and carries on: nothing below reads pHoverMsg again (the text comes in through wszText), so the expired
//  message is drawn once more this frame and is gone by the next.
//  The original inlines the popup header free here.
void __fastcall TEXT_PlaceHoverMsg(D2UnitStrc *pUnit, D2HoverTextStrc *pHoverMsg, Unicode *wszText, int32_t *pnPos)
{
    //  Unsigned (`jbe`): the callee returns uint32_t. The cast is explicit because C's usual conversions do this silently and MSVC then
    //  warns (C4018); it changes no instruction.
    if ((uint32_t)sgTextState.nStaticInfoTimeout > D2COMMON_CHAT_GetTimeoutFromHoverMsg(pHoverMsg))
    {
        D2COMMON_CHAT_FreeHoverMsg(nullptr, pHoverMsg);
        if (pUnit)
            pUnit->pHoverText = nullptr;
    }

    int32_t nMaxTextWidth = 0;
    const enum Font ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);
    D2ClientPopupStrc *pMsg = (D2ClientPopupStrc *)FOG_Alloc(44, szTextCpp, 1625, 0);

    //  D2Win's own name for the 8-byte node is D2SplittedTextStrc; D2Client calls the same layout D2ClientTextLineStrc.
    D2ClientTextLineStrc *pSplitText = (D2ClientTextLineStrc *)D2WIN_10199(wszText, &pMsg->nLineCount, 180);
    const uint16_t wScrollRate = pMsg->wScrollRate;
    pMsg->pLines = pSplitText;
    pMsg->nScrollPixels = 0;
    pMsg->nScrollStepPixels = wScrollRate;
    for (; pSplitText; pSplitText = pSplitText->pNext)
    {
        const int32_t nTextWidth = D2WIN_10121_GetTextWidth(pSplitText->pszText);
        if (nMaxTextWidth <= nTextWidth)
            nMaxTextWidth = nTextWidth;
    }

    //  The line count at +0x04 is read as a word here and as a full int four lines down; both spellings are the original's.
    pMsg->wWidth = (uint16_t)(nMaxTextWidth + 20);
    pMsg->wHeight = (uint16_t)(15 * (uint16_t)pMsg->nLineCount + 6);

    const int32_t nBoxLeft = *pnPos - (int32_t)((uint16_t)(nMaxTextWidth + 20) >> 1);
    const int32_t nBoxTop = pnPos[1] - 50;
    *pnPos = nBoxLeft;
    pnPos[1] = nBoxTop;
    const int32_t nBoxTopAbove = nBoxTop - 15 * pMsg->nLineCount - 3;
    pnPos[1] = nBoxTopAbove;

    RECT rcBox;
    rcBox.left = nBoxLeft;
    rcBox.top = nBoxTopAbove;
    rcBox.right = nBoxLeft + pMsg->wWidth;
    rcBox.bottom = nBoxTopAbove + pMsg->wHeight;

    int32_t nHoverMsgCount = sgTextState.nHoverMsgCount;
    bool bTaken = (nHoverMsgCount >= 16);   // past the sixteenth, no rect is kept

    if (!bTaken)
    {
        const int32_t nSlot = nHoverMsgCount;
        bool bRecord = false;

        if (nHoverMsgCount == 0)
        {
            gHoverMsgRect[0].left = rcBox.left;
            gHoverMsgRect[0].top = rcBox.top;
            gHoverMsgRect[0].right = rcBox.right;
            gHoverMsgRect[0].bottom = rcBox.bottom;
            bTaken = true;
        }
        else if (nHoverMsgCount < 0)
        {
            //  The original's test is `<= 0`, reached only after `== 0` was already taken, so it is this. A negative count would index
            //  gHoverMsgRect below its first element; nothing writes one.
            bRecord = true;
        }
        else
        {
            RECT rcHit;
            int32_t nRectIdx = 0;
            while (!IntersectRect(&rcHit, &gHoverMsgRect[nRectIdx], &rcBox))
            {
                ++nRectIdx;
                if (nRectIdx >= nSlot)
                {
                    nHoverMsgCount = sgTextState.nHoverMsgCount;
                    bRecord = true;
                    break;
                }
            }
        }

        if (bRecord)
        {
            gHoverMsgRect[nSlot].left = rcBox.left;
            gHoverMsgRect[nSlot].top = rcBox.top;
            gHoverMsgRect[nSlot].right = rcBox.right;
            gHoverMsgRect[nSlot].bottom = rcBox.bottom;
            bTaken = true;
        }
        else if (!bTaken)
        {
            //  Something is in the way. Walk the offsets until a 400 x 280 probe is wholly on screen and hits none of the rects taken.
            //  nSrcLeft is rcBox.left throughout; it is reloaded because IntersectRect clobbers the register.
            const int32_t nSrcLeft = rcBox.left;
            for (int32_t nOffsetX = 50; nOffsetX < 400 && !bTaken; nOffsetX += 50)
            {
                for (int32_t nOffsetY = 35; nOffsetY < 280 && !bTaken; nOffsetY += 35)
                {
                    const int32_t nSignCount = (int32_t)gnHoverMsgOffsetSignCount;
                    for (int32_t iSign = 0; iSign < nSignCount && !bTaken; ++iSign)
                    {
                        //  BUG: one load of gnHoverMsgOffsetSigns is spent on both axes, so the eight passes only ever try (+dx, +dy) and
                        //  (-dx, -dy), four each, and never the other two diagonals: the 392 probes (7 x 7 x 8) try 98 distinct rectangles.
                        const int32_t nSign = gnHoverMsgOffsetSigns[iSign];

                        RECT tProbe;
                        tProbe.left = nSrcLeft + nOffsetX * nSign;
                        tProbe.right = tProbe.left + 400;
                        tProbe.top = rcBox.top + nOffsetY * nSign;
                        tProbe.bottom = tProbe.top + 280;

                        if (tProbe.left >= 0 && tProbe.left + 400 <= gnScreenWidth && tProbe.top >= 0 && tProbe.top + 280 <= gnScreenHeight)
                        {
                            RECT rcHit;
                            int32_t bOverlaps = 0;
                            for (int32_t k = 0; k < nSlot; ++k)
                            {
                                if (IntersectRect(&rcHit, &gHoverMsgRect[k], &tProbe))
                                    bOverlaps = 1;
                            }
                            if (!bOverlaps)
                            {
                                gHoverMsgRect[nSlot].left = tProbe.left;
                                gHoverMsgRect[nSlot].top = tProbe.top;
                                gHoverMsgRect[nSlot].right = tProbe.right;
                                gHoverMsgRect[nSlot].bottom = tProbe.bottom;
                                rcBox = tProbe;
                                nHoverMsgCount = sgTextState.nHoverMsgCount;
                                bTaken = true;
                            }
                        }
                    }
                }
            }

            if (!bTaken)
            {
                //  The give-up path: no rect recorded, no draw, the count not advanced.
                //  BUG: the font is not restored (every other exit ends with D2Win_10127_SetFont(nPrevFont)), so font 13 stays set for
                //  whoever draws next.
                TEXT_FreeParsedLines(pMsg->pLines);
                FOG_Free(pMsg, szTextCpp, 1674, 0);
                return;
            }
        }
    }

    sgTextState.nHoverMsgCount = nHoverMsgCount + 1;
    TEXT_DrawHoverMsgIfNotCovered(pMsg, &rcBox.left);
    TEXT_FreeParsedLines(pMsg->pLines);
    FOG_Free(pMsg, szTextCpp, 1674, 0);
    D2WIN_10127_SetFont(ePrevFont);
}


//1.10f: D2Client.0x6FB1B820
//  Draws a hover message through TEXT_DrawHoverBox, but only where an open side panel is not covering it. One caller, TEXT_PlaceHoverMsg.
//  pnCoords is a RECT's `left`, so it is `LONG *` (TEXT_PlaceHoverMsg's pnPos is an `int *`).
//  The state at +0x16 is the dword over field_16 and field_18: 0 becomes 1 on the first call, 3 means finished and returns without drawing.
//  Nothing here writes 3.
int32_t __fastcall TEXT_DrawHoverMsgIfNotCovered(D2ClientPopupStrc *pPopup, LONG *pnCoords)
{
    const uint32_t nDrawState = TEXT_PopupState(pPopup);
    if (nDrawState)
    {
        if (nDrawState == 3)
            return 1;
    }
    else
    {
        TEXT_SetPopupState(pPopup, 1);
    }

    //  0 draw; 1 only left of centre; 2 only right of centre; 3 never.
    const int32_t nUiShift = SCROLL_GetUiShiftScreen();
    if (nUiShift == 1)
    {
        if (*pnCoords <= gnScreenWidth / 2 - 10)
            TEXT_DrawHoverBox(pPopup, *pnCoords, pnCoords[1]);
    }
    else if (nUiShift == 2)
    {
        if (*pnCoords >= gnScreenWidth / 2 + 10)
            TEXT_DrawHoverBox(pPopup, *pnCoords, pnCoords[1]);
    }
    else if (nUiShift != 3)
    {
        TEXT_DrawHoverBox(pPopup, *pnCoords, pnCoords[1]);
    }
    return 1;
}


//1.10f: D2Client.0x6FB1B8A0
//  The monster-and-object path, drawn 10 pixels above the unit where the player path uses 30. A bare decimal tbl index or nothing at all: the
//  message is copied into a flat 256-byte buffer, atoi runs over all of it, and it is drawn only for 1..65534. No marker, no whitelist, no
//  fallback.
void __fastcall TEXT_DrawUnitHoverTextLow(D2UnitStrc *pUnit)
{
    if (!pUnit)
        return;

    D2HoverTextStrc *pHoverMsg = pUnit->pHoverText;
    if (!pHoverMsg)
        return;

    D2CoordStrc tClientCoords;
    D2COMMON_UNITS_GetClientCoords(pUnit, &tClientCoords);
    tClientCoords.nX -= sgTextState.rcTextInfoPanel.left;
    tClientCoords.nY += -10 - sgTextState.rcTextInfoPanel.top;

    const int32_t nUiShift = SCROLL_GetUiShiftScreen();
    if (nUiShift == 1)
        tClientCoords.nX += gnScreenWidth / -4;
    else if (nUiShift == 2)
        tClientCoords.nX += gnScreenWidth / 4;
    else if (nUiShift == 3)
        return;

    if (tClientCoords.nY <= 0 || tClientCoords.nY >= gnScreenHeight || tClientCoords.nX <= -100 || tClientCoords.nX >= gnScreenWidth + 100)
        return;

    char szMessage[256];
    D2COMMON_CHAT_CopyHoverMsgToBuffer(pHoverMsg, szMessage);

    const int32_t nTblId = atoi(szMessage);
    if (nTblId <= 0 || nTblId >= 0xFFFF)
        return;

    const Unicode *pwszFromTbl = D2LANG_GetStringFromTblIndex((D2C_StringIndices)nTblId);
    if (!pwszFromTbl)
        return;

    Unicode wszTblText[200];
    memset(wszTblText, 0, sizeof(wszTblText));
    int32_t nMax = Unicode::strlen(pwszFromTbl);
    if (nMax >= 200)
        nMax = 199;
    Unicode::strncpy(wszTblText, pwszFromTbl, nMax);
    TEXT_PlaceHoverMsg(pUnit, pHoverMsg, wszTblText, &tClientCoords.nX);
}


//1.10f: D2Client.0x6FB1BE70
//  Draws the speech popup and advances its scroll.
//  The scroll is a fixed-point accumulator: nScrollPixels counts 1/1024 of a pixel (0x4800 is 18 pixels, 0x17800 is 94). The popup measures
//  its own rate from the first line (`nStepTicks = (Time - dwLastTick) / wSameTickDraws`) rather than taking one from the data; wSameTickDraws
//  (+0x2A) is the number of draws that found timeGetTime unchanged since dwLastTick.
//  The return is a contract (TEXT_TickStaticInfoPanel tests it on the next instruction): 1 means the text has scrolled past its own extent,
//  the bound being 18 pixels per line past the first plus 112.
//  D2ExpRes patches seven sites in this function; each is a jmp into the plugin, which redoes the position from D2ExpRes.ini for 1024x768
//  and 1280x768 and jumps back. The stock numbers are written here, so replacing this function disables all seven:
//      UINPCDialogueHeightWidth       the 325 x 122 filled box
//      UINPCDialogueEdgeHeightWidth   the tiled frame around it
//      QuestScrollingText2, QuestScrollingText3   the main draw loop's 112 - (nScrollPixels >> 10)
//      QuestScrollingText4, QuestScrollingText5   the line arriving below
//      QuestScrollingText6                        the tail draw
BOOL __fastcall TEXT_DrawPopup(D2ClientPopupStrc *pPopup)
{
    if (!pPopup)
    {
        FOG_DisplayAssert(szAssertTextNode, szTextCpp, 1801);
        exit(-1);
    }

    const enum Font ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTFORMAL11);
    const uint32_t dwNow = timeGetTime();
    const uint32_t dwStartTick = pPopup->dwStartTick;
    if (dwStartTick)
    {
        const uint32_t dwLastTick = pPopup->dwLastTick;
        const uint32_t dwElapsed = dwNow - dwStartTick;
        const int32_t nStepTicks = pPopup->nStepTicks;
        const uint32_t nScrollSteps = dwElapsed >> 2;
        int32_t nAccumTicks = 0;

        if (dwLastTick == dwNow)
        {
            if (nStepTicks)
            {
                nAccumTicks = nStepTicks + pPopup->nAccumTicks;
                pPopup->nAccumTicks = nAccumTicks;
            }
            else
            {
                ++pPopup->wSameTickDraws;
            }
        }
        else
        {
            pPopup->nAccumTicks = 0;
            if (!nStepTicks && pPopup->wSameTickDraws)
                pPopup->nStepTicks = (dwNow - dwLastTick) / pPopup->wSameTickDraws;
            pPopup->dwLastTick = dwNow;
        }
        pPopup->nScrollPixels = pPopup->nScrollStepPixels * (nScrollSteps + nAccumTicks);
    }
    else
    {
        pPopup->dwLastTick = dwNow;
        pPopup->dwStartTick = dwNow;
    }

    //  The frame behind the speech, 325 x 122 with a one-pixel border.
    if (!sgTextState.bPopupIsQuestText && (sgTextState.nTextDisplayDelay || sgTextState.bForceDrawPopupText))
    {
        GFX_DrawFilledRect(gnPopupX, gnPopupY - 5, 325, 122, 0, 1);

        RECT tFrame;
        tFrame.left = gnPopupX - 1;
        tFrame.right = gnPopupX + 326;
        tFrame.bottom = gnPopupY + 117;
        tFrame.top = gnPopupY - 6;
        UIPANELS_DrawTiledFrame(&tFrame);
    }

    D2ClientTextLineStrc *pLines = pPopup->pLines;
    D2ClientTextLineStrc *pLastDrawnLine = nullptr;
    D2ClientTextLineStrc *pFirstDrawnLine = nullptr;
    D2ClientTextLineStrc *pFirstVisibleLine = nullptr;
    uint32_t nLastScrollPixels = 0;
    uint32_t nFadeRaw1 = 0;
    uint32_t nScrollPixels = pPopup->nScrollPixels;

    //  The guard tests pLines, and the block it skips to dereferences it.
    bool bTail = (!nScrollPixels || !pLines);
    if (!bTail)
    {
        do
        {
            if (nScrollPixels < 0x17800)
            {
                if (sgTextState.nTextDisplayDelay || sgTextState.bForceDrawPopupText)
                {
                    D2WIN_10117_DrawText(pLines->pszText, gnPopupX + 16, 112 - (int32_t)(nScrollPixels >> 10) + gnPopupY, 0, 0);
                }
                if (!pFirstDrawnLine)
                {
                    pFirstDrawnLine = pLines;
                    nFadeRaw1 = nScrollPixels;
                }
                pFirstVisibleLine = pFirstDrawnLine;
                nLastScrollPixels = nScrollPixels;
                pLastDrawnLine = pLines;
            }
            if (nScrollPixels < 0x4800)
                break;
            pLines = pLines->pNext;
            nScrollPixels -= 18432;
        } while (pLines);

        //  The line above the first visible one, cropped as it leaves.
        if (pFirstVisibleLine)
        {
            D2ClientTextLineStrc *pLineWalk = pPopup->pLines;
            if (pLineWalk != pFirstVisibleLine)
            {
                D2ClientTextLineStrc *pPrevLine;
                do
                {
                    pPrevLine = pLineWalk;
                    pLineWalk = pLineWalk->pNext;
                } while (pLineWalk != pFirstVisibleLine);

                if (pPrevLine)
                {
                    const int32_t nFadeVal1 = 94 - (int32_t)(nFadeRaw1 >> 10);
                    int32_t nCropHeight1 = nFadeVal1;
                    if (nFadeVal1 >= 18)
                        nCropHeight1 = 18;
                    if (sgTextState.nTextDisplayDelay || sgTextState.bForceDrawPopupText)
                    {
                        D2WIN_10119_DrawCroppedText(pPrevLine->pszText, gnPopupX + 16, nFadeVal1 + gnPopupY, 3, 0, nCropHeight1);
                    }
                }
            }
        }

        //  The line arriving below the last drawn one. `&pNext->pszText` is pNext itself (pszText is at offset 0), so the null test is real.
        if (pLastDrawnLine)
        {
            D2ClientTextLineStrc *pNextLine = pLastDrawnLine->pNext;
            if (pNextLine)
            {
                const uint32_t nFadeVal2 = 112 - ((nLastScrollPixels + 1024) >> 10);
                const int32_t nFadeDelta = (int32_t)(nFadeVal2 - 94);
                const int32_t nMaxWidth = D2WIN_10125();
                if (nFadeDelta < nMaxWidth)
                {
                    const int32_t nCropWidth = nMaxWidth - nFadeDelta;
                    if (sgTextState.nTextDisplayDelay || sgTextState.bForceDrawPopupText)
                    {
                        D2WIN_10119_DrawCroppedText(pNextLine->pszText, gnPopupX + 16, gnPopupY + (int32_t)nFadeVal2 + 19, 0, nFadeDelta, nCropWidth);
                    }
                }
            }
        }

        bTail = (!pFirstDrawnLine && !pLastDrawnLine);
    }

    bool bSkipTailDraw = false;
    if (bTail)
    {
        //  BUG: reached with pPopup->pLines null by the guard above, and the first thing it does is read pLines->pNext, a null dereference.
        //  Reproduced as written.
        D2ClientTextLineStrc *pLastLine = pPopup->pLines;
        uint32_t nScrollRemainder = pPopup->nScrollPixels;
        for (D2ClientTextLineStrc *i = pPopup->pLines->pNext; i; i = i->pNext)
        {
            pLastLine = i;
            nScrollRemainder -= 18432;
        }

        if (nScrollRemainder < 0x1C000)
        {
            const uint32_t nScrollRows = (nScrollRemainder + 1024) >> 10;
            const int32_t nLineY = 112 - (int32_t)nScrollRows;
            int32_t nCropHeight2;
            if (nLineY >= 18)
            {
                nCropHeight2 = 18;
            }
            else
            {
                nCropHeight2 = nLineY;
                if (!nLineY)
                    bSkipTailDraw = true;
            }
            if (!bSkipTailDraw && (sgTextState.nTextDisplayDelay || sgTextState.bForceDrawPopupText))
            {
                D2WIN_10119_DrawCroppedText(pLastLine->pszText, gnPopupX + 16, nLineY + gnPopupY, 3, 0, nCropHeight2);
            }
        }
    }

    //  The zero-height case skips the draw above and not this font restore.
    D2WIN_10127_SetFont(ePrevFont);

    int32_t nLineCountMinus1 = pPopup->nLineCount - 1;
    if (nLineCountMinus1 <= 0)
        nLineCountMinus1 = 1;
    if ((uint32_t)pPopup->nScrollPixels >> 10 <= (uint32_t)(18 * nLineCountMinus1 + 112))
        return 0;

    if (gbPopupDismissPending == 1)
        gbPopupDismissPending = 0;
    return 1;
}


//1.10f: D2Client.0x6FB1E670
//  Packet 0x76: stop showing that unit's speech subtitle. Resolves the unit and frees its hover text, the field TEXT_ChatShowSpeechSubtitle
//  writes for the Japanese, Korean and Chinese-Taiwan builds. The NULL is written whether or not there was a message to free; the store is
//  outside the test.
void __fastcall TEXT_ChatClearUnitHoverText(D2GSPacketSrv76 *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (pUnit)
    {
        if (pUnit->pHoverText)
            D2COMMON_CHAT_FreeHoverMsg(nullptr, pUnit->pHoverText);
        pUnit->pHoverText = nullptr;
    }
}


//  The chat and quick-message lists: the two drawers, the two builders and the scrollback push they share.
static_assert(offsetof(D2ClientTextNodeStrc, wLineCount) == 0x18
                  && offsetof(D2ClientTextNodeStrc, nColor) == 0x1A
                  && offsetof(D2ClientTextNodeStrc, dwExpireTick) == 0x1E
                  && offsetof(D2ClientTextNodeStrc, pNext) == 0x22,
              "the packed message node: pLines[6] fills 0..0x17, and pNext at 0x22 "
              "is what makes the 38-byte clear come out as memset(p, 0, 0x24) plus "
              "a two-byte tail INSIDE the link");
static_assert(UI_HIRICONS == 19 && UI_NEWSTATS == 6 && UI_QUESTLOG == 17,
              "the three panels these drawers move out of the way of, each a bare "
              "number in the original: hireling icons push the chat down to 95, and "
              "the stats or quest panel push the quick messages right to 100");


//1.10f: D2Client.0x6FB1C1C0
//  Draws the chat list top-left and then expires it. The wrap and the left edge both come from SCROLL_GetUiShiftScreen: shift 1 wraps in
//  place, shift 2 wraps and moves to the right half. Shift 3 does not return here (unlike the hover text): with both panels open it still
//  draws, at x = 15 and unwrapped.
//  Under the wrap the split lines are drawn and freed as they are drawn, so the wrap is redone every frame.
//  The original inlines the split-line free and TEXT_FreeMessageList's node free here.
void __cdecl TEXT_DrawChatMessages()
{
    if (!sgTextState.GlobalMessageDisplayList.wCount)
        return;

    const enum Font ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);

    int32_t nTopY = 20;
    if (UI_GetUiWindowState(UI_HIRICONS))
        nTopY = 95;

    int32_t nRowsDrawn = 0;
    int32_t nRowsDrawnNext = 0;
    int32_t nX = 15;
    int32_t bWrapLines = 0;

    const int32_t nUiShift = SCROLL_GetUiShiftScreen();
    if (nUiShift == 1)
    {
        bWrapLines = 1;
    }
    else if (nUiShift == 2)
    {
        nX = gnScreenWidth / 2 + 15;
        bWrapLines = 1;
    }

    for (D2ClientTextNodeStrc *pNode = sgTextState.GlobalMessageDisplayList.pHead; pNode; pNode = pNode->pNext)
    {
        for (int32_t i = 0; i < 6; ++i)
        {
            const Unicode *pLine = pNode->pLines[i];
            if (!pLine)
                continue;

            if (bWrapLines)
            {
                //  The declaration is the 400-element construction loop.
                Unicode wszText[400];
                Unicode::strcpy(wszText, pNode->pLines[i]);

                int32_t nLines;
                D2SplittedTextStrc *pSplitText = D2WIN_10199(wszText, &nLines, 300);
                if (pSplitText)
                {
                    //  12n + 3n in the bytes; the compiler split the multiply.
                    int32_t nY = nTopY + 15 * nRowsDrawn;
                    D2SplittedTextStrc *pSplitNext;
                    do
                    {
                        pSplitNext = pSplitText->pNextLine;
                        const int32_t nTextLength = Unicode::strlen(pSplitText->wszLine);
                        const int32_t nLineWidth = D2WIN_10123(pSplitText->wszLine, nTextLength);
                        if (nLineWidth)
                        {
                            GFX_DrawFilledRect(nX - 5, nY - 14, nLineWidth + 10, 16, 0, 1);
                            D2WIN_10117_DrawText(pSplitText->wszLine, nX, nY, pNode->nColor, 0);
                            nY += 15;
                            ++nRowsDrawnNext;
                        }
                        FOG_Free(pSplitText->wszLine, szTextCpp, 2627, 0);
                        FOG_Free(pSplitText, szTextCpp, 2628, 0);
                        pSplitText = pSplitNext;
                    } while (pSplitNext);
                    nRowsDrawn = nRowsDrawnNext;
                }
            }
            else
            {
                const int32_t nTextLength = Unicode::strlen(pLine);
                const int32_t nLineWidth = D2WIN_10123(pNode->pLines[i], nTextLength);
                if (nLineWidth)
                {
                    const int32_t nY = nTopY + 15 * nRowsDrawn;
                    GFX_DrawFilledRect(nX - 4, nY - 14, nLineWidth + 8, 16, 0, 1);
                    D2WIN_10117_DrawText(pNode->pLines[i], nX, nY, pNode->nColor, 0);
                    nRowsDrawnNext = ++nRowsDrawn;
                }
            }
        }
    }

    //  The expiry sweep.
    D2ClientTextNodeStrc *pMsg = sgTextState.GlobalMessageDisplayList.pHead;
    D2ClientTextNodeStrc *pPrevMsg = nullptr;
    const uint32_t dwNow = GetTickCount();
    while (pMsg)
    {
        if (pMsg->dwExpireTick >= dwNow)
        {
            pPrevMsg = pMsg;
            pMsg = pMsg->pNext;
        }
        else
        {
            --sgTextState.GlobalMessageDisplayList.wCount;
            D2ClientTextNodeStrc *pNext = pMsg->pNext;
            if (sgTextState.GlobalMessageDisplayList.pHead == pMsg)
                sgTextState.GlobalMessageDisplayList.pHead = pMsg->pNext;
            for (int32_t j = 0; j < 6; ++j)
            {
                Unicode *pLineToFree = pMsg->pLines[j];
                if (pLineToFree)
                    FOG_Free(pLineToFree, szTextCpp, 1711, 0);
            }
            FOG_Free(pMsg, szTextCpp, 1714, 0);
            if (pPrevMsg)
                pPrevMsg->pNext = pNext;
            pMsg = pNext;
        }
    }

    D2WIN_10127_SetFont(ePrevFont);
}


//1.10f: D2Client.0x6FB1C450
//  The quick-message list, drawn bottom-up from the screen's foot and expired the same way the chat list is. The left edge steps aside for
//  the stats panel or the quest log.
//  The original ends `jmp D2Win_10127_SetFont`, which is the epilogue and not a return value; the sole caller discards it.
//  The original inlines TEXT_FreeMessageList's node free here.
void __cdecl TEXT_DrawQuickMessages()
{
    if (!sgTextState.QuickMessageDisplayList.wCount)
        return;

    const enum Font ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);

    int32_t nX = 15;
    if (UI_GetUiWindowState(UI_NEWSTATS) || UI_GetUiWindowState(UI_QUESTLOG))
        nX = 100;

    D2ClientTextNodeStrc *pMsg = sgTextState.QuickMessageDisplayList.pHead;
    if (pMsg)
    {
        int32_t nBoxY = 0;
        int32_t nTextY = 0;
        do
        {
            for (int32_t k = 0; k < 6; ++k)
            {
                //  The original's induction variable is the pixel step, 25 per line, and the offset it uses is that halved: 0, 12, 25, 37,
                //  50, 62, so the gaps alternate 12 and 13.
                const int32_t i = 25 * k;
                const Unicode *pLine = pMsg->pLines[k];
                if (pLine)
                {
                    const int32_t nTextLength = Unicode::strlen(pLine);
                    const int32_t nLineWidth = D2WIN_10123(pLine, nTextLength);
                    GFX_DrawFilledRect(nX - 5, nBoxY - i / 2 + gnScreenHeight - 129, nLineWidth + 10, 16, 0, 1);
                    D2WIN_10117_DrawText(pLine, nX, nTextY - i / 2 + gnScreenHeight - 115, pMsg->nColor, 0);
                }
            }
            pMsg = pMsg->pNext;
            nBoxY -= 25;
            nTextY -= 25;
        } while (pMsg);
    }

    D2ClientTextNodeStrc *pMsgSweep = sgTextState.QuickMessageDisplayList.pHead;
    D2ClientTextNodeStrc *pPrevMsg = nullptr;
    const uint32_t dwNow = GetTickCount();
    while (pMsgSweep)
    {
        if (pMsgSweep->dwExpireTick >= dwNow)
        {
            pPrevMsg = pMsgSweep;
            pMsgSweep = pMsgSweep->pNext;
        }
        else
        {
            --sgTextState.QuickMessageDisplayList.wCount;
            D2ClientTextNodeStrc *pNext = pMsgSweep->pNext;
            if (sgTextState.QuickMessageDisplayList.pHead == pMsgSweep)
                sgTextState.QuickMessageDisplayList.pHead = pMsgSweep->pNext;
            for (int32_t j = 0; j < 6; ++j)
            {
                Unicode *pLineToFree = pMsgSweep->pLines[j];
                if (pLineToFree)
                    FOG_Free(pLineToFree, szTextCpp, 1711, 0);
            }
            FOG_Free(pMsgSweep, szTextCpp, 1714, 0);
            if (pPrevMsg)
                pPrevMsg->pNext = pNext;
            pMsgSweep = pNext;
        }
    }

    D2WIN_10127_SetFont(ePrevFont);
}


//1.10f: D2Client.0x6FB1C600
//  Puts one line into the chat list: allocate the 38-byte node, split the text at gnScreenWidth - 70, keep at most six lines and free the
//  rest, append at the tail, mirror into the scrollback, and drop the oldest node once the list holds more than 18 lines (not 18 nodes: the
//  loop sums wLineCount). The tail walk asserts its own count against the list's.
//  The original inlines TEXT_FreeMessageList's node free and the message-log listbox refresh here.
void __fastcall TEXT_PrintChatMessage(Unicode *wszText, int32_t nColor)
{
    D2ClientTextNodeStrc *pMessage = (D2ClientTextNodeStrc *)FOG_Alloc(38, szTextCpp, 3394, 0);

    //  36 + 2 = 38: MSVC emits the whole-record clear as a dword run and a two-byte tail, and the tail lands inside pNext at +0x22.
    memset(pMessage, 0, sizeof(D2ClientTextNodeStrc));
    pMessage->pNext = nullptr;
    pMessage->dwExpireTick = GetTickCount() + 10000;

    const enum Font ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);

    int32_t nLines;
    D2SplittedTextStrc *pSplitText = D2WIN_10199(wszText, &nLines, gnScreenWidth - 70);
    pMessage->wLineCount = (uint16_t)((uint32_t)nLines > 6 ? 6 : nLines);

    if (nLines > 0)
    {
        int32_t iLine = 0;
        do
        {
            //  Past the sixth the split line is freed instead of kept.
            if (iLine >= pMessage->wLineCount)
                FOG_Free(pSplitText->wszLine, szTextCpp, 3414, 0);
            else
                pMessage->pLines[iLine] = pSplitText->wszLine;

            D2SplittedTextStrc *pSplitNext = pSplitText->pNextLine;
            FOG_Free(pSplitText, szTextCpp, 3417, 0);
            ++iLine;
            pSplitText = pSplitNext;
        } while (iLine < nLines);
    }
    pMessage->nColor = (uint8_t)nColor;

    if (sgTextState.GlobalMessageDisplayList.pHead)
    {
        D2ClientTextNodeStrc *pTail = sgTextState.GlobalMessageDisplayList.pHead;
        uint16_t wWalked = 0;
        for (D2ClientTextNodeStrc *pNext = pTail->pNext; pNext; pNext = pNext->pNext)
        {
            pTail = pNext;
            ++wWalked;
        }
        if (wWalked != (uint16_t)(sgTextState.GlobalMessageDisplayList.wCount - 1))
        {
            FOG_DisplayAssert(szAssertGlobalCount, szTextCpp, 3432);
            exit(-1);
        }
        pTail->pNext = pMessage;
    }
    else
    {
        sgTextState.GlobalMessageDisplayList.pHead = pMessage;
    }

    TEXT_PushMessageLogEntry(pMessage);
    ++sgTextState.GlobalMessageDisplayList.wCount;

    //  The cap is eighteen lines over the whole list, not eighteen nodes.
    D2ClientTextNodeStrc *pWalk = sgTextState.GlobalMessageDisplayList.pHead;
    if (pWalk)
    {
        int32_t nTotalLines = 0;
        do
        {
            const uint16_t wLineCount = pWalk->wLineCount;
            pWalk = pWalk->pNext;
            nTotalLines += wLineCount;
        } while (pWalk);

        if (nTotalLines > 18)
        {
            D2ClientTextNodeStrc *pOldest = sgTextState.GlobalMessageDisplayList.pHead;
            --sgTextState.GlobalMessageDisplayList.wCount;
            sgTextState.GlobalMessageDisplayList.pHead = pOldest->pNext;
            for (int32_t j = 0; j < 6; ++j)
            {
                Unicode *pLineToFree = pOldest->pLines[j];
                if (pLineToFree)
                    FOG_Free(pLineToFree, szTextCpp, 1711, 0);
            }
            FOG_Free(pOldest, szTextCpp, 1714, 0);
        }
    }

    SOUNDINST_PlaySound(SOUND_CURSOR_SWITCH, nullptr, 0, 0, 0);

    if (UI_GetUiWindowState(UI_MSGLOG) && sgTextState.pMessageLogListBox)
    {
        void *pListBox = sgTextState.pMessageLogListBox;
        (*(void(__thiscall **)(void *))(*(uint32_t *)pListBox + 60))(pListBox);
        for (D2ClientTextNodeStrc *pEntry = sgTextState.MessageLogDisplayList.pHead; pEntry; pEntry = pEntry->pNext)
        {
            (*(void(__thiscall **)(void *, Unicode *, Unicode *, Unicode *, Unicode *, Unicode *, Unicode *, uint32_t, int32_t, uint32_t, int32_t)) (*(uint32_t *)pListBox + 48))(pListBox, pEntry->pLines[0], pEntry->pLines[1], pEntry->pLines[2],
                pEntry->pLines[3], pEntry->pLines[4], pEntry->pLines[5], 0, pEntry->nColor, 0, 13);
        }
    }

    D2WIN_10127_SetFont(ePrevFont);
}


//1.10f: D2Client.0x6FB1C830
//  Copies a message node into the scrollback: FOG_Alloc(38) and then every field. It deep-copies each line (FOG_Alloc(2 * len + 2)) so the
//  log survives the display list being freed, and caps the log at 0x80 entries.
//  The original computes one pointer difference, `nNodeDelta = (char *)pLogEntry - (char *)pSrcMessage`, and walks both records in step with
//  it. The delta is the distance between two objects and not an offset into anything; it is written here as two cursors.
//  The original inlines TEXT_FreeMessageList's node free here.
void __fastcall TEXT_PushMessageLogEntry(D2ClientTextNodeStrc *pSrcMessage)
{
    D2ClientTextNodeStrc *pLogEntry = (D2ClientTextNodeStrc *)FOG_Alloc(38, szTextCpp, 2508, 0);
    memset(pLogEntry, 0, sizeof(D2ClientTextNodeStrc));
    pLogEntry->pNext = nullptr;

    if (pSrcMessage->wLineCount)
    {
        int32_t iLine = 0;
        do
        {
            const int32_t nLineLen = Unicode::strlen(pSrcMessage->pLines[iLine]);
            Unicode *pCopy = (Unicode *)FOG_Alloc(2 * nLineLen + 2, szTextCpp, 2512, 0);
            pLogEntry->pLines[iLine] = pCopy;
            Unicode::strcpy(pCopy, pSrcMessage->pLines[iLine]);
            ++iLine;
        } while (iLine < pSrcMessage->wLineCount);
    }
    pLogEntry->wLineCount = pSrcMessage->wLineCount;
    pLogEntry->nColor = pSrcMessage->nColor;

    D2ClientTextNodeStrc *pHead = sgTextState.MessageLogDisplayList.pHead;
    if (sgTextState.MessageLogDisplayList.wCount >= 0x80u)
    {
        if (pHead)
        {
            //  Walk to the tail keeping its predecessor.
            D2ClientTextNodeStrc *pPrev = nullptr;
            for (D2ClientTextNodeStrc *pNext = pHead->pNext; pNext; pNext = pNext->pNext)
            {
                pPrev = pHead;
                pHead = pNext;
            }
            for (int32_t j = 0; j < 6; ++j)
            {
                Unicode *pLineText = pHead->pLines[j];
                if (pLineText)
                    FOG_Free(pLineText, szTextCpp, 1711, 0);
            }
            FOG_Free(pHead, szTextCpp, 1714, 0);

            //  BUG: with exactly one node the walk never ran, pPrev is null, and nothing clears MessageLogDisplayList.pHead, which still points
            //  at the node just freed; the re-read below picks it straight back up. It needs wCount >= 0x80 while the list holds one node.
            if (pPrev)
                pPrev->pNext = nullptr;
            if (sgTextState.MessageLogDisplayList.wCount)
                --sgTextState.MessageLogDisplayList.wCount;

            pHead = sgTextState.MessageLogDisplayList.pHead;
        }
    }
    else
    {
        pHead = sgTextState.MessageLogDisplayList.pHead;
    }

    pLogEntry->pNext = pHead;
    ++sgTextState.MessageLogDisplayList.wCount;
    sgTextState.MessageLogDisplayList.pHead = pLogEntry;
}


//1.10f: D2Client.0x6FB1C950
//  Word-wraps a string into a node and puts it on both lists that show it. Six lines maximum, ten seconds to live, at most two on screen.
//  The wrap has a defect (see the BUG note at the end of its loop) and a dead szUtf conversion.
//  The original inlines TEXT_FreeMessageList's node free and the message-log listbox refresh here.
void __fastcall TEXT_AddQuickMessage(const Unicode *pwszText, int32_t nColor)
{
    const enum Font ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);

    D2ClientTextNodeStrc *pMessage = (D2ClientTextNodeStrc *)FOG_Alloc(38, szTextCpp, 3483, 0);
    memset(pMessage, 0, sizeof(D2ClientTextNodeStrc));
    pMessage->pNext = nullptr;
    pMessage->dwExpireTick = GetTickCount() + 10000;

    const int32_t nTotalLen = Unicode::strlen(pwszText);
    int32_t nLen = nTotalLen;
    int32_t nOffset = 0;
    pMessage->wLineCount = 0;

    char szUtf[1024];
    while (1)
    {
        //  Shrink until the slice fits the screen (gnScreenWidth - 70).
        for (; nLen > 0; --nLen)
        {
            if (D2WIN_10123(&pwszText[nOffset], nLen) <= gnScreenWidth - 70)
                break;
        }

        //  Unless the slice runs to the end, back up to a break opportunity.
        if (nOffset + nLen != nTotalLen)
        {
            while (nLen > 0)
            {
                if (Unicode::isLineBreak(&pwszText[nOffset], nLen + 1))
                    break;
                --nLen;
            }
        }
        if (nLen == 0)
            break;

        const Unicode *pSrc = &pwszText[nOffset];

        //  Dead: szUtf (1024 bytes) is written and never read, and this runs before the six-line cap, so the seventh slice is converted and
        //  thrown away.
        Unicode::toUtf(szUtf, pSrc, nLen);

        if (pMessage->wLineCount >= 6u)
            break;

        pMessage->pLines[pMessage->wLineCount] = (Unicode *)FOG_Alloc(2 * nLen + 2, szTextCpp, 3525, 0);
        memset(pMessage->pLines[pMessage->wLineCount], 0, 2 * nLen + 2);
        Unicode::strncpy(pMessage->pLines[pMessage->wLineCount], pSrc, nLen);
        ++pMessage->wLineCount;

        //  BUG: nOffset is assigned the line's length instead of being advanced by it, and the remainder is taken from that same length rather
        //  than from how much has been consumed. Right for the first line (nOffset was 0); from the third line on the wrap restarts at the offset
        //  of the second line's length, so the third and later lines repeat text already shown. It is reached only by a message wider than two
        //  screens' worth of text, and the message is never shown past nTotalLen only because nOffset + nLen still sums to it.
        nOffset = nLen;
        nLen = nTotalLen - nLen;
    }
    pMessage->nColor = (uint8_t)nColor;

    if (sgTextState.QuickMessageDisplayList.pHead)
    {
        D2ClientTextNodeStrc *pTail = sgTextState.QuickMessageDisplayList.pHead;
        uint16_t wWalked = 0;
        for (D2ClientTextNodeStrc *pNext = pTail->pNext; pNext; pNext = pNext->pNext)
        {
            pTail = pNext;
            ++wWalked;
        }
        if (wWalked != (uint16_t)(sgTextState.QuickMessageDisplayList.wCount - 1))
        {
            FOG_DisplayAssert(szAssertQuickCount, szTextCpp, 3551);
            exit(-1);
        }
        pTail->pNext = pMessage;
    }
    else
    {
        sgTextState.QuickMessageDisplayList.pHead = pMessage;
    }
    ++sgTextState.QuickMessageDisplayList.wCount;

    TEXT_PushMessageLogEntry(pMessage);

    //  At most two quick messages are on screen at once.
    if (sgTextState.QuickMessageDisplayList.wCount > 2u)
    {
        D2ClientTextNodeStrc *pOldest = sgTextState.QuickMessageDisplayList.pHead;
        sgTextState.QuickMessageDisplayList.pHead = pOldest->pNext;
        for (int32_t iLine = 0; iLine < 6; ++iLine)
        {
            Unicode *pLineToFree = pOldest->pLines[iLine];
            if (pLineToFree)
                FOG_Free(pLineToFree, szTextCpp, 1711, 0);
        }
        FOG_Free(pOldest, szTextCpp, 1714, 0);
        --sgTextState.QuickMessageDisplayList.wCount;
    }

    if (UI_GetUiWindowState(UI_MSGLOG) && sgTextState.pMessageLogListBox)
    {
        void *pListBox = sgTextState.pMessageLogListBox;
        (*(void(__thiscall **)(void *))(*(uint32_t *)pListBox + 60))(pListBox);
        for (D2ClientTextNodeStrc *pEntry = sgTextState.MessageLogDisplayList.pHead; pEntry; pEntry = pEntry->pNext)
        {
            (*(void(__thiscall **)(void *, Unicode *, Unicode *, Unicode *, Unicode *, Unicode *, Unicode *, uint32_t, int32_t, uint32_t, int32_t)) (*(uint32_t *)pListBox + 48))(pListBox, pEntry->pLines[0], pEntry->pLines[1], pEntry->pLines[2],
                pEntry->pLines[3], pEntry->pLines[4], pEntry->pLines[5], 0, pEntry->nColor, 0, 13);
        }
    }

    D2WIN_10127_SetFont(ePrevFont);
    SOUNDINST_PlaySound(SOUND_CURSOR_SWITCH, nullptr, 0, 0, 0);
}


//1.10f: D2Client.0x6FB1E6C0
//  Maps a quest message's string id to its NPC-menu line. gQuestMenuStrings is 527 records of { uint16 nQuestStringId; uint16 nTblIdx },
//  mostly sorted by key (a few neighbouring pairs are swapped). The id is compared against the key of record 100
//  (`cmp gQuestMenuStrings.nQuestStringId+190h, cx`; 0x190 is 100 * 4), and the linear scan then runs over [0, 100) or [100, count).
//  The split test is unsigned (`jbe`; the key is a uint16 promoted for the compare) and the scan's bounds are signed (the cursor and the
//  count are plain ints).
//  Both failure exits return tbl 3724, whose key is `qstsxxx` and whose text is 'Invalid Quest Value'. The two `jmp D2LANG_GetStringFromTblIndex`
//  at the end make that function's return this one's; the result is used (NPCMENU_BuildTalkMenu hands it straight to DIALOG_AddLine).
const Unicode *__fastcall TEXT_QuestGetQuestMenuString(uint16_t nQuestStringId)
{
    int32_t iRecord;
    int32_t nEnd;

    //  The split: record 100's key, not record 100.
    if (gQuestMenuStrings[100].nQuestStringId <= nQuestStringId)
    {
        nEnd = gnQuestMenuStringCount;
        iRecord = 100;
    }
    else
    {
        iRecord = 0;
        nEnd = 100;
    }

    if (iRecord >= nEnd)
        return D2LANG_GetStringFromTblIndex(STR_IDX_3724_qstsxxx);

    while (gQuestMenuStrings[iRecord].nQuestStringId != nQuestStringId)
    {
        if (++iRecord >= nEnd)
            return D2LANG_GetStringFromTblIndex(STR_IDX_3724_qstsxxx);
    }

    return D2LANG_GetStringFromTblIndex((D2C_StringIndices)gQuestMenuStrings[iRecord].nTblIdx);
}


//1.10f: D2Client.0x6FB1E990
//  The quest-scroll reading panel's entry from the frame (the panel's dismiss half is TEXT_IniScrollOnMouseDown). It switches on
//  pOpenScrollItemsTxt->dwCode, the four bytes at +128; the case labels are little-endian ASCII item codes from misc.txt:
//      0x20736B62  'bks '  Bark Scroll, undeciphered        -> the still frame
//      0x20646B62  'bkd '  Bark Scroll, deciphered          -> the animated frame, or the plain panel
//      0x20317274  'tr1 '  Horadric Quest Info scroll       -> the plain panel
//  The three are the Scroll of Inifuss, the Key to the Cairn Stones and the Horadric Scroll. Anything else returns without drawing.
//  gbQuestScrollOpen selects between the two 'bkd ' halves: TEXT_QuestOnCairnStoneOrderCmd clears it when the server sends the stone order;
//  with it clear the deciphered scroll animates, with it set the panel is drawn plain. The original is three compare-and-jump chains, not a
//  jump table, so the if/else-if below is its shape.
void __cdecl TEXT_ScrollDrawQuestScrollPanel()
{
    if (!sgTextState.pOpenScrollItemsTxt)
        return;

    const int32_t nScrollKind = (int32_t)sgTextState.pOpenScrollItemsTxt->dwCode;

    if (nScrollKind == 0x20736B62)          //  'bks '
    {
        TEXT_DrawQuestScrollFrame(0);
        return;
    }

    if (nScrollKind == 0x20646B62)          //  'bkd '
    {
        if (!gbQuestScrollOpen)
        {
            TEXT_DrawQuestScrollFrame(1);
            return;
        }
    }
    else if (nScrollKind != 0x20317274)     //  'tr1 '
    {
        return;
    }

    TEXT_IniScrollDrawPanel();
}


//1.10f: D2Client.0x6FB1E9E0
//  Draws the scroll panel itself: a filled box in the lower-left quarter, the `scroin` art over it in four quarters, and the close button.
//  gIniScrollImage is loaded lazily on the first draw and left; TEXT_IniScrollOnMouseDown frees the three scroll images.
//  The art is 2x2: frames 0 and 1 across the top at `y - 109`, frames 2 and 3 across the bottom at `y - 49`, each right-hand piece 256 pixels
//  over. Then the record's image is swapped for the buy/sell button sheet and frame 10 is drawn at +277, -61. Every coordinate goes through
//  the letterbox offsets, so the panel follows the resolution border rather than the screen.
//  `gnScreenWidth / 2` is a signed halving in the bytes (`cdq / sub eax, edx / sar eax, 1`), which is C's `/ 2` on an int.
//  The three stores after the memset are redundant and the original's: `rep stosd` has already zeroed the record.
void __cdecl TEXT_IniScrollDrawPanel()
{
    D2GfxDataStrc tData;
    char szName[260];

    D2GFX_DrawBox(gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 224, gnScreenWidth / 2, gnScreenHeight - 48, 0, 5);

    D2CellFileStrc *pImage = gIniScrollImage;
    if (!pImage)
    {
        wsprintfA(szName, szFmtScroin, aDataGlobal);
        pImage = ARCHIVE_CellFileLoad(szName, 0);
        gIniScrollImage = pImage;
    }

    memset(&tData, 0, sizeof(tData));
    tData.nDirection = 0;
    tData.nFrame = 0;
    tData.pCellFile = pImage;

    //  D2ExpRes: CainQuestScrollProc detours at RVA 0x7EA5C and returns at
    //  0x7EB25 -- over the whole four-cel block -- to redo the positions from
    //  D2ExpRes.ini for 1024x768 and 1280x768.
    D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 109, 0xFFFFFFFF, 5, nullptr);
    tData.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 109, 0xFFFFFFFF, 5, nullptr);
    tData.nFrame = 2;
    D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 49, 0xFFFFFFFF, 5, nullptr);
    tData.nFrame = 3;
    D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 49, 0xFFFFFFFF, 5, nullptr);

    tData.pCellFile = UI_GetBuySellButtonImage();
    tData.nFrame = 10;
    //  D2ExpRes: CainQuestScrollCloseButton detours at RVA 0x7EB3F and
    //  returns at 0x7EB4F -- the close button's position alone.
    D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference + 277, gnBorderYDifference + gnScreenHeight - 61, 0xFFFFFFFF, 5, nullptr);
}


//1.10f: D2Client.0x6FB1EB70
//  The scroll's own artwork, and the Cairn Stones pentagon. The loop runs five times over gwCairnStoneOrder (the order the server sent in
//  packet 0x50, quest 4) and each value indexes two parallel D2CoordStrc[5] tables: sgRunePos is the pentagon (242,104) (255,222) (148,310)
//  (47,222) (75,104), and sgRuneOverlayPos places an animated cel from gIniScrollImage3 over each rune. The client only looks the position up.
//  The bounds test and the assert disagree by one: `cmp si, 6 / jnb` skips the stone entirely and `cmp si, 5 / jnb` asserts, so the guard
//  admits 0..5 and the assert fires on exactly 5. Both are the original's and both are reproduced.
//  Frame ladder: each stone starts twelve frames (600 ms at 50 ms a frame) after the one before, from gnQuestScrollFrameThresholds =
//  {0, 12, 24, 36, 48}. The per-stone frame then picks the draw mode: 1 plays sound 2671 and draws mode 0, under 5 is mode 0, under 10 mode 1,
//  under 15 mode 2, and from 15 on mode 5. The overlay's frame is `(frame >= 21 ? 0 : frame)`, written in the bytes as
//  `setnl cl / dec ecx / and ecx, ebx`.
//  The threshold compare is unsigned (`cmp ebx, eax / jbe` at 0x6FB1ECC1) while the frame counter is a plain int, so the threshold is held in
//  an unsigned int and C's usual conversions emit the same compare.
//  DIFFERENCE: the path buffer is one 260-byte (MAX_PATH) buffer. The original clears it as `rep stosd` (256 bytes) plus `stosw` plus `stosb`
//  and assigns the first byte on its own (1 + 256 + 2 + 1 bytes); ours clears the same bytes in one statement rather than three. The first
//  byte comes from gszEmptyString, a one-byte global holding the NUL.
void __fastcall TEXT_DrawQuestScrollFrame(int32_t bAnimate)
{
    if (!CUNIT_GetPlayer())
        return;

    D2GfxDataStrc tData;
    char szPath[260];

    memset(&tData, 0, sizeof(tData));
    TEXT_IniScrollDrawPanel();

    if (!gIniScrollImage2)
    {
        szPath[0] = gszEmptyString;
        memset(&szPath[1], 0, sizeof(szPath) - 1);
        sprintf(szPath, szFmtPathJoin, aDataGlobal, szUiMenuScroin2);
        gIniScrollImage2 = ARCHIVE_CellFileLoad(szPath, 0);
    }
    if (!gIniScrollImage3)
    {
        szPath[0] = gszEmptyString;
        memset(&szPath[1], 0, sizeof(szPath) - 1);
        sprintf(szPath, szFmtPathJoin, aDataGlobal, szUiMenuScroin3);
        gIniScrollImage3 = ARCHIVE_CellFileLoad(szPath, 0);
    }

    if (!bAnimate)
        return;

    const enum Font ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTFORMAL12);
    tData.pCellFile = gIniScrollImage2;

    if (sgTextState.dwQuestScrollLastTick)
    {
        if (sgTextState.dwQuestScrollLastTick + 50 < GetTickCount())
        {
            sgTextState.dwQuestScrollLastTick = GetTickCount();
            ++sgTextState.nQuestScrollFrame;
        }
    }
    else
    {
        sgTextState.dwQuestScrollLastTick = GetTickCount();
        sgTextState.nQuestScrollFrame = 0;
    }

    for (int32_t nStone = 0; nStone < 5; ++nStone)
    {
        //  unsigned, as `jbe` at 0x6FB1ECC1 has it
        const uint32_t nThreshold = (uint32_t)gnQuestScrollFrameThresholds[nStone];
        if ((uint32_t)sgTextState.nQuestScrollFrame <= nThreshold)
            continue;

        const int32_t nStoneFrame = (int32_t)((uint32_t)sgTextState.nQuestScrollFrame - nThreshold);
        tData.nFrame = nStone;

        int32_t eDrawMode;
        if (nStoneFrame == 1)
        {
            SOUNDINST_PlaySound(SOUND_SHRINE_PORTAL, nullptr, 0, 0, 0);
            eDrawMode = 0;
        }
        else if (nStoneFrame < 5)
            eDrawMode = 0;
        else if (nStoneFrame < 10)
            eDrawMode = 1;
        else if (nStoneFrame < 15)
            eDrawMode = 2;
        else
            eDrawMode = 5;

        const uint16_t wStoneIndex = gwCairnStoneOrder[nStone];
        if (wStoneIndex >= 6)
            continue;
        if (wStoneIndex >= 5)
        {
            FOG_DisplayAssert(szAssertRunePos, szTextCpp, 5703);
            exit(-1);
        }

        D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference + sgRunePos[wStoneIndex].nX, sgRunePos[wStoneIndex].nY - gnBorderYDifference, 0xFFFFFFFF, eDrawMode, nullptr);

        const int32_t nOverlayY = sgRuneOverlayPos[wStoneIndex].nY;
        D2GfxDataStrc tOverlayData;
        memset(&tOverlayData, 0, sizeof(tOverlayData));
        tOverlayData.pCellFile = gIniScrollImage3;
        const int32_t nOverlayX = sgRuneOverlayPos[wStoneIndex].nX;
        tOverlayData.nFrame = (nStoneFrame >= 21) ? 0 : nStoneFrame;
        D2GFX_TEXTURE_CelDraw(&tOverlayData, gnBorderXDifference + nOverlayX, nOverlayY - gnBorderYDifference, 0xFFFFFFFF, 3, nullptr);
    }

    D2WIN_10127_SetFont(ePrevFont);
}


//1.10f: D2Client.0x6FB1EDF0
//  The server's Cairn Stone order, off the quest-state packet; QUESTS_DispatchQuestPacket tail-jumps here.
//  gwCairnStoneOrder is uint16[6] and the copy is three dword moves, so all twelve bytes are one object. The third dword is loaded, then
//  gbQuestScrollOpen is cleared, then the dword is stored: three distinct objects, so the order is the compiler's scheduling, written as the
//  bytes have it.
void __fastcall TEXT_QuestOnCairnStoneOrderCmd(uint8_t *ptCmd)
{
    if (*(int16_t *)(ptCmd + 1) != 4)
        return;

    const uint8_t *pOrder = ptCmd + 3;
    //  DIFFERENCE: memcpy, because the array is ours, of 16-bit words, and a store through a `DWORD *` into it is not defined C++; each memcpy
    //  is the same four-byte move.
    memcpy(&gwCairnStoneOrder[0], pOrder, sizeof(uint32_t));
    memcpy(&gwCairnStoneOrder[2], pOrder + 4, sizeof(uint32_t));
    const uint32_t dwLastPair = *(uint32_t *)(pOrder + 8);
    gbQuestScrollOpen = 0;
    memcpy(&gwCairnStoneOrder[4], &dwLastPair, sizeof(uint32_t));
}


//1.10f: D2Client.0x6FB1EE20
//  Opens the quest-scroll panel on a scroll the player used. The second argument (edx) is the scroll's items.txt record, not a unit; the dword
//  it compares at +128 is D2ItemsTxt.dwCode, and the state member pOpenScrollItemsTxt holds that record too.
//  The branch is the scroll's item code: 'bkd ', the deciphered Bark Scroll, sets gbQuestScrollOpen and sends client command 62 (0x3E) with the
//  target item's id, or -1 when there is no target. Anything else, the undeciphered 'bks ', clears the flag and raises field_30 instead, and
//  the panel opens read-only.
//  void: eax at the exit is MSG_SendServerItemMessage's answer on the acting path; on the other path eax is never touched after
//  `mov eax, sgTextState.pOpenScrollItemsTxt` (0x6FB1EE3F), so the scroll pointer is left there, non-zero exactly when a scroll is open.
//  Neither caller reads it.
void __fastcall TEXT_IniScrollOpen(D2UnitStrc *pTargetItem, D2ItemsTxt *pItemsTxt)
{
    sgTextState.pOpenScrollItemsTxt = pItemsTxt;
    UI_SetUIToggle(UI_INISCROLL, UI_TOGGLE, 0);
    GAME_ResetClickState();

    if (sgTextState.pOpenScrollItemsTxt->dwCode == 0x20646B62)  //  'bkd '
    {
        gbQuestScrollOpen = 1;
        const int32_t nUnitId = pTargetItem ? (int32_t)pTargetItem->dwUnitId : -1;
        MSG_SendServerItemMessage(62, nUnitId);
        sgTextState.field_30 = 0;
    }
    else
    {
        gbQuestScrollOpen = 0;
        sgTextState.field_30 = 1;
    }
}


//1.10f: D2Client.0x6FB1EFB0
//  Builds the message-log dialog and fills it from the log list. It opens once: pMessageLogDialog is the guard, and a second call while the
//  dialog exists does nothing. TEXT_CloseMessageLog is the mirror and clears it.
//  No arguments: its sole caller reaches it by tail jump (`jmp TEXT_OpenMessageLog` at 0x6FB22976, out of UI_RegisterPanelHandlers' UI_MSGLOG
//  arm), so ecx still holds that function's own nUiId; the 24 it appears to pass is UI_MSGLOG, read by nobody.
//  The list box is a `new`: SMemAlloc(12, "new", -1, 0), a null test, then the constructor on the block.
//  DIFFERENCE: the compiler's exception frame around the `new` (its unwind funclet frees the block) is not reproduced; the allocation is
//  written as allocate, test, construct.
void __cdecl TEXT_OpenMessageLog()
{
    if (sgTextState.pMessageLogDialog)
        return;

    sgTextState.pMessageLogDialog = DIALOG_Create(5, 30, (DIALOGCALLBACK)TEXT_OnMessageLogClose, nullptr, 0, 0, 0, gnScreenWidth - 20, gnScreenHeight - 130, 0, 0);
    DIALOG_SetHighlight((D2ClientDialogStrc *)sgTextState.pMessageLogDialog, 1);
    DIALOG_AddLine((D2ClientDialogStrc *)sgTextState.pMessageLogDialog, D2LANG_GetStringFromTblIndex(STR_IDX_4035_strMsgLog), 21, 0, 4, 1, nullptr, 0);

    //  computed before the string lookup, as the bytes have it
    const int32_t nCloseLineY = gnScreenHeight - 165;
    DIALOG_AddLine((D2ClientDialogStrc *)sgTextState.pMessageLogDialog, D2LANG_GetStringFromTblIndex(STR_IDX_4144_strClose), nCloseLineY, 0, 0, 1, (DIALOGCALLBACK)TEXT_OnMessageLogClose, 1);

    void *pBlock = STORM_SMemAlloc(12, szNew, -1, 0);
    void *pListBox = pBlock ? LISTBOX_Construct(pBlock, 0, 5, 65, gnScreenWidth - 20, gnScreenHeight - 200, 2, 0, 13, nullptr, 1)
                         : nullptr;
    sgTextState.pMessageLogListBox = pListBox;
    if (!pListBox)
        return;

    DIALOG_AddControl((D2ClientDialogStrc *)sgTextState.pMessageLogDialog, pListBox, 25, 30);

    for (D2ClientTextNodeStrc *pEntry = sgTextState.MessageLogDisplayList.pHead; pEntry; pEntry = pEntry->pNext)
    {
        (*(void(__thiscall **)(void *, Unicode *, Unicode *, Unicode *, Unicode *, Unicode *, Unicode *, uint32_t, int32_t, uint32_t, int32_t)) (*(uint32_t *)sgTextState.pMessageLogListBox + 48))(sgTextState.pMessageLogListBox,
            pEntry->pLines[0], pEntry->pLines[1], pEntry->pLines[2], pEntry->pLines[3], pEntry->pLines[4], pEntry->pLines[5], 0, pEntry->nColor, 0, 13);
    }

    DIALOG_SetSelectedLine((D2ClientDialogStrc *)sgTextState.pMessageLogDialog, -1);
}


//1.10f: D2Client.0x6FB1F140
//  The message log's close handler, registered twice by TEXT_OpenMessageLog: once as the dialog's own callback and once as the `strClose`
//  line's pfnOnClick. Nothing calls it directly; the dialog dispatches it. Returning 1 tells the dialog the event was consumed.
int32_t __cdecl TEXT_OnMessageLogClose()
{
    UI_SetUIToggle(UI_MSGLOG, UI_TURNOFF, 0);
    return 1;
}


//1.10f: D2Client.0x6FB1F160
//  The mirror of TEXT_OpenMessageLog. Reached only by the `jmp` at 0x6FB23107 out of UI_UnregisterPanelHandlers' UI_MSGLOG arm.
//  The second call is the list box's scalar deleting destructor: vtable slot 0 with the deleting flag set (`mov eax, [ecx] / push 1 /
//  call dword ptr [eax]`).
void __cdecl TEXT_CloseMessageLog()
{
    if (sgTextState.pMessageLogDialog)
    {
        DIALOG_Close((D2ClientDialogStrc *)sgTextState.pMessageLogDialog);
        sgTextState.pMessageLogDialog = nullptr;
    }
    if (sgTextState.pMessageLogListBox)
    {
        void *pListBox = sgTextState.pMessageLogListBox;
        (**(void(__thiscall ***)(void *, int32_t))pListBox)(pListBox, 1);
        sgTextState.pMessageLogListBox = nullptr;
    }
}


//1.10f: D2Client.0x6FB1F1C0
//  The on-screen subtitle for a voice line, and only for the double-byte localisations. Two gates, both narrow: the language must be JAPANESE,
//  KOREAN or CHINESETWN (`cmp eax, 6 / jl`, `cmp eax, 7 / jle`, `cmp eax, 9 / jnz`, so 6, 7 and 9 pass and CHINESESIN (8) does not), and the
//  sound id must be inside the voice block [2934, 4656] (signed compares).
//  Two tables, one stride: a player's subtitle comes from a per-class table (gpSpeechSubtitleTables, seven pointers indexed by dwClassId, each
//  up to 0x4A records) and a monster's from one six-record table. Both are { uint16 nSoundId; uint16 nStringId } walked at a four-byte stride,
//  searched linearly with an unsigned bound (`cmp eax, 4Ah / jb`, `cmp eax, 6 / jb`).
//  The player format carries two 0xFF lead bytes (`push -1 / push -1 / push offset "%c%c%d"`), which is how the double-byte builds mark a
//  subtitle line; a char of -1 is what %c is given.
//  An empty result clears rather than sets: the formatted string is measured with an inline strlen; zero length frees any existing hover text
//  and stores NULL, otherwise a fresh D2HoverTextStrc replaces it. Both arms free the old one first, so the field is never leaked.
void __fastcall TEXT_ChatShowSpeechSubtitle(D2UnitStrc *pUnit, D2C_Sounds nSoundId)
{
    if (!pUnit)
        return;

    const D2C_Language eLanguage = D2LANG_STRTABLE_GetLanguage();
    if ((int32_t)eLanguage < LANGUAGE_JAPANESE)
        return;
    if ((int32_t)eLanguage > LANGUAGE_KOREAN && eLanguage != LANGUAGE_CHINESETWN)
        return;
    //  The id is cast to int at each use: the bytes compare it signed (`cmp edi, 0B76h / jl`, `cmp edi, 1230h / jg`) and D2C_Sounds' base is
    //  uint32_t.
    if ((int32_t)nSoundId < 2934 || (int32_t)nSoundId > 4656)
        return;

    char szSubtitle[12];

    if (!pUnit->dwUnitType)
    {
        const D2ClientSpeechSubtitleStrc *pTable = gpSpeechSubtitleTables[pUnit->dwClassId];
        uint32_t nIndex = 0;
        while (pTable[nIndex].nSoundId != (int32_t)nSoundId)
        {
            if (++nIndex >= 0x4A)
                return;
        }
        sprintf(szSubtitle, szFmtSubtitlePlayer, -1, -1, pTable[nIndex].nStringId);
    }
    else
    {
        if (pUnit->dwUnitType != UNIT_MONSTER)
            return;
        uint32_t nIndex = 0;
        while (gMonsterSpeechSubtitles[nIndex].nSoundId != (int32_t)nSoundId)
        {
            if (++nIndex >= 6)
                return;
        }
        sprintf(szSubtitle, szFmtSubtitleMonster, gMonsterSpeechSubtitles[nIndex].nStringId);
    }

    //  read a second time, after the formatting -- the original does not reuse the value it gated on
    const uint8_t nLangId = (uint8_t)D2LANG_STRTABLE_GetLanguage();

    if (strlen(szSubtitle))
    {
        D2HoverTextStrc *pHoverMsg = D2COMMON_CHAT_AllocHoverMsg(nullptr, szSubtitle, sgTextState.nStaticInfoTimeout);
        D2COMMON_CHAT_SetLangIdInHoverMsg(pHoverMsg, nLangId);
        if (pUnit->pHoverText)
            D2COMMON_CHAT_FreeHoverMsg(nullptr, pUnit->pHoverText);
        pUnit->pHoverText = pHoverMsg;
    }
    else
    {
        if (pUnit->pHoverText)
            D2COMMON_CHAT_FreeHoverMsg(nullptr, pUnit->pHoverText);
        pUnit->pHoverText = nullptr;
    }
}


static_assert(sizeof(D2C_Sounds) == 4 && SOUND_NONE == 0 && SOUND_CURSOR_PASS == 1 && SOUND_CURSOR_SELECT == 2
                  && SOUND_CURSOR_ERROR == 3 && SOUND_CURSOR_BUTTON_CLICK == 4 && SOUND_CURSOR_POINT_DROP == 5
                  && SOUND_CURSOR_SWITCH == 6 && SOUND_CURSOR_LEVEL_UP == 7 && SOUND_CURSOR_QUESTDONE == 14
                  && SOUND_CURSOR_REPAIR_ITEM == 15 && SOUND_CURSOR_HOSTILE == 16
                  && SOUND_QUEST_HELLFORGE_SMASH == 35 && SOUND_EVENT_THUNDER_1 == 202
                  && SOUND_ITEM_FLIPPY == 216 && SOUND_ITEM_GEM == 217 && SOUND_ITEM_GOLD == 221
                  && SOUND_ITEM_PICKUP == 235 && SOUND_ITEM_POTION == 237 && SOUND_ITEM_RING == 241
                  && SOUND_ITEM_SCROLL == 243 && SOUND_WEAPON_BOW_DRAW_1 == 314
                  && SOUND_IMPACT_STEAL_LIFE == 396 && SOUND_IMPACT_STEAL_MANA == 397
                  && SOUND_ANDARIEL_QUAKE_LOOP == 452 && SOUND_DRUIDPOD_DEATH_1 == 790
                  && SOUND_MINION_DEATH_A1 == 1308 && SOUND_MINION_DEATH_B1 == 1311
                  && SOUND_MINION_DEATH_C1 == 1314 && SOUND_MINION_DEATH_D1 == 1317
                  && SOUND_SPIDER_WEB_1 == 1830 && SOUND_PLAYER_TOWNPORTAL_ENTER == 2231
                  && SOUND_SORCERESS_FIREBALL_IMPACT_1 == 2419 && SOUND_NECROMANCER_CONFUSE == 2456
                  && SOUND_NECROMANCER_CORPSEEXP_1 == 2458 && SOUND_NECROMANCER_REVERSEVAMPIRE == 2474
                  && SOUND_BARBARIAN_GRUNT_SMALL_1 == 2505 && SOUND_BARBARIAN_LEAP_LAND == 2517
                  && SOUND_BARBARIAN_WHIRLWIND == 2531 && SOUND_SHRINE_PORTAL == 2671
                  && SOUND_GUARD_HALT == 3983 && SOUND_NIHLATHAK_HURRYUP == 4560
                  && SOUND_WUSSIE_CHEER_1 == 4603 && SOUND_WUSSIE_HELP_ME == 4607
                  && SOUND_MONSTER_DIABLO_TAUNT_1 == 4638 && SOUND_MONSTER_DIABLO_TAUNT_EX == 4640
                  && SOUND_MUSIC_QUEST_COMPELLING == 4687 && SOUND_MUSIC_QUEST_FORGE == 4689
                  && SOUND_MUSIC_QUEST_HORADRIC == 4690 && SOUND_MUSIC_QUEST_IZUAL == 4691,
              "the 49 Sounds.txt ids the C names in place of the original's literals (TODO.md T136"
              " part 3): each member the value its call sites' bytes push -- the 144 replacements are in"
              " SWEEP_LOG_16.md, the T136 part 3 entry. Every file that names one leans on this");


//1.10f: D2Client.0x6FB1F320
//  An OFF then an ON, not a toggle: the message log is rebuilt from its list rather than merely hidden. Guarded on the dialog existing; on
//  the refusal path the dialog pointer is what comes back in eax.
void __cdecl TEXT_MsgLogReopen()
{
    if (!sgTextState.pMessageLogDialog)
        return;

    UI_SetUIToggle(UI_MSGLOG, UI_TURNOFF, 0);
    UI_SetUIToggle(UI_MSGLOG, UI_TURNON, 0);
}


//1.10f: D2Client.0x6FB1D3C0
//  Server packet 0x5A -- every system line the game prints.
//  The outer switch is `cmp eax, 12h / ja default`, so events 0..18 have arms and everything above falls through to the epilogue. Events 12
//  and 14 have no arm either: their jump-table entries point at the epilogue, so INV_TradeRestoreHeldItem's event 14 does nothing.
//  Seven arms check their tbl string against 260 before formatting, because the player's name is pasted in afterwards: 260 is
//  MAX_GLOBAL_MESSAGE_LEN - MAX_PLR_NAME_LEN - 30 = 306 - 16 - 30. The slain arm uses the same budget differently:
//  strlen(name) + strlen(line) + 16 >= 306 traces and returns instead of asserting.
//  The slain arm names its killer three ways, by nParam: 0 is a player name out of the packet's second slot, 1 is a monster (a monstats id at
//  +3, bounds-checked) or a super unique (its id in the second slot's first word), 2 is an object, and an object class of 11 is BarrelEx,
//  which prints 'an Exploding Barrel'. Anything that fails a lookup falls to 'was slain.' with no killer.
//  In this function and in TEXT_OnMessageListPacket and TEXT_HandleChatMessage a colour is a byte written into a dword slot and passed where
//  the callee takes an int (`mov dl, [ebp+2]`); that is the ABI for a char argument, and the byte is what is passed.
void __fastcall TEXT_HandleSystemMessage(D2GSPacketSrv5A *pPacket)
{
    Unicode wszText[306];
    Unicode src[16];
    Unicode wszName2Ex[16];

    char *pszName1 = pPacket->szText;
    if (strlen(pPacket->szText) >= 15)
        pPacket->szText[15] = 0;

    //  0 means "nothing to add"; the original reaches these two tails by jump, and they are the only shared code in the switch.
    D2C_StringIndices nAppendStrId = (D2C_StringIndices)0;
    D2C_StringIndices nReplaceStrId = (D2C_StringIndices)0;

    switch (pPacket->nType)
    {
    case 0:
        if ((int32_t)Unicode::strlen(D2LANG_GetStringFromTblIndex(STR_IDX_3639_sysmsg1)) >= 260)
        {
            FOG_DisplayAssert(szAssertLenDropped1, szTextCpp, 4410);
            exit(-1);
        }
        Unicode::win2Unicode(src, pPacket->szText, 16);
        Unicode::strcpy(wszText, src);
        nAppendStrId = STR_IDX_3639_sysmsg1;
        break;

    case 1:
        if ((int32_t)Unicode::strlen(D2LANG_GetStringFromTblIndex(STR_IDX_3640_sysmsg2)) >= 260)
        {
            FOG_DisplayAssert(szAssertLenDropped2, szTextCpp, 4416);
            exit(-1);
        }
        Unicode::win2Unicode(src, pPacket->szText, 16);
        Unicode::strcpy(wszText, src);
        nAppendStrId = STR_IDX_3640_sysmsg2;
        break;

    case 2:
        //  a client does not announce its own arrival
        if (CUNIT_GetPlayer() && !strcmp(pPacket->szText, ROSTER_GetPlayerDisplayName(CUNIT_GetPlayer())))
        {
            break;
        }
        if ((int32_t)Unicode::strlen(D2LANG_GetStringFromTblIndex(STR_IDX_3641_sysmsg3)) >= 260)
        {
            FOG_DisplayAssert(szAssertLenJoined, szTextCpp, 4426);
            exit(-1);
        }
        Unicode::win2Unicode(src, pszName1, 16);
        if (strlen(&pPacket->szText[16]))
        {
            //  constructed here, as the original does, not at the top
            Unicode wszName2[16];
            Unicode::win2Unicode(wszName2, &pPacket->szText[16], 16);
            Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3643_sysmsg3a), src, wszName2);
        }
        else
        {
            Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3641_sysmsg3), src);
        }
        break;

    case 3:
        if ((int32_t)Unicode::strlen(D2LANG_GetStringFromTblIndex(STR_IDX_3642_sysmsg4)) >= 260)
        {
            FOG_DisplayAssert(szAssertLenQuit, szTextCpp, 4440);
            exit(-1);
        }
        Unicode::win2Unicode(src, pszName1, 16);
        if (strlen(&pPacket->szText[16]))
        {
            Unicode wszName2[16];
            Unicode::win2Unicode(wszName2, &pPacket->szText[16], 16);
            Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3644_sysmsg4a), src, wszName2);
        }
        else
        {
            Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3642_sysmsg4), src);
        }
        break;

    case 4:
        if ((int32_t)Unicode::strlen(D2LANG_GetStringFromTblIndex(STR_IDX_3645_sysmsg5)) >= 260)
        {
            FOG_DisplayAssert(szAssertLenNotInGame, szTextCpp, 4454);
            exit(-1);
        }
        Unicode::win2Unicode(src, pPacket->szText, 16);
        Unicode::strcpy(wszText, src);
        nAppendStrId = STR_IDX_3645_sysmsg5;
        break;

    case 5:
        if ((int32_t)Unicode::strlen(D2LANG_GetStringFromTblIndex(STR_IDX_3646_sysmsg6)) >= 260)
        {
            FOG_DisplayAssert(szAssertLenNotInBattlenet, szTextCpp, 4466);
            exit(-1);
        }
        Unicode::win2Unicode(src, pPacket->szText, 16);
        Unicode::strcpy(wszText, src);
        nAppendStrId = STR_IDX_3646_sysmsg6;
        break;

    case 6:
    {
        Unicode::win2Unicode(src, pPacket->szText, 16);

        D2C_StringIndices nSlainStrId = (D2C_StringIndices)0;
        const Unicode *pwszKiller = nullptr;

        if (!pPacket->nParam)
        {
            //  a player killed them: the name is the packet's second slot
            if (strlen(&pPacket->szText[16]))
            {
                Unicode::win2Unicode(wszName2Ex, &pPacket->szText[16], 16);
                nSlainStrId = STR_IDX_3648_sysmsg8;
                pwszKiller = wszName2Ex;
            }
        }
        else if (pPacket->nParam == 1)
        {
            if (pPacket->szText[16])
            {
                //  a super unique: its id is the first word of slot two
                D2SuperUniquesTxt *pSuperUnique = D2COMMON_DATATBLS_GetSuperUniquesTxtRecord(*(uint16_t *)&pPacket->szText[16]);
                if (pSuperUnique)
                {
                    nSlainStrId = STR_IDX_3648_sysmsg8;
                    pwszKiller = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pSuperUnique->wNameStr);
                }
            }
            else
            {
                //  an ordinary monster: a monstats id at +3, bounds-checked
                const int32_t nKillerMonId = (int32_t)pPacket->dwParam;
                if (nKillerMonId >= 0 && nKillerMonId < sgptDataTables->nMonStatsTxtRecordCount)
                {
                    D2MonStatsTxt *pKiller = &sgptDataTables->pMonStatsTxt[nKillerMonId];
                    if (pKiller)
                    {
                        nSlainStrId = STR_IDX_3647_sysmsg7;
                        pwszKiller = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pKiller->wNameStr);
                    }
                }
            }
        }
        else if (pPacket->nParam == 2)
        {
            D2ObjectsTxt *pObject = D2COMMON_DATATBLS_GetObjectsTxtRecord((int32_t)pPacket->dwParam);
            if (pObject)
            {
                nSlainStrId = STR_IDX_3647_sysmsg7;
                //  class 11 is BarrelEx, 'an Exploding Barrel'
                pwszKiller = (pPacket->dwParam == 11) ? D2LANG_GetStringFromTblIndex(STR_IDX_3224_BarrelEx)
                                 : pObject->wszName;
            }
        }

        if (!pwszKiller)
        {
            //  no killer could be named: 'was slain.'
            Unicode::strcpy(wszText, src);
            nAppendStrId = STR_IDX_3649_sysmsg9;
            break;
        }

        //  the same 306-character budget, spent as a sum rather than a cap
        const int32_t nLineLen = (int32_t)Unicode::strlen(D2LANG_GetStringFromTblIndex(nSlainStrId));
        if ((int32_t)Unicode::strlen(pwszKiller) + nLineLen + 16 >= 306)
        {
            const int32_t nKillerNameLen = (int32_t)Unicode::strlen(pwszKiller);
            const int32_t nSlainCheckLen = (int32_t)Unicode::strlen(D2LANG_GetStringFromTblIndex(nSlainStrId));
            const int32_t nNameLen = (int32_t)Unicode::strlen(src);
            FOG_Trace(szTraceSlainLens, (uint16_t)nSlainStrId, nNameLen, nSlainCheckLen, nKillerNameLen);
            return;
        }
        Unicode::strcpy(wszText, src);
        Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(nSlainStrId));
        Unicode::strcat(wszText, pwszKiller);
        break;
    }

    case 7:
    {
        //  the party events, and the only arm that goes to the quick list
        D2RosterUnitStrc *pEntry = ROSTER_FindEntryByUnitOrCorpse(pPacket->dwParam);
        Unicode wszName2[16];
        char szRosterName[16];
        D2UnitStrc *pPlayer = CUNIT_GetPlayer();

        if (pEntry)
        {
            strcpy(szRosterName, ROSTER_GetDisplayName(pEntry));
        }
        else
        {
            //  a byte-at-a-time copy out of the one-byte empty string; it copies exactly the NUL
            int32_t nCopyIdx = 0;
            char chCopy;
            do
            {
                chCopy = (&gszEmptyString)[nCopyIdx];
                szRosterName[nCopyIdx++] = chCopy;
            } while (chCopy);
        }
        Unicode::win2Unicode(wszName2, szRosterName, 16);

        switch (pPacket->nParam)
        {
        case 1:
            Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_10106_Party1X), wszName2);
            break;
        case 2:
            Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_10107_Party2X), wszName2);
            break;
        case 3:
            //  hostility declared -- the only party event with a sound
            Unicode::strcpy(wszText, wszName2);
            Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4026_Party3));
            SOUNDINST_PlaySound(SOUND_CURSOR_HOSTILE, nullptr, 0, 0, 0);
            TEXT_AddQuickMessage(wszText, pPacket->nColor);
            return;
        case 4:
            Unicode::strcpy(wszText, wszName2);
            Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4027_Party4));
            TEXT_AddQuickMessage(wszText, pPacket->nColor);
            return;
        case 5:
            Unicode::strcpy(wszText, wszName2);
            Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4028_Party5));
            break;
        case 6:
            Unicode::strcpy(wszText, wszName2);
            Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4029_Party6));
            break;
        case 7:
            Unicode::strcpy(wszText, wszName2);
            Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4030_Party7));
            break;
        case 8:
            //  the one that puts the string first and the name after
            Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4031_Party8));
            Unicode::strcat(wszText, wszName2);
            break;
        case 9:
            Unicode::strcpy(wszText, wszName2);
            Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4032_Party9));
            break;
        case 10:
            Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_10108_Party10), wszName2);
            break;
        case 11:
            Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_10109_Party11), wszName2);
            break;
        default:
            //  eleven 1-based events; anything else leaves wszText empty and still prints, which is the original's behaviour
            break;
        }

        //  ROSTERINFOFLAG_SQUELCH, the original's 4: the line is dropped when the local player has squelched the player it names
        if (!pPlayer || !PLAYERLIST_RosterTestInfoFlag(pPlayer->dwUnitId, ROSTER_GetEntryUnitId(pEntry), ROSTERINFOFLAG_SQUELCH))
        {
            TEXT_AddQuickMessage(wszText, pPacket->nColor);
        }
        return;
    }

    case 8:
        Unicode::win2Unicode(src, pPacket->szText, 16);
        Unicode::strcpy(wszText, src);
        nAppendStrId = STR_IDX_4044_strTradeBusy;
        break;

    case 9:
        nReplaceStrId = STR_IDX_4047_strTradeTimeout;
        break;

    case 10:
        //  a name present means their box, absent means yours
        if (*pszName1)
        {
            Unicode::win2Unicode(src, pPacket->szText, 16);
            Unicode::strcpy(wszText, src);
            nAppendStrId = STR_IDX_4134_strTradeOtherBox;
        }
        else
        {
            nReplaceStrId = STR_IDX_4135_strTradeBox;
        }
        break;

    case 11:
        Unicode::win2Unicode(src, pPacket->szText, 16);
        Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_1691_strGuildMasterKicked), src);
        break;

    case 13:
        if ((int32_t)Unicode::strlen(D2LANG_GetStringFromTblIndex(STR_IDX_3652_sysmsg12)) >= 260)
        {
            FOG_DisplayAssert(szAssertLenNotListening, szTextCpp, 4460);
            exit(-1);
        }
        Unicode::win2Unicode(src, pPacket->szText, 16);
        Unicode::strcpy(wszText, src);
        nAppendStrId = STR_IDX_3652_sysmsg12;
        break;

    case 15:
        Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3651_sysmsg11), pPacket->dwParam);
        break;

    case 16:
        nReplaceStrId = STR_IDX_10808_strHostileTimeout;
        break;

    case 17:
        Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_11004_SOJSold), pPacket->dwParam);
        break;

    case 18:
        //  Diablo Walks the Earth: the announcement also darkens the sky and shakes the screen for four seconds
        Unicode::sprintf(306, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_11005_DiabloWalksEarth));
        if (gpAct)
        {
            D2COMMON_ENVIRONMENT_InitializeEnvironment(gpAct, CUNIT_GetPlayerRoom(), 5, 0, 1);
        }
        VIEWEFFECTS_StartScreenShake(6, 4000, 10000, 4000);
        SOUNDINST_PlaySound(SOUND_MONSTER_DIABLO_TAUNT_EX, nullptr, 0, 0, 0);
        break;

    default:
        //  12 and 14 land here too -- their jump-table entries point at the epilogue, so INV_TradeRestoreHeldItem's event 14 does nothing
        return;
    }

    if (nReplaceStrId)
        Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(nReplaceStrId));
    else if (nAppendStrId)
        Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(nAppendStrId));

    TEXT_PrintChatMessage(wszText, pPacket->nColor);
}


//1.10f: D2Client.0x6FB1DE10
//  Server packet 0x27 -- the lines an NPC has available. Three unit types, three jobs: a MONSTER (type 1) with a single menu-3 line gets a hover
//  bubble; anything else from a monster rebuilds the client's message list, which is what the talk menu walks. An OBJECT (type 2) with a
//  menu-3 line gets the scrolling popup, and otherwise hands the string to the quest-scroll text. Any other type frees the list and clears it.
//  The display time is proportional to the length: nTimeoutBase = 25 * (strlen(text) + 25) and gnStaticInfoDeadline = GetTickCount() +
//  8 * nTimeoutBase, which is 200 * (length + 25) milliseconds: five seconds for an empty string and a fifth of a second per character after
//  that. The 25 inside the parentheses is a minimum length, not a scale.
//  The popup is freed by hand: the original inlines TEXT_FreeParsedLines here, and the header free after it is a plain FOG_Free.
void __fastcall TEXT_OnMessageListPacket(D2GSPacketSrv27 *pPacket)
{
    if (pPacket->nUnitType == 1)
    {
        D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, UNIT_MONSTER);
        if (pUnit && pPacket->pMessageList.nCount == 1 && pPacket->pMessageList.pMessages[0].nMenu == 3)
        {
            char szNumber[20];

            if (pUnit->pHoverText)
                D2COMMON_CHAT_FreeHoverMsg(nullptr, pUnit->pHoverText);
            sprintf(szNumber, szFmtSubtitleMonster, pPacket->pMessageList.pMessages[0].nStringId);
            pUnit->pHoverText = D2COMMON_CHAT_AllocHoverMsg(nullptr, szNumber, sgTextState.nStaticInfoTimeout);
            return;
        }

        if (gpMessageListHead)
            D2COMMON_TEXT_FreeTextHeader(gpMessageListHead);
        gpMessageListHead = D2COMMON_TEXT_AllocTextHeader(nullptr);
        D2COMMON_TEXT_CreateTextHeaderFromMessageList(gpMessageListHead, &pPacket->pMessageList);
        D2COMMON_TEXT_SortTextNodeListByStringId(gpMessageListHead);
        return;
    }

    if (pPacket->nUnitType != 2)
    {
        //  note the asymmetry: this arm frees without testing, where the monster arm tests first. Both are the original's.
        D2COMMON_TEXT_FreeTextHeader(gpMessageListHead);
        gpMessageListHead = NULL;
        return;
    }

    D2UnitStrc *pObject = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, UNIT_OBJECT);

    if (pPacket->pMessageList.pMessages[0].nMenu != 3)
    {
        sgTextState.field_84 = 0;
        sgTextState.dwPopupObjectGUID = pPacket->dwUnitGUID;
        if (pObject)
            sgTextState.field_84 = (uint16_t)pObject->dwClassId;
        TEXT_ShowQuestScrollingText(pPacket->pMessageList.pMessages[0].nStringId, 0, 1);
        return;
    }

    const int16_t nStringId = (int16_t)pPacket->pMessageList.pMessages[0].nStringId;

    if (sgPopup.pParsedText)
    {
        D2ClientTextLineStrc *pLine = sgPopup.pParsedText->pLines;
        D2ClientPopupStrc *pPopup = sgPopup.pParsedText;
        TEXT_FreeParsedLines(pLine);
        FOG_Free(pPopup, szTextCpp, 1674, 0);
        sgPopup.pParsedText = nullptr;
    }

    const Unicode *pwszText = D2LANG_GetStringFromTblIndex((D2C_StringIndices)nStringId);
    const enum Font ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);

    D2ClientPopupStrc *pMessage = (D2ClientPopupStrc *)FOG_Alloc(44, szTextCpp, 1625, 0);
    pMessage->pLines = (D2ClientTextLineStrc *)D2WIN_10199(pwszText, &pMessage->nLineCount, 180);
    TEXT_MeasureMessageBox(pMessage);
    sgPopup.pParsedText = pMessage;
    D2WIN_10127_SetFont(ePrevFont);

    if (!sgPopup.pParsedText)
        return;

    //  one dword store across two uint16 members, as the bytes have it
    *(uint32_t *)&sgPopup.pParsedText->field_16 = 1;

    const int32_t nTimeoutBase = 25 * ((int32_t)Unicode::strlen(pwszText) + 25);
    gnStaticInfoDeadline = GetTickCount() + 8 * nTimeoutBase;
    sgTextState.bDisplayingStaticInfo = 1;
    sgTextState.bKeepNpcInteraction = 1;
    sgTextState.nStaticInfoPanelY = 100 - (sgPopup.pParsedText->wHeight >> 2);
    sgTextState.nStaticInfoPanelX = 320 - (sgPopup.pParsedText->wWidth >> 1);
}


//1.10f: D2Client.0x6FB1E120
//  Server packet 0x26 -- every line of text the server sends.
//  The language filter is deliberate and drops silently. The double-byte languages (6, 7, 9 and 12) are converted with
//  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ...); everything else goes through Unicode::win2Unicode. Then, if the message's language is
//  not this client's, any character >= 0x80 drops the whole message without display, so a foreign-script line never arrives as mojibake.
//  Korean (7) and English-Korean (12) are excused from that test for each other. The walk stops at the first character >= 0x80 (return) or at
//  the NUL (fall through and display).
//  Message types: 1 ordinary chat, prefixed with the sender's name in colour 4; 2 whisper; 4 system message, printed before any unit lookup
//  at all; 5 the speech bubble over a unit's head; 6 whisper-failure notice; 7 a recipe scroll, shown by NEWSTATS_RecipeScrollShow; anything
//  else traces `invalid msg param`.
//  Type 4 returns before the three big construction loops, which is why it is tested twice, once at the top and once inside the switch;
//  type 1 with a unit type below 2 joins it there.
//  The length guards are a pair and disagree by one: types 1 and 2 test the name length as 0 < n < 16 and the message length as n <= 305
//  after adding one for the NUL. A name outside the window skips the prefix entirely and prints the bare line; a message over the window
//  traces `invalid msg len` and prints nothing.
void __fastcall TEXT_HandleChatMessage(D2GSPacketSrv26 *pPacket)
{
    //  a squelched player is dropped before anything is built
    if (!pPacket->nUnitType && !ROSTER_IsUnitNotSquelched(pPacket->dwUnitGUID))
        return;

    Unicode wszWide[256];

    char *szMessage = pPacket->szMessage;
    const int32_t nLang = pPacket->nLang;

    if (nLang == 7 || nLang == 12 || nLang == 6 || nLang == 9)
    {
        MultiByteToWideChar(0, 1, pPacket->szMessage, 256, (LPWSTR)wszWide, 256);
        const D2C_Language eLanguage = D2LANG_STRTABLE_GetLanguage();
        if ((int32_t)eLanguage != nLang && !(eLanguage == LANGUAGE_KOREAN && nLang == 12) && !(eLanguage == LANGUAGE_ENGLISHKOR && nLang == 7))
        {
            for (const Unicode *pwch = wszWide; pwch->ch; ++pwch)
            {
                if (pwch->ch >= 0x80)
                    return;
            }
        }
    }
    else
    {
        Unicode::win2Unicode(wszWide, pPacket->szMessage, 256);
    }

    if (pPacket->nMessageType == 4)
    {
        TEXT_PrintChatMessage(wszWide, pPacket->nMessageColor);
        return;
    }

    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);

    Unicode wszText[612];
    Unicode dest[612];
    Unicode src[16];

    switch (pPacket->nMessageType)
    {
    case 6:
        Unicode::win2Unicode(dest, pPacket->szMessage, 256);
        Unicode::win2Unicode(src, pPacket->szName, 16);
        Unicode::sprintf(612, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3658_strwhisperworked), src, dest);
        UIPANELS_TextStripColorCodes(wszText);
        TEXT_PrintChatMessage(wszText, 2);
        break;

    case 2:
    {
        const int32_t nNameLen = (int32_t)strlen(pPacket->szName);
        const uint32_t nMessageLen = (uint32_t)strlen(pPacket->szMessage) + 1;
        if (nNameLen <= 0 || nNameLen >= 16)
        {
            if ((int32_t)(nMessageLen - 1) <= 305)
            {
                Unicode::strcpy(wszText, wszWide);
                TEXT_PrintChatMessage(wszText, 2);
            }
            else
            {
                FOG_Trace(szTraceInvalidMsgLen, 4776, szTextCpp);
            }
        }
        else
        {
            Unicode::win2Unicode(src, pPacket->szName, 16);
            Unicode::strcpy(wszText, src);
            Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3654_chatmsg1));
            Unicode::strcpy(dest, wszWide);
            Unicode::strcat(wszText, dest);
            TEXT_PrintChatMessage(wszText, 2);
        }
        break;
    }

    case 1:
    {
        //  a unit type below 2 is the plain system form, which is type 4's
        if (pPacket->nUnitType < 2)
        {
            TEXT_PrintChatMessage(wszWide, pPacket->nMessageColor);
            return;
        }
        const int32_t nNameLen = (int32_t)strlen(pPacket->szName);
        const uint32_t nMessageLen = (uint32_t)strlen(pPacket->szMessage) + 1;
        if (nNameLen <= 0 || nNameLen >= 16)
        {
            if ((int32_t)(nMessageLen - 1) <= 305)
            {
                Unicode::strcpy(wszText, wszWide);
                TEXT_PrintChatMessage(wszText, pPacket->nMessageColor);
            }
            else
            {
                FOG_Trace(szTraceInvalidMsgLen, 4821, szTextCpp);
            }
        }
        else
        {
            Unicode wszLine[16];
            Unicode::win2Unicode(wszLine, pPacket->szName, 16);
            Unicode::strcpy(wszText, wszLine);
            UI_AddColorCodeToWstring((wchar_t *)&wszText->ch, 4);
            Unicode::strcpy(dest, D2LANG_GetStringFromTblIndex(STR_IDX_4048_SysmsgPlayer1));
            Unicode::strcat(dest, wszWide);
            UI_AddColorCodeToWstring((wchar_t *)&dest->ch, 0);
            Unicode::strcat(wszText, dest);
            TEXT_PrintChatMessage(wszText, pPacket->nMessageColor);
        }
        break;
    }

    case 5:
        //  the speech bubble: an empty message clears it rather than setting one, and both arms free the previous bubble first
        if (pUnit)
        {
            const uint32_t nMessageLen = (uint32_t)strlen(pPacket->szMessage) + 1;
            const uint8_t nLangId = pPacket->nLang;
            if (nMessageLen == 1)
            {
                if (pUnit->pHoverText)
                    D2COMMON_CHAT_FreeHoverMsg(nullptr, pUnit->pHoverText);
                pUnit->pHoverText = nullptr;
            }
            else
            {
                D2HoverTextStrc *pHoverMsg = D2COMMON_CHAT_AllocHoverMsg(nullptr, szMessage, sgTextState.nStaticInfoTimeout);
                D2COMMON_CHAT_SetLangIdInHoverMsg(pHoverMsg, nLangId);
                if (pUnit->pHoverText)
                    D2COMMON_CHAT_FreeHoverMsg(nullptr, pUnit->pHoverText);
                pUnit->pHoverText = pHoverMsg;
            }
        }
        break;

    case 7:
    {
        //  the recipe scroll
        //  BUG: the language walk here has no effect: its body is empty in the bytes (`add eax, 2 / cmp word ptr [eax], 0 / jnz`), so it runs to
        //  the NUL and nothing is tested or kept, where the identical-looking walk at the top of the function returns on a wide character.
        //  Reproduced as written.
        Unicode wszRecipe[256];
        MultiByteToWideChar(0, 1, pPacket->szMessage, 256, (LPWSTR)wszRecipe, 256);
        if (pPacket->nLang != (int32_t)D2LANG_STRTABLE_GetLanguage())
        {
            const Unicode *pwch = wszRecipe;
            while (pwch->ch)
                ++pwch;
        }
        NEWSTATS_RecipeScrollShow(wszRecipe, 0, pPacket->nMessageColor);
        break;
    }

    default:
        FOG_Trace(szTraceInvalidMsgParam, 4867, szTextCpp);
        break;
    }
}
