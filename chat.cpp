#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "chat.h"
#include "Select.h"

//  The IME handlers call imm32 directly (ImmGetContext and the rest, declared through windows.h). The project carries no AdditionalDependencies,
//  so the import library is named here.
#pragma comment(lib, "imm32.lib")

//  chat.cpp -- the chat input line of D2Client.dll 1.10: the IME handlers, drawing the chat box and the typed line, the history keys, the chat
//  commands, and the Battle.net message pump CHAT_Update.
//  The original source file is UI\chat.cpp (the path string at 0x6FB783B4, which CHAT_OnImeNotify and CHAT_UnregisterHandlers hand to
//  FOG_Alloc and FOG_Free). Address range 0x6FAD1710..0x6FAD36FF.
//  Every handler here is reached through one of the two tables gChatMessageHandlers and gChatImeHandlers (chat.h), never by a call.
//  gnChatLanguage is D2Lang's STRTABLE_GetLanguage(), stored once by CHAT_RegisterInputHandlers. Every IME branch tests one of five languages:
//  Japanese (6), Korean (7), the two Chinese (8 simplified, 9 traditional) and ENGLISHKOR (12), which CHAT_OnImeNotify's case 6 pairs with
//  Korean. gnChatImeMode is a different number: which IME the input line is driving (0 "Alphanumeric", 1 Hangul, 3 Japanese, 4 and 5 Chinese
//  traditional and simplified, as the writers pair them with the languages).

//  The source path, handed to FOG_Alloc and FOG_Free. In the original it is __FILE__, a single object at 0x6FB783B4.
static const char szChatCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\chat.cpp";

//  Layout checks: the Windows constants the original carries as literals, CANDIDATELIST and D2Win's split-text record at fixed offsets, and
//  one tbl string.
static_assert(GCS_RESULTSTR == 0x800 && GCS_COMPSTR == 8,
              "test ch, 8 at 0x6FAD17E5 is lParam & 0x800; test byte ptr "
              "[ecx+0Ch], 8 at 0x6FAD18AF is lParam & 8");
static_assert(IMN_OPENSTATUSWINDOW == 2 && IMN_CHANGECANDIDATE == 3
                  && IMN_CLOSECANDIDATE == 4 && IMN_OPENCANDIDATE == 5
                  && IMN_SETCONVERSIONMODE == 6 && IMN_SETSENTENCEMODE == 7
                  && IMN_SETOPENSTATUS == 8,
              "add eax, -2 / cmp eax, 6 at 0x6FAD1AA6: seven slots from 2, "
              "slot 7 among the default's");
static_assert(IME_CMODE_NATIVE == 1 && IME_CAND_CODE == 2
                  && CP_ACP == 0 && MB_PRECOMPOSED == 1,
              "test byte, 1 at 0x6FAD1B63; cmp [ecx+4], 2 at 0x6FAD1BE7; "
              "push ebx (0) / push 1 at 0x6FAD1828 / 0x6FAD1826");
static_assert(IME_CMODE_ALPHANUMERIC == 0,
              "push 0 at 0x6FAD20A4, the mode CHAT_UnregisterHandlers sets");
static_assert(offsetof(CANDIDATELIST, dwStyle) == 4
                  && offsetof(CANDIDATELIST, dwCount) == 8,
              "cmp dword ptr [ecx+4], 2 / [ecx+8], 1 at 0x6FAD1BE7 and "
              "0x6FAD1BED");
static_assert(offsetof(CANDIDATELIST, dwPageStart) == 0x10
                  && offsetof(CANDIDATELIST, dwPageSize) == 0x14
                  && offsetof(CANDIDATELIST, dwOffset) == 0x18,
              "mov ecx, [eax+14h] at 0x6FAD21AB, mov esi, [eax+10h] at "
              "0x6FAD21B8, lea edx, [edi+esi*4+18h] at 0x6FAD2207");
static_assert(IME_CHOTKEY_IME_NONIME_TOGGLE == 0x10
                  && IME_THOTKEY_IME_NONIME_TOGGLE == 0x70,
              "push 10h at 0x6FAD1FCD, push 70h at 0x6FAD1F8D");
static_assert(offsetof(D2SplittedTextStrc, wszLine) == 0
                  && offsetof(D2SplittedTextStrc, pNextLine) == 4,
              "mov ecx, [esi] at 0x6FAD244E, mov ebp, [esi+4] at 0x6FAD2460");
static_assert(STR_IDX_3404_strMenuMain15 == 0xD4C,
              "the caret: mov ecx, 0D4Ch at 0x6FAD2538 and 0x6FAD26AB");
static_assert(VK_UP == 0x26 && VK_DOWN == 0x28,
              "sub esi, 26h / sub esi, 2 at 0x6FAD278A and 0x6FAD2793");
static_assert((WC_COMPOSITECHECK | WC_DEFAULTCHAR | WC_DISCARDNS) == 0x250,
              "push 250h at 0x6FAD2AB1");
static_assert(GAMETYPE_SINGLEPLAYER == 0 && GAMETYPE_SINGLEPLAYER_UNCAPPED == 1,
              "test eax, eax / cmp eax, 1 at 0x6FAD2B3F and 0x6FAD2B48");
static_assert(offsetof(D2ChatPrefixStrc, nKind) == 4
                  && offsetof(D2ChatPrefixStrc, bTakesTargetName) == 6
                  && offsetof(D2ChatPrefixStrc, bAllowedInSinglePlayer) == 0x0A,
              "cmp word ptr [esi+4], 3 at 0x6FAD2E1D, the rows' +6 at 0x6FAD2EBA, "
              "mov eax, [esi+0Ah] at 0x6FAD2E0D");
//  The two tbl strings CHAT_Update picks between and the whisper packet's two header bytes after the id.
static_assert(STR_IDX_5332_From_s_s == 0x14D4 && STR_IDX_5333_Sent_to_s_s == 0x14D5,
              "mov ecx, 14D5h at 0x6FAD3210, mov ecx, 14D4h at 0x6FAD3217");
static_assert(offsetof(D2GSPacketClt15, nMessageType) == 1
                  && offsetof(D2GSPacketClt15, nLang) == 2,
              "the block at [esp+1Ch]: its +1 at 0x6FAD36C4, its +2 at 0x6FAD36C9");


