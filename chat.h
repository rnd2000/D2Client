//  chat.h -- the globals of chat.cpp that are ours: the chat input line's handler tables, the history ring and cursors, the blink and IME
//  state, the command strings and the input buffers.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global
//  gives its address in the original. The four Unicode buffers (gChatbuff, gwszImePrefix, gChatHistory, gwszImeComposition) are arrays of
//  `Unicode`, built by static initialisation.
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB782F0
//  The chat input line's table, 4 records, registered by CHAT_RegisterInputHandlers and dropped by CHAT_UnregisterHandlers:
//  WM_CHAR CHAT_HandleInputKey, VK_UP and VK_DOWN CHAT_OnHistoryKey (one handler, which takes the direction from the message),
//  WM_LBUTTONDOWN CHAT_OnLButtonDown.
inline D2WinCommandRegisterStrc gChatMessageHandlers[4] =
{
    { D2_WINPROC_MESSAGE,   WM_CHAR,                    &CHAT_HandleInputKey },
    { D2_WINPROC_KEYDOWN,   VK_UP,                      &CHAT_OnHistoryKey },
    { D2_WINPROC_KEYDOWN,   VK_DOWN,                    &CHAT_OnHistoryKey },
    { D2_WINPROC_MESSAGE,   WM_LBUTTONDOWN,             &CHAT_OnLButtonDown },
};
static_assert(sizeof(gChatMessageHandlers) == 48, "gChatMessageHandlers: the item's 48 bytes in the .i64");

//  D2Client 0x6FB78320
//  The chat line's IME table, 7 records, registered by CHAT_RegisterInputHandlers and dropped by CHAT_UnregisterHandlers: WM_IME_NOTIFY,
//  WM_IME_COMPOSITION, WM_IME_STARTCOMPOSITION and WM_IME_ENDCOMPOSITION to the four CHAT_OnIme* handlers, then 0x284, 0x285 and 0x283 to
//  CHAT_ImeIgnoreMessage. This is what lets the Japanese, Korean and Chinese builds compose text in the chat line.
inline D2WinCommandRegisterStrc gChatImeHandlers[7] =
{
    { D2_WINPROC_MESSAGE, WM_IME_NOTIFY, &CHAT_OnImeNotify },
    { D2_WINPROC_MESSAGE, WM_IME_COMPOSITION, &CHAT_OnImeComposition },
    { D2_WINPROC_MESSAGE, WM_IME_STARTCOMPOSITION, &CHAT_OnImeStartComposition },
    { D2_WINPROC_MESSAGE, WM_IME_ENDCOMPOSITION, &CHAT_OnImeEndComposition },
    { D2_WINPROC_MESSAGE, 0x284, &CHAT_ImeIgnoreMessage },
    { D2_WINPROC_MESSAGE, 0x285, &CHAT_ImeIgnoreMessage },
    { D2_WINPROC_MESSAGE, 0x283, &CHAT_ImeIgnoreMessage },
};
static_assert(sizeof(gChatImeHandlers) == 84, "gChatImeHandlers: the item's 84 bytes in the .i64");

//  D2Client 0x6FB782E8
//  The write head of the chat history: the slot CHAT_HandleInputKey stores the next committed line into. It is decremented there (wrapping from
//  0 to 7) and reset to 7 by CHAT_ResetHistory. It is not gnChatHistoryCursor, the arrow keys' cursor, which CHAT_HandleInputKey and
//  CHAT_ResetHistory write too.
inline int32_t gnChatHistoryIndex = 7;
static_assert(sizeof(gnChatHistoryIndex) == 4, "gnChatHistoryIndex: the item's 4 bytes in the .i64");

//  D2Client 0x6FB782EC
//  Write position in the 8-slot chat history ring once it is full: decremented and wrapped to 7 from below, so the ring is walked backwards.
inline int32_t gnChatHistoryHead = 7;
static_assert(sizeof(gnChatHistoryHead) == 4, "gnChatHistoryHead: the item's 4 bytes in the .i64");

