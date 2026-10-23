#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"

//  CUnitEvent.cpp -- the client's per-unit event list of D2Client.dll 1.10: a doubly linked list of 32-byte D2UnitEventStrc records on
//  D2UnitStrc +0x90 (pCltTimerList, the client arm of the union there), allocated from the unit's own memory pool. Four functions,
//  0x6FB2A700..0x6FB2A874. Three of them name this file in the path they hand to the pool allocator and its free.
//
//  The record as the client fills it (CUNITEVENT_AddEvent):
//      +0x00  the event id, stored and compared as a DWORD (the struct declares a byte, a byte and a word there: nUnitEvent, unk0x01, nFlags)
//      +0x04  nKind        (nQueueNo)    +0x08  nParam       (unk0x08)
//      +0x0C  nSkillId     (nGUID1)      +0x10  nSkillLevel  (nGUID2)
//      +0x14  the handler  (pCallback)   +0x18  pPrevious    +0x1C  pNext
//  The code keeps the struct's member names and reads and writes +0 as the DWORD.
//
//  Callers: SKILLS_StatesAddCltEventFunc adds, PLRSKILLS_StatesRunStateRemFunc removes by key, MISSILE_ClientOnHit fires and CUNIT_DestroyUnit
//  frees the list. SKILLS_StatesAddCltEventFunc refuses a handler index outside [0, 2) and a NULL slot of gpStateCltEventFuncs before it adds,
//  so every record CUNITEVENT_FireEvent calls has a handler.

//  The path literal handed to FOG_AllocPool and FOG_FreePool by the three functions that name this file (__FILE__ in the original).
static const char szCUnitEventCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\CUnitEvent.cpp";

//  What +0x14 holds, as CUNITEVENT_FireEvent calls it: ecx the event, edx the unit, then the source unit, the unused slot and the record's
//  +0x0C and +0x10 on the stack.
typedef int32_t(__fastcall *CUNITEVENT_HANDLER)(int32_t nEvent, D2UnitStrc *pUnit, D2UnitStrc *pSourceUnit, int32_t nUnused, int32_t nSkillId, int32_t nSkillLevel);

static_assert(offsetof(D2UnitEventStrc, nQueueNo) == 0x04
              && offsetof(D2UnitEventStrc, unk0x08) == 0x08
              && offsetof(D2UnitEventStrc, nGUID1) == 0x0C
              && offsetof(D2UnitEventStrc, nGUID2) == 0x10
              && offsetof(D2UnitEventStrc, pCallback) == 0x14
              && offsetof(D2UnitEventStrc, pPrevious) == 0x18
              && offsetof(D2UnitEventStrc, pNext) == 0x1C,
              "AddEvent stores +4, +8, +0Ch, +10h, +14h and +1Ch, the unlink reads +18h "
              "and +1Ch, and FireEvent calls through +14h");


//1.10f: D2Client.0x6FB2A700
//  Frees the list: each record goes back to the unit's pool, its next taken before it goes, and the head is cleared whether the list held
//  anything or not.
void __fastcall CUNITEVENT_FreeAllForUnit(D2UnitStrc *pUnit)
{
    D2UnitEventStrc *pEvent;
    D2UnitEventStrc *pNext;

    if (!pUnit)
        return;

    pEvent = pUnit->pCltTimerList;
    if (pEvent)
    {
        do
        {
            pNext = pEvent->pNext;
            FOG_FreePool(pUnit->pMemoryPool, pEvent, szCUnitEventCpp, 47, 0);
            pEvent = pNext;
        } while (pNext);
    }
    pUnit->pCltTimerList = NULL;
}