//1.10f: D2Client.0x6FAD1790
//
//  WM_IME_COMPOSITION, the second record of gChatImeHandlers. Two flags of lParam, each an arm of its own, and the message is claimed on every
//  path at the end with a call to SMsgBreakHandlerChain (OnImeNotify claims first, this one last).
//  GCS_RESULTSTR (the committed text) is converted to wide and appended to gChatbuff: gwszImePrefix first, then the result, each under its own
//  `< 127` guard, and both IME buffers are cleared. GCS_COMPSTR (the text still being composed) goes to gwszImeComposition.
//  gwszImePrefix is never filled, so its strlen is 0 and its strcat appends nothing; written as the bytes have it.
//  The lengths that come from ImmGetCompositionStringA are compared UNSIGNED (the two clamps to 199 and the composition arm's `!= 0`), so an
//  IMM error, a negative LONG, clamps to 199 rather than passing as a small number. The second composition call's answer, the two 127 guards
//  and the two length tests are SIGNED.
//  The overflow arm is REACHABLE: the composition arm zeroes the WCHAR[32], converts with cchWideChar = 32 and only then measures, and
//  MultiByteToWideChar handed a string longer than the buffer writes all 32 characters, no terminator, and fails with
//  ERROR_INSUFFICIENT_BUFFER. So a composition of 32 or more characters leaves the buffer full, the length comes out at least 32, and the
//  strncpy of 31, the hand-written terminator and the beep (SOUNDINST_PlaySound, D2Client's own) all run. The result arm writes its own
//  terminator at [31] before measuring, so the committed text is cut to 31 without a beep.
//  DIFFERENCE: in that overflow Unicode::strlen runs past element 31; in the original what follows is the multibyte buffer, in ours whatever our
//  frame puts there. The length is at least 32 either way and is only compared with 31 and 32, so the same arm runs and the same stores are made.
void __stdcall CHAT_OnImeComposition(SMSGHANDLER_PARAMS *pMsg)
{
    char szMultiByte[200];
    HIMC hImc;
    uint32_t dwBufLen;
    int32_t nResultLen;
    int32_t nPrefixLen;
    int32_t nCompLen;

    memset(szMultiByte, 0, sizeof(szMultiByte));
    hImc = ImmGetContext(D2GFX_WINDOW_GetWindow());
    if (hImc)
    {
        Unicode wszText[32];

        if (pMsg->lParam & GCS_RESULTSTR)
        {
            dwBufLen = (uint32_t)ImmGetCompositionStringA(hImc, GCS_RESULTSTR, NULL, 0);
            if (dwBufLen > 199)
                dwBufLen = 199;
            ImmGetCompositionStringA(hImc, GCS_RESULTSTR, szMultiByte, dwBufLen);
            szMultiByte[199] = 0;
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szMultiByte, -1, (LPWSTR)wszText, 32);
            wszText[31].ch = 0;
            nResultLen = Unicode::strlen(wszText);
            nPrefixLen = Unicode::strlen(gwszImePrefix);
            if (nPrefixLen + Unicode::strlen(gChatbuff) < 127)
            {
                Unicode::strcat(gChatbuff, gwszImePrefix);
                gnChatInputLength += Unicode::strlen(gwszImePrefix);
            }
            if (Unicode::strlen(gChatbuff) + nResultLen < 127)
            {
                Unicode::strcat(gChatbuff, wszText);
                gnChatInputLength += nResultLen;
            }
            gwszImePrefix[0].ch = 0;
            gwszImeComposition[0].ch = 0;
        }

        if ((pMsg->lParam & GCS_COMPSTR) && ImmGetOpenStatus(hImc))
        {
            dwBufLen = (uint32_t)ImmGetCompositionStringA(hImc, GCS_COMPSTR, NULL, 0);
            if (dwBufLen != 0)
            {
                memset(szMultiByte, 0, sizeof(szMultiByte));
                memset(wszText, 0, sizeof(wszText));
                if (dwBufLen > 199)
                    dwBufLen = 199;
                if (ImmGetCompositionStringA(hImc, GCS_COMPSTR, szMultiByte, dwBufLen) > 0)
                {
                    szMultiByte[199] = 0;
                    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szMultiByte, -1, (LPWSTR)wszText, 32);
                    nCompLen = Unicode::strlen(wszText);
                    if (nCompLen < 31)
                        wszText[nCompLen].ch = 0;
                    else
                        wszText[31].ch = 0;
                    if (nCompLen < 32)
                    {
                        Unicode::strcpy(gwszImeComposition, wszText);
                    }
                    else
                    {
                        Unicode::strncpy(gwszImeComposition, wszText, 31);
                        gwszImeComposition[31].ch = 0;
                        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
                    }
                }
            }
        }

        ImmReleaseContext(D2GFX_WINDOW_GetWindow(), hImc);
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAD19B0
//
//  WM_IME_STARTCOMPOSITION. Shows the caret, empties both IME buffers, and for the two Chinese languages sets the IME mode (traditional 4,
//  simplified 5); every other language leaves it alone.
//  void: both exits are tail jumps into SMsgBreakHandlerChain, and Storm's dispatcher discards whatever that leaves in eax.
void __stdcall CHAT_OnImeStartComposition(SMSGHANDLER_PARAMS *pMsg)
{
    D2C_Language eLanguage;

    eLanguage = gnChatLanguage;
    gbChatCaretVisible = 1;
    gwszImeComposition[0].ch = 0;
    gwszImePrefix[0].ch = 0;
    if (eLanguage == LANGUAGE_CHINESETWN)
        gnChatImeMode = 4;
    else if (eLanguage == LANGUAGE_CHINESESIN)
        gnChatImeMode = 5;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAD1A20
//
//  WM_IME_ENDCOMPOSITION. Empties both IME buffers, puts -2 into gnChatInputDrawTick, and hides the caret only under Japanese. Claims the
//  message and tail-jumps into SMsgBreakHandlerChain.
void __stdcall CHAT_OnImeEndComposition(SMSGHANDLER_PARAMS *pMsg)
{
    D2C_Language eLanguage;

    eLanguage = gnChatLanguage;
    gwszImeComposition[0].ch = 0;
    gwszImePrefix[0].ch = 0;
    gnChatInputDrawTick = -2;
    if (eLanguage == LANGUAGE_JAPANESE)
        gbChatCaretVisible = 0;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAD1A70
//
//  WM_IME_NOTIFY, the first record of gChatImeHandlers. It claims the message FIRST and breaks the chain before any IMM work; several arms then
//  store the same two fields again, which is harmless and reproduced. A NULL context returns at once, with nothing to release.
//  The switch on wParam covers the IMN_ ids 2 to 8; IMN_SETSENTENCEMODE (7) and everything outside go straight to the release.
//
//      2  IMN_OPENSTATUSWINDOW   Chinese traditional: the mode from ImmIsIME
//      3  IMN_CHANGECANDIDATE    Chinese traditional: refill the list
//      4  IMN_CLOSECANDIDATE     either Chinese: free the list
//      5  IMN_OPENCANDIDATE      either Chinese: refill the list
//      6  IMN_SETCONVERSIONMODE  either Korean: mode 1 in native (Hangul) mode
//      8  IMN_SETOPENSTATUS      Japanese 3, simplified 5, traditional 4
//
//  Case 2 is `ImmIsIME(...) ? 0 : 4` (neg / sbb / and al, 0FCh / add eax, 4 at 0x6FAD1D38..0x6FAD1D3E): opening the status window under
//  Chinese traditional sets mode 4 when the keyboard layout is NOT an IME and 0 when it is.
//  fdwConversion is a local the compiler put in pMsg's own stack slot: pMsg lives in edi, so its incoming slot is dead, and case 6 hands
//  ImmGetConversionStatus that slot's address; the C declares the local. In case 6 the two stores after the byte test of the native mode go
//  mode first, caret second, the other way round from case 8's; the C follows the bytes in both.
//  The candidate-list refill (cases 3 and 5) is written out in place three times; the original inlines it.
//  BUG: FOG_Alloc's answer is dereferenced untested: ImmGetCandidateListA is handed it, then `cmp dword ptr [ecx+4], 2` reads it (0x6FAD1BE7,
//  0x6FAD1CF7), so a NULL faults there. LATENT: the size is the IME's own count, tested non-zero, so only a failed allocation reaches it.
void __stdcall CHAT_OnImeNotify(SMSGHANDLER_PARAMS *pMsg)
{
    HIMC hImc;
    uint32_t dwBufLen;
    DWORD fdwConversion;
    DWORD fdwSentence;
    DWORD dwListCount;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    hImc = ImmGetContext(D2GFX_WINDOW_GetWindow());
    if (!hImc)
        return;

    switch (pMsg->wParam)
    {
    case IMN_OPENSTATUSWINDOW:
        if (gnChatLanguage == LANGUAGE_CHINESETWN)
        {
            gnChatImeMode = ImmIsIME(GetKeyboardLayout(0)) ? 0 : 4;
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
        }
        break;

    case IMN_CHANGECANDIDATE:
        if (gnChatLanguage == LANGUAGE_CHINESETWN)
        {
            dwBufLen = ImmGetCandidateListCountA(hImc, &dwListCount);
            if (ImmGetCandidateListA(hImc, 0, NULL, 0) && dwBufLen)
            {
                if (gpChatCandidateList)
                    FOG_Free(gpChatCandidateList, szChatCpp, 229, 0);
                gpChatCandidateList = (CANDIDATELIST *)FOG_Alloc(dwBufLen, szChatCpp, 231, 0);
                ImmGetCandidateListA(hImc, 0, gpChatCandidateList, dwBufLen);
                if (gpChatCandidateList->dwStyle == IME_CAND_CODE && gpChatCandidateList->dwCount == 1)
                {
                    FOG_Free(gpChatCandidateList, szChatCpp, 236, 0);
                    gpChatCandidateList = NULL;
                }
            }
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
        }
        break;

    case IMN_CLOSECANDIDATE:
        if (gnChatLanguage == LANGUAGE_CHINESETWN)
        {
            if (gpChatCandidateList)
            {
                FOG_Free(gpChatCandidateList, szChatCpp, 346, 0);
                gpChatCandidateList = NULL;
            }
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
        }
        if (gnChatLanguage == LANGUAGE_CHINESESIN)
        {
            if (gpChatCandidateList)
            {
                FOG_Free(gpChatCandidateList, szChatCpp, 357, 0);
                gpChatCandidateList = NULL;
            }
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
        }
        break;

    case IMN_OPENCANDIDATE:
        if (gnChatLanguage == LANGUAGE_CHINESETWN)
        {
            dwBufLen = ImmGetCandidateListCountA(hImc, &dwListCount);
            if (ImmGetCandidateListA(hImc, 0, NULL, 0) && dwBufLen)
            {
                if (gpChatCandidateList)
                    FOG_Free(gpChatCandidateList, szChatCpp, 229, 0);
                gpChatCandidateList = (CANDIDATELIST *)FOG_Alloc(dwBufLen, szChatCpp, 231, 0);
                ImmGetCandidateListA(hImc, 0, gpChatCandidateList, dwBufLen);
                if (gpChatCandidateList->dwStyle == IME_CAND_CODE && gpChatCandidateList->dwCount == 1)
                {
                    FOG_Free(gpChatCandidateList, szChatCpp, 236, 0);
                    gpChatCandidateList = NULL;
                }
            }
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
        }
        if (gnChatLanguage == LANGUAGE_CHINESESIN)
        {
            dwBufLen = ImmGetCandidateListCountA(hImc, &dwListCount);
            if (ImmGetCandidateListA(hImc, 0, NULL, 0) && dwBufLen)
            {
                if (gpChatCandidateList)
                    FOG_Free(gpChatCandidateList, szChatCpp, 229, 0);
                gpChatCandidateList = (CANDIDATELIST *)FOG_Alloc(dwBufLen, szChatCpp, 231, 0);
                ImmGetCandidateListA(hImc, 0, gpChatCandidateList, dwBufLen);
                if (gpChatCandidateList->dwStyle == IME_CAND_CODE && gpChatCandidateList->dwCount == 1)
                {
                    FOG_Free(gpChatCandidateList, szChatCpp, 236, 0);
                    gpChatCandidateList = NULL;
                }
            }
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
        }
        break;

    case IMN_SETCONVERSIONMODE:
        ImmGetConversionStatus(hImc, &fdwConversion, &fdwSentence);
        if (gnChatLanguage == LANGUAGE_KOREAN || gnChatLanguage == LANGUAGE_ENGLISHKOR)
        {
            if (fdwConversion & IME_CMODE_NATIVE)
            {
                gnChatImeMode = 1;
                gbChatCaretVisible = 1;
            }
            else
            {
                gbChatCaretVisible = 0;
                gnChatImeMode = 0;
            }
        }
        break;

    case IMN_SETOPENSTATUS:
        switch (gnChatLanguage)
        {
        case LANGUAGE_JAPANESE:
            if (ImmGetOpenStatus(hImc))
            {
                gbChatCaretVisible = 1;
                gnChatImeMode = 3;
            }
            else
            {
                gbChatCaretVisible = 0;
                gnChatImeMode = 0;
            }
            break;

        case LANGUAGE_CHINESESIN:
            if (ImmGetOpenStatus(hImc))
            {
                gbChatCaretVisible = 1;
                gnChatImeMode = 5;
            }
            else
            {
                gbChatCaretVisible = 0;
                gnChatImeMode = 0;
            }
            break;

        case LANGUAGE_CHINESETWN:
            if (ImmGetOpenStatus(hImc))
            {
                gbChatCaretVisible = 1;
                gnChatImeMode = 4;
            }
            else
            {
                gbChatCaretVisible = 0;
                gnChatImeMode = 0;
            }
            break;
        }
        break;
    }

    ImmReleaseContext(D2GFX_WINDOW_GetWindow(), hImc);
}


//1.10f: D2Client.0x6FAD1D80
//
//  WM_IME_COMPOSITIONFULL, WM_IME_SELECT and WM_IME_CONTROL (0x284, 0x285 and 0x283), gChatImeHandlers' last three records in that order. The
//  message is claimed and the chain NOT broken, so it is marked handled and still travels on -- the difference from INV_ConsumeMessage.
void __stdcall CHAT_ImeIgnoreMessage(SMSGHANDLER_PARAMS *pMsg)
{
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
}


//1.10f: D2Client.0x6FAD1DA0
//
//  Hooks the chat line onto the game window; UI_RegisterPanelHandlers calls it when the chat box opens. gChatMessageHandlers' four records
//  always; then, for the five IME languages, an IME and gChatImeHandlers' seven -- but only where the machine agrees with the build:
//
//      Japanese              the layout is an IME, and the system LANGID 0x411
//      Korean, ENGLISHKOR    the layout is an IME, and 0x412 or 0x812
//      Chinese simplified    the system LANGID 0x804 or 0x1004
//      Chinese traditional   the system LANGID 0x404 or 0xC04
//
//  A mismatch returns before anything IME is registered. The LANGID tests are sixteen bits wide (LANGID is a WORD).
//  The two Chinese builds do not ask whether the layout is an IME before going on. They ask afterwards and, when it is not, switch it with
//  ImmSimulateHotKey -- IME_THOTKEY_IME_NONIME_TOGGLE (0x70) for traditional, IME_CHOTKEY_IME_NONIME_TOGGLE (0x10) for simplified. Korean forces
//  IME_CMODE_NATIVE, which is Hangul mode, and reads back whether it took.
//  The system LANGID is asked for twice on the Chinese paths, once for the gate and once for the switch, and the switch re-reads gnChatLanguage
//  from memory instead of the register that still holds it. Both as the bytes.
//  gbChatInputActive is 1 only while the IME table is registered: this is its one writer, it is cleared once the language is an IME language
//  and set on the four IME paths, and CHAT_UnregisterHandlers reads it as exactly that.
//  Storm is handed our two tables here and CHAT_UnregisterHandlers hands the same tables back.
void __cdecl CHAT_RegisterInputHandlers()
{
    D2C_Language eLanguage;
    LANGID nSystemLangId;
    HIMC hImc;
    DWORD fdwConversion;
    DWORD fdwSentence;

    WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gChatMessageHandlers, 4);
    eLanguage = D2LANG_STRTABLE_GetLanguage();
    gnChatLanguage = eLanguage;
    if (eLanguage != LANGUAGE_JAPANESE && eLanguage != LANGUAGE_CHINESETWN && eLanguage != LANGUAGE_CHINESESIN && eLanguage != LANGUAGE_KOREAN && eLanguage != LANGUAGE_ENGLISHKOR)
        return;

    gbChatInputActive = 0;
    if (eLanguage == LANGUAGE_CHINESETWN)
    {
        nSystemLangId = GetSystemDefaultLangID();
        if (nSystemLangId != 0x404 && nSystemLangId != 0xC04)
            return;
    }
    else if (eLanguage == LANGUAGE_CHINESESIN)
    {
        nSystemLangId = GetSystemDefaultLangID();
        if (nSystemLangId != 0x804 && nSystemLangId != 0x1004)
            return;
    }
    else if (!ImmIsIME(GetKeyboardLayout(0)))
    {
        return;
    }

    nSystemLangId = GetSystemDefaultLangID();
    hImc = ImmGetContext(D2GFX_WINDOW_GetWindow());
    if (!hImc)
        return;

    gwszImeComposition[0].ch = 0;
    gwszImePrefix[0].ch = 0;
    switch (gnChatLanguage)
    {
    case LANGUAGE_JAPANESE:
        if (nSystemLangId != 0x411)
            break;
        gbChatInputActive = 1;
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gChatImeHandlers, 7);
        if (ImmSetOpenStatus(hImc, TRUE))
        {
            gbChatCaretVisible = 1;
            gnChatImeMode = 3;
        }
        else
        {
            gbChatCaretVisible = 0;
            gnChatImeMode = 0;
        }
        break;

    case LANGUAGE_KOREAN:
    case LANGUAGE_ENGLISHKOR:
        if (nSystemLangId != 0x412 && nSystemLangId != 0x812)
            break;
        ImmSetOpenStatus(hImc, TRUE);
        ImmGetConversionStatus(hImc, &fdwConversion, &fdwSentence);
        ImmSetConversionStatus(hImc, IME_CMODE_NATIVE, fdwSentence);
        ImmGetConversionStatus(hImc, &fdwConversion, &fdwSentence);
        if (fdwConversion & IME_CMODE_NATIVE)
        {
            gnChatImeMode = 1;
            gbChatCaretVisible = 1;
        }
        else
        {
            gnChatImeMode = 0;
        }
        gbChatInputActive = 1;
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gChatImeHandlers, 7);
        break;

    case LANGUAGE_CHINESESIN:
        gbChatCaretVisible = 0;
        gbChatInputActive = 1;
        if (!ImmIsIME(GetKeyboardLayout(0)))
            ImmSimulateHotKey(D2GFX_WINDOW_GetWindow(), IME_CHOTKEY_IME_NONIME_TOGGLE);
        ImmSetOpenStatus(hImc, TRUE);
        gnChatImeMode = 5;
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gChatImeHandlers, 7);
        break;

    case LANGUAGE_CHINESETWN:
        gbChatCaretVisible = 0;
        gbChatInputActive = 1;
        if (!ImmIsIME(GetKeyboardLayout(0)))
            ImmSimulateHotKey(D2GFX_WINDOW_GetWindow(), IME_THOTKEY_IME_NONIME_TOGGLE);
        ImmSetOpenStatus(hImc, TRUE);
        gnChatImeMode = 4;
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gChatImeHandlers, 7);
        break;
    }

    ImmReleaseContext(D2GFX_WINDOW_GetWindow(), hImc);
}