//  D2Client 0x6FB78374
//  The chat input line's blink state, 0 or 1. Half of a pair with gnChatInputBlinkMs: the state flips once GetTickCount has moved past
//  gnChatBlinkLastTick by that interval (0x6FAD2583 sets the interval to 100 and the state to 0, 0x6FAD25A6 to 500 and 1).
//  CHAT_DrawImeComposition drives the same pair, so the composition line blinks on the same clock as the input line.
inline BOOL gbChatInputBlinkOn = 1;
static_assert(sizeof(gbChatInputBlinkOn) == 4, "gbChatInputBlinkOn: the item's 4 bytes in the .i64");

//  D2Client 0x6FB78378
//  The chat blink interval in milliseconds, 100 or 500: one phase lasts 100 ms and the other 500 ms. Written as a word, read as a dword
//  against GetTickCount and masked to 16 bits. Pairs with gbChatInputBlinkOn.
inline uint16_t gnChatInputBlinkMs = 500;
static_assert(sizeof(gnChatInputBlinkMs) == 2, "gnChatInputBlinkMs: the item's 2 bytes in the .i64");

//  D2Client 0x6FB7837C
//  The three chat command prefixes the client recognises -- "/", "!" and "@" -- read only by CHAT_ParseCommandPrefix. The walk's terminator is
//  the address of Delimiter, the one-character string that follows the table. "@" is the only one that takes a name after it, and "!" is the
//  only one allowed in a single-player game.
inline D2ChatPrefixStrc gChatCommandPrefixes[3] =
{ { .pszPrefix = "/", .nKind = 3 }, { .pszPrefix = "!", .bAllowedInSinglePlayer = 1 }, { .pszPrefix = "@", .nKind = 2, .bTakesTargetName = 1 } };
static_assert(sizeof(gChatCommandPrefixes) == 42, "gChatCommandPrefixes: the item's 42 bytes in the .i64");

//  D2Client 0x6FB783A6
//  The one-character string that ends the prefix walk; also the " " that CHAT_HandleWhisperCommand tokenizes on.
inline char Delimiter[2] = " ";
static_assert(sizeof(Delimiter) == 2, "Delimiter: the item's 2 bytes in the .i64");

//  D2Client 0x6FB78404
//  The string "/?", a chat command prefix compared against typed input, read once by CHAT_HandleInputKey.
inline char gszChatHelpCommand[3] = "/?";
static_assert(sizeof(gszChatHelpCommand) == 3, "gszChatHelpCommand: the item's 3 bytes in the .i64");

//  D2Client 0x6FB78408
inline char pszCommand[6] = "/help";
static_assert(sizeof(pszCommand) == 6, "pszCommand: the item's 6 bytes in the .i64");

//  D2Client 0x6FB78424
inline char szCommand[5] = "/msg";
static_assert(sizeof(szCommand) == 5, "szCommand: the item's 5 bytes in the .i64");

//  D2Client 0x6FB7845C
inline char pStr2[9] = "/players";
static_assert(sizeof(pStr2) == 9, "pStr2: the item's 9 bytes in the .i64");

//  D2Client 0x6FB78474
inline char String2[5] = "/fps";
static_assert(sizeof(String2) == 5, "String2: the item's 5 bytes in the .i64");

//  D2Client 0x6FB7847C
//  The string " " (a space and its terminator): a separator shared by the chat parser (CHAT_ParseCommandPrefix, CHAT_RunBattleNetCommand) and
//  MONUNIQUE_BuildMonsterNameText, each of which takes it a different way (a pointer, a word, an index).
inline char gszSpace[2] = " ";
static_assert(sizeof(gszSpace) == 2, "gszSpace: the item's 2 bytes in the .i64");

//  D2Client 0x6FB78480
//  The string "<BattleNet>: ", the sender prefix on a Battle.net system line, read once by CHAT_Update.
inline char gszBattleNetPrefix[14] = "<BattleNet>: ";
static_assert(sizeof(gszBattleNetPrefix) == 14, "gszBattleNetPrefix: the item's 14 bytes in the .i64");

