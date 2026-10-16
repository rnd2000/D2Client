#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "WINMAIN.h"
#include "Cursor.h"
#include "Msg.h"

//  Msg.cpp -- the packets D2Client.dll 1.10 sends around a game and the join and leave message pumps: the session packets (new game, join,
//  act loaded, end game, the 0x6C keepalive, the load-progress pair, the character upload), MSG_SendPacketToServer with its eleven senders,
//  the callbacks of the join and leave pumps, and MSG_AppModeJoinGame and MSG_AppModeLeaveGame.
//  The original source file is GAME\Msg.cpp (MSG_Send_D2CLTSYS_OPENCHAR and MSG_SendPacketToServer assert with its path).
//  Address range from MSG_Send_D2CLTSYS_NEWGAME at 0x6FAAD260 to MSG_AppModeLeaveGame at 0x6FAADE60.

//  The original's source path, as its asserts push it: one object for every assert of this unit.
static const char szMsgCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\Msg.cpp";


//  Session packets, 0x6FAAD260..0x6FAAD556: the packets the client sends around joining and leaving a game, plus the load-progress pair.
//  Unlike the senders further down, five of these call D2Net themselves (`push 0` as its first argument, where MSG_SendPacketToServer
//  passes 1) and so skip the duplicate window; each keeps gPacketAllSize and gPacketCounter itself, in the same two stores.
//  The ids are D2C_ClientSystemMessage: 0x66 NEWGAME, 0x67 JOINGAME, 0x68 ENDGAME, 0x6A JOINACT, 0x6B OPENCHAR. D2CLTSYS_0x6C is a
//  placeholder (no string names 0x6C), and its sender is MSG_SendPing6C.
//  MSG_Send_D2CLTSYS_JOINACT, _JOINGAME and _ENDGAME are void: the original leaves gPacketCounter + 1 in eax for the first two and the
//  game type for _ENDGAME, and no caller reads it.

static_assert(D2CLTSYS_NEWGAME == 0x66 && D2CLTSYS_JOINACT == 0x6A
              && D2CLTSYS_0x6C == 0x6C && D2CLTSYS_ENDGAME == 0x68
              && D2CLTSYS_JOINGAME == 0x67 && D2CLTSYS_OPENCHAR == 0x6B,
              "the header bytes the senders store: `mov [esp+..], 66h` "
              "0x6FAAD26E, 6Ah 0x6FAAD38A, 6Ch 0x6FAAD3D3, 68h 0x6FAAD42A, "
              "67h 0x6FAAD4B3, 6Bh 0x6FAAD6B5 and 0x6FAAD77C");

//1.10f: D2Client.0x6FAAD260
//  Sends the host's create-game packet, client packet 0x66 (D2CLTSYS_NEWGAME), 46 bytes. Sole caller GAME_AppModeGame, for every game
//  type except the three that send 0x67 instead (BNET, OBNET_JOIN, LAN_JOIN).
//  The game-type byte is 3 for SINGLEPLAYER, 1 for OBNET_HOST, 2 for LAN_HOST and 0 for every other type.
//  If the launch config's nArenaFlags is 0 it is set to 4 and then OR'd with 0x100000, in gpLaunchConfig itself and not in a copy, so the
//  default outlives this packet; the global is written each time because the original reloads it before each of the stores.
//  BUG: both names are copied with SStrCopy(..., 0x7FFFFFFF). The game name is the caller's szJoinGameName, 24 bytes, into szGameName[16]:
//  a name of 16 to 23 characters runs on into nGameType..szClientName, which are stored after the copy and overwrite it, so the packet stays
//  46 bytes but szGameName then has no terminator. NOT ESTABLISHED: GAME_ApplyParamJoinInfo copies the launch config's own 24-byte field
//  unbounded, so D2Client admits 23 characters and only D2Launch can say whether it hands over more than 15.
void __fastcall MSG_Send_D2CLTSYS_NEWGAME(const char *szName)
{
    D2GSPacketClt66 tPacket;
    D2ConfigStrc *pConfig;

    tPacket.nHeader = D2CLTSYS_NEWGAME;
    STORM_SStrCopy(tPacket.szGameName, szName, 0x7FFFFFFF);

    switch (gnGameType)
    {
    case GAMETYPE_SINGLEPLAYER:
        tPacket.nGameType = 3;
        break;

    case GAMETYPE_OBNET_HOST:
        tPacket.nGameType = 1;
        break;

    case GAMETYPE_LAN_HOST:
        tPacket.nGameType = 2;
        break;

    default:
        tPacket.nGameType = 0;
        break;
    }

    if (ARENA_IsArenaPlayerClassForced())
        tPacket.nPlayerClass = (uint8_t)ARENA_GetArenaPlayerClass();
    else
        tPacket.nPlayerClass = gnPlayerClass;

    tPacket.nLocale = (int8_t)D2LANG_STRTABLE_GetLanguage();

    if (!(gpLaunchConfig)->nArenaFlags)
    {
        (gpLaunchConfig)->nArenaFlags = 4;
        (gpLaunchConfig)->nArenaFlags |= 0x100000;
    }
    pConfig = gpLaunchConfig;

    tPacket.wArena = (int16_t)pConfig->wArena;
    tPacket.nGameFlags = pConfig->nArenaFlags;
    tPacket.nTemplate = (int8_t)pConfig->nArenaTemplate;
    tPacket.unk0x2B = (int8_t)pConfig->_0209[0];
    tPacket.unk0x2C = (int8_t)pConfig->_0209[1];
    tPacket.nDifficulty = (int8_t)pConfig->nArenaDifficulty;
    STORM_SStrCopy(tPacket.szClientName, gLocalPlayerName, 0x7FFFFFFF);

    D2NET_CLIENT_Send(0, &tPacket.nHeader, (int32_t)sizeof(tPacket));
    gPacketAllSize += sizeof(tPacket);
    ++gPacketCounter;
}

static_assert(offsetof(D2GSPacketClt66, szGameName) == 0x01
              && offsetof(D2GSPacketClt66, nGameType) == 0x11
              && offsetof(D2GSPacketClt66, nPlayerClass) == 0x12
              && offsetof(D2GSPacketClt66, nTemplate) == 0x13
              && offsetof(D2GSPacketClt66, nDifficulty) == 0x14
              && offsetof(D2GSPacketClt66, szClientName) == 0x15
              && offsetof(D2GSPacketClt66, wArena) == 0x25
              && offsetof(D2GSPacketClt66, nGameFlags) == 0x27
              && offsetof(D2GSPacketClt66, unk0x2B) == 0x2B
              && offsetof(D2GSPacketClt66, unk0x2C) == 0x2C
              && offsetof(D2GSPacketClt66, nLocale) == 0x2D,
              "MSG_Send_D2CLTSYS_NEWGAME's stores: pDest -2Fh, "
              "var_1F..var_1B, var_B, var_9, var_5, var_4, var_3 against "
              "pBuffer at -30h");