//1.10f: D2Client.0x6FB2A740
//  Adds an event at the head: a 32-byte record from the unit's pool, cleared, filled and pushed on the front of the list, the old head's
//  pPrevious pointed back at it. Five stack arguments (retn 14h). The stores follow the original's order, and the head is read twice (for the
//  new record's next and for the back link), as the original does.
//  BUG: FOG_AllocPool's answer is cleared untested (0x6FB2A773). LATENT: only a failed 32-byte allocation reaches the clear.
D2UnitEventStrc *__fastcall CUNITEVENT_AddEvent(D2UnitStrc *pUnit, int32_t nCltEvent, int32_t nSkillId, int32_t nSkillLevel, void *pfnHandler, int32_t nKind, int32_t nParam)
{
    D2UnitEventStrc *pNewEvent;

    if (!pUnit)
        return NULL;

    pNewEvent = (D2UnitEventStrc *)FOG_AllocPool(pUnit->pMemoryPool, sizeof(D2UnitEventStrc), szCUnitEventCpp, 72, 0);
    memset(pNewEvent, 0, sizeof(D2UnitEventStrc));
    pNewEvent->nGUID1 = nSkillId;
    //  +0 is a DWORD in the client's use
    *(int32_t *)&pNewEvent->nUnitEvent = nCltEvent;
    pNewEvent->nGUID2 = nSkillLevel;
    pNewEvent->pCallback = (D2UnitEventCallbackFunction)pfnHandler;
    pNewEvent->pNext = pUnit->pCltTimerList;
    pNewEvent->nQueueNo = (uint32_t)nKind;
    pNewEvent->unk0x08 = nParam;
    if (pUnit->pCltTimerList)
        pUnit->pCltTimerList->pPrevious = pNewEvent;
    pUnit->pCltTimerList = pNewEvent;
    return pNewEvent;
}


//1.10f: D2Client.0x6FB2A7C0
//  Removes every event with this key, the pair (nKind, nParam) at +0x04 and +0x08; the whole list is walked, not just to the first match.
//  The next is taken before anything else and is what the walk moves on to, so a freed record cannot lose the cursor. The unlink reads the
//  record's next a second time for the back link, as the original does.
void __fastcall CUNITEVENT_RemoveEventsByKey(D2UnitStrc *pUnit, int32_t nKind, int32_t nParam)
{
    D2UnitEventStrc *pEvent;
    D2UnitEventStrc *pNextSaved;

    if (!pUnit)
        return;

    pEvent = pUnit->pCltTimerList;
    if (!pEvent)
        return;

    do
    {
        pNextSaved = pEvent->pNext;
        if ((int32_t)pEvent->nQueueNo == nKind && pEvent->unk0x08 == nParam)
        {
            if (pEvent->pPrevious)
                pEvent->pPrevious->pNext = pNextSaved;
            else
                pUnit->pCltTimerList = pNextSaved;
            if (pEvent->pNext)
                pEvent->pNext->pPrevious = pEvent->pPrevious;
            FOG_FreePool(pUnit->pMemoryPool, pEvent, szCUnitEventCpp, 128, 0);
        }
        pEvent = pNextSaved;
    } while (pNextSaved);
}


//1.10f: D2Client.0x6FB2A830
//  Fires an event: every record whose +0 DWORD is nEvent has its handler called with six arguments (the typedef above). The next is taken
//  before the call, so a handler may free its own record. The answer is the last matching handler's, 0 when none matched; the one caller,
//  MISSILE_ClientOnHit, does not read it.
int32_t __fastcall CUNITEVENT_FireEvent(int32_t nEvent, D2UnitStrc *pUnit, D2UnitStrc *pSourceUnit, int32_t nUnused)
{
    int32_t nHandlerResult;
    D2UnitEventStrc *pEvent;
    D2UnitEventStrc *pNextSaved;

    nHandlerResult = 0;
    if (pUnit)
    {
        pEvent = pUnit->pCltTimerList;
        if (pEvent)
        {
            do
            {
                pNextSaved = pEvent->pNext;
                if (*(int32_t *)&pEvent->nUnitEvent == nEvent)
                    nHandlerResult = ((CUNITEVENT_HANDLER)pEvent->pCallback)(nEvent, pUnit, pSourceUnit, nUnused, pEvent->nGUID1, pEvent->nGUID2);
                pEvent = pNextSaved;
            } while (pNextSaved);
        }
    }
    return nHandlerResult;
}
