#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "Fault.h"

//  Fault.cpp -- not a D2Client source file: D2Hell's Fault.cpp compiled into D2Client.dll 1.10. It holds the fault module's assert handler
//  slot, its list of message sources and the lock around it, with the Storm list template (TSList, TSLink) instantiated for that list.
//  Address range 0x6FB6BB35..0x6FB6BD99. No function pushes a path, so the file name is ours.
//  The C runtime's functions between these are not in this file: CRT_StaticInit24 and CRT_MSGSRC_StaticDestruct (the list's dynamic
//  initialiser and its atexit destructor), CRT_MSGSRC_Destruct (~TSList), CRT_TLINKEDLIST_Initialize and CRT_TLINKEDLIST_UnlinkAll.

//  The allocation tag of every node, TMESSAGESOURCE's RTTI type name; the line passed with it is -2.
static const char szMessageSourceTag[] = ".?AUTMESSAGESOURCE@@";

//  The members and numbers this file reads that no other file pins.
static_assert(offsetof(D2MessageSourceStrc, tLink) == 0 && offsetof(D2MessageSourceStrc, pfnMessageSource) == 8,
              "FAULT_RegisterMessageSource: `mov [eax+8], edi` 0x6FB6BBC2; FreeAll unlinks the node itself, `mov ecx, esi` 0x6FB6BC6E");
static_assert((MB_OKCANCEL | MB_ICONHAND | MB_TASKMODAL) == 0x2011 && IDCANCEL == 2,
              "FAULT_PromptErrorOrCancel: `push 2011h` 0x6FB6BC27, `dec eax` / `dec eax` 0x6FB6BC3B");
//  gMessageSourceList's construction.
//  DIFFERENCE: the original builds the list in its runtime's static initialiser (the link offset 0, the sentinel's previous link its own
//  address and its next the complement of that address: an empty list) and hands a destructor to atexit, which unlinks every node and then
//  the sentinel. Ours does the same two things in this DLL's static initialisation and teardown, in the object below. Its constructor writes
//  the end state of the original's five stores; gMessageSourceList is zero before it runs. FAULT_Init registers FAULT_AtExit with this DLL's
//  atexit table, so the list is freed and the lock deleted when that table runs; FAULT_AtExit runs before the object's destructor, as it ran
//  before the original's.
struct MessageSourceListLifetime
{
    MessageSourceListLifetime()
    {
        gMessageSourceList.nLinkOffset = 0;
        gMessageSourceList.tSentinel.pPrevLink = &gMessageSourceList.tSentinel;
        gMessageSourceList.tSentinel.nNext = ~(int32_t)&gMessageSourceList.tSentinel;
    }
    ~MessageSourceListLifetime()
    {
        while (gMessageSourceList.tSentinel.nNext > 0)
            FAULT_TLinkUnlink((TSLink *)(gMessageSourceList.tSentinel.nNext + gMessageSourceList.nLinkOffset));
        FAULT_TLinkUnlink(&gMessageSourceList.tSentinel);
    }
};
static MessageSourceListLifetime sgMessageSourceListLifetime;


//1.10f: D2Client.0x6FB6BB88
//  Installs an assert handler and answers the one it replaces: two instructions over gpfnFaultAssertHandler. The slot's two references are
//  this function's, so the two handlers installed -- GAME_TeardownGraphics from GAME_InitGame and RECORD_CleanupHandler -- are stored and
//  never called. `void *` because the two installers pass different function types. __fastcall: edx is unused and the return is a bare retn.
void *__fastcall FAULT_SetAssertHandler(void *pfnNewHandler)
{
    void *pfnOldHandler;

    pfnOldHandler = gpfnFaultAssertHandler;
    gpfnFaultAssertHandler = pfnNewHandler;
    return pfnOldHandler;
}

//1.10f: D2Client.0x6FB6BB94
//  Adds a message source: the lock made on first use (FAULT_Init, behind gbMsgSourceLockReady), taken, a node allocated and linked at the
//  head of gMessageSourceList (TSList::NewNode with type 1, no extra bytes, no flags), the source stored in it, the lock released. Its one
//  caller is LNG_Initialize, with LNG_MessageSource.
void __fastcall FAULT_RegisterMessageSource(int32_t (__stdcall *pfnMessageSource)(DWORD dwMessageId, LPSTR pszBuffer, int32_t nMaxLength))
{
    if (!gbMsgSourceLockReady)
        FAULT_Init();
    EnterCriticalSection(&gMsgSrcCriticalSection);
    //  BUG: the node is written without a NULL test. UNREACHABLE: Storm's SMemAlloc never answers NULL.
    FAULT_TLinkedListAllocNode(&gMessageSourceList, 0, 1, 0, 0)->pfnMessageSource = pfnMessageSource;
    LeaveCriticalSection(&gMsgSrcCriticalSection);
}