//1.10f: D2Client.0x6FAD2030
//
//  Unhooks it; UI_UnregisterPanelHandlers calls it. The four message handlers always; while gbChatInputActive is set, the seven IME ones too,
//  and the IME is closed -- ImmSetOpenStatus(hImc, FALSE) -- under Korean after clearing the conversion mode and keeping the sentence mode.
//  Then both IME buffers are emptied and the candidate list freed, whether or not a context was obtained.
void __cdecl CHAT_UnregisterHandlers()
{
    HIMC hImc;
    DWORD fdwConversion;
    DWORD fdwSentence;

    WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gChatMessageHandlers, 4);
    if (!gbChatInputActive)
        return;

    WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gChatImeHandlers, 7);
    hImc = ImmGetContext(D2GFX_WINDOW_GetWindow());
    if (hImc)
    {
        ImmSetOpenStatus(hImc, FALSE);
        if (gnChatLanguage == LANGUAGE_KOREAN || gnChatLanguage == LANGUAGE_ENGLISHKOR)
        {
            ImmGetConversionStatus(hImc, &fdwConversion, &fdwSentence);
            ImmSetConversionStatus(hImc, IME_CMODE_ALPHANUMERIC, fdwSentence);
        }
        ImmReleaseContext(D2GFX_WINDOW_GetWindow(), hImc);
    }

    gwszImePrefix[0].ch = 0;
    gwszImeComposition[0].ch = 0;
    if (gpChatCandidateList)
    {
        FOG_Free(gpChatCandidateList, szChatCpp, 663, 0);
        gpChatCandidateList = NULL;
    }
}