static_assert(offsetof(D2ConfigStrc, wArena) == 0x202
              && offsetof(D2ConfigStrc, nArenaFlags) == 0x204
              && offsetof(D2ConfigStrc, nArenaTemplate) == 0x208
              && offsetof(D2ConfigStrc, _0209) == 0x209
              && offsetof(D2ConfigStrc, nArenaDifficulty) == 0x20B,
              "the launch config's arena fields, [eax+202h] to [eax+20Bh] at "
              "0x6FAAD302..0x6FAAD33B");


//1.10f: D2Client.0x6FAAD380
//  Sends client packet 0x6A (D2CLTSYS_JOINACT), one byte: the client's answer to the server's act-loaded packet, sent by
//  SCMD_HandleSrv02_ActLoaded. It takes no argument: `push ecx` only makes the slot, and the packet is its top byte, as in MSG_SendCmd1.
void __cdecl MSG_Send_D2CLTSYS_JOINACT()
{
    D2GSPacketClt6A tPacket;

    tPacket.nHeader = D2CLTSYS_JOINACT;
    D2NET_CLIENT_Send(0, &tPacket.nHeader, (int32_t)sizeof(tPacket));
    ++gPacketAllSize;
    ++gPacketCounter;
}


//1.10f: D2Client.0x6FAAD3B0
//  Sends the keepalive, client packet 0x6C, nine bytes, at most once per 5000 ms (`cmp ecx, 1388h / jb`, unsigned, so the tick wrap
//  behaves as the subtraction does). GetTickCount is called twice: the first answer is the throttle's and is stored as the last ping, the
//  second (0x6FAAD3D8) is the one the packet carries as nSendTick. The second dword, nHalfLatency, is GAME_GetNetworkLatencyRunningMean()
//  halved with `shr eax, 1`, a logical shift, so it is taken unsigned: the send tick and half the client's own latency estimate.
void __cdecl MSG_SendPing6C()
{
    uint32_t dwNow;
    D2GSPacketClt6C tPacket;

    dwNow = GetTickCount();
    if (dwNow - gnLastPing6CTick >= 5000)
    {
        gnLastPing6CTick = dwNow;
        tPacket.nHeader = D2CLTSYS_0x6C;
        tPacket.nSendTick = (int32_t)GetTickCount();
        tPacket.nHalfLatency = (int32_t)((uint32_t)GAME_GetNetworkLatencyRunningMean() >> 1);
        D2NET_CLIENT_Send(0, &tPacket.nHeader, (int32_t)sizeof(tPacket));
        gPacketAllSize += sizeof(tPacket);
        ++gPacketCounter;
    }
}

static_assert(offsetof(D2GSPacketClt6C, nSendTick) == 1
              && offsetof(D2GSPacketClt6C, nHalfLatency) == 5,
              "MSG_SendPing6C: var_B and var_7 against pBuffer at -0Ch");


//1.10f: D2Client.0x6FAAD420
//  Sends client packet 0x68 (D2CLTSYS_ENDGAME), one byte, and then the game type decides (gnGameType is loaded once, at 0x6FAAD446):
//      0, 1 (single player), 7, 8, 9    nothing more here
//      6 (OBNET_HOST)                   straight to the interface call
//      2..5 and anything above 9        gbDisconnecting = 1, gbInGame = 0, gbConnectedToGame = 0
//  Then 6 and 7, the two open Battle.net types, fetch WINMAIN_GetBnClientInterface() and, if it is not NULL, leave through
//  `jmp dword ptr [eax+90h]` (0x6FAAD4A2): a tail call into the interface, which is a struct of function pointers. The jump sets up no
//  argument. The tail jmp is a call here whose answer is dropped: the function is void, and nobody reads the eax the original leaves
//  (the game type, 0 when the interface is NULL, or the interface's answer).
void __cdecl MSG_Send_D2CLTSYS_ENDGAME()
{
    D2GSPacketClt68 tPacket;
    eD2GameTypes nGameType;
    struct BnClientInterface *pBnClient;

    tPacket.nHeader = D2CLTSYS_ENDGAME;
    D2NET_CLIENT_Send(0, &tPacket.nHeader, (int32_t)sizeof(tPacket));
    ++gPacketAllSize;
    ++gPacketCounter;

    nGameType = gnGameType;
    switch (nGameType)
    {
    case GAMETYPE_SINGLEPLAYER:
    case GAMETYPE_SINGLEPLAYER_UNCAPPED:
    case GAMETYPE_OBNET_HOST:
    case GAMETYPE_OBNET_JOIN:
    case GAMETYPE_LAN_HOST:
    case GAMETYPE_LAN_JOIN:
        break;

    default:
        gbDisconnecting = 1;
        gbInGame = 0;
        gbConnectedToGame = 0;
        break;
    }

    if (nGameType == GAMETYPE_OBNET_HOST || nGameType == GAMETYPE_OBNET_JOIN)
    {
        pBnClient = WINMAIN_GetBnClientInterface();
        if (pBnClient)
        {
            //  a struct of function pointers; +0x90 takes nothing, and its answer is dropped
            (*(int32_t(__stdcall **)(void))((char *)pBnClient + 0x90))();
        }
    }
}


//1.10f: D2Client.0x6FAAD4B0
//  Sends client packet 0x67 (D2CLTSYS_JOINGAME), 29 bytes, the joining client's announcement: the token and game id the join handed over,
//  the player class (the arena's forced class if it has one, as 0x66 does), the language, the player name, and at +8 the version the
//  Battle.net interface answers (GetVersion at +0x5C, called through GAME_GetComInterface with no argument pushed, 0x6FAAD4C1).
//  The stores run header, token (+1), game id (+5), version (+8), language (+0Ch), class (+7), name (+0Dh): scheduling around the calls.
//  BUG: the interface pointer is not tested, so without one the call faults on [5Ch]. NOT ESTABLISHED: gpComInterface is written only when the
//  launch config has an interface, and this packet goes out for BNET, OBNET_JOIN and LAN_JOIN; whether D2Launch hands over an interface for
//  a TCP/IP join cannot be read from D2Client. Every other reader of the global tests it first.
void __fastcall MSG_Send_D2CLTSYS_JOINGAME(int16_t nGameId, int32_t nTokenId)
{
    D2GSPacketClt67 tPacket;
    struct BnClientInterface *pComInterface;

    tPacket.nHeader = D2CLTSYS_JOINGAME;
    tPacket.nTokenId = nTokenId;
    tPacket.nGameId = (uint16_t)nGameId;

    //  the Battle.net interface, untested; +0x5C is GetVersion
    pComInterface = GAME_GetComInterface();
    tPacket.nVersion = (*(int32_t(__stdcall **)(void))((char *)pComInterface + 0x5C))();

    tPacket.nLocale = (uint8_t)D2LANG_STRTABLE_GetLanguage();
    if (ARENA_IsArenaPlayerClassForced())
        tPacket.nPlayerClass = (uint8_t)ARENA_GetArenaPlayerClass();
    else
        tPacket.nPlayerClass = gnPlayerClass;
    STORM_SStrCopy(tPacket.szClientName, gLocalPlayerName, 0x7FFFFFFF);

    D2NET_CLIENT_Send(0, &tPacket.nHeader, (int32_t)sizeof(tPacket));
    gPacketAllSize += sizeof(tPacket);
    ++gPacketCounter;
}