//  D2Client 0x6FBB1A58
//  The chat input buffer: 257 Unicode characters (514 bytes; CHAT_ClearInputLine clears 128 dwords and a word).
inline Unicode gChatbuff[257];
static_assert(sizeof(gChatbuff) == 514, "gChatbuff: the item's 514 bytes in the .i64");

//  D2Client 0x6FBB1C60
//  Drawn immediately before gwszImeComposition by CHAT_DrawImeComposition, and appended to gChatbuff before the committed IME result by
//  CHAT_OnImeComposition. Nothing ever fills it (only the first character is cleared to zero), so it is always the empty string and both
//  uses are dead. What it was meant to hold is not established.
inline Unicode gwszImePrefix[32];
static_assert(sizeof(gwszImePrefix) == 64, "gwszImePrefix: the item's 64 bytes in the .i64");

//  D2Client 0x6FBB1CA0
//  Which slot of the 8-entry chat history the arrow keys are currently on. CHAT_OnHistoryKey walks it -- VK_UP increments, VK_DOWN decrements,
//  each wrapping, past 7 back to 0 and below 0 up to 7 -- and copies gChatHistory[cursor] into gChatbuff and sets gnChatInputLength from its
//  length (on VK_UP the copy runs before the step, on VK_DOWN after it). Both directions undo the step if it would land on a slot never
//  filled: `(WORD)gnChatHistoryCount < 7 && 8 - cursor > gnChatHistoryCount`. CHAT_HandleInputKey resets it when a line is committed,
//  CHAT_ResetHistory clears it. It is not gnChatHistoryIndex, the write head.
inline int32_t gnChatHistoryCursor;
static_assert(sizeof(gnChatHistoryCursor) == 4, "gnChatHistoryCursor: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1CA8
//  The chat command history: 8 entries of 514 bytes, each a 257-character Unicode line. Read only by CHAT_OnHistoryKey, which copies 0x202
//  bytes of the selected entry into gChatbuff; written by CHAT_HandleInputKey when a line is committed.
inline Unicode gChatHistory[8][257];
static_assert(sizeof(gChatHistory) == 4112, "gChatHistory: the item's 4112 bytes in the .i64");

//  D2Client 0x6FBB2CB8
//  The IME composition string being typed into the chat line (GCS_COMPSTR), copied in by CHAT_OnImeComposition and drawn by
//  CHAT_DrawImeComposition. Cleared by both IME start and end, by CHAT_RegisterInputHandlers and by CHAT_UnregisterHandlers. 32 Unicode
//  characters; the overflow path of CHAT_OnImeComposition fills 31 and writes the terminator into element [31] by hand.
inline Unicode gwszImeComposition[32];
static_assert(sizeof(gwszImeComposition) == 64, "gwszImeComposition: the item's 64 bytes in the .i64");

//  D2Client 0x6FBB2CF8
//  Caret blink state for the chat input line; when this and gbChatInputActive are both set the caret is skipped.
inline uint8_t gbChatCaretVisible;
static_assert(sizeof(gbChatCaretVisible) == 1, "gbChatCaretVisible: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB2CFC
//  STRTABLE_GetLanguage(), cached by CHAT_RegisterInputHandlers at startup and read wherever chat needs to know which build it is. It is also
//  byte 2 of the outgoing chat packet that CHAT_HandleWhisperCommand builds (client packet 0x15), so the sender's language travels with every
//  whisper.
inline D2C_Language gnChatLanguage;
static_assert(sizeof(gnChatLanguage) == 4, "gnChatLanguage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB2D00
//  How many of gChatHistory's eight slots have been filled. Only the low word is used: it is written as a word by CHAT_HandleInputKey and
//  CHAT_ResetHistory, and CHAT_OnHistoryKey loads it as a dword and uses the low word. Its job is to stop the arrow keys walking onto slots
//  that have never held a line (`(WORD)count < 7 && 8 - cursor > count` undoes the step); once seven are stored the guard stops applying and
//  the ring wraps without checking.
inline uint16_t gnChatHistoryCount;
static_assert(sizeof(gnChatHistoryCount) == 2, "gnChatHistoryCount: the item's 2 bytes in the .i64");

//  D2Client 0x6FBB2D04
//  The characters currently in the chat input line. Reset to zero by CHAT_ClearInputLine along with gChatbuff; incremented on a keypress,
//  increased by the whole length when an IME composition commits several at once, and capped by the test `cmp gnChatInputLength, 0FFh`
//  (0x6FAD28F5): 255 is the limit.
inline uint32_t gnChatInputLength;
static_assert(sizeof(gnChatInputLength) == 4, "gnChatInputLength: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB2D08
//  Incremented once per CHAT_DrawInputLine call: a frame counter for the input line, not a position. Reset to 0 by CHAT_ClearInputLine beside
//  gnChatInputLength, and set to -2 by CHAT_OnImeEndComposition, so ending a composition puts it two frames behind zero. CHAT_HandleInputKey
//  reads it. What it is for beyond counting is not established (a caret blink is the obvious guess).
inline int32_t gnChatInputDrawTick;
static_assert(sizeof(gnChatInputDrawTick) == 4, "gnChatInputDrawTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB2D0C
//  Which IME the chat input is driving, set by CHAT_RegisterInputHandlers once the system locale has been checked: 0 none, 1 Korean,
//  3 Japanese, 4 Chinese traditional, 5 Chinese simplified. The values are not the language ids: languages 7 and 12 both map to mode 1 (only
//  where `test [esp+14h], bl` at 0x6FAD1F3E does not jump, otherwise 0), and the two Chinese languages 8 and 9 map to 5 and 4, in the opposite
//  order to their language numbers.
inline int32_t gnChatImeMode;
static_assert(sizeof(gnChatImeMode) == 4, "gnChatImeMode: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB2D10
//  The Win32 IME candidate list. Not a linked list: the "list" is CANDIDATELIST's own dwOffset[] array, each entry a byte offset from the head
//  of the block to a null-terminated ANSI string. CHAT_OnImeNotify case 5 (IMN_OPENCANDIDATE) allocates it and fills it with
//  ImmGetCandidateListA; it is freed and zeroed on IMN_CLOSECANDIDATE, and dropped at once when dwStyle == 2 (IME_CAND_CODE) and dwCount == 1.
//  CHAT_DrawInputLine bounds the page with dwPageStart / dwPageSize / dwCount and clamps to 20 when dwPageSize is 0.
inline CANDIDATELIST *gpChatCandidateList;
static_assert(sizeof(gpChatCandidateList) == 4, "gpChatCandidateList: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB2D14
//  Whether the IME handler table is registered. Its one writer, CHAT_RegisterInputHandlers, clears it for an IME language (0x6FAD1DEF) and
//  sets it on the four IME paths; CHAT_UnregisterHandlers takes the IME table down only when it is set. In CHAT_HandleInputKey it lets any
//  character past D2Win #10128's filter and gates the two wide-character scans; in CHAT_DrawInputLine it gates the IME mode label and the
//  candidate list, and, with gbChatCaretVisible, the composition in place of the caret.
inline uint8_t gbChatInputActive;
static_assert(sizeof(gbChatInputActive) == 1, "gbChatInputActive: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB2D18
//  The tick at which the chat caret last toggled -- the third member of the blink triple with gbChatInputBlinkOn and gnChatInputBlinkMs. The
//  caret toggles when GetTickCount minus this tick passes the interval (`sub ecx, ebp / cmp ecx, edx / jbe`, the interval masked to a word by
//  `and edx, 0FFFFh`), and the tick is then stored back: a deadline in the only form that survives GetTickCount wrapping.
//  CHAT_DrawImeComposition runs the identical sequence, so the composition caret blinks off the same clock.
inline uint32_t gnChatBlinkLastTick;
static_assert(sizeof(gnChatBlinkLastTick) == 4, "gnChatBlinkLastTick: the item's 4 bytes in the .i64");