//1.10f: D2Client.0x6FAD2100
//
//  Draws the chat box; UIPANELS_DrawInGamePanels calls it while UI_CHATBOX is open. void: the value was D2WIN_10127_SetFont's answer, which
//  its one caller discards.
//
//  The IME mode, while an IME is up: a label at (20, gnScreenHeight - 128), switched on gnChatImeMode (its one reader) over 0..5:
//
//      0      "Alphanumeric", a literal through Unicode::win2Unicode, no tbl
//      1      U+D55C U+AE00, Hangul
//      2, 3   U+65E5 U+672C U+8A9E, Nihongo
//      4, 5   U+6F22, Han -- and the candidate list
//
//  Nothing writes 2; the writers set 0, 1, 3, 4 and 5.
//  BUG: the Chinese label never draws. Its arm stores U+6F22 into element 1 and the terminator into element 2 and nothing into element 0, which
//  the construction has zeroed (D2Lang's Unicode constructor #10017 is `mov word ptr [ecx], 0 / retn`), so the string handed to D2Win is empty.
//  REACHABLE: in the two Chinese languages the IME mode is shown by no label.
//  The candidate page runs from dwPageStart for dwPageSize, bounded by dwCount -- or the first twenty when dwPageSize is 0 -- numbered "%d: "
//  at x 20 with the candidate at x 30, from gnScreenHeight - 368 down by D2Win #10125's answer (the line step), stopping once the drop passes
//  280. dwCount and the page are compared SIGNED.
//  BUG: the candidate buffer is constructed 256 wide and MultiByteToWideChar is told 257. This cannot hurt the original: its frame slot is
//  516 bytes, shared with the text path's 257-wide buffer, so a 257th character lands inside the frame.
//  DIFFERENCE: ours is declared 257 so that the call stays defined, and the one extra construction stores a zero nothing reads.
//
//  The typed line: gnChatInputDrawTick counts the calls. The box is GFX_DrawFilledRect(127, gnScreenHeight - 103, gnScreenWidth - 255, 47, 0,
//  1). A disconnect turns UI_CHATBOX off, and this call draws on regardless.
//  gChatbuff is copied into a 257-wide buffer (256 characters and room for the terminator), split by D2Win #10199 at gnScreenWidth - 265, and
//  drawn from (131, gnScreenHeight - 88) fifteen apart while the line's offset is under 45: three lines. The rest are split and walked, and not
//  drawn -- there is no scroll. After the last line, while an IME is up and its caret visible, CHAT_DrawImeComposition draws the composition;
//  with an empty line it draws it at the start.
//  The caret otherwise: tbl 3404 at the width of the last line DRAWN plus 131, on line min(count - 1, 2), while gbChatInputBlinkOn is 1. The y
//  is 15 * i (the bytes split the multiply). The in-loop store of the line index is overwritten after the loop whenever the loop ran; written
//  as the bytes.
//  The blink re-stamps gnChatBlinkLastTick and flips gbChatInputBlinkOn once GetTickCount has moved past the low word of gnChatInputBlinkMs:
//  500 ms on, 100 ms off, stored as words and read as the dword masked to 16 bits. CHAT_DrawImeComposition ends with the same sequence,
//  written in place in both.
void __cdecl CHAT_DrawInputLine()
{
    enum Font ePrevFont;
    int32_t nTextWidth;
    int32_t nLastLine;
    uint32_t dwNow;

    ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);
    if (gbChatInputActive)
    {
        Unicode wszMode[20];

        switch (gnChatImeMode)
        {
        case 0:
            Unicode::win2Unicode(wszMode, "Alphanumeric", 20);
            D2WIN_10117_DrawText(wszMode, 20, gnScreenHeight - 128, 0, 0);
            break;

        case 1:
            wszMode[0].ch = 0xD55C;
            wszMode[1].ch = 0xAE00;
            wszMode[2].ch = 0;
            D2WIN_10117_DrawText(wszMode, 20, gnScreenHeight - 128, 0, 0);
            break;

        case 2:
        case 3:
            wszMode[0].ch = 0x65E5;
            wszMode[1].ch = 0x672C;
            wszMode[2].ch = 0x8A9E;
            wszMode[3].ch = 0;
            D2WIN_10117_DrawText(wszMode, 20, gnScreenHeight - 128, 0, 0);
            break;

        case 4:
        case 5:
            //  Element 0 keeps the construction's zero: the label is empty.
            wszMode[1].ch = 0x6F22;
            wszMode[2].ch = 0;
            D2WIN_10117_DrawText(wszMode, 20, gnScreenHeight - 128, 0, 0);
            if (gpChatCandidateList)
            {
                CANDIDATELIST *pList;
                int32_t nFirst;
                int32_t nEnd;
                int32_t nIndex;
                int32_t nNumber;
                int32_t nLineHeight;
                int32_t nDrop;
                DWORD *pOffsets;

                pList = gpChatCandidateList;
                if (pList->dwPageSize)
                {
                    nFirst = (int32_t)pList->dwPageStart;
                    nEnd = nFirst + (int32_t)pList->dwPageSize;
                    if ((int32_t)pList->dwCount < nEnd)
                        nEnd = (int32_t)pList->dwCount;
                }
                else
                {
                    nFirst = 0;
                    nEnd = (int32_t)pList->dwCount;
                    if (nEnd >= 20)
                        nEnd = 20;
                }
                nNumber = 0;
                nLineHeight = D2WIN_10125();
                nIndex = nFirst;
                if (nIndex < nEnd)
                {
                    nDrop = 0;
                    pOffsets = &pList->dwOffset[nFirst];
                    do
                    {
                        Unicode wszNumber[20];
                        char szNumber[20];
                        const char *pszCandidate;

                        pszCandidate = (const char *)pList + *pOffsets;
                        ++nNumber;
                        nDrop += nLineHeight;
                        sprintf(szNumber, "%d: ", nNumber);
                        Unicode::win2Unicode(wszNumber, szNumber, 20);
                        D2WIN_10117_DrawText(wszNumber, 20, gnScreenHeight + nDrop - 368, 0, 0);
                        {
                            Unicode wszCandidate[257];      //  256 wide in the original

                            if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszCandidate, -1, (LPWSTR)wszCandidate, 257))
                                D2WIN_10117_DrawText(wszCandidate, 30, gnScreenHeight + nDrop - 368, 0, 0);
                        }
                        if (nDrop > 280)
                            break;
                        ++nIndex;
                        ++pOffsets;
                    } while (nIndex < nEnd);
                }
            }
            break;
        }
    }

    ++gnChatInputDrawTick;
    GFX_DrawFilledRect(127, gnScreenHeight - 103, gnScreenWidth - 255, 47, 0, 1);
    nTextWidth = 0;
    if (GAME_IsDisconnecting())
        UI_SetUIToggle(UI_CHATBOX, UI_TURNOFF, 0);

    nLastLine = 0;
    if (Unicode::strlen(gChatbuff) <= 0)
    {
        if (gbChatInputActive && gbChatCaretVisible)
            CHAT_DrawImeComposition(131, gnScreenHeight - 88);
    }
    else
    {
        Unicode wszText[257];
        D2SplittedTextStrc *pFirst;
        D2SplittedTextStrc *pLine;
        D2SplittedTextStrc *pNext;
        int32_t nSplitLines;
        int32_t nLineCount;
        int32_t nLineY;

        Unicode::strncpy(wszText, gChatbuff, 256);
        pFirst = D2WIN_10199(wszText, &nSplitLines, gnScreenWidth - 265);
        nLineCount = 0;
        if (pFirst)
        {
            nLineY = 0;
            pLine = pFirst;
            do
            {
                nLastLine = nLineCount;
                if (nLineY < 45)
                {
                    D2WIN_10117_DrawText(pLine->wszLine, 131, nLineY + gnScreenHeight - 88, 0, 0);
                    nTextWidth = D2WIN_10121_GetTextWidth(pLine->wszLine);
                }
                pNext = pLine->pNextLine;
                if (!pNext && gbChatInputActive && gbChatCaretVisible)
                    CHAT_DrawImeComposition(D2WIN_10121_GetTextWidth(pLine->wszLine) + 131, nLineY + gnScreenHeight - 88);
                ++nLineCount;
                nLineY += 15;
                pLine = pNext;
            } while (pNext);
        }
        D2WIN_10200(pFirst);
        if (nLineCount > 0)
            nLastLine = nLineCount - 1 <= 2 ? nLineCount - 1 : 2;
    }

    if (gbChatInputActive && gbChatCaretVisible)
    {
        D2WIN_10127_SetFont(ePrevFont);
        return;
    }

    if (gbChatInputBlinkOn == 1)
        D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_3404_strMenuMain15), nTextWidth + 131, gnScreenHeight + 15 * nLastLine - 88, 0, 0);

    dwNow = GetTickCount();
    if (dwNow - gnChatBlinkLastTick > gnChatInputBlinkMs)
    {
        gnChatBlinkLastTick = dwNow;
        if (gbChatInputBlinkOn)
        {
            gnChatInputBlinkMs = 100;
            gbChatInputBlinkOn = 0;
        }
        else
        {
            gnChatInputBlinkMs = 500;
            gbChatInputBlinkOn = 1;
        }
    }
    D2WIN_10127_SetFont(ePrevFont);
}


//1.10f: D2Client.0x6FAD25F0
//
//  Draws the IME composition at (nX, nY), from CHAT_DrawInputLine's two sites. gwszImePrefix, then gwszImeComposition beside it, each only
//  when non-empty and each moved to (131, nY + 15) when it would run past gnScreenWidth - 265; then the caret, tbl 3404, while
//  gbChatInputBlinkOn is 1; then the blink, the sequence CHAT_DrawInputLine ends with. Colour 4 throughout, where the typed line uses 0.
//  The composition's width is measured before its emptiness is tested, so the caret stands after it either way. The prefix is always empty,
//  so its arm never runs and its width stays 0.
//  The original also stores nY into a four-byte local at entry (push ecx) that is never used.
void __fastcall CHAT_DrawImeComposition(int32_t nX, int32_t nY)
{
    int32_t nPrefixWidth;
    int32_t nCompWidth;
    uint32_t dwNow;

    nPrefixWidth = 0;
    if (Unicode::strlen(gwszImePrefix))
    {
        nPrefixWidth = D2WIN_10121_GetTextWidth(gwszImePrefix);
        if (nPrefixWidth + nX - 131 > gnScreenWidth - 265)
        {
            nX = 131;
            nY += 15;
        }
        D2WIN_10117_DrawText(gwszImePrefix, nX, nY, 4, 0);
    }

    nCompWidth = D2WIN_10121_GetTextWidth(gwszImeComposition);
    if (Unicode::strlen(gwszImeComposition))
    {
        if (nX + nCompWidth + nPrefixWidth - 131 > gnScreenWidth - 265)
        {
            nX = 131;
            nY += 15;
        }
        D2WIN_10117_DrawText(gwszImeComposition, nX + nPrefixWidth, nY, 4, 0);
    }

    if (gbChatInputBlinkOn == 1)
        D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_3404_strMenuMain15), nX + nCompWidth + nPrefixWidth, nY, 4, 0);

    dwNow = GetTickCount();
    if (dwNow - gnChatBlinkLastTick > gnChatInputBlinkMs)
    {
        gnChatBlinkLastTick = dwNow;
        if (gbChatInputBlinkOn)
        {
            gnChatInputBlinkMs = 100;
            gbChatInputBlinkOn = 0;
        }
        else
        {
            gnChatInputBlinkMs = 500;
            gbChatInputBlinkOn = 1;
        }
    }
}


//1.10f: D2Client.0x6FAD2720
//
//  WM_LBUTTONDOWN while the chat box is open, gChatMessageHandlers' fourth record. A click whose y is past gnScreenHeight - 49 turns UI_CHATBOX
//  off: `cmp eax, edx / jle` (0x6FAD2733) returns for y <= height - 49, so it is a click in the bottom 49 rows, the control panel's, that
//  closes the chat box, and a click in the world does not.
//  nYpos is the message's +0x0E word, zero-extended and compared signed. The message is not claimed. The call is a tail jump into
//  UI_SetUIToggle that reuses pMsg's slot for its stack argument.
void __stdcall CHAT_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    if (pMsg->nYpos > gnScreenHeight - 49)
        UI_SetUIToggle(UI_CHATBOX, UI_TURNOFF, 0);
}