static_assert(offsetof(D2GSPacketClt67, nTokenId) == 0x01
              && offsetof(D2GSPacketClt67, nGameId) == 0x05
              && offsetof(D2GSPacketClt67, nPlayerClass) == 0x07
              && offsetof(D2GSPacketClt67, nVersion) == 0x08
              && offsetof(D2GSPacketClt67, nLocale) == 0x0C
              && offsetof(D2GSPacketClt67, szClientName) == 0x0D,
              "MSG_Send_D2CLTSYS_JOINGAME: var_1F, var_1B, var_19, var_18, "
              "var_14 and pDest against pBuffer at -20h");


//1.10f: D2Client.0x6FAAD530
//  Sets gnLoadProgressRatio to (nDone << 8) / nTotal (`shl eax, 8 / cdq / idiv`, a signed division), or to 0 when nTotal is 0. The result
//  runs 0..256, the pixel width UI_DrawLoadingScreen draws. void: eax is left holding nDone on the zero path, and neither caller reads it.
void __fastcall MSG_SetLoadProgressRatio(int32_t nDone, int32_t nTotal)
{
    if (nTotal)
        gnLoadProgressRatio = (nDone << 8) / nTotal;
    else
        gnLoadProgressRatio = 0;
}


//1.10f: D2Client.0x6FAAD550
//  Returns gnLoadProgressRatio; UI_DrawLoadingScreen reads the bar through it. MSG_Send_D2CLTSYS_OPENCHAR writes the global directly.
int32_t __cdecl MSG_GetLoadProgressRatio()
{
    return gnLoadProgressRatio;
}


//  The D2CLTSYS_OPENCHAR packet, packed as a wire format has to be: +0 nHeader (always 0x6B), +1 nSize (the payload length), +2 nTotalSize
//  (the whole file's size), +6 saveData. The header is six bytes; the payload extent (258) is the gap between szTail and the next local.
#pragma pack(push, 1)
typedef struct
{
    uint8_t  nHeader;
    uint8_t  nSize;
    int32_t  nTotalSize;
    char     saveData[258];
} D2ClientPacketClt6B;
#pragma pack(pop)

static_assert(offsetof(D2ClientPacketClt6B, nSize) == 0x01,
              "the length byte is [esp+2Dh], one past the header byte");
static_assert(offsetof(D2ClientPacketClt6B, nTotalSize) == 0x02,
              "the total size is [esp+2Eh] -- UNALIGNED, which is what makes "
              "this a packed wire struct rather than four separate locals");
static_assert(offsetof(D2ClientPacketClt6B, saveData) == 0x06,
              "the payload starts at [esp+32h], six bytes in; D2MOO's "
              "D2GSPacketClt6B agrees at 0x06");
static_assert(sizeof(D2ClientPacketClt6B) == 264,
              "szTail is [esp+32h] and the next local is [esp+134h] -- 258 "
              "bytes of payload behind six of header");

//  The size the packet really has (6 + 255), beside the 262 the original sends.
static_assert(offsetof(D2ClientPacketClt6B, saveData) + 255 == 261,
              "a full chunk occupies 261 bytes; the original pushes 106h");


//1.10f: D2Client.0x6FAAD560
//  Uploads the local .d2s to the server, 255 bytes at a time, and puts up the "sending character" wait on every open Battle.net or LAN
//  game. Sole caller GAME_InitGame passes bShowLoadingScreen = 1 for the four network game types, so every path below is live. It calls
//  D2Net itself (first argument 0) and does not come through MSG_SendPacketToServer. void: no caller reads eax.
//  The wait is deliberate and slow: eight draws of the loading screen with Sleep(250) between them before the first packet, and eight more
//  after every chunk, two seconds per 255 bytes. The bar is driven by hand: gnLoadProgressRatio is assigned directly, nShown /
//  (8 * nFullChunks), with nShown advancing 256 per sub-step and 2048 per chunk; the result runs 0..256.
//  Two guards are dead in practice (LATENT, both reproduced): inside the loop `test ebp, ebp / jnz` (0x6FAAD720) guards
//  nShown / (8 * nFullChunks), and the loop only runs when nFullChunks > 0, so its zero arm needs the product to wrap (a file of about
//  136 GB); at the exit `test ebx, ebx / jnz` (0x6FAAD7F6) guards (nFullChunks << 8) / nFullChunks, whose zero arm is reachable only for a
//  file under 255 bytes, which a .d2s cannot be.
//  A missing character file is fatal: a failed fopen, a failed fseek and a failed FOG_Alloc all reach FOG_DisplayAssert("pbData", Msg.cpp,
//  472) and exit(-1), so the player gets a crash naming an internal pointer. In the binary the two early failures are gotos into the middle
//  of the `if (!pbData)` arm; here they are the nesting with pbData NULL on all three paths, which behaves the same.
//  nTailBytes is nFileSize - nFullChunks * 255 stored as a byte, and the division by 255 is the usual signed magic multiply.
//  BUG: the null check comes after the read. If FOG_Alloc returns NULL the original calls fread(NULL, nFileSize, 1, pStream) and fclose and
//  only then asserts (0x6FAAD620..0x6FAAD63E). LATENT: FOG_Alloc reports and exits on exhaustion rather than returning NULL, and the request
//  is one character file. Reproduced: the order of the three calls is the bug.
//  DIFFERENCE: the file calls are our runtime's, and UCRT's fread refuses a NULL buffer through its invalid-parameter handler, which ends
//  the process before the pbData assert; the original's CRT_fread wrapper tests nothing itself. LATENT either way.
void __fastcall MSG_Send_D2CLTSYS_OPENCHAR(int32_t bShowLoadingScreen)
{
    FILE *pStream;
    int32_t nFileSize;
    void *pbData;
    char *pbCursor;
    int32_t nFullChunks;
    int32_t nRemainingChunks;
    int32_t nSent;
    int32_t nShown;
    int32_t nDenominator;
    int32_t nStep;
    int32_t nTailBytes;
    int32_t i;
    D2ClientPacketClt6B packet;
    char szFilePath[500];
    char szSavePath[1024];

    if (!gLocalPlayerName[0])
    {
        FOG_DisplayAssert((uint32_t)"gszPlrName[0] != '\\0'", (uint32_t)szMsgCpp, 401);
        exit(-1);
    }

    FOG_GetSavePath((uint32_t)szSavePath, 1024);
    sprintf(szFilePath, "%s%s.d2s", szSavePath, gLocalPlayerName);

    nFileSize = 0;
    pbData = NULL;

    //  our runtime's file calls; the stream is opened and closed here and handed to nothing else
    pStream = fopen(szFilePath, "rb");
    if (pStream)
    {
        if (fseek(pStream, 0, SEEK_END))
        {
            fclose(pStream);
        }
        else
        {
            nFileSize = ftell(pStream);
            fseek(pStream, 0, SEEK_SET);
            pbData = (void *)FOG_Alloc(nFileSize, (uint32_t)szMsgCpp, 421, 0);
            fread(pbData, nFileSize, 1, pStream);
            fclose(pStream);
        }
    }

    if (!pbData)
    {
        FOG_DisplayAssert((uint32_t)"pbData", (uint32_t)szMsgCpp, 472);
        exit(-1);
    }

    //  Eight draws before anything is sent, at 250 ms each: two seconds of loading screen with the bar pinned at zero.
    if (bShowLoadingScreen)
    {
        for (nStep = 8; nStep; --nStep)
        {
            gnLoadProgressRatio = 0;
            UI_DrawLoadingScreen(0);
            Sleep(250);
        }
    }

    pbCursor = (char *)pbData;
    nFullChunks = nFileSize / 255;

    if (nFullChunks > 0)
    {
        nSent = 0;
        nRemainingChunks = nFullChunks;

        do
        {
            packet.nHeader = D2CLTSYS_OPENCHAR;
            packet.nTotalSize = nFileSize;
            packet.nSize = 0xFF;

            for (i = 0; i < 255; ++i)
                packet.saveData[i] = *pbCursor++;

            //  BUG: the length is 262 where the packet is 261 (6 + 255): the author counted the header as seven bytes, and it is six.
            //  REACHABLE: every full chunk sends saveData[255], which the copy loop never writes (uninitialised stack, the same byte on every
            //  chunk of one upload). The server takes the length from the packet's own nSize field, so the save is not corrupted.
            D2NET_CLIENT_Send(0, (const uint8_t *)&packet, 262);
            gPacketAllSize += 262;
            ++gPacketCounter;

            if (bShowLoadingScreen)
            {
                nShown = nSent;
                nDenominator = 8 * nFullChunks;

                for (nStep = 8; nStep; --nStep)
                {
                    if (nDenominator)
                        gnLoadProgressRatio = nShown / nDenominator;
                    else
                        gnLoadProgressRatio = 0;

                    UI_DrawLoadingScreen(0);
                    Sleep(250);
                    nShown += 256;
                }
            }

            nSent += 2048;
        }
        while (--nRemainingChunks);
    }

    packet.nHeader = D2CLTSYS_OPENCHAR;
    packet.nTotalSize = nFileSize;
    nTailBytes = nFileSize - 255 * nFullChunks;
    packet.nSize = (uint8_t)nTailBytes;

    for (i = 0; i < nTailBytes; ++i)
        packet.saveData[i] = *pbCursor++;

    //  BUG: nTailBytes + 7 where the packet is nTailBytes + 6, the same one byte. REACHABLE: it is saveData[nTailBytes], a stale byte of the
    //  character file that the last full chunk wrote.
    D2NET_CLIENT_Send(0, (const uint8_t *)&packet, nTailBytes + 7);
    gPacketAllSize += nTailBytes + 7;
    ++gPacketCounter;

    FOG_Free((uint32_t)pbData, (uint32_t)szMsgCpp, 520, 0);

    if (nFullChunks)
        gnLoadProgressRatio = (nFullChunks << 8) / nFullChunks;
    else
        gnLoadProgressRatio = 0;
}