//1.10f: D2Client.0x6FB6BBCF
//  The lock, once: initialised, FAULT_AtExit handed to atexit, the flag set (a ten-byte `mov gbMsgSourceLockReady, 1`, so the flag is a
//  dword). Not itself thread-safe; its one caller tests the flag first.
void __cdecl FAULT_Init(void)
{
    if (!gbMsgSourceLockReady)
    {
        InitializeCriticalSection(&gMsgSrcCriticalSection);
        atexit(FAULT_AtExit);
        gbMsgSourceLockReady = 1;
    }
}

//1.10f: D2Client.0x6FB6BBF9
//  The atexit cleanup: under the lock, every node unlinked and freed (FAULT_TLinkedListFreeAll, TSList::Clear), then the lock released and
//  deleted and the flag cleared. Entered only through FAULT_Init's `push offset`.
void __cdecl FAULT_AtExit(void)
{
    EnterCriticalSection(&gMsgSrcCriticalSection);
    FAULT_TLinkedListFreeAll(&gMessageSourceList);
    LeaveCriticalSection(&gMsgSrcCriticalSection);
    DeleteCriticalSection(&gMsgSrcCriticalSection);
    gbMsgSourceLockReady = 0;
}

//1.10f: D2Client.0x6FB6BC27
//  An error box with OK and Cancel (MB_OKCANCEL | MB_ICONHAND | MB_TASKMODAL, 0x2011), captioned *gpszD2ClientCaption ("D2Client.dll"), and
//  TRUE when the answer was IDCANCEL. Its one caller is LNG_Initialize.
BOOL __fastcall FAULT_PromptErrorOrCancel(LPCSTR lpText)
{
    return MessageBoxA(NULL, lpText, gpszD2ClientCaption, MB_OKCANCEL | MB_ICONHAND | MB_TASKMODAL) == IDCANCEL;
}

//1.10f: D2Client.0x6FB6BC59
//  TSList::Clear: while there is a head -- the terminator's nNext when it is positive -- the node is destroyed (~TMESSAGESOURCE, which is
//  its link's destructor, FAULT_TLinkUnlinkThunk, which unlinks it) and freed under the template's tag and line -2. Its one caller is
//  FAULT_AtExit.
void __fastcall FAULT_TLinkedListFreeAll(TSExplicitList *pList)
{
    int32_t nHead;
    D2MessageSourceStrc *pNode;

    for (;;)
    {
        nHead = pList->tSentinel.nNext;
        pNode = nHead > 0 ? (D2MessageSourceStrc *)(uintptr_t)nHead : NULL;
        if (!pNode)
            break;

        FAULT_TLinkUnlinkThunk(&pNode->tLink);
        STORM_SMemFree(pNode, szMessageSourceTag, -2, 0);
    }
}

//1.10f: D2Client.0x6FB6BC89
//  TSList::NewNode: nExtraBytes more than a node allocated under the template's tag and line -2, with flag 8 forced into the caller's
//  flags (`or al, 8`, the byte form of `| 8`); flag 8 is Storm's zero-fill. The link's two dwords are cleared (TMESSAGESOURCE's
//  constructor; the source slot is not written here), and the node is linked when nLinkType is not 0, with no anchor. Returns the node.
D2MessageSourceStrc *__fastcall FAULT_TLinkedListAllocNode(TSExplicitList *pList, int32_t nEdxUnused, uint32_t nLinkType, uint32_t nExtraBytes, uint32_t dwAllocFlags)
{
    D2MessageSourceStrc *pNode;

    pNode = (D2MessageSourceStrc *)STORM_SMemAlloc(nExtraBytes + sizeof(D2MessageSourceStrc), szMessageSourceTag, -2, dwAllocFlags | 8);
    if (pNode)
    {
        pNode->tLink.pPrevLink = NULL;
        pNode->tLink.nNext = 0;
    }

    //  BUG: a failed allocation is linked too, as the terminator. UNREACHABLE: Storm's SMemAlloc never answers NULL.
    if (nLinkType)
        FAULT_TLinkedListLink(pList, 0, pNode, nLinkType, NULL);
    return pNode;
}