//1.10f: D2Client.0x6FAD2760
//
//  VK_UP and VK_DOWN walk the history: gChatMessageHandlers' second and third records. Claims the message first, then ignores an auto-repeat
//  (lParam bit 30, CMDTBL_OnKeyDown's own test).
//  The two keys are not mirror images: VK_UP copies the entry at the cursor and then steps the cursor forward; VK_DOWN steps it back and then
//  copies the entry it lands on. Either step wraps -- past 7 to 0, below 0 to 7 -- and is undone when the ring is not yet full and the step
//  would pass the filled slots: `(WORD)count < 7` (unsigned) and `8 - cursor > count` (signed, the count's dword masked to its word). The
//  copy is 514 bytes (`rep movsd` of 0x80 and a `movsw`) and gnChatInputLength becomes the entry's strlen.
//  gnChatHistoryCursor holds the unwrapped value for a moment: the step is stored before the wrap is tested, so the global is -1 or 8 in
//  between -- which is why every test on it is signed.
void __stdcall CHAT_OnHistoryKey(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nOldCursor;
    int32_t nCursor;
    const Unicode *pwszEntry;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    if (pMsg->lParam & 0x40000000)
        return;

    if (pMsg->wParam == VK_UP)
    {
        pwszEntry = gChatHistory[gnChatHistoryCursor];
        memcpy(gChatbuff, pwszEntry, 514);
        gnChatInputLength = Unicode::strlen(pwszEntry);

        nOldCursor = gnChatHistoryCursor;
        nCursor = nOldCursor + 1;
        gnChatHistoryCursor = nCursor;
        if (nCursor >= 8)
        {
            nCursor = 0;
            gnChatHistoryCursor = nCursor;
        }
        else if (nCursor < 0)
        {
            nCursor = 7;
            gnChatHistoryCursor = nCursor;
        }
        if (gnChatHistoryCount < 7 && 8 - nCursor > (int32_t)gnChatHistoryCount)
            gnChatHistoryCursor = nOldCursor;
    }
    else if (pMsg->wParam == VK_DOWN)
    {
        nOldCursor = gnChatHistoryCursor;
        nCursor = nOldCursor - 1;
        gnChatHistoryCursor = nCursor;
        if (nCursor >= 8)
        {
            nCursor = 0;
            gnChatHistoryCursor = nCursor;
        }
        else if (nCursor < 0)
        {
            nCursor = 7;
            gnChatHistoryCursor = nCursor;
        }
        if (gnChatHistoryCount < 7 && 8 - nCursor > (int32_t)gnChatHistoryCount)
        {
            nCursor = nOldCursor;
            gnChatHistoryCursor = nCursor;
        }
        pwszEntry = gChatHistory[nCursor];
        memcpy(gChatbuff, pwszEntry, 514);
        gnChatInputLength = Unicode::strlen(pwszEntry);
    }
}


//1.10f: D2Client.0x6FAD2890
//
//  WM_CHAR in the chat box, gChatMessageHandlers' first record.
//
//  Nothing is taken, or claimed, until the input line has been drawn at least once since it was cleared: `cmp gnChatInputDrawTick, 1 / jl`
//  (0x6FAD289B) returns before the claim. CHAT_ClearInputLine sets the counter to 0 and CHAT_OnImeEndComposition to -2, so after an IME
//  composition ends three draws pass before a character is taken.
//
//  Backspace drops the last counted character; Escape closes the box; any other character is appended at the string's END, gChatbuff[strlen],
//  with no terminator written after it -- the buffer is zero past its text while the counts agree. It is taken only when an IME is up or D2Win
//  #10128 accepts it (its byte, `mov cl, [ecx+8]` -- the import takes a uint8_t), below 255 characters by both counts (unsigned on
//  gnChatInputLength, signed on the strlen), and while the text splits into at most three lines at gnScreenWidth - 265 whose last is no wider
//  than gnScreenWidth - 275.
//  BUG: a character that makes the third line too wide stays in the buffer uncounted. The width is measured again after the store and only the
//  count is withheld, so strlen runs one past gnChatInputLength. Backspace then cuts at the count, leaving that character behind the new
//  terminator, and the next character appended brings it back. And one that wraps onto a FOURTH line is kept and counted (the recheck asks only
//  `lines != 3 || width < ...`), though CHAT_DrawInputLine draws three. REACHABLE, cosmetic: the extra character is drawn and sent, and one
//  Backspace takes it with the one before it.
//
//  Enter: leading whitespace below 0x100 is skipped (the CRT's isspace); an empty rest closes the box. Otherwise the line goes into the
//  history ring: a full ring (the count's word is 8) overwrites at gnChatHistoryHead and steps the head back, wrapping to 7; a filling one steps
//  gnChatHistoryIndex back unless it is the first line, and counts it. The cursor is set to the slot written.
//  BUG: the history copy reads 514 bytes from the TRIMMED start, so it reads two bytes past gChatbuff per whitespace skipped, into the globals
//  that follow it. The original's copy is a forward `rep movsd` of 128 dwords and a `movsw`; with 40 or more skipped and slot 0 the
//  destination, source and destination overlap. The text itself ends inside gChatbuff, before any overlapping byte.
//
//  Then the WHOLE of gChatbuff, whitespace included, is converted with WideCharToMultiByte(CP_ACP, 0x250, ..., 256) -- WC_COMPOSITECHECK |
//  WC_DEFAULTCHAR | WC_DISCARDNS -- and the text is taken from the converted buffer at the whitespace's count, one byte per skipped character,
//  which holds because every skipped character is below 0x100.
//
//  The commands, compared on that text, in this order:
//
//      "/fps"             DRAW_ToggleDebugOverlay, and close
//      "/framerate"       DRAW_GameToggleDebugOverlay, and close
//      "/players" (8 characters, case-blind: _strnicmp) under game types 0, 1,
//                         6 or 8: the count after it, atoi'd and clamped to
//                         1..8, to D2Game's PLAYER_SetVirtualPlayerCount, and
//                         "players set to %d" written over the text -- which
//                         then falls through the two below and is SENT
//      "/nopickup"        gbNoPickup toggled, and close
//      "soundchaosdebug"  WITHOUT a slash, SOUNDINST_ToggleChaosDebug, and close
//
//  Anything else is a chat message: the block at -218h is a D2GSPacketClt15 -- 0x15, message type 1, gnChatLanguage's byte, the text zeroed to
//  256 -- handed to CHAT_ParseCommandPrefix. "!" (kind 0) sends it as 0x14, overhead; "@name" (kind 2) and no prefix send 0x15; "/" (kind 3)
//  goes to the whisper commands /msg, /m, /whisper and /w, then the Battle.net commands /help and /?, and last to the Battle.net client's
//  ChatCommand (+0x14; `call dword ptr [eax+14h]`, ecx the text). Every Enter closes the chat box.
//
//  BUG: the wide-character test tests the terminator. Twice, under an IME, the line is scanned for a character above 0xFF before it is parsed:
//  `mov dx, word ptr gChatbuff[eax*2]` (0x6FAD2C1E, 0x6FAD2CB0) with eax the LENGTH, while ecx counts. So every pass reads gChatbuff[length],
//  the terminator, and neither exit is ever taken. REACHABLE on every Enter in an IME language: a line holding CJK text is parsed and run like
//  any other. It is also why the packet's name is always written -- the first scan's exit is the one path to the send that skips the parser,
//  which clears the name on entry.
//
//  BUG: the converted buffer is 260 bytes in the original, at the top of the frame, right under the return address (`sub esp, 220h`;
//  [esp+12Ch]), and three things reach past it. WideCharToMultiByte handed a line longer than 256 bytes -- 128 or more CJK characters -- fills
//  all 256, no terminator, and fails with 122, so the commands and the parser read on into the frame. "players set to %d" is written at the
//  whitespace's count, and with 244 or more leading spaces its 17 bytes run past the 260 onto the return address. And the message is copied
//  out 255 bytes at a time from wherever the parser left the text, up to 510 bytes in. The name the parser copies runs up to 49 characters
//  into D2GSPacketClt15's 16, on into the consumed head of the converted buffer, and is never read again. REACHABLE: the write is reachable in
//  the original.
//  DIFFERENCE: ours is 512 bytes, zeroed, so every one of those stays inside it, a failed conversion is terminated at 256, and the return
//  address is not written -- undefined behaviour is not reproduced. The parser's name runs into the block's own tail instead.
void __stdcall CHAT_HandleInputKey(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nChatLen;
    int32_t nSkip;
    int32_t nLen;
    int32_t nLines;
    int32_t nWidth;
    int32_t nIndex;
    int32_t nPlayers;
    int32_t i;
    uint16_t nCount;
    enum Font ePrevFont;
    char *pszText;
    D2SplittedTextStrc *pSplit;
    D2SplittedTextStrc *pLine;
    const Unicode *pwszTrimmed;
    union
    {
        D2GSPacketClt15 tPacket;
        uint8_t pRoom[3 + 256 + 50];     //  the parser's name can run 49 past szName
    } uBlock;
    char szMultiByte[512] = {};           //  260 in the original

    if (gnChatInputDrawTick < 1)
        return;

    nChatLen = Unicode::strlen(gChatbuff);
    if (pMsg->wParam == VK_BACK)
    {
        if (gnChatInputLength)
        {
            --gnChatInputLength;
            gChatbuff[gnChatInputLength].ch = 0;
        }
        goto Claim;
    }
    if (pMsg->wParam == VK_RETURN)
        goto Submit;
    if (pMsg->wParam == VK_ESCAPE)
        goto CloseChat;

    if (!gbChatInputActive && !D2WIN_10128((uint8_t)pMsg->wParam))
        goto Claim;
    if (gnChatInputLength >= 0xFF || nChatLen >= 0xFF)
        goto Claim;

    ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);
    pSplit = D2WIN_10199(gChatbuff, &nLines, gnScreenWidth - 265);
    nWidth = 0;
    for (pLine = pSplit; pLine; pLine = pLine->pNextLine)
        nWidth = D2WIN_10121_GetTextWidth(pLine->wszLine);
    D2WIN_10200(pSplit);
    if ((uint32_t)nLines <= 3 && (nLines != 3 || nWidth <= gnScreenWidth - 275))
    {
        gChatbuff[nChatLen].ch = (uint16_t)pMsg->wParam;
        pSplit = D2WIN_10199(gChatbuff, &nLines, gnScreenWidth - 265);
        nWidth = 0;
        for (pLine = pSplit; pLine; pLine = pLine->pNextLine)
            nWidth = D2WIN_10121_GetTextWidth(pLine->wszLine);
        D2WIN_10200(pSplit);
        if (nLines != 3 || nWidth < gnScreenWidth - 275)
            ++gnChatInputLength;
    }
    D2WIN_10127_SetFont(ePrevFont);
    goto Claim;