//  Packet senders, 0x6FAAD830..0x6FAADBBF: MSG_SendPacketToServer, and eleven functions that each build one packet on their own stack and
//  end in it. All twelve are void: the eleven senders hand MSG_SendPacketToServer's eax straight through (a tail call), and no caller in
//  D2Client reads it.
//  Seven of the senders put the caller's command id in byte 0 and one to four words or dwords behind it. The shapes are declared here and
//  the member names are the functions' parameter names, since what a slot means is decided by the id.
#pragma pack(push, 1)
typedef struct
{
    uint8_t  nCmd;
    uint16_t nValue;
} D2ClientPacketCmdWord;

typedef struct
{
    uint8_t  nCmd;
    uint16_t nX;
    uint16_t nY;
} D2ClientPacketCmdXY;

typedef struct
{
    uint8_t  nCmd;
    uint16_t nWord1;
    uint16_t nWord2;
    uint16_t nWord3;
} D2ClientPacketCmd3Words;

typedef struct
{
    uint8_t  nCmd;
    uint32_t nUnitGUID;
} D2ClientPacketCmdDword;

typedef struct
{
    uint8_t  nCmd;
    uint32_t nUnitType;
    uint32_t nUnitGUID;
} D2ClientPacketCmdUnit;

typedef struct
{
    uint8_t  nCmd;
    uint32_t nUnitType;
    uint32_t nUnitGUID;
    uint32_t nParam;
} D2ClientPacketCmdUnitParam;

typedef struct
{
    uint8_t  nCmd;
    uint32_t nDword1;
    uint32_t nDword2;
    uint32_t nDword3;
    uint32_t nDword4;
} D2ClientPacketCmd4Dwords;
#pragma pack(pop)

static_assert(offsetof(D2ClientPacketCmdWord, nValue) == 1
              && sizeof(D2ClientPacketCmdWord) == 3,
              "MSG_SendCmdWord: `mov [esp+4+var_3], dx` is +1, and "
              "`mov edx, 3` at 0x6FAAD99A is the length");
static_assert(offsetof(D2ClientPacketCmdXY, nX) == 1
              && offsetof(D2ClientPacketCmdXY, nY) == 3
              && sizeof(D2ClientPacketCmdXY) == 5,
              "MSG_SendCmdXY: var_7 and var_5 are +1 and +3; the length "
              "is `mov edx, 5` at 0x6FAAD9C1");
static_assert(offsetof(D2ClientPacketCmd3Words, nWord1) == 1
              && offsetof(D2ClientPacketCmd3Words, nWord2) == 3
              && offsetof(D2ClientPacketCmd3Words, nWord3) == 5
              && sizeof(D2ClientPacketCmd3Words) == 7,
              "MSG_SendCmd3Words: dx to +1, the first stack word to +3, "
              "the second to +5; `mov edx, 7` at 0x6FAAD9FB");
static_assert(offsetof(D2ClientPacketCmdDword, nUnitGUID) == 1
              && sizeof(D2ClientPacketCmdDword) == 5,
              "MSG_SendServerItemMessage: `mov [esp+8+var_7], edx` is +1, and "
              "`mov edx, 5` at 0x6FAADA2B");
static_assert(offsetof(D2ClientPacketCmdUnit, nUnitType) == 1
              && offsetof(D2ClientPacketCmdUnit, nUnitGUID) == 5
              && sizeof(D2ClientPacketCmdUnit) == 9,
              "MSG_SendCmdUnit: edx to +1, the stack dword to +5; "
              "`mov edx, 9` at 0x6FAADA4F");
static_assert(offsetof(D2ClientPacketCmdUnitParam, nUnitType) == 1
              && offsetof(D2ClientPacketCmdUnitParam, nUnitGUID) == 5
              && offsetof(D2ClientPacketCmdUnitParam, nParam) == 9
              && sizeof(D2ClientPacketCmdUnitParam) == 13,
              "MSG_SendCmdUnitParam: +1, +5, +9; `mov edx, 0Dh` at "
              "0x6FAADA87");