//1.10f: D2Client.0x6FB6BCD3
//  TSLink's destructor, whose body is `this->Unlink()`: five bytes, `jmp FAULT_TLinkUnlink`. Called by FAULT_TLinkedListFreeAll's node
//  destructor (~TMESSAGESOURCE is its one member's).
void __fastcall FAULT_TLinkUnlinkThunk(TSLink *pLink)
{
    FAULT_TLinkUnlink(pLink);
}

//1.10f: D2Client.0x6FB6BCD8
//  TSList::LinkNode. The node's link (the node plus the link offset, or the terminator for NULL) is unlinked first if it is linked; the
//  anchor's link is found the same way. Type 1 puts the node after the anchor: its pPrevLink the anchor, its nNext the anchor's, the link
//  after the anchor given the node's link as its pPrevLink (TSLink::NextLink, inlined, with the list's offset), and the anchor's nNext the
//  node; with no anchor that is the head. Any other type puts it before the anchor: the type is tested for 1 alone, there is no third arm.
//  NextLink ends the list at nNext < 0 (`jge`), not <= 0. The node and the anchor are pointers to objects.
void __fastcall FAULT_TLinkedListLink(TSExplicitList *pList, int32_t nEdxUnused, D2MessageSourceStrc *pNode, uint32_t nLinkType, D2MessageSourceStrc *pExisting)
{
    TSLink *pLink;
    TSLink *pAnchor;
    TSLink *pAnchorPrev;
    int32_t nNext;
    int32_t nOffset;
    TSLink *pNextLink;

    if (pNode)
        pLink = (TSLink *)((char *)pNode + pList->nLinkOffset);
    else
        pLink = &pList->tSentinel;
    if (pLink->pPrevLink)
        FAULT_TLinkUnlink(pLink);

    if (pExisting)
        pAnchor = (TSLink *)((char *)pExisting + pList->nLinkOffset);
    else
        pAnchor = &pList->tSentinel;

    if (nLinkType == 1)
    {
        pLink->pPrevLink = pAnchor;
        pLink->nNext = pAnchor->nNext;

        nNext = pAnchor->nNext;
        nOffset = pList->nLinkOffset;
        if (nNext < 0)
        {
            pNextLink = (TSLink *)~nNext;
        }
        else
        {
            if (nOffset < 0)
                nOffset = (int32_t)((uintptr_t)pAnchor - pAnchor->pPrevLink->nNext);
            pNextLink = (TSLink *)((uintptr_t)nNext + nOffset);
        }
        pNextLink->pPrevLink = pLink;
        pAnchor->nNext = (int32_t)(uintptr_t)pNode;
    }
    else
    {
        pAnchorPrev = pAnchor->pPrevLink;
        pLink->pPrevLink = pAnchorPrev;
        pLink->nNext = pAnchorPrev->nNext;
        pAnchorPrev->nNext = (int32_t)(uintptr_t)pNode;
        pAnchor->pPrevLink = pLink;
    }
}

//1.10f: D2Client.0x6FB6BD6E
//  TSLink::Unlink with NextLink(-1) folded in: nothing for a link that is not linked (pPrevLink NULL); otherwise the next link -- ~nNext at
//  the end of the list, else this link moved from its own object to the next one -- takes this link's pPrevLink, the previous link takes
//  its nNext, and both are cleared. The end test is `jge`.
void __fastcall FAULT_TLinkUnlink(TSLink *pLink)
{
    TSLink *pPrevLink;
    int32_t nNext;
    TSLink *pNextLink;

    pPrevLink = pLink->pPrevLink;
    if (!pPrevLink)
        return;
    nNext = pLink->nNext;
    if (nNext >= 0)
        pNextLink = (TSLink *)((uintptr_t)pLink - pPrevLink->nNext + nNext);
    else
        pNextLink = (TSLink *)~nNext;
    pNextLink->pPrevLink = pPrevLink;
    pLink->pPrevLink->nNext = pLink->nNext;
    pLink->pPrevLink = NULL;
    pLink->nNext = 0;
}