Submit:
    for (nSkip = 0; gChatbuff[nSkip].ch < 0x100 && isspace(gChatbuff[nSkip].ch); ++nSkip)
        ;
    pwszTrimmed = gChatbuff + nSkip;
    if (!Unicode::strlen(pwszTrimmed))
        goto CloseChat;

    nCount = gnChatHistoryCount;
    if (nCount == 8)
    {
        nIndex = gnChatHistoryHead;
        gnChatHistoryIndex = nIndex;
        gnChatHistoryHead = nIndex - 1;
        if (nIndex - 1 < 0)
            gnChatHistoryHead = 7;
    }
    else
    {
        nIndex = gnChatHistoryIndex;
        if (nCount)
        {
            --nIndex;
            gnChatHistoryIndex = nIndex;
            if (nIndex < 0)
            {
                nIndex = 7;
                gnChatHistoryIndex = 7;
            }
        }
        gnChatHistoryCount = (uint16_t)(nCount + 1);
    }

    {
        //  Two memcpy of the same 512 + 2 bytes into the row (the original copies forward with `rep movsd` / `movsw`); the cursor's store stays
        //  between them, as the bytes order it.
        memcpy(gChatHistory[nIndex], pwszTrimmed, 128 * sizeof(uint32_t));
        gnChatHistoryCursor = nIndex;
        memcpy((char *)gChatHistory[nIndex] + 128 * sizeof(uint32_t), (const char *)pwszTrimmed + 128 * sizeof(uint32_t), sizeof(uint16_t));
    }

    WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR | WC_DISCARDNS, (LPCWCH)gChatbuff, -1, szMultiByte, 256, NULL, NULL);
    memset(uBlock.tPacket.szMessage, 0, sizeof(uBlock.tPacket.szMessage));
    uBlock.tPacket.nLang = (uint8_t)gnChatLanguage;
    pszText = &szMultiByte[nSkip];

    if (!_strcmpi(pszText, "/fps"))
    {
        DRAW_ToggleDebugOverlay();
        goto CloseChat;
    }
    if (!_strcmpi(pszText, "/framerate"))
    {
        DRAW_GameToggleDebugOverlay();
        goto CloseChat;
    }
    //  _strnicmp is ours: the original calls its runtime's, whose C-locale arm folds A..Z alone (no lead byte is tested) and is the only arm it
    //  takes, since the DLL never leaves the "C" locale; ours folds the same.
    if (!_strnicmp(pszText, "/players", 8) && (GAME_GetGameType() == GAMETYPE_SINGLEPLAYER || GAME_GetGameType() == GAMETYPE_SINGLEPLAYER_UNCAPPED || GAME_GetGameType() == GAMETYPE_OBNET_HOST || GAME_GetGameType() == GAMETYPE_LAN_HOST))
    {
        nPlayers = atoi(pszText + 8);
        if (nPlayers <= 1)
            nPlayers = 1;
        else if (nPlayers >= 8)
            nPlayers = 8;
        D2GAME_PLAYER_SetVirtualPlayerCount(nPlayers);
        sprintf(pszText, "players set to %d", nPlayers);
    }
    if (!_strcmpi(pszText, "/nopickup"))
    {
        gbNoPickup = gbNoPickup == 0;
        goto CloseChat;
    }
    if (!_strcmpi(pszText, "soundchaosdebug"))
    {
        SOUNDINST_ToggleChaosDebug();
        goto CloseChat;
    }

    uBlock.tPacket.nMessageType = 1;
    uBlock.tPacket.nHeader = 0x15;
    uBlock.tPacket.nLang = (uint8_t)gnChatLanguage;
    if (gbChatInputActive)
    {
        nLen = Unicode::strlen(gChatbuff);
        for (i = 0; i < nLen; ++i)
        {
            if (gChatbuff[nLen].ch > 0xFF)     //  indexed by the length, as the bytes
                goto KindCheck;
        }
    }
    if (CHAT_ParseCommandPrefix((const char **)&pszText, &uBlock.tPacket.nMessageType, uBlock.tPacket.szName) && !uBlock.tPacket.nMessageType)
    {
        uBlock.tPacket.nHeader = 0x14;
        goto Send;
    }

KindCheck:
    if (uBlock.tPacket.nMessageType != 3)
        goto Send;
    if (gbChatInputActive)
    {
        nLen = Unicode::strlen(gChatbuff);
        for (i = 0; i < nLen; ++i)
        {
            if (gChatbuff[nLen].ch > 0xFF)     //  likewise
                goto Send;
        }
    }
    if (!CHAT_HandleWhisperCommand(pszText, "/msg", 1) && !CHAT_HandleWhisperCommand(pszText, "/m", 1) && !CHAT_HandleWhisperCommand(pszText, "/whisper", 1) && !CHAT_HandleWhisperCommand(pszText, "/w", 1)
        && !CHAT_RunBattleNetCommand(pszText, "/help") && !CHAT_RunBattleNetCommand(pszText, gszChatHelpCommand))
    {
        struct BnClientInterface *pBnClient = WINMAIN_GetBnClientInterface();

        if (pBnClient)
            (*(BOOL (__fastcall **)(const char *))((char *)pBnClient + 0x14))(pszText);
    }
    goto CloseChat;

Send:
    if (Unicode::strlen(gChatbuff) > 0)
    {
        memcpy(uBlock.tPacket.szMessage, pszText, 255);
        uBlock.tPacket.szMessage[255] = 0;
        MSG_SendChatMessage(&uBlock.tPacket);
    }

CloseChat:
    UI_SetUIToggle(UI_CHATBOX, UI_TURNOFF, 0);

Claim:
    pMsg->lResult = 0;
    pMsg->bReturn = 1;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAD2DB0
//
//  Reads a chat prefix; CHAT_HandleInputKey is its one caller. Clears the caller's name first (0x6FAD2DCD), so the packet's name is empty
//  unless a prefix fills it, then walks gChatCommandPrefixes, three fourteen-byte rows:
//
//      "/"   kind 3   no name   multiplayer only    matches `length >= 1`
//      "!"   kind 0   no name   single player too   needs a character after
//      "@"   kind 2   a name    multiplayer only    needs a character after
//
//  A kind-3 row answers at once, the text left as it is. The others advance the caller's text past the prefix -- and for "@", past the name
//  and one separator: SStrTokenize on " " (gszSpace), refused when nothing follows or when the name is 50 characters or more (unsigned), and
//  then copied to the caller with SStrCopy's limit 0x7FFFFFFF -- up to 49 characters into a 16-character field.
//  The loop runs a signed index as a pointer: `add esi, 0Eh` and `cmp esi, offset Delimiter / jl` (0x6FAD2E86..0x6FAD2E90), where Delimiter
//  (the " " that CHAT_HandleWhisperCommand tokenizes on) is the table's end, 0x6FB7837C + 3 * 14; edi counts beside it for the kind's lookup.
//  MSVC's form of `for (i = 0; i < 3; ++i)`, written so.
//  The text is first copied into 256 bytes with the CRT's strncpy, which leaves it unterminated when it fills them -- reachable only through
//  the unterminated conversion in CHAT_HandleInputKey, when the original's SStrLen and SStrTokenize read on into the frame.
//  DIFFERENCE: ours has a 257th byte, zero, where they stop. Every use of what they read past is a compare with a one-character prefix, a test
//  for emptiness or the 50-character refusal, which the long text meets either way.
BOOL __fastcall CHAT_ParseCommandPrefix(const char **ppszInput, uint8_t *pnKind, char *pszTargetName)
{
    char szCopy[257];
    char szName[260];                    //  the frame's slot; SStrTokenize is told 256
    char *pszCursor;
    int32_t nInputLen;
    int32_t nPrefixLen;
    int32_t i;
    D2ChatPrefixStrc *pPrefix;

    *pszTargetName = 0;
    pszCursor = szCopy;
    strncpy(szCopy, *ppszInput, 256);
    szCopy[256] = 0;                     //  ours only: the terminator the original lacks
    nInputLen = (int32_t)STORM_SStrLen(szCopy);

    for (i = 0; i < 3; ++i)
    {
        pPrefix = &gChatCommandPrefixes[i];
        nPrefixLen = (int32_t)STORM_SStrLen(pPrefix->pszPrefix);
        if (!pPrefix->bAllowedInSinglePlayer && !GAME_GetGameType())
            continue;
        if (pPrefix->nKind == 3)
        {
            if (nInputLen >= nPrefixLen && !STORM_SStrCmpI(pszCursor, pPrefix->pszPrefix, nPrefixLen))
            {
                *pnKind = (uint8_t)gChatCommandPrefixes[i].nKind;
                return TRUE;
            }
            continue;
        }
        if (nInputLen > nPrefixLen && !STORM_SStrCmpI(pszCursor, pPrefix->pszPrefix, nPrefixLen) && pszCursor[nPrefixLen])
            break;
    }
    if (i >= 3)
        return FALSE;

    pszCursor += nPrefixLen;
    if (!gChatCommandPrefixes[i].bTakesTargetName)
    {
        *ppszInput += nPrefixLen;
    }
    else
    {
        STORM_SStrTokenize((const char **)&pszCursor, szName, 256, gszSpace, NULL);
        if (!strlen(pszCursor))
            return FALSE;
        if (strlen(szName) >= 50)
            return FALSE;
        STORM_SStrCopy(pszTargetName, szName, 0x7FFFFFFF);
        *ppszInput += strlen(szName) + nPrefixLen + 1;
    }
    *pnKind = (uint8_t)gChatCommandPrefixes[i].nKind;
    return TRUE;
}