static_assert(offsetof(D2ClientPacketCmd4Dwords, nDword1) == 1
              && offsetof(D2ClientPacketCmd4Dwords, nDword2) == 5
              && offsetof(D2ClientPacketCmd4Dwords, nDword3) == 9
              && offsetof(D2ClientPacketCmd4Dwords, nDword4) == 13
              && sizeof(D2ClientPacketCmd4Dwords) == 17,
              "MSG_SendCmd4Dwords: +1, +5, +9, +0Dh; `mov edx, 11h` at "
              "0x6FAADABF");

//  Member offsets of the party and chat packets, as the stores use them.
static_assert(offsetof(D2GSPacketClt5D, nType) == 1
              && offsetof(D2GSPacketClt5D, nToggle) == 2
              && offsetof(D2GSPacketClt5D, dwPlayerGUID) == 3,
              "MSG_SendClt5D: `mov [esp+8+pPacket+1], cl`, `mov "
              "[esp+8+pPacket+2], 1`, `mov dword ptr [esp+8+pPacket+3], edx`");
static_assert(offsetof(D2GSPacketClt5E, nType) == 1
              && offsetof(D2GSPacketClt5E, dwPlayerGUID) == 2,
              "MSG_SendClt5E: `mov [esp+8+pPacket+1], cl` and `mov dword "
              "ptr [esp+8+pPacket+2], edx`");
static_assert(offsetof(D2GSPacketClt15, szMessage) == 3
              && offsetof(D2GSPacketClt15, szName) == 259,
              "MSG_SendChatMessage copies the message from `[esi+3]` and "
              "the name from `[esi+103h]`");


//1.10f: D2Client.0x6FAAD830
//  Sends a one-byte command. `push ecx` makes a four-byte slot and the packet is its top byte (`mov [esp+4+pPacket], cl` is [esp+3]); the
//  other three bytes are ecx's and are not sent. Ids sent: 0x40 (the quest log opening), 0x41 (UI_SetUIToggle), 0x48 (VIDEO_PlayCinematic),
//  0x53 and 0x54 (CMDTBL_ActionRun and its release), 0x60 (the weapon swap).
void __fastcall MSG_SendCmd1(uint8_t nCmd)
{
    uint8_t nPacket;

    nPacket = (uint8_t)nCmd;
    MSG_SendPacketToServer(&nPacket, 1);
}


//1.10f: D2Client.0x6FAAD850
//  The send that the eleven senders below all end in: an identical packet sent too soon after the last one is dropped. MSG_Send_D2CLTSYS_
//  OPENCHAR does not come here. nLen is asserted under 512 (`cmp ebx, 200h / jl` at 0x6FAAD85B..0x6FAAD864; "nLen < (D2NET_MAX_MESSAGE_SIZE )"
//  at Msg.cpp:531), then exit(-1).
//  BUG: that compare is signed, so a negative nLen would pass it and both copies below would run over memory. LATENT: ten call sites pass a
//  constant and the chat sender's computed length is at least 6; reproduced by writing the test as it is.
//  The duplicate window depends on the id (`add ecx, 0FFFFFFFBh / cmp ecx, 35h / ja` at 0x6FAAD88F..0x6FAAD895, then a byte-indexed jump table):
//      0x6FAAD8A6  ids 0x05..0x0A and 0x0C..0x11    50 ms  (`cmp ecx, 32h`)
//      0x6FAAD8D6  id  0x3A                         never compared
//      0x6FAAD8B5  every other id, 0x0B included   200 ms  (`cmp ecx, 0C8h`)
//  Both window compares are unsigned (they share one `jnb` at 0x6FAAD8C5), so the tick wrap at 49.7 days behaves as the subtraction does.
//  Inside the window the packet is compared whole with the last one sent (`repe cmpsb` over nLen bytes at 0x6FAAD8D2, after an
//  `xor edx, edx` that sets ZF so that a zero count compares equal, as memcmp's 0 does) and dropped only if every byte matches.
//  An accepted packet is copied into gLastSentPacket, the tick is restamped, and the packet is copied again into a 512-byte stack buffer;
//  that copy is what D2Net gets, with 1 as its first argument (the upload passes 0). With nLen under 512 neither copy overruns.
//  void: the original's eax at the exits is three different things -- GetTickCount()'s answer when the packet is suppressed (eax is never
//  written again on the `jz` to the epilogue at 0x6FAAD8D4), 0 when D2Net's send failed, and the packet counter after this packet when it
//  worked -- and nobody reads it. gPacketAllSize and gPacketCounter move only when D2Net says the send worked.
void __fastcall MSG_SendPacketToServer(uint8_t *pPacket, int32_t nLen)
{
    uint32_t dwNow;
    uint8_t pBuffer[512];

    if (nLen >= 512)
    {
        FOG_DisplayAssert((uint32_t)"nLen < (D2NET_MAX_MESSAGE_SIZE )", (uint32_t)szMsgCpp, 531);
        exit(-1);
    }

    dwNow = GetTickCount();

    switch (pPacket[0])
    {
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
    case 0x10:
    case 0x11:
        if (dwNow - gnLastPacketSentTick < 50 && !memcmp(gLastSentPacket, pPacket, nLen))
        {
            return;
        }
        break;

    case 0x3A:
        break;

    default:
        if (dwNow - gnLastPacketSentTick < 200 && !memcmp(gLastSentPacket, pPacket, nLen))
        {
            return;
        }
        break;
    }

    memcpy(gLastSentPacket, pPacket, nLen);
    gnLastPacketSentTick = dwNow;
    memcpy(pBuffer, pPacket, nLen);

    if (D2NET_CLIENT_Send(1, pBuffer, nLen))
    {
        gPacketAllSize += nLen;
        ++gPacketCounter;
    }
}


//1.10f: D2Client.0x6FAAD990
//  Sends {u8 nCmd, u16 nValue}, three bytes. As in MSG_SendCmd1, `push ecx` only makes the slot: byte 3 keeps ecx's top byte and is not sent.
//  The body reads cl and nothing else of ecx. Ids sent: 0x1C (the body location clicks), 0x3A (CHAR_StatsOnLButtonUp), 0x3B
//  (SKILLSTREE_OnLButtonUp), 0x3F (the CMDTBL_ActionSay* tail jumps), 0x4D (NPCMENU_OpenMainMenu), 0x58 (the quest log), 0x61 (the
//  hireling's inventory).
void __fastcall MSG_SendCmdWord(uint8_t nCmd, int16_t nValue)
{
    D2ClientPacketCmdWord tPacket;

    tPacket.nCmd = (uint8_t)nCmd;
    tPacket.nValue = (uint16_t)nValue;
    MSG_SendPacketToServer((uint8_t *)&tPacket, (int32_t)sizeof(tPacket));
}


//1.10f: D2Client.0x6FAAD9B0
//  Sends {u8 nCmd, u16 nX, u16 nY}, five bytes (`retn 4`: nY is the one stack argument). It is the location form of the movement and skill
//  commands: UNITMODE_PerformClientCommand sends its own nClientCmd through it, and UNITMODE_SyncUnitPosition sends 0x5F.
void __fastcall MSG_SendCmdXY(uint8_t nCmd, int16_t nX, int16_t nY)
{
    D2ClientPacketCmdXY tPacket;

    tPacket.nCmd = (uint8_t)nCmd;
    tPacket.nX = (uint16_t)nX;
    tPacket.nY = (uint16_t)nY;
    MSG_SendPacketToServer((uint8_t *)&tPacket, (int32_t)sizeof(tPacket));
}


//1.10f: D2Client.0x6FAAD9E0
//  Sends {u8 nCmd, u16, u16, u16}, seven bytes (`retn 8`). The stores run nWord1, nWord3, nWord2 (scheduling). Every caller sends 0x4F: the
//  trade, cube and stash panels and the gold dialog.
void __fastcall MSG_SendCmd3Words(uint8_t nCmd, int16_t nWord1, int16_t nWord2, int16_t nWord3)
{
    D2ClientPacketCmd3Words tPacket;

    tPacket.nCmd = (uint8_t)nCmd;
    tPacket.nWord1 = (uint16_t)nWord1;
    tPacket.nWord2 = (uint16_t)nWord2;
    tPacket.nWord3 = (uint16_t)nWord3;
    MSG_SendPacketToServer((uint8_t *)&tPacket, (int32_t)sizeof(tPacket));
}


//1.10f: D2Client.0x6FAADA20
//  Sends {u8 nCmd, u32 nUnitGUID}, five bytes; both arguments are registers. nUnitGUID is a unit's GUID (its dwUnitId, gNpcUnitGUID for 0x34
//  and 0x62, gnSelectedUnitGUID for 0x3D) or -1 (0x4C, when the cursor state is 8). Ids sent: 0x17, 0x19, 0x24, 0x34, 0x37, 0x3D, 0x3E,
//  0x4C, 0x62, 0x63.
void __fastcall MSG_SendServerItemMessage(uint8_t nCmd, int32_t nUnitGUID)
{
    D2ClientPacketCmdDword tPacket;

    tPacket.nCmd = (uint8_t)nCmd;
    tPacket.nUnitGUID = (uint32_t)nUnitGUID;
    MSG_SendPacketToServer((uint8_t *)&tPacket, (int32_t)sizeof(tPacket));
}


//1.10f: D2Client.0x6FAADA40
//  Sends {u8 nCmd, u32 nUnitType, u32 nUnitGUID}, nine bytes (`retn 4`), the most used sender of the module. The second dword is a unit type
//  only for the majority of ids: 0x49 passes gnWaypointUnitGUID there, 0x27 a unit's dwUnitId, and 0x31 gNpcUnitGUID, a unit's dwUnitId
//  or -1. The name follows the majority.
void __fastcall MSG_SendCmdUnit(uint8_t nCmd, int32_t nUnitType, int32_t nUnitGUID)
{
    D2ClientPacketCmdUnit tPacket;

    tPacket.nCmd = (uint8_t)nCmd;
    tPacket.nUnitType = (uint32_t)nUnitType;
    tPacket.nUnitGUID = (uint32_t)nUnitGUID;
    MSG_SendPacketToServer((uint8_t *)&tPacket, (int32_t)sizeof(tPacket));
}


//1.10f: D2Client.0x6FAADA70
//  Sends {u8 nCmd, u32, u32, u32}, thirteen bytes (`retn 8`). The stores run nParam before nUnitGUID (scheduling). Ids sent: 0x16, 0x20,
//  0x26 and 0x38.
void __fastcall MSG_SendCmdUnitParam(uint8_t nCmd, int32_t nUnitType, int32_t nUnitGUID, int32_t nParam)
{
    D2ClientPacketCmdUnitParam tPacket;

    tPacket.nCmd = (uint8_t)nCmd;
    tPacket.nUnitType = (uint32_t)nUnitType;
    tPacket.nUnitGUID = (uint32_t)nUnitGUID;
    tPacket.nParam = (uint32_t)nParam;
    MSG_SendPacketToServer((uint8_t *)&tPacket, (int32_t)sizeof(tPacket));
}


//1.10f: D2Client.0x6FAADAA0
//  Sends {u8 nCmd, u32 x 4}, seventeen bytes (`retn 0Ch`). The four dwords are generic, as MSG_SendCmd3Words' words are: what each holds
//  depends on the id. Ids sent: 0x44 (the anvil), 0x18, 0x32, 0x33, 0x59, 0x65 (SOUNDCHAN_SendCodeIntegrityProbe), and one whose id is a
//  register (`mov cl, bl` in NPCMENU_SendTransactionAndWait, 0x6FAFA8C1).
void __fastcall MSG_SendCmd4Dwords(uint8_t nCmd, int32_t nDword1, int32_t nDword2, int32_t nDword3, int32_t nDword4)
{
    D2ClientPacketCmd4Dwords tPacket;

    tPacket.nCmd = (uint8_t)nCmd;
    tPacket.nDword1 = (uint32_t)nDword1;
    tPacket.nDword2 = (uint32_t)nDword2;
    tPacket.nDword3 = (uint32_t)nDword3;
    tPacket.nDword4 = (uint32_t)nDword4;
    MSG_SendPacketToServer((uint8_t *)&tPacket, (int32_t)sizeof(tPacket));
}


//1.10f: D2Client.0x6FAADAE0
//  Sends client packet 0x5D, seven bytes (`retn 4`). Each of the four callers passes as dwPlayerGUID the `[reg+10h]` it has just handed
//  PLAYERLIST_RosterTestInfoFlag as the target GUID, and that call's answer as bToggle: nType 1, 2 and 3 from ROSTER_ToggleLootAndNotify,
//  _ToggleIgnoreAndNotify and _ToggleSquelchAndNotify, 4 from ROSTER_PartySendHostileToggle.
//  The buffer is zeroed first (all seven bytes) and only then filled; the store at 0x6FAADB07 is under `if (bToggle)`, so a false bToggle
//  leaves the zero and nToggle comes out 0 or 1.
void __fastcall MSG_SendClt5D(D2C_PartyCommands nType, int32_t dwPlayerGUID, int32_t bToggle)
{
    D2GSPacketClt5D tPacket;

    memset(&tPacket, 0, sizeof(tPacket));
    tPacket.nHeader = 0x5D;
    tPacket.nType = nType;
    tPacket.dwPlayerGUID = (uint32_t)dwPlayerGUID;
    if (bToggle)
    {
        tPacket.nToggle = 1;
    }

    MSG_SendPacketToServer(&tPacket.nHeader, (int32_t)sizeof(tPacket));
}


//1.10f: D2Client.0x6FAADB20
//  Sends client packet 0x5E, six bytes. The buffer is zeroed first like 0x5D, though the three fields then cover all six bytes, so the
//  zeroing is dead stores the compiler kept. The header is stored last (scheduling). Callers: ROSTER_PartySendButtonCommand (nType 6, 8, 9 or 7
//  for the entry's button state 0, 2, 3 or 4, 0 for a state above 4, and state 1 asserts) and ROSTER_PartySendLeaveParty (9).
void __fastcall MSG_SendClt5E(D2C_PartyCommands nType, int32_t dwPlayerGUID)
{
    D2GSPacketClt5E tPacket;

    memset(&tPacket, 0, sizeof(tPacket));
    tPacket.nHeader = 0x5E;
    tPacket.nType = (uint8_t)nType;
    tPacket.dwPlayerGUID = dwPlayerGUID;
    MSG_SendPacketToServer(&tPacket.nHeader, (int32_t)sizeof(tPacket));
}