//1.10f: D2Client.0x6FAD2F60
//
//  Empties the chat history. Five stores, and the two sevens are the thing to notice: the history is a ring of EIGHT 514-byte slots and both
//  cursors start at its LAST one, so the first line written lands at index 0. Sole caller GAME_InitGame, so this runs once per game rather than
//  once per chat session.
//  The store to gnChatHistoryCount is sixteen bits (mov word ptr, ax, 0x6FAD2F76), as in CHAT_HandleInputKey; gnChatHistoryCount is a 16-bit
//  object.
//  void: the `xor eax, eax` at 0x6FAD2F6F is the ZERO BEING STORED into the three globals below it, not a chosen result; the single call site
//  does not use the value.
void __cdecl CHAT_ResetHistory()
{
    gnChatHistoryIndex = 7;
    gnChatHistoryHead = 7;
    gnChatHistoryCursor = 0;
    gnChatHistoryCount = 0;
    gnChatInputLength = 0;
}


//1.10f: D2Client.0x6FAD2F90
//
//  Empties the input line. UI_SetUIToggle calls it when the chat box is switched ON (UI_CHATBOX), after the box's handlers are registered --
//  not when a line is sent, and not when the box closes. All 514 bytes of gChatbuff (`rep stosd` of 0x80 dwords and a `stosw`), then two
//  dwords: the length, and the draw tick that CHAT_HandleInputKey waits on.
//  void: the `xor eax, eax` at 0x6FAD2FA1 is the zero stored into the two globals below it. The one call site jumps straight on to
//  CURSOR_GetMouseX, which overwrites eax.
void __cdecl CHAT_ClearInputLine()
{
    memset(gChatbuff, 0, 514);
    gnChatInputLength = 0;
    gnChatInputDrawTick = 0;
}


//  The record BnClient's message queue fills for CHAT_Update, as its frame holds it: the id at [esp+2Ch] and the text at [esp+44h]. It is
//  Bnclient.dll's record; two of its fields are read here. It has to be ONE object in the C: the queue writes both fields through the one pointer.
struct D2ChatBnMessageStrc
{
    uint32_t nEventId;          //  +0x00, the switch below
    uint32_t dwUnknown[5];      //  +0x04, not read here
    char szText[464];           //  +0x18, the sender's name, then the text after its NUL
};
static_assert(sizeof(D2ChatBnMessageStrc) == 0x1E8
                  && offsetof(D2ChatBnMessageStrc, szText) == 0x18,
              "lea edx, [esp+2Ch] at 0x6FAD2FD2; the text read at [esp+44h] "
              "(0x6FAD3044); szBaseName at [esp+214h]");


//1.10f: D2Client.0x6FAD2FB0
//
//  The Battle.net message pump. GAME_UpdateFrame calls it once a frame, after the game packets are routed, and it takes ONE message off
//  BnClient's queue: slot +0x24 of the interface WINMAIN_GetBnClientInterface returns, ecx the kind and edx the record. Nothing more is done
//  unless the call answers non-zero AND the kind it wrote is 0; then the record's event id picks the line:
//
//      1 2 3 5 7 9 21 22 23   nothing printed
//      4 6 10                 the name and the text, colour 2: tbl 5333
//                             "Sent to %s: %s" for 10, 5332 "From %s: %s"
//                             for 4 and 6
//      anything else          "<BattleNet>: " and the text, colour 3 -- 0 and
//                             every id above 23 too (`ja` at 0x6FAD3064, on
//                             the id less one, unsigned)
//
//  The ids are Battle.net's chat events: EID_WHISPER 4, EID_BROADCAST 6, EID_WHISPERSENT 10, the channel traffic among the silent ones, EID_INFO
//  18 and EID_ERROR 19 in the default arm (the protocol's public numbering).
//
//  The name: the record's text is the sender's name and then, after its NUL, the message (taken before the switch for every event). The name is
//  cut at its first '*' -- Battle.net's "character*account" -- unless it BEGINS with one, when it is copied whole (SStrCopy at 0x6FAD30E0,
//  reached from both sides). Then:
//
//      "*account"          printed as it is
//      "char*account"      "char (*account)"
//      "name", no star     "name (*name)"
//      ""                  "" -- the first test
//
//  The comparison in the middle never decides anything. It is reached only when the cut name begins with '*', which it does only when it was
//  copied whole; so both sides take the text after the same first '*' of the same string, and the inlined strcmp (0x6FAD3138..0x6FAD3161)
//  answers 0 every time. Written as the bytes have it. "abc*abc" prints "abc (*abc)". The "account" is the text after the first '*' when there
//  is text after it, and the whole name otherwise -- one idiom three times over (0x6FAD30FF, 0x6FAD3120, 0x6FAD316C), written in place three
//  times.
//
//  The text: three 340-element Unicode buffers, constructed name, message, line. Element 339 of the name and of the message is zeroed by hand
//  after each win2Unicode. The named arm formats through Unicode::sprintf bounded to 339 and zeroes the line's element 339. The default arm puts
//  "<BattleNet>: " in the name (gszBattleNetPrefix, copied byte by byte into 20 bytes), and when the two together reach 340 it cuts the MESSAGE
//  at 339 less the name's length -- `wszMessage[339 - n]` -- before the strcat. The name there is the prefix, 13 characters, so the arm that
//  empties the message for a name of 339 never runs. The colour goes in dl over whatever edx held; TEXT_PrintChatMessage stores only its low
//  byte.
//
//  DIFFERENCE: szBaseName and szDisplayName are 340 bytes in the original's frame and filled with no bound (SStrCopy's 0x7FFFFFFF, sprintf).
//  The record's text is up to 463 characters, so in the original a long name runs on into wszName and wszLine, the buffers above them -- and
//  both are written again before anything reads them, so the line printed is the same. Ours are sized to the record, 464 and 931 (463 + " (*" +
//  463 + ")" + NUL), and the copies stay inside them.
void __cdecl CHAT_Update()
{
    struct BnClientInterface *pBnClient;
    uint32_t nMessageType;
    D2ChatBnMessageStrc tMessage;
    int32_t nEventId;
    const char *pszMessage;
    const char *pszStar;
    const char *pszAccount;
    const char *pszBaseAccount;
    int32_t nMessageLen;
    int32_t nNameLen;
    char szPrefix[20];
    char szBaseName[464];               //  340 in the original's frame
    char szDisplayName[463 + 3 + 463 + 1 + 1];  //  340 in the original's frame

    pBnClient = WINMAIN_GetBnClientInterface();
    if (!pBnClient)
        return;
    if (!(*(int32_t (__fastcall **)(uint32_t *, void *))((char *)pBnClient + 0x24))(&nMessageType, &tMessage))
        return;
    if (nMessageType)
        return;

    Unicode wszName[340];
    Unicode wszMessage[340];
    Unicode wszLine[340];

    nEventId = (int32_t)tMessage.nEventId;
    pszMessage = &tMessage.szText[strlen(tMessage.szText) + 1];
    switch (nEventId)
    {
    case 1:
    case 2:
    case 3:
    case 5:
    case 7:
    case 9:
    case 21:
    case 22:
    case 23:
        return;

    case 4:
    case 6:
    case 10:
        if (tMessage.szText[0] == '*' || !(pszStar = STORM_SStrChr(tMessage.szText, '*')))
        {
            STORM_SStrCopy(szBaseName, tMessage.szText, 0x7FFFFFFF);
        }
        else
        {
            memcpy(szBaseName, tMessage.szText, pszStar - tMessage.szText);
            szBaseName[pszStar - tMessage.szText] = 0;
        }
        if (!szBaseName[0])
            goto PlainName;
        if (szBaseName[0] == '*')
        {
            pszAccount = STORM_SStrChr(tMessage.szText, '*');
            if (!pszAccount || !pszAccount[1])
                pszAccount = tMessage.szText;
            else
                ++pszAccount;
            pszBaseAccount = STORM_SStrChr(szBaseName, '*');
            if (!pszBaseAccount || !pszBaseAccount[1])
                pszBaseAccount = szBaseName;
            else
                ++pszBaseAccount;
            if (!strcmp(pszBaseAccount, pszAccount))     //  always 0, as the bytes have it
                goto PlainName;
        }
        pszAccount = STORM_SStrChr(tMessage.szText, '*');
        if (!pszAccount || !pszAccount[1])
            pszAccount = tMessage.szText;
        else
            ++pszAccount;
        sprintf(szDisplayName, "%s (*%s)", szBaseName, pszAccount);
        goto Convert;

    PlainName:
        sprintf(szDisplayName, "%s", szBaseName);

    Convert:
        Unicode::win2Unicode(wszName, szDisplayName, 340);
        Unicode::win2Unicode(wszMessage, pszMessage, 340);
        wszName[339].ch = 0;
        wszMessage[339].ch = 0;
        Unicode::sprintf(339, wszLine, D2LANG_GetStringFromTblIndex(nEventId == 10 ? STR_IDX_5333_Sent_to_s_s : STR_IDX_5332_From_s_s), wszName, wszMessage);
        wszLine[339].ch = 0;
        TEXT_PrintChatMessage(wszLine, 2);
        return;

    default:
        strcpy(szPrefix, gszBattleNetPrefix);
        Unicode::win2Unicode(wszName, szPrefix, 340);
        Unicode::win2Unicode(wszMessage, pszMessage, 340);
        wszName[339].ch = 0;
        wszMessage[339].ch = 0;
        nMessageLen = Unicode::strlen(wszMessage);
        if (Unicode::strlen(wszName) + nMessageLen >= 340)
        {
            nNameLen = Unicode::strlen(wszName);
            if (nNameLen >= 339)
                wszMessage[0].ch = 0;
            else
                wszMessage[339 - nNameLen].ch = 0;
        }
        Unicode::strcat(wszName, wszMessage);
        wszName[339].ch = 0;
        TEXT_PrintChatMessage(wszName, 3);
        return;
    }
}