//1.10f: D2Client.0x6FAADB50
//  Sends the chat packet. The caller builds a block of id, message type and language, a 256-byte message at +3 and a 16-byte name at +259;
//  this copies the three header bytes, then the two strings with SStrCopy, and sends exactly as many bytes as the text needs.
//  The id is the caller's and not always 0x15: CHAT_HandleInputKey replaces it by 0x14 (overhead chat) when CHAT_ParseCommandPrefix succeeds
//  with kind 0, and CHAT_HandleWhisperCommand sends 0x15 with 2 in byte 1.
//  SStrCopy copies at most max - 1 characters, terminates, and returns the count, and the cursor advances past the terminator each time. So
//  the packet is the header, the message, NUL, the name, NUL, and then one more zero written at the cursor and counted: the length is
//  `pszAfterName - packet + 1`. The longest message (255) and name (15) put that last zero at offset 275, one byte past D2GSPacketClt15
//  (size 275); the original's buffer is its whole 516-byte frame, so the buffer here is that size, as a union over the struct.
//  BUG: `lea eax, [esi+103h] / test eax, eax / jz` (0x6FAADB85) copies the name only if `pMsg + 259` is not NULL, the shape of
//  `if (pMsg->szName)` on an array, which the original's compiler did not fold. UNREACHABLE: it fails only for pMsg == 0xFFFFFEFD, which no
//  caller passes. It is written on integers because the pointer form could be folded away, and the arm would then be gone.
void __fastcall MSG_SendChatMessage(const void *pMsg)
{
    const uint8_t *pSrc;
    size_t nCopied;
    char *pszAfterMessage;
    char *pszAfterName;
    union
    {
        D2GSPacketClt15 tPacket;
        uint8_t pFrame[0x204];
    } uPacket;

    pSrc = (const uint8_t *)pMsg;
    uPacket.tPacket.nHeader = pSrc[0];
    uPacket.tPacket.nMessageType = pSrc[1];
    uPacket.tPacket.nLang = pSrc[2];

    nCopied = STORM_SStrCopy(uPacket.tPacket.szMessage, (const char *)&pSrc[3], 256);
    pszAfterMessage = (char *)&uPacket.pFrame[3 + nCopied + 1];

    //  the original's `if (pMsg->szName)`, on integers
    if ((uintptr_t)pSrc + 259 == 0)
    {
        *pszAfterMessage = 0;
        pszAfterName = pszAfterMessage + 1;
    }
    else
    {
        pszAfterName = pszAfterMessage + STORM_SStrCopy(pszAfterMessage, (const char *)&pSrc[259], 16) + 1;
    }
    *pszAfterName = 0;

    MSG_SendPacketToServer(uPacket.pFrame, (int32_t)(pszAfterName - (char *)uPacket.pFrame) + 1);
}

static_assert(sizeof(D2GSPacketClt15) == 275
              && offsetof(D2GSPacketClt15, szName) + 16 == 275,
              "the chat packet's last zero can land at offset 275 -- one past "
              "D2GSPacketClt15 -- which is why the buffer is the frame");
static_assert(sizeof(((D2GSPacketClt15 *)0)->szMessage) == 256
              && sizeof(((D2GSPacketClt15 *)0)->szName) == 16,
              "the two SStrCopy limits, `push 100h` at 0x6FAADB5A and `push "
              "10h` at 0x6FAADB8F");


//1.10f: D2Client.0x6FAADBC0
//  The join pump's callback. The three pump callbacks of this file are `BOOL __stdcall (int)`, each `retn 4` with the argument never read:
//  WNDPROC_WindowRunMessagePump calls its idle callback with the idle counter and returns as soon as it answers non-zero.
//  MSG_AppModeJoinGame stamps gnGameJoinStartTickCount, clears gbJoinGameLoopDone and runs the pump on this once the connection wait has
//  finished. gnGameType is read once, before the frame is made (0x6FAADBC0). Single player with D2GFX_WINDOW_GetState non-zero goes
//  straight to the packet. Otherwise:
//    * more than 10000 ms since the stamp (`cmp eax, 2710h / jbe` at 0x6FAADBE9, unsigned) fails the join: GAME_BeginDisconnect(6),
//      gbJoinGameLoopDone = 1, gbConnectedToGame = 0, stop;
//    * D2Net #10017 answering 0 stops with nothing else set.
//  Then one system packet is dequeued into a 512-byte buffer (`sub esp, 200h`) and handed to SCMD_SaveCharacterFile, and the pump stops
//  once gbConnectedToGame is set (`setnz al` at 0x6FAADC57).
BOOL __stdcall MSG_JoinGameMessageLoop(int32_t nUnused)
{
    eD2GameTypes nGameType;
    int32_t nLen;
    uint8_t pBuffer[512];

    nGameType = gnGameType;
    if ((nGameType != GAMETYPE_SINGLEPLAYER && nGameType != GAMETYPE_SINGLEPLAYER_UNCAPPED) || !D2GFX_WINDOW_GetState())
    {
        if (GetTickCount() - gnGameJoinStartTickCount > 10000)
        {
            GAME_BeginDisconnect(D2JOINERR_failed_to_join_game_6);
            gbJoinGameLoopDone = 1;
            gbConnectedToGame = 0;
            return TRUE;
        }
        if (!D2NET_10017())
        {
            return TRUE;
        }
    }

    nLen = D2NET_CLIENT_DequeueSystemPacket(pBuffer, sizeof(pBuffer));
    SCMD_SaveCharacterFile(pBuffer, nLen);
    return gbConnectedToGame != 0;
}


//1.10f: D2Client.0x6FAADC70
//  Releases the local game, once. If gbGameAllocated is set it is cleared and the function leaves through a tail `jmp D2GAME_10050`
//  (0x6FAADC86), answering what D2Game answers; otherwise it answers 0. The flag is the one MSG_InitGame raises after
//  D2GAME_GAME_InitGameDataTable, so this is that setup's undo. Sole caller WINMAIN_D2ClientEntrypoint discards the answer.
//  `__cdecl`: the function takes no argument and reads no register.
int32_t __cdecl MSG_FreeGame()
{
    if (!gbGameAllocated)
    {
        return 0;
    }

    gbGameAllocated = 0;
    return D2GAME_10050();
}


//1.10f: D2Client.0x6FAADC90
//  Returns gbJoinGameLoopDone, the continue test of MSG_AppModeJoinGame's message pump: the global is cleared before the loop and raised
//  by either MSG_JoinGameMessageLoop or the idle callback, so the pump ends when either says so. Sole caller GAME_InitGame.
int32_t __cdecl MSG_IsJoinGameLoopDone()
{
    return gbJoinGameLoopDone;
}


//1.10f: D2Client.0x6FAADCA0
//  The connection wait's callback. MSG_AppModeJoinGame runs the pump on this on all five of its arms, after D2Net's CLIENT_Initialize and
//  before the join pump. Single player with D2GFX_WINDOW_GetState non-zero keeps waiting (0). Otherwise D2Net #10025 decides:
//      2   single player (D2Net's mode word, the first argument MSG_AppModeJoinGame hands CLIENT_Initialize, is 1 or 2 there and 0 for
//          every other type), or the connect thread has finished and this call has started the receive thread -- stop
//      1   the connect thread has not finished (a 100 ms wait) -- go on
//      0   no socket -- the join fails as the join pump's timeout does: GAME_BeginDisconnect(6), gbJoinGameLoopDone = 1,
//          gbConnectedToGame = 0, stop
//  `xor ecx, ecx / cmp eax, 2 / setz cl` (0x6FAADCE6..0x6FAADCEB) turns the 2 test into a 0 or 1 answer.
BOOL __stdcall MSG_JoinGameIdlePoll(int32_t nUnused)
{
    eD2GameTypes nGameType;
    int32_t nNetState;

    nGameType = gnGameType;
    if ((nGameType == GAMETYPE_SINGLEPLAYER || nGameType == GAMETYPE_SINGLEPLAYER_UNCAPPED) && D2GFX_WINDOW_GetState())
    {
        return FALSE;
    }

    nNetState = D2NET_10025();
    if (nNetState)
    {
        return nNetState == 2;
    }

    GAME_BeginDisconnect(D2JOINERR_failed_to_join_game_6);
    gbJoinGameLoopDone = 1;
    gbConnectedToGame = 0;
    return TRUE;
}


//1.10f: Inlined in D2Client.0x6FAADD00 MSG_AppModeJoinGame
//  Three statements the original emits once, at the label both the single-player paths and the host paths jump to: not a function in the
//  binary. Written as a helper because the alternative is the same three lines three times.
static void MSG_InitGame()
{
    D2GAME_GAME_InitGameDataTable(&gGameData, &gGameList);
    gbGameAllocated = 1;
    D2GAME_10046();
}

//1.10f: D2Client.0x6FAADD00
//  gafnAppModeEntryPoints[APPMODE_JOIN_GAME]. Brings up the server for the game types that need one, connects the client, runs the
//  connection-wait pump, then the join pump, and hands the next mode back off the queue. The original inlines MSG_InitGame here.
//  For OBNET_HOST and LAN_HOST CLIENT_Initialize gets 0 as its first argument (`push offset gszServerIP; push 0` at 0x6FAADD90), the same
//  as the default arm. gbConnectedToGame = 0 at the top. The switch arms are the compiler's; the game types are written by name.
D2C_AppMode __fastcall MSG_AppModeJoinGame()
{
    gbConnectedToGame = 0;

    switch (gnGameType)
    {
    case GAMETYPE_SINGLEPLAYER:
        D2NET_SERVER_Initialize(1, 1);
        D2NET_CLIENT_Initialize(1, NULL);
        WNDPROC_WindowRunMessagePump(MSG_JoinGameIdlePoll);
        D2NET_SERVER_SetHackListEnabled(FALSE);
        MSG_InitGame();
        break;

    case GAMETYPE_SINGLEPLAYER_UNCAPPED:
        D2NET_SERVER_Initialize(2, 1);
        D2NET_CLIENT_Initialize(2, NULL);
        WNDPROC_WindowRunMessagePump(MSG_JoinGameIdlePoll);
        D2NET_SERVER_SetHackListEnabled(FALSE);
        MSG_InitGame();
        break;

    case GAMETYPE_BNET_INTERNAL:
        //  gszBattleNetIP holds the hardcoded "207.82.87.139" unless GAME_ApplyParamBattleNetIp wrote over it.
        D2NET_CLIENT_Initialize(0, gszBattleNetIP);
        WNDPROC_WindowRunMessagePump(MSG_JoinGameIdlePoll);
        break;

    case GAMETYPE_OBNET_HOST:
    case GAMETYPE_LAN_HOST:
        D2NET_SERVER_Initialize(0, 1);
        D2NET_SERVER_SetMaxClientsPerGame(8);
        D2NET_SERVER_SetHackListEnabled(FALSE);
        D2NET_CLIENT_Initialize(0, gszServerIP);
        WNDPROC_WindowRunMessagePump(MSG_JoinGameIdlePoll);
        MSG_InitGame();
        break;

    default:
        D2NET_CLIENT_Initialize(0, gszServerIP);
        WNDPROC_WindowRunMessagePump(MSG_JoinGameIdlePoll);
        break;
    }

    gnGameJoinStartTickCount = GetTickCount();
    gbJoinGameLoopDone = 0;
    WNDPROC_WindowRunMessagePump(MSG_JoinGameMessageLoop);

    return WINMAIN_AppModeDequeue();
}


//1.10f: D2Client.0x6FAADDF0
//  The leave pump's callback. MSG_AppModeLeaveGame stamps gnGameLeaveStartTickCount and runs the pump on this before it releases D2Net's
//  client. It calls D2GFX_WINDOW_GetState first and throws the answer away (0x6FAADDF6), as the image does. In single player D2Game's
//  network messages are processed (D2GAME_GAME_ProcessNetworkMessages), since the server is in this process; then one system packet goes
//  to SCMD_SaveCharacterFile as in the join pump, and the pump stops after more than 10000 ms (`jbe` at 0x6FAADE39, unsigned) or once
//  gbConnectedToGame is clear (`setz al`).
BOOL __stdcall MSG_LeaveGameMessageLoop(int32_t nUnused)
{
    eD2GameTypes nGameType;
    int32_t nLen;
    uint8_t pBuffer[512];

    D2GFX_WINDOW_GetState();

    nGameType = gnGameType;
    if (nGameType == GAMETYPE_SINGLEPLAYER || nGameType == GAMETYPE_SINGLEPLAYER_UNCAPPED)
    {
        D2GAME_GAME_ProcessNetworkMessages();
    }

    nLen = D2NET_CLIENT_DequeueSystemPacket(pBuffer, sizeof(pBuffer));
    SCMD_SaveCharacterFile(pBuffer, nLen);

    if (GetTickCount() - gnGameLeaveStartTickCount > 10000)
    {
        return TRUE;
    }
    return gbConnectedToGame == 0;
}


//1.10f: D2Client.0x6FAADE60
//  gafnAppModeEntryPoints[APPMODE_LEAVE_GAME]. Runs the leave pump, releases the client, and releases the server on every game type that
//  started one, which is the set MSG_AppModeJoinGame calls SERVER_Initialize for. The binary folds the first two game types into one
//  unsigned compare, `(unsigned)gnGameType < 2`; written by name here.
D2C_AppMode __fastcall MSG_AppModeLeaveGame()
{
    gnGameLeaveStartTickCount = GetTickCount();
    WNDPROC_WindowRunMessagePump(MSG_LeaveGameMessageLoop);
    D2NET_CLIENT_Release();

    if (gnGameType == GAMETYPE_SINGLEPLAYER || gnGameType == GAMETYPE_SINGLEPLAYER_UNCAPPED || gnGameType == GAMETYPE_OBNET_HOST || gnGameType == GAMETYPE_LAN_HOST)
    {
        D2NET_SERVER_Release();
    }

    return WINMAIN_AppModeDequeue();
}