//1.10f: D2Client.0x6FAD3350
//
//  One Battle.net command; CHAT_HandleInputKey calls it twice, for "/help" and for gszChatHelpCommand "/?", after the four whisper commands.
//  TRUE when the input was this command, whatever happened to it; FALSE when it was not. Every exit is `mov eax, 1` or `xor eax, eax`, and both
//  call sites test it.
//
//  The match is by hand: the input no shorter than the command (unsigned lengths, `jb` 0x6FAD338C), then each character through the CRT's
//  toupper, both sides sign-extended (`movsx`) -- the bytes step the command and reach the input by the distance between the two, the C
//  indexes both -- and the character after the command a space or the end, so "/helpme" is not "/help". Then strtok on a local " " -- the word
//  at gszSpace copied at 0x6FAD33E9, the shape of `char sz[] = " "` -- counting tokens to two: exactly ONE, the command alone, is handed to
//  BnClient's ChatCommand (+0x14) as it now stands, cut at its first space; none, or two and more, print the usage. strtok and toupper are
//  ours; the original's toupper is the "C" locale's, as ours is.
//
//  The usage line is a server packet with holes in it: "<BattleNet>: Correct usage is: %s" goes into a D2GSPacketSrv26 on the stack -- message
//  type 4, colour 3, the text's 256 bytes at +0x1A -- and to TEXT_HandleChatMessage, as if the server had sent it. Nothing else of the packet
//  is written, and for type 4 TEXT_HandleChatMessage reads three of the holes before it prints: nLang (+2) picks the conversion, which for
//  this ASCII line gives the same text either way; and nUnitType (+3), when it is 0, sends dwUnitGUID (+4) to ROSTER_IsUnitNotSquelched and
//  DROPS the line if that is a squelched roster member's GUID.
//  BUG: the holes are not random. The packet is at -21Ch in this frame, and CHAT_HandleInputKey calls this function and
//  CHAT_ParseCommandPrefix from one stack depth, so bytes 0..7 of the packet are the two registers the parser's prologue pushed there
//  (0x6FAD2DB6, 0x6FAD2DB7): ebp, the handler's count of leading whitespace, and ebx, its Unicode::strlen(gChatbuff) (0x6FAD28BC) -- neither
//  written again on the Enter path. So nLang and nUnitType are 0, the count's high bytes, and the GUID tested is THE LENGTH OF THE LINE TYPED.
//  REACHABLE: with the player whose GUID is 7 squelched, "/help x" prints nothing; player GUIDs start at 1 and count up, so a game's players
//  carry the small numbers that are ordinary line lengths.
//  DIFFERENCE: reading the bytes uninitialised is undefined in C, so ours clears the packet and stores the one value that decides, the line's
//  length measured again (gChatbuff is not written between the handler's measurement and here). nLang and nUnitType are then the 0 the
//  original has. That Unicode::strlen call is ours, not the original's.
BOOL __fastcall CHAT_RunBattleNetCommand(char *pszInput, const char *pszCommand)
{
    size_t nCommandLen;
    size_t nLeft;
    size_t i;
    int32_t nTokens;
    struct BnClientInterface *pBnClient;
    D2GSPacketSrv26 tUsage;
    char szUsage[256];

    nCommandLen = strlen(pszCommand);
    if (strlen(pszInput) < strlen(pszCommand))
        return FALSE;
    nLeft = strlen(pszCommand);
    for (i = 0; nLeft; ++i)
    {
        --nLeft;
        if (toupper(pszInput[i]) != toupper(pszCommand[i]))
            return FALSE;
    }
    if (pszInput[nCommandLen] != ' ' && pszInput[nCommandLen])
        return FALSE;

    char szDelimiter[] = " ";
    nTokens = 0;
    if (strtok(pszInput, szDelimiter))
    {
        do
        {
            if (nTokens >= 2)
                break;
            ++nTokens;
        } while (strtok(NULL, szDelimiter));
        if (nTokens == 1)
        {
            pBnClient = WINMAIN_GetBnClientInterface();
            if (pBnClient)
                (*(BOOL (__fastcall **)(const char *))((char *)pBnClient + 0x14))(pszInput);
            return TRUE;
        }
    }

    memset(&tUsage, 0, sizeof(tUsage));     //  ours only: the original's holes
    tUsage.dwUnitGUID = (uint32_t)Unicode::strlen(gChatbuff);     //  the hole's value
    tUsage.nMessageType = 4;
    tUsage.nMessageColor = 3;
    sprintf(szUsage, "<BattleNet>: Correct usage is: %s", pszCommand);
    memcpy(tUsage.szMessage, szUsage, 256);
    TEXT_HandleChatMessage(&tUsage);
    return TRUE;
}


//1.10f: D2Client.0x6FAD34A0
//
//  The whisper commands; CHAT_HandleInputKey calls it four times, for "/msg", "/m", "/whisper" and "/w", each with bWhisperInGame 1, so the
//  `!bWhisperInGame` path is taken by none of them. TRUE when the input was this command, FALSE when not. `retn 4` purges bWhisperInGame.
//
//  The match is CHAT_RunBattleNetCommand's. Then the input is copied whole into a block (the CRT's strcpy inlined) and tokenized in place on
//  the " " at Delimiter: token 2 is the NAME, copied out; at token 3 the MESSAGE is copied from the COPY, at the token's offset, so it keeps its
//  spaces -- the rest of the line. After the loop the bytes test for three tokens again (`cmp esi, 3` at 0x6FAD358E) on a path where it cannot
//  hold, which is the shape of `while (token) {...} if (n != 3)` with a `break` at three, and is written so.
//  Fewer than three tokens print "<BattleNet>: Correct usage is: %s (name) (message)" through the same holed D2GSPacketSrv26 as
//  CHAT_RunBattleNetCommand, but here the packet is at -21Ch of a frame that starts four bytes lower, so its holes are the parser's saved esi
//  and ebp: nLang and nUnitType are the top two bytes of D2Lang's Unicode::strlen address, which the handler holds in esi (0x6FAD2BF9) -- 0x6F
//  on top at D2Lang's base -- and no squelch test is made. The line always prints.
//  DIFFERENCE: ours zeroes the packet, which tests GUID 0, never issued, so it always prints too; a difference in the bytes and none in what is
//  shown.
//
//  Where the whisper goes: with bWhisperInGame set the party roster is walked (ROSTER_PartyGetNextEntry, ROSTER_GetDisplayName, the CRT's
//  _strcmpi), and a member of that name gets a D2GSPacketClt15 built in the same block -- 0x15, message type 2, gnChatLanguage's low byte, the
//  message by strncpy at 255 and closed at +258, the name at +259 -- sent by MSG_SendChatMessage through the GAME server. Anyone else gets the
//  line rebuilt over the caller's input as "%s %s %s" -- the command as spelled here, the name, the message -- and handed to BnClient's
//  ChatCommand (+0x14). The rebuilt line is never longer than the input: the command has the input's length, strtok collapsed each gap to one
//  space, and the message is the input's own tail.
//  DIFFERENCE: the block is 276 bytes in the original's frame, and the name is copied to +259 until its NUL: a name of 17 characters or more
//  runs past the block into szTargetName, the very buffer being read -- each write lands 17 bytes behind the read, so the copy finishes intact,
//  and the overwritten head of szTargetName is not read again. MSG_SendChatMessage then takes at most 15 characters of it (SStrCopy's 16).
//  Ours has room for the longest name the line can hold, and the same packet is sent. The input itself outgrows the block only through the
//  unterminated conversion in CHAT_HandleInputKey; there the original's copies read on through that function's frame, and ours stop at its 256.
BOOL __fastcall CHAT_HandleWhisperCommand(char *szInput, const char *szCommand, int32_t bWhisperInGame)
{
    size_t nCommandLen;
    size_t nLeft;
    size_t i;
    int32_t nTokens;
    char *pszToken;
    D2RosterUnitStrc *pEntry;
    struct BnClientInterface *pBnClient;
    union
    {
        D2GSPacketClt15 tPacket;
        char szLine[259 + 256];         //  276 in the original's frame
    } uBlock;
    char szTargetName[256];
    D2GSPacketSrv26 tUsage;
    char szMessage[256];

    nCommandLen = strlen(szCommand);
    if (strlen(szInput) < strlen(szCommand))
        return FALSE;
    nLeft = strlen(szCommand);
    for (i = 0; nLeft; ++i)
    {
        --nLeft;
        if (toupper(szInput[i]) != toupper(szCommand[i]))
            return FALSE;
    }
    if (szInput[nCommandLen] != ' ' && szInput[nCommandLen])
        return FALSE;

    strcpy(uBlock.szLine, szInput);
    pszToken = strtok(szInput, " ");
    nTokens = 0;
    while (pszToken)
    {
        ++nTokens;
        if (nTokens == 2)
        {
            strcpy(szTargetName, pszToken);
        }
        else if (nTokens == 3)
        {
            strcpy(szMessage, &uBlock.szLine[pszToken - szInput]);
            break;
        }
        pszToken = strtok(NULL, " ");
    }
    if (nTokens != 3)
    {
        memset(&tUsage, 0, sizeof(tUsage));     //  ours only: the original's holes
        tUsage.nMessageType = 4;
        tUsage.nMessageColor = 3;
        sprintf(uBlock.szLine, "<BattleNet>: Correct usage is: %s (name) (message)", szCommand);
        memcpy(tUsage.szMessage, uBlock.szLine, 256);
        TEXT_HandleChatMessage(&tUsage);
        return TRUE;
    }

    if (bWhisperInGame)
    {
        for (pEntry = ROSTER_PartyGetNextEntry(NULL); pEntry; pEntry = ROSTER_PartyGetNextEntry(pEntry))
        {
            if (!_strcmpi(ROSTER_GetDisplayName(pEntry), szTargetName))
            {
                strncpy(uBlock.tPacket.szMessage, szMessage, 255);
                uBlock.tPacket.szMessage[255] = 0;
                strcpy(&uBlock.szLine[offsetof(D2GSPacketClt15, szName)], szTargetName);
                uBlock.tPacket.nMessageType = 2;
                uBlock.tPacket.nLang = (uint8_t)gnChatLanguage;
                uBlock.tPacket.nHeader = 0x15;
                MSG_SendChatMessage(&uBlock.tPacket);
                return TRUE;
            }
        }
    }

    sprintf(szInput, "%s %s %s", szCommand, szTargetName, szMessage);
    pBnClient = WINMAIN_GetBnClientInterface();
    if (pBnClient)
        (*(BOOL (__fastcall **)(const char *))((char *)pBnClient + 0x14))(szInput);
    return TRUE;
}
