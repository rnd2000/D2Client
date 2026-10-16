# BUGS

**Every defect and every illogicality found in the ORIGINAL `D2Client.dll` 1.10,
with the measurement that settled each one and a verdict on whether the game can
hit it.**

Opened 2026-09-11, while the rewrite was working through `inv.cpp`. An entry
is written only after thorough verification: that is the whole standard of
this file, and it is why the last section exists.

## What this is NOT

* **It records the ORIGINAL's mistakes**, not the rewrite's. When a reading here
  turns out to be wrong, the entry moves to REFUTED.
* **It is not a list of things to fix.** The standing rule of the rewrite is
  that a bug in the original is **reproduced and written down, never fixed** —
  a rewrite that quietly corrects the original is a different program that
  happens to compile. Every entry below is reproduced in the `.cpp` files unless the entry says otherwise and says why.
* **It is a summary.** Each entry gives its addresses and names the file whose
  code it concerns.

## The verdicts

| verdict | what it means |
|---|---|
| **REACHABLE** | ordinary play, or ordinary input, can reach it. The argument for reachability is given, not assumed |
| **INERT** | the defect is real and reached, and **no consumer can observe it** — every reader masks the damaged bits, or the value is overwritten before use. Stronger than "latent": the code runs, and nothing downstream can tell |
| **LATENT** | the defect is real and the data or the callers prevent it. What prevents it is named |
| **UNREACHABLE** | the code cannot execute at all. Usually a dead arm or a flag nothing sets |
| **NOT ESTABLISHED** | a real defect whose reachability was looked at and not settled. **This is a finding, not a placeholder** — the entry says what would settle it |
| **REFUTED** | it looked like a bug and is not. The proof is given, because the appearance will recur |

**A verdict without a measurement is not an entry.** Every line below cites the
instruction, the address, or the tool run that establishes it.

---

# REACHABLE

## `HIREABLES_HirIconsOnF11` claims the message and then keeps running

`0x6FADB98D`, `Hireables.cpp`.

When `gnPartyPortraitMode == 2` the handler sets `bReturn = 1`, calls
`SMsgBreakHandlerChain` — and **there is no `retn`**. Execution falls into the
rest of the body, which can set `bReturn` back to 0 on the next branch.

    6FADB99E  call SMsgBreakHandlerChain
    6FADB9A3  call CUNIT_GetPlayer      <- no retn between them

**Why reachable:** `gnPartyPortraitMode` is not a dead mode.
`HIREABLES_CmdTblActionShowPortraits` — the portraits toggle key — writes 2 into it at
`0x6FADBDB7` and `0x6FADBDE2`, two of its
fourteen references. Toggle portraits until the mode is 2, then press F11. If the
player has an associated unit of type 7 the message is reported **unhandled
after its chain was already broken**; if not, the chain is broken a second time.

All three decompilers agree, which is worth stating because a missing return is
exactly the shape a rendering invents or hides. Every sibling handler in the
same batch — RECIPESCROLL, INISCROLL, MINISKILL — ends that block with a return.

## The left and right arrows move a dialog's selection the wrong way round

`0x6FAD6300` and `0x6FAD63C0`, `dialog.cpp`. **Found 2026-09-12** by reading
the four handlers together.

They are ONE BODY instantiated four times -- all 178 bytes, and measured byte
for byte they differ in only three things beyond the call displacements:

                    assert line   control vtable slot   step
        OnArrowUp       626            +0x1C            dec esi
        OnArrowDown     656            +0x20            inc esi
        OnArrowRight    565            +0x24            dec esi
        OnArrowLeft     597            +0x28            inc esi

Four CONSECUTIVE vtable slots in the order up, down, right, left, which is
what confirms the names against `gDialogMessageHandlers` rather than resting
on them. The table maps VK_UP, VK_DOWN, VK_RIGHT and VK_LEFT to exactly those
four.

**So LEFT steps the selection forward, like DOWN, and RIGHT steps it back,
like UP.** In a vertical list of dialog lines the conventional mapping is
left with up and right with down, and this is the opposite.

**REACHABLE.** A dialog with more than one selectable line is the ordinary
case -- every NPC talk menu is one -- and the arrow handlers are registered
by both constructors. `nSelectableCount > 1` is the only other condition and
DIALOG_AddLine maintains it.

**And the wheel settles which pair is the odd one.** `DIALOG_OnMouseWheel`
uses the same two loop bodies and pairs wheel-up with `dec` and wheel-down
with `inc`, agreeing with the vertical arrows. So three of the five inputs
agree with each other and the horizontal pair does not.

Reproduced, all four written out in full, because they are four functions in
the image.

## `UIPANELS_BuildOtherPlayerLabel` has its clip axes crossed

`0x6FB22663`, `ui_panels.cpp`.

    6FB22670  mov eax, gnScreenHeight     <- and the label's X is compared
    6FB22675  add eax, 32h                   against the HEIGHT
    6FB22678  cmp ecx, eax

and the Y is bounded by `gnScreenWidth` the same way. **The two axes are
swapped.**

**Why reachable:** at 800x600 the label of any player whose X has reached 650
vanishes — the right eighth of the screen — while a Y between 600 and 800 passes
a test it should have failed. At 640x480 the limits become 530 and 640 and it
bites the other way round. **Only a square resolution hides it.**

Re-measured from the bytes rather than trusted from the pseudocode, because a
swap like this reads as a decompiler artefact until it is checked.

## `MSG_Send_D2CLTSYS_OPENCHAR` sends one byte too many, every packet

`0x6FAAD6D8` and `0x6FAAD7B8`, `Msg.cpp`.

    6FAAD6CD  cmp  eax, 0FFh        the copy loop fills saveData[0..254]
    6FAAD6D8  push 106h             262 -- but 6 + 255 is 261

    6FAAD7B8  lea  esi, [eax+7]     nTailBytes + 7, and 6 + n is n + 6

**One conceptual error, not two: the author counted the header as SEVEN bytes.
It is six** — measured from the packet's own layout, and D2MOO's independent
reconstruction of the same packet agrees at 0x06.

**Why reachable:** the sole caller `GAME_InitGame` passes a literal
`bShowLoadingScreen = 1` at `0x6FAA953D`, guarded only by the four network game
types, so both paths are live in every network game.

## `WALL_InsertNodeByLayer` never compares the last node

`0x6FAB8290`, `Wall2.cpp`.

The layer sort advances on `pNode->pNext` and compares against `pNode`, so it
stops one node early. When `pNode` is the last element `pNext` is already NULL,
the loop body never runs for it, and the tail appends unconditionally.

**A list of exactly one node is never compared at all.** With N nodes the first
N−1 are ordered and the last two can be inverted — a draw-order defect on the
wall layer.

All three decompilers print the same shape; Ghidra prints it clearest.

## The gold dialog dereferences a NULL it modelled

`0x6FB23E88`, `ui.cpp`.

All four controls are built with `SMemAlloc(12, "new", -1, 0)` and an explicit
`else pControl = NULL`, so **the original models the allocation failing**. Three
are then guarded and one is not:

    6FB23E1C  mov ecx, gpGoldDialogSpinner / test ecx,ecx / jz    GUARDED
    6FB23E88  mov ecx, gpGoldDialogOkButton / mov edx,[ecx]       NOT

Twelve bytes, so out of memory is not a thing that happens in practice — but
this is not latent in the ordinary sense either, because **the program takes a
branch specifically for the NULL case and then reads through it two
instructions later**. Verdict: reachable on allocation failure.

## `INV_NpcShopBuildTabs` leaves the Close tab's cursor from the previous NPC

`0x6FAE922B`, `inv.cpp`. **Found 2026-09-11.**

The shop's tab bar is four records of 20 bytes at `0x6FBB37A8`:

    +0  enabled   +4  selected   +8  string id
    +10 panel     +14 action     +16 CURSOR

On the merchant path `INV_NpcShopBuildTabs` writes slot 3 as the Close tab with
four direct stores — enabled `0x6FAE922B`, string `0x6FAE9231`, panel
`0x6FAE923A`, action `0x6FAE9244` — and **never writes the fifth field, the
cursor at +16 (`0x6FBB37F4`)**. There is no setter call on this path; the
setters are the only code that writes that field.

**The field is read.** `INV_NpcShopApplyTabSelection` at `0x6FAEAD0F` does
`mov ecx, gNpcShopTabCursors[esi]` with `esi = 5 * gnNpcShopTabHit`, i.e. byte
offset `20 * hit`, and hands it straight to `CURSOR_SetCursorBuySell`. Clicking
the Close tab takes that path: the guard above it excludes only string 10095
(Repair-all), and the Close tab's string is 4144.

**What it holds.** `ida_bytes.is_loaded` is False for the whole slot-3 record —
it is inside `.data`'s virtual extent but past its raw size — so the loader
zero-fills it and it is **0** until something writes it. The only writer is
`INV_NpcShopSetTabRepairAll`, which puts **8** there, and only smiths take that
path. So:

    a session that has never opened a smith    the Close tab's cursor is 0
    after any smith's panel has been opened    it is 8, the repair-all cursor

**Why reachable:** open Charsi, Fara, Hratli, Halbu or Larzuk, close, then open
any merchant and click Close. `0x6FBB37F4` has **no
direct cross-reference at all**, which is why the field has no name and why the omission is
easy to miss reading the source of either function alone.

**What it looks like on screen is NOT ESTABLISHED** — that needs
`CURSOR_SetCursorBuySell`'s handling of 8 against 0, and what panel mode 9 does
next.

## The mercenary's life row: a floor at one that cannot fire

`INV_MercInvDrawStatRows`, 0x6FAEDB3A, `inv.cpp`. **Found 2026-09-12.**

The life row prints `cur / max`, cur being the max scaled by the roster's life
percent, and then

    if (!CUNIT_IsUnitDead(pMerc) && nLife < 0)  nLife = 1;

-- `test ebp, ebp` / `jge` at 0x6FAEDB3A; Hex-Rays, Ghidra and Binary Ninja all
print `< 0`. A store of 1 means something only as a floor for a living
hireling, and **no value the test can see is negative**: the percent is the
roster's `nLifePercent`, written from packet 0x7F's word zero-extended (`and
edx, 0FFFFh` at 0x6FAB42B2), and the max life is positive.

**REACHABLE**: the server's percent is `100 * hp / maxhp` in integer division
(D2MOO's D2Common, `UNITS_GetCurrentLifePercentage`, which D2Game's 0x7F
sends), so a hireling alive with less than 1% of its life gets 0 and the row
reads "0 / <max>". `ROSTERPETS_GetLifePercent` also answers 0 for a pet the
roster does not hold. The database's comment said "a living mercenary never
reads 0"; corrected there by appending.

## Nihlathak's "hurry up" line is timed to wait two minutes and plays at once

`0x6FAF6215`, `NPCMENU_TickAndValidate`, `npcmenu.cpp`. **Found 2026-09-13**,
batch 5 -- the database had recorded the behaviour since 2026-08-24; the
reachability argument is new.

With the waiting dialog up and the active NPC's class 514 (`nihlathak`,
monstats `hcIdx`), a first pass arms

    gnNihlathakHurryTick = GetTickCount() + 200 * (5 * (seed % 30) + 600)

-- 120 000 to 149 000 ms ahead, from the player's seed -- and then

    6FAF6213  call edi                      ; GetTickCount
    6FAF6215  cmp  gnNihlathakHurryTick, eax
    6FAF621B  jbe  skip                     ; skip when the deadline PASSED

so sounds.txt 4560 `nihlathak_hurryup` plays while the deadline is still
AHEAD, on the frame it was armed, and `gbNihlathakHurrySoundPlayed` stops it
ever playing again. The deadline never decides anything; only a tick count
wrapping inside the window could. Ghidra prints `GetTickCount() < deadline`,
Binary Ninja `data_6fbb5d90 u> eax_24`, Hex-Rays `gnNihlathakHurryTick >
GetTickCount()`. Whether a delay was meant is not in the binary; that none
happens is.

**REACHABLE:** `NPCMENU_SendTransactionAndWait` creates the waiting dialog
after every buy, sell or repair it sends; `gaUiToggleRules[UI_NPCMENU][UI_NPCSHOP]`
is 0, so opening the shop leaves UI_NPCMENU on and
`UIPANELS_DrawInGamePanels` keeps calling the tick; the main menu, which the tick
would draw first, is closed on the way into the shop by
`NPCMENU_OnTradeClicked`. So the first transaction with Nihlathak in a game
plays the line at once -- given at least one frame between the send and the
server's answer, which a network game gives and a single-player game may not.
The three data comments that said "when his menu opens" were corrected to
"when a transaction waits".

## The identify arms test `ITEMS_CheckItemFlag` for exactly 1, and it answers 0x10

`NPCMENU_TransactionCost` (0x6FAFB200) and `NPCMENU_BeginTransaction`
(0x6FAFA260), `npcmenu.cpp`. **Found 2026-09-13** in batch 9a's reading,
settled in batch 9b.

Both functions refuse an already-identified item at a Cain the same way, at
the same source line:

    6FAFB2ED  push 927h                    line 2343, in both functions
    6FAFB2F5  call ITEMS_CheckItemFlag     (pItem, IFLAG_IDENTIFIED, ...)
    6FAFB2FA  cmp  eax, 1                  0x6FAFA52B in BeginTransaction
    6FAFB2FD  jnz  price_it

and D2Common's `ITEMS_CheckItemFlag` does not return a BOOL:

    6FD98764  mov  eax, [eax+18h]          the item's flag word
    6FD9876B  and  eax, ecx                & dwFlag: 0 or 0x10

So the compare never succeeds. TransactionCost prices an identified item for
identifying -- tbl 3332 'Identify cost: ' and 100, or 0 once quest 4's reward
is granted -- where the arm meant to clear the text and answer 1; and
BeginTransaction would start transaction 4 on it. D2MOO writes the test as a
plain `if (ITEMS_CHECK_FLAG(...))`, which would work; the binary's 1 does not.

**REACHABLE in TransactionCost, through the belt.** Talk to cain2, cain3 or
cain5 (244, 245, 265) and hover a potion in the belt:

* `PANEL_GameOnMouseMove` reaches `PANEL_BeltUpdateHoveredItem`, which sets
  `gbBeltHoverActive` and `gpBeltHoverItem` (0x6FB0164E, 0x6FB016D3). The NPC
  menu is a dialog, `DIALOG_OnMouseMove` breaks the handler chain only when a
  control takes the move -- its line path lets the message run on -- and
  turning UI_NPCMENU on registers no handler table (ui.cpp names it only in
  the turn-off switch);
* `UIPANELS_DrawInGamePanels` calls `PANEL_Draw800BorderFrame` with no test
  (0x6FB21FF7), and that calls `PANEL_DrawBeltItemHint` last, with none
  (0x6FAFF9AC);
* the hint, in inventory mode 1 -- the idle value, which
  `INV_IsPanelModeNormal` accepts -- calls TransactionCost with `mov edx, 1`
  (0x6FB00BA7), an owned item, while the NPC is active; the Cain arm returns 1
  with its label and the hint appends it under a newline.

So every belt potion shows 'Identify cost: 100' there. D2MOO's D2Game sets
IFLAG_IDENTIFIED where it initialises an item unit (`ItemMode.cpp`,
`sub_6FC4BE80`) and clears it only in the quality arms (lines 6572, 6636,
6666), so a belt potion is identified and the arm meant to leave no line.

**BeginTransaction's copy is NOT ESTABLISHED.** Its seven callers are click
handlers of the inventory, the stash, the cube and the shop. The shop panel,
UI_NPCSHOP, is turned on only by `NPCMENU_OnTradeClicked` and
`NPCMENU_OnGambleClicked` (`mov ecx, 0Ch` with `xor edx, edx` before the calls
at 0x6FAF5CE3 and 0x6FAF5E71; every other UI_SetUIToggle of panel 12 in the
module passes 1, and the Esc menu restores only windows 6, 7, 10, 17, 21 and
35); those two are reached only through `gNpcMenuDescriptors`, and no Cain
record (16..19, 38) holds either -- they ship Talk and Identify.
`gaUiToggleRules[UI_NPCMENU]` refuses the inventory (2) and errors on the stash
and the cube (3). What is not measured is an interaction that starts while the
stash or the cube is open, whose rule 3 refuses UI_NPCMENU: whether
`gbNpcInteractionActive` is set by then decides it.

Reproduced as written: a static helper carries line 2343, and both callers
compare its answer with 1.

## The price and the transaction disagree about two Cains

`NPCMENU_TransactionCost` (0x6FAFB200) against `NPCMENU_BeginTransaction`
(0x6FAFA260), `npcmenu.cpp`. **Found 2026-09-13** in batch 9a's reading.

BeginTransaction identifies at 244, 245, 246, 265 and 520 -- cain2 to cain6,
the five records `NPCMENU_ShowTransactionResult` trims. TransactionCost's
switch (0x6FAFB29C .. 0x6FAFB34C, three jump tables) puts 246 in its SELL arm,
beside 252 asheara, and has no 520 at all, so the default answers 0. At cain4
an owned item is priced as a sale -- 'Sell value: ' and the
TRANSACTIONTYPE_SELL figure, after `ITEMS_IsNotQuestItem` -- and at cain6 it is
not priced.

**REACHABLE**, by the belt route of the entry above: at cain4 a hovered belt
potion shows 'Sell value: N', at cain6 no second line, and 'Identify cost: 100'
at the other three. Which list the source meant is not in the binary; D2MOO's
reconstruction identifies at cain2 to cain5 and has neither 252 nor 520.
BeginTransaction's side is the same NOT ESTABLISHED as above.

Reproduced as written: each switch carries its own list.

## The interrupt dialog reads Akara's abort flag for the two palace guards

`NPCMENU_BeginInteraction` (0x6FAF8250) -- its inlined interrupt-dialog opener
-- over `NPCMENU_FindEntryIndexForNpc` (0x6FAF8E40), `npcmenu.cpp`. **Found** in
batch 1, which measured the miss and left its reachability open; **settled
2026-09-14**, batch 10b.

FindEntryIndexForNpc answers 0 both for "found at 0" and for "not found", and
the opener uses the answer unchecked: `mov al, byte ptr
(gNpcMenuEntries+15h)[ebp*2]` at 0x6FAF8837 and the store of 0 at 0x6FAF8877 --
on a miss, entry 0's abort flag, Akara's (class 148).

**REACHABLE.** A miss needs an NPC that reaches the opener with no entry:

    the entries    46 records holding 45 classes, 257 twice (gNpcMenuEntries out of the stock DLL's
                   .data, gnNpcMenuEntryCount 46)
    interactable   47 classes with `interact` 1 in stock monstats.txt, by
                   hcIdx: the 45 and two more, 377 act2guard4 and 378
                   act2guard5
    the arm        the opener runs when the message list holds no type 1 or 2
                   node and a type 0 one. D2MOO's A2Q7.cpp gives act2guard4 a
                   row on every arm of its activate callback and A2Q8.cpp
                   gives act2guard5 one under its quest's conditions, all with
                   nMenu 1; QUESTS_InitScrollTextChain stores nMenu 1 as 0
                   (D2MOO's D2Game Quests.cpp:329..334); TEXT_GetStringIdOfMenuType1Or2Node
                   skips a 0 node and TEXT_GetStringIdOfMenuType0Or2Node
                   answers it (D2Text.cpp:133 and :159)

So a click on act2guard4, the Lut Gholein palace guard, reaches it every time.

**What it changes is small, and not all of it can be seen.**
`NPCMENU_AbortOnPlayerAction` raises all 46 flags on any move, attack or death
of the player's, so the flag read is normally 1, and the opener asks
`OBJECT_PickNpcSpeechSound` for the guard's kind-1 sound -- what a record of
his own would do. The damage is the clear: Akara's flag goes to 0, and her
greeting after `NPCMENU_OpenMainMenu` needs it at 1 (batch 10a). That shows
only if her main menu opens with no action of the player's since the guard, an
act away; whether it can is NOT ESTABLISHED.

Reproduced as written.

## Packet 0x20's level arm refreshes the LOCAL player, not the unit it names

`0x6FAB30A0`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 6.

Packets 0x19..0x1F carry a stat for the local player; **0x20 carries one for a
player named by the GUID at +1** (`xor edx, edx` at 0x6FAB30A3 forces
UNIT_PLAYER and the GUID follows). Both handlers then run the same follow-up,
instruction for instruction, and its STAT_LEVEL arm builds the twelve-byte 0x47
refresh message out of **`CUNIT_GetPlayer()`** rather than out of the unit it
has just changed:

    6FAB3108  call CUNIT_GetPlayer        <- the LOCAL player
              pMsg[1] = pPlayer->dwUnitType
              memcpy(&pMsg[3], &pPlayer->dwUnitId, 4)
              call ITEM_RefreshUnitFromMsg

In 0x19..0x1F the unit IS the local player, so the shared code is right there.
In 0x20 it need not be.

**Why reachable:** any party member levelling up within sight sends 0x20 with
their own GUID. The client then refreshes its OWN items and never theirs, so the
other player's per-level item bonuses are not re-evaluated client-side until
something else refreshes them. Ordinary multiplayer play.

Reproduced as written; the shared follow-up has no assert to prove an inline, so
it is written out twice, as the bytes have it twice.

## `SCMD_HandleSrvAC` gives STAT_PIERCE_IDX the two coordinates added together

`0x6FAB4BD0`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 11.

The monster-assignment packet sets stat 328 on every monster it creates, and the
value it sets is that monster's two coordinates summed:

    6FAB4EE5  mov  eax, [esp+24h]      ; nPosY
              mov  ecx, [esp+28h]      ; nPosX
              add  eax, ecx
              and  eax, 0FFFFh
              call STATLIST_SetUnitStat(pMonster, STAT_PIERCE_IDX, ...)

Those two slots hold nPosY and nPosX at that point: **the layer slot is not
reused for the stat layer until 0x6FAB4F98, after this call**, so the
shared-slot reading IDA's own comment warned against does not apply. Binary
Ninja prints the same sum. The dword loads also take two uninitialised bytes
above each word, which the `and eax, 0FFFFh` discards.

**Why reachable:** it runs on every 0xAC, which is every monster the server
assigns to this client. STAT_PIERCE_IDX is `pierce_idx`, itemstatcost.txt row
328.

Reproduced as written, as `(uint16_t)(nPosY + nPosX)` with a MARKED comment. The
IDA comment that called the line a decompiler artefact was CORRECTED at the
unit's close, 2026-09-15.

## The monster name seed is read sixteen bits wide and kept eight

`0x6FAB4BD0`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 11.

    6FAB4DE1  call FOG_BITMANIP_Read    ; sixteen bits consumed
              movzx si, al              ; the LOW BYTE survives

Sixteen bits come off the wire and eight reach `CUNIT_CreateMonsterUnit`'s name
seed. The stream position is right either way, so nothing downstream
desynchronises -- only the seed is narrower than what was sent.

**Why reachable:** every monster carrying a name seed. The visible consequence is
that a unique monster's generated name is drawn from 256 seeds where the server
offered 65536.

Reproduced as written, with a MARKED comment.

## `SCMD_HandleSrvAA` prints packet 0xA8's text for one of its two bad-stat traces

`0x6FAB4950`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 11.

The handler has two diagnostic paths and they name two different packets:

    6FAB4AFE  stat id out of range  -> "sSCmd_UnitStateAllEx, Attempt to set
                                        an invalid stat!"
    6FAB4B22  nSendBits is zero     -> "sSCmd_UnitStateOnValEx ..."

`sSCmd_UnitStateOnValEx` is **0xA8's** name and the second trace is inside 0xAA,
so anyone reading a trace log is sent to the wrong handler.

**Why reachable:** both paths are ordinary input validation on a variable-length
packet, and the trace leaves through `FOG_Trace` whenever the build's trace level
admits it.

Reproduced as written -- the string is the one the bytes push.

---

## The hover-bubble search reads its sign table once and spends it on both axes

`0x6FB1B6E7`, `text.cpp`, `TEXT_PlaceHoverMsg`. Found at text.cpp's batch 4,
verdict at its close, 2026-09-15.

When a unit's speech bubble would overlap one already placed, the function
walks candidate offsets -- 50 to 400 in steps of 50 on each axis -- and flips
the direction with `gnHoverMsgOffsetSigns` (0x6FB8D27C), eight dwords holding
`{ 1, 1, -1, 1, 1, -1, -1, -1 }` with its count 8 agreeing with its extent.
**One load of that table serves both axes:**

    6FB1B6E7  mov  ecx, gnHoverMsgOffsetSigns[ebp*4]
    6FB1B6F2  mov  eax, ecx
    6FB1B6F4  imul eax, [esp+5Ch+nOffsetX]      the X displacement
    6FB1B6F9  imul ecx, ebx                     the Y displacement

so the probe is always `(+dx, +dy)` or `(-dx, -dy)` and **never the other two
diagonals**. Of the eight passes four are (+,+) and four are (-,-): 392 probes
try 98 distinct rectangles where the table implies 196.

**Why reachable:** it is the ordinary path whenever two units speak near each
other -- the placement search exists for exactly that -- and half the
candidate positions are never tried, so a bubble is pushed further away, or
gives up, where a free slot on the opposite diagonal existed. Nothing gates
it: the loop runs on every placement that is not clear at its first position.

Binary Ninja renders the single read independently, `int32_t ecx_18 =
*((ebp_1 << 2) + &data_6fb8d27c)` followed by two multiplications, so this is
not Hex-Rays folding two loads into one. Reproduced in `text.cpp`.

## The list box hit-tests rows it never draws, and the hire list hires one

`0x6FAF0F34`..`0x6FAF0FC3` in `LISTBOX_HitTestEntry`, `Listbox.cpp`, with a
second effect at `0x6FAF0DF2`..`0x6FAF0E0A` in `LISTBOX_ImplOnArrowUp`. Found
at the unit's re-check, 2026-09-17.

The draw and the hit test walk the same rows and stop in different places.
ImplDraw draws only what `LISTBOX_CountFittingEntries` lets through -- rows
while the running height is at most `nHeight - 20` (`jg` at `0x6FAF007A`).
HitTestEntry walks from `nTopEntry` to `wNumEntries` with no page bound
(`cmp eax, edi / jl` at `0x6FAF0FC1`) and answers any row whose band holds,
drawn or not:

    nY + start + 12  <  mouseY  <  nY + end + 12      0x6FAF0F98..0x6FAF0FB0

**The hire list is where it shows.** `NPCMENU_DrawHireMercenaryList` builds
the box 490 wide and 280 high with a gap of 6 (`push 6`, `push 118h` at
`0x6FAF4FB2`, `0x6FAF4FB4`) and gives every offer two columns: the name line,
and a description that starts with tbl 3995, a space, twice (`0x6FAF5443`,
`0x6FAF5458`) and so is never empty. Every row is therefore 2 * 14 + 6 = 34
pixels; seven fit 260 (238 -- eight would be 272), so seven are drawn. A
fresh NPC offers ten: D2MOO's D2Game (`D2GAME_NPC_FirstFn_6FCC67D0`) marks
exactly ten names available, and the stock `hireling.txt` gives every hiring
act 20 to 67 names to draw them from. Two consequences, both from ordinary
input:

1. **The empty strip under the seventh row hires the eighth mercenary.** Row
   `nTopEntry + 7` answers in `(nY + 250, nY + 278)` -- inside the box, since
   OnMouseDown takes any press with `mouseY < nY + 280` (`jl` at
   `0x6FAF0ACF`), and below the last drawn line. A press there stores it as
   `nHoveredEntry`, a release on the same spot matches it, and `pfnOnSelect`
   -- `NPCMENU_OnHireListConfirmClicked` -- reads back the selection
   HitTestEntry has just stored (`call [eax+2Ch]` at `0x6FAF560A`), bounds it
   and its user data below 10, and hires that offer or asks to. The player
   never saw its name, level or price.
2. **The up arrow scrolls the list down.** OnMouseUp hit-tests the release
   wherever it lands, and `DIALOG_OnMouseUp` hands every release to every
   control with no position test (`0x6FAD5E42`..`0x6FAD5E61`). A press on the
   rows released more than 284 and less than 312 pixels under the list's top
   stores `nSelection = nTopEntry + 8`. Pressing the up arrow then runs
   OnArrowUp -- ImplDraw's repeat fires on the first held frame -- and the
   selection steps to `nTopEntry + 7`, which is not below `nTopEntry +
   nVisibleRows` with seven rows counted, so `0x6FAF0E09`..`0x6FAF0E0A` moves
   the TOP one row down. The close had called that test dead; see the
   CORRECTED note under "The list box's page-follow block is dead in
   ScrollByTrackClick".

The message log is not affected: `TEXT_OpenMessageLog` passes no
`pfnOnSelect` (`push 0` at `0x6FB1F09B`), so HitTestEntry answers -1 at once
and OnArrowUp snaps the selection to the top before it steps.

Reproduced in `Listbox.cpp`, both sites marked.

## The Smite line adds the item damage percent instead of multiplying by it

`0x6FB0EF14`..`0x6FB0EF5D` in `SKILLDESC_DamFunc10`, `SkillDesc.cpp`.
**Found 2026-09-17**, at the unit's re-check against the bytes; verdict the
same day.

skilldesc.txt `descdam` 10 -- in stock data Smite's alone -- is the damage
line the character screen draws for the skill (`CHAR_CharPanelDraw` ->
`SKILLDESC_DrawBothSkillInfos` -> `SKILLDESC_DrawSkillInfo`). It starts from
the shield's items.txt `nMinDam`/`nMaxDam`, builds a damage percent from the
skill's `Param3`/`Param4`, the shield's strength and dexterity bonuses and
STAT_DAMAGEPERCENT, reads STAT_ITEM_MINDAMAGE_PERCENT and
STAT_ITEM_MAXDAMAGE_PERCENT off the player (`push 12h` at 0x6FB0EEC6, `push
11h` at 0x6FB0EED0) -- and adds those two OUTSIDE the multiplication:

    6FB0EF14  mov  ecx, esi          ; the damage percent
    6FB0EF16  mov  edx, [esp+80h]    ; the item MIN percent, stat 18
    6FB0EF1D  imul ecx, ebx          ; * nMinDam
    6FB0EF23  add  ecx, edx          ; + the min percent, unmultiplied
                                     ; / 100 (51EB851Fh, sar edx, 5)
    6FB0EF42  add  ebx, edx          ; + nMinDam

so the line is `min + (min * dmg% + minPct) / 100`, and the max the same with
stat 17 (0x6FB0EF3B..0x6FB0EF5D). Every sibling multiplies:
`SKILLDESC_AddWeaponPhysDamage`, `SKILLDESC_ComputeThrowDamage` and
`SKILLDESC_DamFunc21` take `min + min * (dmg% + pct) / 100`, and so does the
server by D2MOO -- `SKILLS_SrvDo150_Smite`
(D2Game/src/SKILLS/SkillPal.cpp:146) hands the shield to
`SUNITDMG_ApplyDamageBonuses`, which adds `D2_ComputePercentage(min, dmg% +
the STAT_ITEM_MINDAMAGE_PERCENT value)` (D2Game/src/UNIT/SUnitDmg.cpp:228,
and :229 for the max). The skill's own percent is the same on both sides:
Smite's `calc1` is `ln34`, and the handler's
`dwParam[2] + (nSkillLevel - 1) * dwParam[3]` is that formula. A percent of
a hundred moves the line by one point where it doubles the hit.

**Why reachable.** The two stats are the `dmg%` property's -- Properties.txt
func 7, whose D2MOO body writes 18 and 17
(D2Common/src/Items/ItemMods.cpp:3380) -- and they reach the PLAYER: an item
equipped by a unit that cannot dual-wield is merged with `bType` 1
(`ITEM_ApplyGemMods`, `Item.cpp`, the last merge of the body), and
D2MOO's D2Common turns that into adding every stat of the item, the
damage-related ones included, to the owner
(`D2COMMON_10475_PostStatToStatList`, D2Common/src/D2StatList.cpp:1141;
`bType` 0 is the arm that withholds them). A Paladin cannot dual-wield --
D2MOO's `UNITS_CanDualWield` answers yes only for the Barbarian and the
Assassin -- and stock 1.10 hands him the stat on the very shield Smite
strikes with: **Alma Negra**, a unique Sacred Rondache (UniqueItems.txt, base
`pac`, `dmg%` 40-75), whose base carries smite damage 35-58 and a strength
bonus of 100 (Armor.txt). With the 109 strength the base requires, Smite at
level 1 and nothing else adding, the percent is 15 + 109 = 124, and Alma
Negra at 75 makes the line's range 79 to 130: `35 + (75 + 35 * 124) / 100`
and `58 + (75 + 58 * 124) / 100`. The multiplying form gives 104 to 173,
which is what D2MOO's server formula gives in its 8.8 figures, to the
fraction. Every weapon with enhanced damage carries the stat too, and so do
four more stock armour pieces (War Bonnet, Frostburn, Steelrend, Tancred's
Skull).

Reproduced in `SkillDesc.cpp`, marked in the prose and at the formula.

## The tab hover label is never centred -- its right edge is the literal 90

`SKILLSTREE_DrawTabLabel`, `0x6FB17800`, `skillstree.cpp`. The literal
was marked in batch 6; the arithmetic is the close's, 2026-09-18.

Each of the three rows' draw calls passes the hover box's LEFT edge and then
the literal 90 where a right edge belongs:

    mov     ecx, gnScreenWidth
    mov     esi, gnBorderXDifference
    sub     ecx, esi
    push    eax                 ; wszText
    push    5Ah                 ; nRight
    lea     edx, [ebp+14h]      ; nY
    sub     ecx, 59h            ; nLeft = width - borderX - 89
    call    CHAR_DrawTextCentered                     0x6FB179BB

Three copies, one per row -- `nRight` pushed at `0x6FB179B3` (bottom),
`0x6FB179E3` (middle) and `0x6FB17A13` (top). The box beside the text is
filled at the same left edge, 90 wide (`push 5Ah ; nWidth` at `0x6FB17983`),
so the right edge the call wants is the box's own, `width - borderX`.

`CHAR_DrawTextCentered` (`0x6FACFCD0`, `char.cpp`) centres by slack, and this
slack is negative:

    nX        = nLeft;
    nBoxWidth = nRight - nLeft + 1;    /* 90 - (width - borderX - 89) + 1 */
    nTextWidth = D2WIN_10121_GetTextWidth(wszText);
    if (nTextWidth < nBoxWidth)
        nX += (nBoxWidth - nTextWidth) >> 1;

`nBoxWidth` works out to `180 - (width - borderX)`. The panel is 320 pixels
wide and anchored to that same right edge -- `SKILLSTREE_DrawPanel`'s chart
corners are `width - borderX - 320` and `width - borderX - 64` -- so on any
screen the panel is drawn on, the box width is at most -140, `nTextWidth` is
never below it, and the centring branch cannot run. The name is drawn at
`nX = nLeft`: at the box's left edge, left-aligned; a label wider than 90
pixels would run past the box's right edge rather than be clipped.

**Why reachable:** the label is drawn on every frame the cursor is in the
91-pixel tab column and a row window -- an ordinary hover of the skill panel,
itself an ordinary toggle. The labels are `Skill Tab 1`/`2`/`3` (tbl
4224..4226), all narrower than the box, so the miss is one of position and is
cosmetic: a left-aligned name in a box built to centre it. The unit's three
other callers of the same function pass real edges --
`SKILLSTREE_DrawUnavailableSkills` (`0x6FB16A50`), whose "0" goes between
`width - borderX - 65` and `- 25` (the call at `0x6FB16AD9`),
`SKILLSTREE_DrawLabels` at `0x6FB17080` and `SKILLSTREE_DrawLabelFromTop480`
at `0x6FB177C0` (named `PANEL_DrawRightAlignedLine` until 2026-09-18) -- and
those lines do centre. Until the re-check of 2026-09-18 this said two
callers, without the first.

Reproduced as the bytes have it in `skillstree.cpp`.

## `SELECT_ApplySelectedUnit(NULL)` keeps the selection while a popup with no source unit is up

`0x6FAB5AF9..0x6FAB5B0D`, `Select.cpp`. **Argued 2026-09-23** at
Select.cpp's close. **REACHABLE** -- a stale selection, cosmetic as far as
followed.

After the lock, the first test is

    if (TEXT_IsPopupActive() && TEXT_GetPopupSourceUnit() == pUnit)
    {
        TEXT_PopupSetOwnerUnit(pUnit);          /* 0x6FAB5B0D */
        return;
    }

TEXT_GetPopupSourceUnit (0x6FB1D380) answers NULL when the popup's NPC, or
the object named by `dwPopupObjectGUID`, cannot be found. So with pUnit NULL
-- six of the function's nine calls, five of them SELECT_UpdateSelectedUnit's
"clear the selection" gates -- an active popup with no findable source takes
this arm and returns before the `gbHasSelectedUnit = 0` the call was made
for.

**Ordinary play reaches it.** The quest log's replayed speech
(`TEXT_ShowQuestScrollingText(nTailStringId, 1, 0)`, QuestLog.cpp:514) opens
a popup with `bPopupIsNpcSpeech` 0 and `bPopupActive` 1 and never writes
`dwPopupObjectGUID` -- its one writer is the object-message path,
text.cpp:3640 -- so the source is looked up under whatever object GUID an
earlier popup left, and is NULL once that object is gone. A monster the scan
selects on the right half while the quest panel is open then stays selected
when the cursor returns over the panel or the popup: every frame's gate
calls SELECT_ApplySelectedUnit(NULL), and this arm keeps it. What a click
does with that selection was not followed.

Reproduced -- the arm is written as the bytes have it.

## The /framerate overlay formats its cache lines twice, and the tile line carries a `%`

`0x6FAA2B94` and the five `push ecx/eax` before `D2GFX_OutputString` at
0x6FAA2BE5..0x6FAA2CFD, `Draw.cpp` (DRAW_GameDrawDebugOverlay).
**Argued 2026-09-23** at Draw.cpp's close. **REACHABLE.**

Each of the six cache-statistics lines is built with sprintf into szLine,
and szLine is then handed to D2GFX_OutputString as its FORMAT, with no
argument behind it. Five of the six carry only digits and text. The tile
line's own format ends its hit rate with "%%", so szLine holds
"...12.34%, Misses: ..." and D2Gfx formats "%," a second time. What reaches
the screen is D2Gfx's formatter's reading of that pair, which was not read.
Reachable by ordinary input: `/framerate` is a chat command (compared at
0x6FAD2B06) whose DRAW_GameToggleDebugOverlay turns the overlay on at page 0,
where the six lines are drawn -- the tile line whenever D2CMP #10092
reports accesses. (`/fps`, 0x6FAD2AD0, ends at page 1, which skips them.)

Reproduced -- szLine is the format.

# INERT

Real defects, reached in ordinary play, that nothing downstream can observe.

## The partial store of the vendor page — five call sites

`inv.cpp`. **Opened 2026-09-10, settled 2026-09-11.**

Five call sites write **only the low byte** of a register that holds something
else and hand the whole dword on as an `int` page number:

    6FAEA175  ecx = gInvNpcShopRect.nLeft, then `mov cl, gnVendorPage`
              push ecx  ->  INV_PixelToGridCell            INV_NpcShopOnMouseMove
    6FAEA587  ecx = SOUNDINST_PlaySound's leftover, then `mov cl, 3`
                        ->  INV_GetVendorPageInventory     (__fastcall, ecx)
    6FAEA7F2  ecx = CUNIT_FindUnitByGuidAndType's leftover
              push ecx  ->  INV_OnGridLeftClick
    6FAE93D3  ecx never set, then `mov cl, gnVendorPage`
                        ->  INV_GetVendorPageInventory     INV_NpcShopBuildTabs
    6FB05FB7  the quest screen's copy -- see its own entry below

**The contamination is real and reachable.** `gInvNpcShopRect.nLeft` is a screen
X of a few hundred, so bits 8..15 are non-zero in every layout, not merely in
principle.

**And every consumer masks or overwrites.** Traced to the last reader, across a
module boundary:

    NPCMENU_GetInteractingNpc            OVERWRITES the slot -- four
                                         instructions, `mov [ecx], eax`
                                         unconditional at 0x6FAFBB55
    INV_GetVendorPageInventory           `and ebp, 0FFh`   0x6FAE9079
    INV_PixelToGridCell                  `and ebx, 0FFh`   0x6FAE884B, plus
                                         byte reads at 0x6FAE8885, 0x6FAE88E8
    INV_OnGridLeftClick                  `cmp bl, 0FFh`    0x6FAE75CA -- a byte
                                         test, and nothing else
    D2COMMON_UNITS_GetInventoryRecordId  `and ecx, 0FFh`
    D2COMMON_INVENTORY_GetItemFromInventoryPage   `and edx, 0FFh`

Both D2Client functions forward the whole dword, and both forward it only to the
two D2Common functions, which mask.

**Nothing reads above bit 7.** The defect runs and cannot be observed.

**This entry corrected an earlier reading of its own.** The rewrite recorded on
2026-09-10 that `INV_GetVendorPageInventory` "DOES NOT MASK". It does; the
earlier reading stopped at `mov ebp, ecx` and at the clamp that rewrites only the
low byte of a stack slot, and never reached `0x6FAE9079`. Binary Ninja prints the
mask outright as `(&var_14)[ebp & 0xff]`, and it was there to read the first
time.

Reproduced at all five sites, because the store is what the bytes do and the
harmlessness is a property of the callees, not of the store.

## The quest screen's copy of the same store

`0x6FB05FB7`, `QuestLog.cpp`.

`mov cl, <page>` over a register holding something else, then the whole dword
forwarded. **Cannot fire at all**: an assert eleven lines above bounds the
counter at six, so the register's upper bytes are zero on every path that
reaches it. Recorded here because it is the same shape and because finding it
first is what made the shop's instances legible.

## `INV_AppendChanceToBlockText` hands `win2Unicode` the whole buffer

`0x6FAE4FC6`, `inv.cpp`. `push 20h` -- a count of 32 for a 32-element
scratch, where every other conversion in the unit passes the array minus one.

**The minus one is not style.** D2Lang's `Unicode::win2Unicode`
(0x6FC11BD0) copies at most `count` characters, and when the source is EXACTLY
`count` long the loop exits with `i == count`, finds `src[i]` is the NUL, and
stores the terminator at `dest[count]` -- one past a `count`-element array.
Every caller that passes `N - 1` is guarding against that.

**INERT**: the source is `"%d%%\n"`, at most thirteen characters for any
`int`, so it is never 32 long and the store past the end never happens.

## `INV_AppendAttackSpeedText` appends its speed half to whatever is there

`inv.cpp`. Only the weapon-class half `strcpy`s; the speed half `strcat`s. A
weapon whose type is in none of `gWeaponClassDescs`' fifteen rows skips the
class half and appends the speed label to the caller's buffer as it stands.

**INERT**: both call sites in `INV_DrawItemDescription` hand it a buffer the
caller constructed and has not written -- `var_7C00` through
`CRT_VectorConstructorIterator` at 0x6FAE2245, `var_2800` by the entry loop
at 0x6FAE1A94 -- so the append lands on an empty string and prints what a
`strcpy` would.

## The damage lines' two-line shape does not reset its adjusted flag

`INV_AppendElementalDamageText`, 0x6FAE43D0, `inv.cpp`. A barbarian's
1-or-2-handed sword gets both damage ranges, and the one `bAdjusted` both
share is cleared once, before the two-handed range (0x6FAE4684) -- not again
before the one-handed one (0x6FAE47E1). So a two-handed range with a bonus
would colour the one-handed minimum too, bonus or not.

**INERT**: no stock property raises the two-handed range without the
one-handed one. Measured over `Properties.txt`: `dmg-min`, `dmg-max` and
`dmg%` go through D2Common's property functions 5-7, and 5 writes
STAT_MINDAMAGE and STAT_SECONDARY_MINDAMAGE alike for a weapon that has both
ranges (D2MOO's ItemMods.cpp); the percent stats' ItemStatCost `op stat`
columns name both ranges; and the one property that touches only one range,
`dmg-norm`, touches the ONE-handed stats -- the second range measured, where
the flag is set anyway. D2MOO and BFE's port carry the same shape.

## The mercenary slot centres an item vertically by the box WIDTH

`INV_MercInvDrawPanel`, 0x6FAED983, `inv.cpp`. **Found 2026-09-12.**

Each equipped item is centred in its slot as `top + (slot height - item height
* box) / 2`, and the box is `gInvMercGrid.nGridBoxWidth` in both axes: loaded
into ecx once at 0x6FAED948 and multiplied again at 0x6FAED983. The player's
panel uses `nGridBoxHeight` for the same term (`mov dl, ah` at 0x6FAE03DB), so
it is a slip rather than a convention. Binary Ninja prints the one variable in
both axes too.

**INERT**: inventory.txt gives both Hireling records, `Hireling` and
`Hireling2` -- the two resolutions -- a box of 29 by 29, so the two terms are
equal. A grid that is not square would put the items off-centre vertically.

## The mercenary captions test their alignment on the border sum

`INV_MercInvDrawPanel`, 0x6FAED8A7, `inv.cpp`. **Found 2026-09-12.**

A caption with a right edge is centred, one without is left-aligned -- and the
test is `test edi, edi` on `gnBorderXDifference + nRight`, not on nRight. At
800x600 the border is 80, so the eight one-line captions whose nRight is 0
(tbl 4057 to 4062, 4064 and 4068, measured over the table and string.txt)
take the CENTRED path with a right edge of 80 and a left edge of 95 or more.

**INERT**: `CHAR_DrawTextCentered` (0x6FACFCD0) shifts text only when it is
narrower than `nRight - nLeft + 1`, which is negative here, so it draws at
nLeft in colour 0 -- the same `D2Win_10117_DrawText` call the left-aligned
path makes. At 640x480 the border is 0 and the left-aligned path is taken
itself. The database's comment on `gMercPanelLabelRight` had recorded the test
"as read, not as observed in game"; this is the rest of the reading.

## The trade's cancel flag is written seven times and read nowhere

`gbTradeCancelSent`, 0x6FBB9FA8, `trade.cpp`. **Found 2026-09-12.**

Every place the client enters trade state 7 through a cancel sets it to 1 --
TRADE_SendCancelOnce (0x6FB1F6B9) and five writes in TRADE_HandleStateMessage
(0x6FB1F852, 0x6FB1FA0A, 0x6FB1FA5E, 0x6FB1FB7E and 0x6FB1FC8B -- six arms,
since the one entered at 0x6FB1F9ED from the jump at 0x6FB1F75B reaches the
same store at 0x6FB1FA0A) -- and TRADE_ResetState clears it (0x6FB1F377). Each
write sits immediately after `MSG_SendCmd3Words(79, 2, 0, 0)` and the
`GetTickCount` stamp into gnTradeDeadline, so the flag means "a cancel went
out and we are waiting".

**INERT**: over the whole module there are SEVEN writes
and NO read, and no data reference takes its address, so nothing can observe
what it says. The name is after the writes because that is the whole of the
evidence; it was `gSomeBYTE_6FBB9FA8` until today. Reproduced -- the writes
are in `trade.cpp` where the bytes have them.

## `NPCMENU_RefreshHireIconsForOffer` searches the offers and throws the answer away

`0x6FAFBE30`. **Found 2026-09-12**, when the function was recovered from a
tail chunk of `QUESTS_DispatchQuestPacket` (0x6FAB8E00, named
NPCMENU_OnQuestStateMessage until 2026-09-23), which jumps to it at
0x6FAB8E31. Twenty-five bytes, and this is all of them:

    6FAFBE30  mov eax, offset gNpcHirelingOffers
    6FAFBE35  cmp [eax], cx                    wMercName == the argument?
    6FAFBE38  jz  short loc_6FAFBE44           found -- leave the loop
    6FAFBE3A  add eax, 10h
    6FAFBE3D  cmp eax, offset gNpcUnitGUID     the array ends where that begins
    6FAFBE42  jl  short loc_6FAFBE35
    6FAFBE44  jmp HIREABLES_HireIconsResetAndShow

The loop walks the ten sixteen-byte offers for the mercenary name it was
handed, and **the cursor it leaves in `eax` is never read by anything**. The
exit is a tail `jmp` to `HIREABLES_HireIconsResetAndShow`, whose prototype is
`void __cdecl()` -- no argument, in any register, on the stack or not. So the
one parameter of this function reaches the comparison and nothing else, and
the icons are reset and shown identically whether the name is in the table or
not.

**INERT**: at most ten 16-bit compares, no store, and no step out of bounds --
`gNpcHirelingOffers` is 0x6FBB5C55..0x6FBB5CF5 and `gNpcUnitGUID` begins
exactly at its end, so the bound stops `eax` after the last element. The
search is the residue of a call that used to take what it found.

NOT reproduced in C yet: the function is outside every span written so far.
This entry is the record for the batch that writes it. **Reproduced since
2026-09-12**, npcmenu.cpp batch 1 -- the loop is written as the bytes have it
(note of 2026-09-13; the line above was not updated when it was).

## `NPCMENU_OnInterruptNewsClicked` hands the speech id over in DX alone

`0x6FAF9856`, `npcmenu.cpp`. **Found 2026-09-13**, batch 4.

    6FAF9856  mov  dx, word ptr gwNpcSpeechTblId
    6FAF985D  push ebx
    6FAF985E  mov  ecx, eax
    6FAF9860  call TEXT_ShowSpeechPopup

The low half of edx is the id; the high half is whatever
`CUNIT_FindUnitByGuidAndType` (or the call before it) left there -- Binary
Ninja prints `edx_1.w = data_6fbb5c47`. Five more of TEXT_ShowSpeechPopup's
eleven callers load DX the same way.

**INERT**, traced to the last reader: TEXT_ShowSpeechPopup stores `bx` into
sgTextState.nPopupStringId and hands ecx on to D2Lang #10004, which compares
`si` only and pushes esi to strtable_12E60, which does `and ecx, 0FFFFh`
before its first index; the saved dword goes to
OBJECT_FindSpeechSoundByTblIndex, which compares `cx`. Nothing reads the high
half. The database's `int nTblStringId` became `__int16` the same day, and
`npcmenu.cpp` passes the word.

## The hire list bounds a row's offer number, then hires by the row's index

`NPCMENU_OnHireListConfirmClicked` (0x6FAF55F0) and
`NPCMENU_ConfirmHireMercenary` (0x6FAF56D0), `npcmenu.cpp`. **Found
2026-09-13**, batch 8.

`NPCMENU_DrawHireMercenaryList` adds one list-box row per used offer with the
offer's number as the row's user data -- `push ebp` at 0x6FAF5511, ebp the
loop index loaded at 0x6FAF54FF -- or one placeholder row with user data 10
(`push 0Ah`, 0x6FAF5564). The click handler bounds both numbers and keeps one:

    6FAF560A  call dword ptr [eax+2Ch]     the selected ROW
    6FAF5613  cmp  eax, 0Ah                the row, signed (jge at 6FAF5621)
    6FAF5616  mov  gnSelectedHirelingRow, eax
    6FAF5630  call dword ptr [edx+34h]     that row's user data, the OFFER
    6FAF5633  cmp  eax, 0Ah                the offer, unsigned (jnb), dropped
    6FAF56DB  shl  eax, 4                  ConfirmHireMercenary: the ROW again
    6FAF56E3  mov  cx, word ptr gNpcHirelingOffers[eax]

**INERT**: the two numbers are equal at every row. The used offers are always
a prefix of the ten. NPCMENU_AddHirelingOffer takes the first free slot, and
the five other writers of `bUsed` clear all ten --
NPCMENU_ClearHirelingOffers and its inlined copies in NPCMENU_NpcFreeVendorInventory
(0x6FAF78E0), NPCMENU_NpcResyncVendorInventory (0x6FAF797D), NPCMENU_OpenMainMenu
(0x6FAF9B7C) and NPCMENU_NpcEndInteraction (0x6FAFBBD0), each a loop from
`offers[0].bUsed` to `offset gNpcClassId`. The
whole item has ten references and these six are its `+0x8` cursors. The
placeholder is row 0 with user data 10, which the second bound turns away.

Reproduced as written: the C bounds the user data and indexes by the row.

## The packet router measures every packet against the WHOLE buffer, not the bytes left in it

`0x6FAB50B0`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 1.

`SCMD_RouteServerPackets` walks a buffer of concatenated packets and asks
`D2NET_SERVER_GetServerPacketSize` for each one's length. That call's second
argument is the buffer size, and it is the WHOLE buffer on every pass:

    6FAB50B7  the incoming nSize is loaded into edi
    6FAB50F0  mov edx, edi          <- every pass, the whole size
    6FAB518B  edi is overwritten with the unit found for this packet
    6FAB51ED  edi is RESTORED from [esp+14h] -- the whole size again

The running remainder lives at `[esp+1Ch]` and never reaches edx; D2Net's own
callers pass the remainder. The consequence would be that the minimum-length
guards inside `SERVER_GetServerPacketSize` -- `if (nBufferSize < 0xD) return 0`
and its siblings -- are satisfied by bytes belonging to the next packet or to
nothing at all.

**Why INERT:** `GAME_UpdateFrame` hands the router a 512-byte buffer and
D2Net's dequeue copies **one packet into it per call** (measured, SCmd.cpp
batch 1), so on the only pass the remainder and the whole buffer are the same
number. No consumer can observe the difference.

Reproduced as written, with a MARKED comment on the argument.

## The mercenary resurrection price crosses the wire as 32 bits and is read as 16

`0x6FAB43D0`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 2.

Packet 0x9B carries the cost as a dword at +3. The handler takes a word:

    6FAB43D0  xor edx, edx
              mov dx, [ecx+3]        <- the low half only
              call NPCMENU_SetResurrectMercOffer

and that edx is stored whole in `gnMercResurrectCost`, so a cost of 65536 or
more would be displayed and charged modulo 65536 on the client's side.

**Why INERT:** the server cannot produce such a cost. D2Common's
`MONSTERS_GetHirelingResurrectionCost` is

    nCost = 15 * nLevel * nLevel / 2;
    if (nCost > 50000) nCost = 50000;

(D2MOO `D2Common/src/Monsters/Monsters.cpp:401`), and D2Game's
`NPC_ResurrectMerc` is the only sender of a priced 0x9B. **50000 < 65536**, so
the high half is always zero and the truncation loses nothing at any level.

Reproduced as written, with a MARKED comment.

## Item message 0x05 re-places an item with its X passed as its Y

`0x6FB2C105`, `Item.cpp`, `ITEM_ItemMsg05_ToInventoryGrid`. Found at Item.cpp's
batch 5, verdict at its close, 2026-09-15.

In the arm that re-places an item the client already has -- neither
IFLAG_QUANTITY nor IFLAG_NEWITEM in the decoded flags -- one register is
pushed for both coordinates:

    6FB2C0EB  mov  ecx, dword ptr [esp+22Ch+pItemSave.nX]
    6FB2C0FC  and  ecx, 0FFFFh
    6FB2C105  push ecx          ; nY
    6FB2C106  push ecx          ; nX

so ITEM_SetClientItemMode receives the item's X as its Y. Message 0x04 does
the same job with two registers, so this is a slip in one of two
near-identical arms, not the family's idiom.

**Why INERT:** ITEM_SetClientItemMode (0x6FB2A8E0) reads the pair only in its
IMODE_ONGROUND and IMODE_DROPPING arms, and this arm never arrives with either
mode. D2MOO's D2Game sends action 5 without IFLAG_QUANTITY from two places:
the pick-up in `PLAYER/PlrMsg.cpp`, which sends and only then calls
`UNITS_ChangeAnimMode(pItem, IMODE_ONCURSOR)`, so the record carries the
container's IMODE_STORED; and the command-flag send in
`INVENTORY/InvMode.cpp`, whose flag `ITEMS/ItemMode.cpp` raises after its own
`UNITS_ChangeAnimMode(pItem, IMODE_ONCURSOR)`. Neither mode places anything,
and the statements after the call put the item on the cursor.

Reproduced in `Item.cpp`.

## Item message 0x07 clears the weapon class of the hand being filled

`0x6FB2C79F`, `Item.cpp`, `ITEM_ItemMsg07_SwapBodyLocItems`. Found at Item.cpp's
batch 6, verdict at its close, 2026-09-15.

When an item goes into one hand and pushes the other hand's item onto the
cursor, the cached weapon class is cleared for the PACKET's body location --
the hand being filled -- and not for the hand being emptied:

    6FB2C79F  mov  al, [esp+34h+pItemSave.nBodyloc]
    6FB2C7A3  cmp  al, 4
    6FB2C7A7  mov  gnWeaponClassRArm, 0
    6FB2C7B6  mov  gnWeaponClassLArm, 0      ; for 5

The inlined ITEM_CacheWeaponClassForBodyLoc at 0x6FB2CA44 then writes the same
slot with the arriving item's class, so the clear is always overwritten and
the emptied hand keeps the class of the item that left it.

**Why INERT:** nothing reads either slot. Each of
gnWeaponClassRArm (0x6FBBC208) and gnWeaponClassLArm (0x6FBBC20A) has ten
references, and all twenty are stores, and D2Client exports nothing but
QueryInterface, so no other module reaches them.

Reproduced in `Item.cpp`.

## The key files are read and written across two stack locals, three times

`0x6FAD49E0`, `0x6FAD4890` and `0x6FAD41E0`, `CmdTbl.cpp`. **Found
2026-09-16**, from a bug report against our own build.

Three places in the key-binding code carry a file across the boundary between
two locals and rely on the layout the original's compiler chose:

    CMDTBL_CommandLoadDefaultKeyBindings
        _DWORD  pReadBuffer[286]     [ebp-47Ch]   1144 bytes
        __int16 wZeroTerm            [ebp-4h]     immediately behind it

        Fog_10110(File, pReadBuffer, 1146, ...)   reads 1146 INTO 1144
        qmemcpy(File, pReadBuffer, 0x478u)        copies 1144 back out
        File[572] = wZeroTerm                     and takes the last two
                                                  from the local behind

    CMDTBL_SaveBindingsToFiles
        __int16 nBindingsHeader      [ebp-478h]
        _BYTE   abBindingsBody[1142] [ebp-476h]
        Fog_10111(hObject, &nBindingsHeader, 1142, ...)    2 + 1140, one write

        int     nDefaultKeyMagic     [ebp-584h]
        __int16 nDefaultKeyLength    [ebp-580h]
        Fog_10111(FileA, &nDefaultKeyMagic, 6, ...)        4 + 2, one write

The read overruns its array by two bytes; the two writes read past one object
into the next. All three are undefined in C++, and all three work in the
shipped DLL because the 1998 compiler placed each pair adjacent and in that
order — the read's spill lands exactly on `wZeroTerm`, which the very next
line puts back at offset 1144.

**INERT**: the bytes that reach the file, and the bytes that reach the
allocated copy, are the intended ones on every path. Nothing downstream can
tell, and the overrun never leaves the frame.

**NOT reproduced, and the difference is declared.** The rewrite never writes
UB to look faithful, so ours makes each file ONE object — `aReadBuffer[1146]`,
`abBindingsFile[1142]`, `abDefaultHeader[6]` — with every read and write
starting at its base. The bytes on disk and in memory are identical.

**Our first transcription was neither faithful nor defined**, which is how
this entry came to be written: it kept the 1144-byte buffer and copied 1146
out of it, and kept both writes from the header locals' addresses. MSVC 2022
lays those locals out its own way, so `/RTC1` reported *"Stack around the
variable 'aReadBuffer' was corrupted"* and the two files were written
malformed.

**The repair was verified in a RUNNING GAME** the same day: `/RTC1` is silent, every key works from the moment the game
starts, and the character's own bindings file is written --
`<install>\Save\<character>.key`. That is the only check that settles this
one; a green build never could.

## The list box's thumb cannot be dragged: the grab position is stored and never loaded

`0x6FAF0BAA`, `Listbox.cpp`, `LISTBOX_ImplOnMouseDown`. Found at the unit's
batch, verdict at its close, 2026-09-17.

The five-dword block at +0x18071C holds four press flags and the mouse Y the
scroll thumb was grabbed at. The whole stock module holds exactly ONE
reference to the fifth dword, and it is the store:

    6FAF0BAA  mov [esi+18072Ch], eax       ; the grab Y
    6FAF0BB0  mov dword [esi+180728h], 1   ; LBPRESS_TRACK, next instruction

Measured by scanning `D2Client.dll` for the disp32 `0x0018072C`: **one
occurrence in 1085505 bytes.** The other four slots of the same block carry
4, 6, 6 and 4 references (the constructor's zeroing, the draw's held-arrow
tests, the press gates and the release clears), so the block is live and only
its fifth slot is write-only.

**Re-measured 2026-09-17 over the decoded instructions of the whole `.text`**,
since a byte scan alone cannot see a slot reached by an index: the same 1, 4,
6, 6 and 4; no operand with an index register anywhere in the tail block
`0x180700`..`0x180776` (the detector's control: both `[reg+ecx*2+300Ah]`
operands of the unit, found); and no immediate in that range except the
allocation size `0x180776`, so no pointer into the block is built by
arithmetic either.

**Why INERT:** the press is reached -- pressing the thumb sets
`LBPRESS_TRACK` and records the Y -- and no instruction anywhere reads it
back, so nothing downstream can be wrong. What the player gets is the absence
of a feature, not a wrong value: **the thumb cannot be dragged in the shipped
game.** A press on the thumb does nothing on release, because
ScrollByTrackClick returns 0 for a click between the thumb's edges; the
arrows, the wheel and track clicks all work.

Reproduced in `Listbox.cpp`, where the slot is named `LBPRESS_GRAB_Y`
and marked DEAD.

## The descline dispatcher stores two text indices as words and reads them back whole

`SKILLDESC_BuildDescLine`, `SkillDesc.cpp`. **Found 2026-09-17**, at the
unit's re-check against the bytes; verdict the same day.

Two stack slots of the dispatcher are written sixteen bits wide and read
thirty-two bits wide, and nothing else writes their upper halves first:

    6FB10A5F  mov  [esp+10h], dx       ; desctextbN, the B index -- a WORD
    6FB10C84  mov  ecx, [esp+10h]      ; case 2: read whole into GetString
    6FB10B39  mov  [esp+14h], cx       ; case 1: the skill's wStrMana -- a WORD
    6FB10BCC  mov  edx, [esp+1Ch]      ; the same slot two pushes later, whole

The B index is read whole at every use that hands it on -- to
`D2LANG_GetStringFromTblIndex` and as the stack argument of the append
helpers -- and case 1's mana label goes whole into `AppendIntAndFraction`,
`AppendFixedPointValue` and `AppendTextThenValue` (0x6FB10BCC, 0x6FB10BEB,
0x6FB10C0F). The upper half is whatever the stack held before the call.

**Why INERT: every consumer uses the low word.** All 41 tests against 5382
in the unit's window are word compares -- 31 in the dispatcher (`cmp word
ptr [esp+10h], 1506h`, `cmp cx, 1506h`, `cmp di, 1506h`) and ten in the
helpers (`cmp bp, 1506h`, `cmp cx, 1506h`). And D2Lang's
`D2LANG_GetStringFromTblIndex` (#10004, 0x6FC13740) branches on `si` alone
(`cmp si, 4E20h` at 0x6FC13743, `cmp si, 2710h` at 0x6FC1378F) and hands
the index -- rebased with `lea eax, [esi-4E20h]` or `lea edx, [esi-2710h]`,
or as it is -- to a lookup that compares `cx` and then does `and ecx,
0FFFFh` (0x6FC12E76, 0x6FC12E99), so the upper half is gone before it can
select anything.

The C zero-extends both reads -- an uninitialised upper half is not a value
the C can reproduce -- and says so beside the two stores.

## Two construction loops build buffers nothing reads -- 1024 Unicodes per icon, 50 per frame

`SKILLSTREE_DrawSkillIcon` (`0x6FB166C0`) and `SKILLSTREE_DrawTabLabel`
(`0x6FB17800`), `skillstree.cpp`. Found in batches 3 and 6, measured
at the close, 2026-09-18.

Both functions construct a scratch buffer on entry -- DrawSkillIcon before
anything else, DrawTabLabel right after its two cursor reads (`0x6FB1780B`,
`0x6FB17812`) -- and neither buffer is ever used:

    DrawSkillIcon    sub esp, 87Ch
      6FB166D4  lea     esi, [esp+88Ch+var_800]
      6FB166DB  mov     ebx, 400h                    ; 1024
      6FB166E0  mov     ecx, esi
      6FB166E2  call    ??_FUnicode@@QAEXXZ          ; Unicode::Unicode()
      6FB166E8  add     esi, 2
      6FB166EB  dec     ebx
      6FB166EC  jnz     short loc_6FB166E0
    DrawTabLabel     sub esp, 78h
      6FB17819  lea     edi, [esp+88h+var_64]
      6FB1781D  mov     ebp, 32h                     ; 50
      6FB17822  mov     ecx, edi
      6FB17824  call    ??_FUnicode@@QAEXXZ
      6FB1782A  add     edi, 2
      6FB1782D  dec     ebp
      6FB1782E  jnz     short loc_6FB17822

Neither block is read afterwards. The only other constructor call in either
function is DrawSkillIcon's ten-Unicode level buffer (`0x6FB169B3`), and that
one is filled by `Unicode::toUnicode` (`0x6FB169E4`) and drawn under the
icon; the 0x800-byte and 100-byte blocks are dead the moment their loops end.

**INERT:** both loops run in ordinary play -- DrawSkillIcon once per icon it
draws, and DrawTabLabel once a frame while the panel is open, before the hit
test -- and no consumer can observe it, because the constructed objects are
discarded. What it costs is the constructor's own work, repeated per icon and
per frame. The C keeps both declarations, with the reason beside them.

The icons are counted by SKILL, not by cell: both walks call DrawSkillIcon
for every skill whose page is shown, whatever its cell, and the stock
`skills.txt` and `skilldesc.txt` give each of the seven classes ten skills on
each page, in ten distinct cells (210 class skills, read 2026-09-18) -- ten
calls and 10240 constructions a frame. Until the re-check of that day this
said "up to eighteen cells a frame".

## `SELECT_SetClickRegion` returns an unwritten eax when it clears

`0x6FAB6480..0x6FAB648A`, `Select.cpp`. **Argued 2026-09-23** at
Select.cpp's close. **INERT.**

The set path returns `pRect[2]`, the last load; the clear path stores
`gbClickRegionValid = 0` and returns with eax as its caller left it.
Over the 22 call sites, 11 discard eax, and of the
11 it leaves undecided, 6 are tail positions in `void` functions (the anvil's
USEITEM_AnvilCloseOnInteractionStop, USEITEM_AnvilOnButtonUp, USEITEM_AnvilOnCloseKey,
USEITEM_AnvilTickPanel and USEITEM_AnvilOnPanelPacket twice), 3 are overwritten before
their callers return (DIALOG_Close returns FOG_Free's answer, dialog.cpp;
TEXT_Shutdown and USEITEM_AnvilShutdown are `void`), and 2 are USEITEM_AnvilOpenPanel's
`return`s -- whose callers are USEITEM_AnvilOnPanelPacket (`void`) and
USEITEM_AnvilOpenPanelLocal, whose one caller discards it (the same tool, both
runs). No reader sees the value.

Reproduced with a declared difference: C cannot leave eax unwritten, so
ours answers 0 on the clear path, said beside the code.

## `SELECT_SkillsFindEnemyInFrontOfUnit` hands D2Common an untested room

`0x6FAB68A0`, `0x6FAB692E`, `0x6FAB6948`, `Select.cpp`. **Argued
2026-09-23** at Select.cpp's close. **INERT.**

The room is UNITS_GetRoom's answer for the probing unit (0x6FAB68A0), NULL
for a unit with no path, and each of the five probes hands it on untested to
COLLISION_CheckMaskWithSize and D2Common's #10407. Both refuse it in D2Common's
own bytes: CheckMaskWithSize, for size 1, tests the room and returns 0x27
(0x6FD430C4 -> 0x6FD431EF) -- non-zero, so the probe goes on -- and #10407
tests it first and returns 0 (0x6FDC33C7 -> 0x6FDC3626). Every probe finds
nothing; the function returns NULL and sets no target.

Reproduced -- the room is passed untested.

## `gbAskedForGDI` is raised and never read

`0x6FAA287A`, `Draw.cpp` (DRAW_SelectGdiDisplay). **Argued
2026-09-23** at Draw.cpp's close. **INERT.**

The GDI choice stores 1 into gbAskedForGDI (0x6FB9A968) beside
gDisplayType. The global has one reference in
the image: this store. It runs whenever the launch config's byte at +4 is
set (GAME_ApplyParamGDI, 0x6FAABD30) and nothing can tell. D2MOO marks it
"Unused ?".

Reproduced -- the store is kept.

---

# LATENT

Real defects the data or the callers prevent. What prevents each one is named.

## Both dialog constructors erase the list link they just wrote

`0x6FAD6BE0` and `0x6FAD6950`, `dialog.cpp`. **Found 2026-09-12.**

Each `memset`s the whole 0xB10 block, links the new dialog into `gpDialogList`,
and then memsets the SAME block again:

    6FAD6C3F  rep stosd                 2C4h dwords -- 2832 bytes
    6FAD6C41  mov  eax, gpDialogList
    6FAD6C4A  mov  [esi+0B0Ch], eax     pDialog->pNext = gpDialogList
    6FAD6C59  mov  gpDialogList, esi
    6FAD6C5F  rep stosd                 the same 2832 bytes, +0B0Ch included

`pNext` is at +0xB0C, inside the block, so the second memset clears it. A
dialog created by either constructor always has `pNext == nullptr`.
`DIALOG_CreateFromImage` does the same at 0x6FAD69D5 / 0x6FAD69E3 /
0x6FAD69F8.

**LATENT**: nothing can observe it, because nothing is ever second in the
list. Both constructors assert `gnDialogNestDepth == 1` on the way out and
`DIALOG_Close` asserts it reaches 0, so a second dialog halts the game before
it exists. The two consumers of the chain are harmless in consequence:
`DIALOG_Shutdown`'s `while (pNext)` frees exactly one record, and
`DIALOG_Close` restores a head that reads null -- which is what it was.

The defect is real and the guard against it is an assert rather than the
code. Reproduced, both memsets, in the order the bytes have them.

## The image dialog bounds its image index from above only

`0x6FAD6950`, `DIALOG_CreateFromImage`, `dialog.cpp`. **Found 2026-10-11**
(d2-reader R4, the globals plan's batch P3).

The constructor refuses an image index before it allocates --

    6FAD698F  cmp  dword ptr [esp+16Ch], 1     nImageIndex
    6FAD6997  jl   short 6FAD69A8h             go on below 1: SIGNED

-- and indexes `gDialogImageNames` (0x6FB78EC0, ONE entry,
`dialogbackground`) with it at 0x6FAD6A5B, `mov edx, [ecx*4+6FB78EC0h]`. A
negative index passes the `jl` and reads a dword below the table as the
string's pointer.

**LATENT**: the one caller, `UI_OpenGoldEntryDialog` (the call at
0x6FB23C59), pushes 0 for it (`push 0`, 0x6FB23C46); it has one direct
call, no immediate of the address and no data slot.
Reproduced: our `if (nImageIndex >= 1)` is the same signed test, and our one
caller (ui.cpp) passes 0. In ours the table is dialog.h's one-element array,
so a negative index would be undefined behaviour where the original reads its
own `.data`; no path makes one.

## `WINMAIN_AppModeEnqueue` / `WINMAIN_AppModeDequeue` — a five-slot queue whose dequeue shifts three

`WINMAIN.cpp`. **Unreachable in practice, proved by counting both functions'
callers**: the queue never gets deeper than one.

## `WALL_AllocDrawNode` counts attempts, not allocations

`Wall2.cpp`. The draw-node cursor is incremented before its bound is tested, so
`nDrawNodesUsed` counts ATTEMPTS. Past 3000 the writes stop and the count does
not.

**Corrected 2026-09-08 from REACHABLE to LATENT.** Nothing in D2Client reads the
counter to bound a walk: two module-wide searches put its ten sites in six
functions — three that zero it and seven read-modify-write allocator sites. The
consumers walk the LINKED LISTS, and a node is linked only where the bound
passed.

## `UIPANELS_DrawTextCenteredInPanelRect` copies into a 128-Unicode local with no length check

`0x6FB22280`, `ui_panels.cpp`. **All three callers pass a fixed panel-label
buffer.** The frame proves the size twice: `sub esp, 110h` is one RECT plus 128
Unicodes, and a `static_assert` in the rewrite says so.

## `UIPANELS_DrawInGamePanels` uses a four-byte stack slot as a string buffer

`0x6FB21E39`, `ui_panels.cpp`. `STRTABLE_GetLanguageString` writes the language
code into `&pXAdjust`, an `int32_t` local, and the next two slots are `pYAdjust`
and `pState` — and `pState` is still live, it is the view pointer forwarded to
`TEXT_TickStaticInfoPanel` at the very end.

**Margin exactly zero.** Every D2 language code is three characters — ENG, ESP,
DEU, FRA, POR, ITA, JPN, KOR, SIN, CHI, POL, RUS — so three plus a NUL fills the
slot exactly and writes nothing past it. A four-character code would corrupt the
view pointer.

## The corpse paren copy

`0x6FB20E5E` and `0x6FB20EA3`, `ui_panels.cpp`. `gszAffixOpenParen` /
`gszAffixCloseParen` are copied UNTIL NUL into a four-byte stack slot whose next
two slots are LIVE. Both globals are exactly two bytes (`28 00`, `29 00`) and
`ida_global_users` reports **no writer anywhere in the module** for either. Same
shape and margin as the entry above.

## The cursor's idle timeout is not wrap-safe

`0x6FB578FA`, `Cursor.cpp`. `add ecx, 1388h` then `cmp ecx, eax` then **`jnb`**
— the unsigned branch, so this is the original's comparison and not a cast added
to silence a warning. The wrap-safe spelling is `now - stored > 5000`;
`stored + 5000 < now` is not.

**Latent:** within 5000 ms of `GetTickCount`'s 49.7-day rollover the sum wraps
to a small number and the test reads TRUE with no time elapsed. The cursor drops
to the opening hand for that one window every 49.7 days of uptime, and nothing
else depends on it.

## `ARCHIVE_GetFileSize`'s high dword is never read

`Archive.cpp`. The sole caller `ARCHIVE_LoadFile` passes `&dwSizeHigh` on its own
frame and then uses only the return. **A file at or above 4 GB would load short
and silently.** Not reachable with an MPQ member; recorded because the shape is
in the code rather than in the data.

## `INV_StashLoadImages` guards on the first handle and loads two

`0x6FAEC5C0`, `inv.cpp`. The whole body is behind `if (!gpStashPanelImage)`, and
inside it a SECOND image is loaded behind `FOG_IsExpansion()`. **An installation
that had `bank` but not `TradeStash` would never retry the second.** The
expansion MPQ carries both. The test is `FOG_IsExpansion` and not
`GAME_IsExpansionGame` because the question is about the installation, not
about the game being joined — and `NEWSTATS_RecipeScrollDraw` a few hundred bytes away
makes the opposite choice for the same kind of question.

## `USEITEM_AnvilOnPanelPacket` case 5 skips the null check its siblings have

`useitem.cpp`. Every other arm of the family null-checks the player before
reaching into its inventory; case 5 does not. **Reachable only if the player
unit goes away between the request and the answer** — which is exactly what
`USEITEM_AnvilTickPanel`'s `CUNIT_IsPlayerDeadOrMissing` arm exists to handle, one
frame later.

## The three SOUND accessors fault on their own bounds-check arm

`0x6FB53C30`, `0x6FB53C60`, `0x6FB53C90`. The bounds-check arm zeroes eax and
then reads the field off it, faulting at `0x60` / `0x3D` / `0x3E`. The null-table
arm is correct in all three, and `SOUNDHDR_GetRecord` next door gets both right —
**so it is a slip and not a convention**.

Hex-Rays prints it as `MEMORY[0x60]`, and that rendering reads like an artefact.
It is not. Reproduced: these three fault in the rewrite too.

**Corrected 2026-10-06.** Until this date the last
sentence above was not true: no C of the three existed. And the "slip"
misreads the shape. Past their own `if (gpSoundHeaders)` the three ARE
`SOUNDHDR_GetRecord` inlined -- compared against its body
0x6FB53C09..0x6FB53C23 matches 8 + 1, 8 + 1 and 7 + 1 + 1 instructions, the
hunks being GetRecord's two `retn` and, in IsLooping, the field read scheduled
into the copy -- with the copy's table test dropped (the site made it) and its
answer read without a test. GetRecord answers NULL out of range, as it
should; the callers' `->nGroupBaseIndex`, `->nGroupSize` and `->bLoop` on that
NULL are the defect -- the shape `UIPANELS_DrawUnitNameLabel` has below, an
inlined lookup whose NULL is not honoured. **Reproduced now**: SoundHdr.cpp
writes each as `if (!gpSoundHeaders) return 0; return
SOUNDHDR_GetRecord(nSoundId)->field;`, the copy routed. And the compiler keeps
the read: a standalone copy of the three, `cl /O2 /Oi /Gy /sdl /permissive-
/std:c++20 /c /FA` with MSVC 19.51 for x86, emits

    $LN5@GetGroupBa:
        xor     eax, eax
        mov     eax, DWORD PTR [eax+96]

and `mov al, BYTE PTR [eax+61]` / `movzx eax, BYTE PTR [eax+62]` after the
same `xor` for the other two -- the original's three arms. The project's own
build is checked at the build that follows the four sound units' close.
**Reachability not yet argued**: at that close.

**Argued 2026-10-06 at that close: LATENT** -- the three test their table
first, and an id past it is one no stock table or constant makes (the entry
"The sound units read a sounds.txt row off ids they bound below only").

## `UIPANELS_DrawUnitNameLabel` inlines a lookup twice and honours the NULL once

`0x6FB20D73`, `ui_panels.cpp`. `DATATBLS_GetMonStatsTxtRecord` is inlined twice.
At `0x6FB21151` the 417/418 arm tests `cmp eax, ebx` / `jz` and bails; at
`0x6FB20D15` the general arm runs the same bounds test, does `xor edi, edi` at
`0x6FB20D4A` and then `mov al, [edi+0Dh]` at `0x6FB20D73` with nothing in
between — **faulting at address 0x0D**. All three decompilers print it.

**Reachability deliberately left open** and recorded under NOT ESTABLISHED below.

## The `Tile Cache Size: %dMB` trace prints a literal from outside its own `if`

`WINMAIN.cpp`. Reproduced.

## `MINIPANEL_LoadResolutionFromRegistry` reads an uninitialised registry value slot

Recorded in the rewrite's own `Reproduced so far` table since before this file
existed. Reproduced.

**Argued 2026-09-29** at minipanel.cpp's close. **REACHABLE; its outcome NOT
ESTABLISHED.** `push ecx` (0x6FB09C90) allocates the slot and gives it its
value, the caller's ecx. Storm's SRegLoadValue (#423, 0x6FFD5AC0, read in
the Storm database) writes the out-parameter only when its registry read
succeeds with type 1, through strtoul (0x6FFD5B80), or type 4
(0x6FFD5B5D); a missing "Resolution" value, or one of another type, leaves
the slot as it was, and the test for 1 -- `mov eax, [esp+4+pValue]` /
`dec eax` / `jnz`, 0x6FB09CA9..0x6FB09CAE -- then decides 800x600 on that
ecx. At both call sites it is a callee's leftover: GAME_InitGame's
call at 0x6FAA950B comes after VIEW_Alloc, or after the Color.txt archive
calls when the view exists, and SCMD_HandleSrv61 tail-jumps at 0x6FAB3F25
straight after QUESTS_PlayCinematicFromPacket, itself a tail jump to
VIDEO_PlayCinematic. So with no "Resolution" value the game starts at
800x600 only if that leftover is exactly 1, and nothing read here says
whether it can be. D2MOO initialises the slot to FALSE. The value is there
once the escape menu's video page has saved it (Restart.cpp,
`STORM_SRegSaveValue(..., "Resolution", ...)`). **What would settle it:**
the ecx each of those callees leaves, on every path to the two calls.

**Where Storm reads and writes the value** -- read in Storm's
sub_6FFD58B0, 2026-09-29. SRegLoadValue hands
sub_6FFD58B0 its OWN 256-byte buffer, type and size (0x6FFD5B16..
0x6FFD5B39), never the caller's slot, so no Win32 call can write the slot.
sub_6FFD58B0 builds the key from SRegGetBaseKey --
"Software\Blizzard Entertainment\" under flags 0 (0x6FFD5822;
"Software\Battle.net\" under flag 2) -- and "Diablo II", then asks
HKEY_CURRENT_USER (RegOpenKeyExA 0x6FFD592D,
RegQueryValueExA 0x6FFD5957) and, failing that, HKEY_LOCAL_MACHINE
(0x6FFD5989, 0x6FFD59B3). Both failing, it sets the last error and answers
0 (0x6FFD59E3), and SRegLoadValue returns at 0x6FFD5B42 with the slot
untouched. SRegSaveValue writes through sub_6FFD5C10: RegCreateKeyExA under
HKEY_CURRENT_USER -- 0x80000001, plus one under flag 4 (0x6FFD5C5C..
0x6FFD5C7F) -- and RegSetValueExA, type 4, four bytes (0x6FFD5CAA). So once
the escape menu's video page has saved "Resolution", every later load finds
a REG_DWORD. And the uninitialised read is narrower than "no value":
D2MOO's reconstruction of Game.exe saves "Resolution" = 0 on every start of
a classic game (`if (!pCfg->bIsExpansion)`, Game/src/Main.cpp:459), and the
stock Game.exe carries that literal beside "Diablo II" (file offset
0xFD40) -- its code is in packed sections (.uako, .mnkam) with no plain
reference to either string, and was not read. So the slot is read
uninitialised only in an expansion game on an installation where neither
hive holds the value -- before the video page first saves it -- or holds it
with a type other than 1 or 4. Whether an installer writes it was not read.

The rewrite reads its own uninitialised local there, not the caller's ecx
-- the same defect, a slot compared that nothing in D2Client wrote, with
our frame's leftover in place of the original's ecx. **Kept,
2026-09-29**: the rewrite reproduces the original's defects. The original does the
same -- `push ecx` (0x6FB09C90) allocates the slot and only SRegLoadValue's
success writes it -- so the C keeps `unsigned int nValue;` uninitialised,
against the rewrite's boundary on undefined behaviour, by that decision. The
fix, when it is wanted, is one initialiser: D2MOO's FALSE.

## `CONFIG_OnUpKey`'s row walk is unbounded

`config.cpp`. The third instance of this shape in the rewrite, after the ESC
menu's two arrow keys:

    do   CONFIG_MoveSelection(-1);
    while (gpCfgControlsList[gnCfgSelectedRow].eCmd == HOTKEY_MAX_COUNT);

**A list whose every row is a separator — `eCmd == HOTKEY_MAX_COUNT`, 57 —
spins forever**, and nothing counts the rows already passed. Latent: the
controls list is built from a static table with real bindings in it.

## `INV_DrawItemDescription` hands the spell description the quantity line's buffer

`0x6FAE37D4` / `0x6FAE388F`, `inv.cpp`. On the path the player reads
"Quantity: N" on arrows from -- an identified, unsocketed item -- the caller
writes the quantity line into `var_9400` and then passes **the same buffer**
to `INV_AppendSpellDescText` as its output:

    6FAE37D4  strcpy(var_9400, tbl 3462 "Quantity:")  ... number, newline
    6FAE3888  lea  edx, var_9400
    6FAE388F  call INV_AppendSpellDescText

and that function `strcpy`s in its cases 1-3 and `strcat`s in case 4. So a
stackable item with a spell description loses its quantity line to cases 1-3,
or has the description run on after it in case 4.

**BFE's port of the caller transcribes the same collision independently** --
`ItemDesc::AppendClassBonusText(pItem, s.wszQuantity, pItemsTxt)` right after
the quantity line goes into `s.wszQuantity`
(`D2Client_ItemDesc_Internal.h`, 1182-1186).

**LATENT**: no stock item has both. No row of `Misc.txt` carries a `maxstack`
and a `spelldesc`, and `Weapons.txt` and `Armor.txt` have no `spelldesc`
column at all -- measured over the three tables. **A mod that gives a stackable
item a spell description gets the collision**, which is worth knowing in the
repository this was split out of.

## The item name's length check guards two arms, and one caller passes twice its buffer

`INV_BuildItemNameText`, 0x6FADD360, `inv.cpp`, and its caller
`UIPANELS_BuildItemLabelText`, 0x6FB20740, `ui_panels.cpp`. Four facts, each
measured:

* **Only the magic and rare arms check.** Both test
  `nTotalLen + strlen(base) + 3 >= nMaxLen`; the set, unique, runeword and
  every other path write into the caller's buffer with no length test at all.
* **The check does not count the owner's name.** Personalisation places it
  AFTER the test -- inside the rare arm, and in the shared tail for the magic
  one.
* **Failing it ends the process**: `FOG_DisplayAssert("Len < MaxLen", ...,
  2243 / 2080)` and `exit(-1)`, from inside a tooltip. BFE's port returns
  instead, and says so.
* **One of the eight call sites passes a capacity twice its buffer.**
  `UIPANELS_BuildItemLabelText` pushes `100h` at 0x6FB20978 for a buffer it
  constructs as element 0 plus 7Fh -- 128 elements -- right beside the call.
  The other seven fit, each capacity against its buffer's construction
  count: 128 into 128 at 0x6FB00ACE, 191 into 192 at 0x6FAFB81D, and at the
  five in `INV_DrawItemDescription` 128 and 256 into 1024, 256 into 256, and
  128 into 128 twice. **And the short buffer is the frame's highest local**
  -- `sub esp, 228h` comes before the register pushes -- so a name of 128
  characters writes its terminator over `UIPANELS_BuildItemLabelText`'s return
  address.

**LATENT**: over the stock English tables no arm builds a name of 128. The
widest bound, taken arm by arm from the longest string each piece can resolve
to, is the magic arm's **117**: 14 + 22 + 56 + the format's two spaces, plus
the owner and possessive -- at most 15 + 5, from `szPlayerName[16]` and
`Unicode::Personalize` (D2Lang 0x6FC11D50) writing only when
`owner + name + 5 <= cap` -- plus the quest colour's 3. The 56 is the Potion
of Life's two-line text, so no item comes near the bound. The test itself, at
95 for magic and 77 for rare against the smallest capacity passed, 128, never
fires. **A mod with longer names reaches it**, and at the label site reaches
the return address before any assert.

## `INV_DrawItemDescription` tells the socket-filler builder 512 for a 256-element buffer

`0x6FAE3070`, `inv.cpp`. `push 200h` for `var_7E00`, which the entry constructs
as 256 elements (the loop at 0x6FAE190A).

* **The capacity bounds nothing anyway.** MODS_BuildSocketFillerText renders
  each of its four blocks through MODS_AppendMagicModsText -- a hard-coded
  1024-element scratch, the caller's capacity accepted and never read -- or
  through MODS_AppendPropertySetText, which calls it. Only the data limits
  the text.
* **In the original an overrun lands nowhere live.** Up to 1024 elements from
  the buffer's start, what lies above it is `var_7C00`, `var_7A00` and
  `var_7800` -- the set arm's buffers, dead on this path. In the rewrite the
  neighbours are the compiler's.

**LATENT**: the longest stock socket-filler text, rendered block by block from
`Gems.txt`, `Properties.txt`, `ItemStatCost.txt` and the 1.10 English tables
-- description functions 1 to 4, one line per resistance group and per
element's damage -- is the Ist rune's, **241 characters** with tbl 11080 and
the colour code the caller adds: under 256 by 15. None of the 68 gems and
runes goes over 255. **A mod with longer property strings reaches it**, and
this rewrite, unlike the original, would overwrite a live buffer.

## Three NPC entries have no speech lines, and 'gossip' divides by their count

`0x6FAF4495` in `NPCMENU_ShowDescriptorSpeech` (and `0x6FAF4C0A` in
`NPCMENU_ShowSpeechLine`), `npcmenu.cpp`. **Found 2026-09-13**, batch 7a;
settled in batch 7b.

    6FAF4494  cdq
    6FAF4495  idiv dword ptr [esi+9]     ; gpNpcSpeechDesc->nSpeechLineCount

`gNpcMenuEntries` 6, 7 and 8 -- monstats `hcIdx` 331 `act2guard2`, 266 `navi`
and 408 `malachai` -- carry a line count of **0** (all 46 records read; the
other 43 hold 3 to 16). A 'gossip' click on one of them would raise #DE there.
On the way there, measured: `gNpcMenuDescriptors` 32, 33 and 34 give all three
a Talk row; `NPCMENU_OnTalkClicked` selects the entry by class id;
`NPCMENU_BuildTalkMenu` adds the 'gossip' row whenever the message list exists.

**LATENT**: what prevents it is `NPCMENU_IsSpeechOnlyNpc` (0x6FAF8970), which
answers true for all three. The talk menu opens from the main menu's Talk row,
or from `NPCMENU_OnSpeechDismissed`, which only the talk menu's own rows arm.
The main menu opens only through `NPCMENU_OpenMainMenu`, and both of its call
sites are behind that test: `NPCMENU_BeginInteraction` calls it at 0x6FAF88AE
before 0x6FAF8901, and `NPCMENU_ChooseNextSpeech` carries it inlined before
0x6FAF8DBF. Every other caller of `NPCMENU_BuildMainMenu` handles a dialog or a
transaction the main menu starts. Descriptor rows with a count behind them that
nothing can open. Reproduced as a `%` on the raw count.

## The character file's chunks are copied at the first chunk's length, unbounded

`0x6FAB1CB0`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 4.

`SCMD_SaveCharacterFile` accumulates the `.d2s` file out of packet 0xB2 chunks.
The buffer is allocated once, at the dword the FIRST chunk carries at +3, and
every later chunk is copied in at `gnScmdBufferSize` **whatever its own total
says**. The only test is the equality after the copy:

    *(_DWORD *)(pBuffer + 3) != pBuffer[1] + gnScmdBufferSize

so a chunk that pushes the running sum PAST the declared total writes beyond
the block, and a sum that steps over the total never equals it -- which also
means the buffer is kept until a restart flag arrives.

**What prevents it:** every chunk of one file carries the same total at +3, so
the sum reaches it exactly. The total is D2Game's, written once per save.

Reproduced as written.

## The save path is built in 500 bytes out of a 1024-byte path

`0x6FAB1CB0`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 4.

The frame holds `szFilePath` at `[esp+10h]` and `szSavePath` at `[esp+204h]` --
**500 bytes apart** -- and the file name is built with an unbounded `sprintf`
of `"%s%s.d2s"` from `FOG_GetSavePath`'s answer and `gszPlrName`. A save path
longer than about 480 characters writes through the destination into the source
it is reading.

**What prevents it:** the save path is the install directory plus `Save\`, and
the name is a character name of at most 16 characters. `Msg.cpp`'s reader has
the same two buffers at the same distance.

Reproduced as written.

## Packets 0x53 and 0x7E use the local player without testing it

`0x6FAB3BC0` and `0x6FAB4280`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp
batches 8 and 9.

Both handlers call `CUNIT_GetPlayer()` and use the answer at once:

    0x53  6FAB3BD9  mov edx, [eax+1Ch]       -- straight after the call, and
                                                the null test is on the way OUT
    0x7E  hands the same answer to D2Common's UNITS_GetRoom, which asserts
          "ptUnit" and calls _exit on a null unit (D2Common
          0x6FDBE274..0x6FDBE291, `test eax, eax` through `call _exit`)

**What prevents them:** both packets belong to a loaded game. 0x53 initialises
the environment for an act and 0x7E acts on the player's own room, and the
server sends neither before the player's unit exists -- the unit arrives with
packet 0x09 during the act load, ahead of both.

Reproduced as written.

## `ITEM_GetUseSound` guards on the drop sound and returns the use sound

`0x6FB2E5BC`, `Item.cpp`, `ITEM_GetUseSound`. Found at Item.cpp's batch 1,
verdict at its close, 2026-09-15.

All three arms -- items.txt, then UniqueItems.txt for ITEMQUAL_UNIQUE and
SetItems.txt for ITEMQUAL_SET -- test `wDropSound` and read `wUseSound`, the
field two bytes further on:

    items.txt  6FB2E5BC  cmp word ptr [eax+124h], 0    6FB2E5C6  movsx edi, word ptr [eax+126h]
    unique     6FB2E60C  cmp word ptr [eax+84h], 0     6FB2E616  movsx edi, word ptr [eax+86h]
    set        6FB2E65C  cmp word ptr [eax+82h], 0     6FB2E666  movsx edi, word ptr [eax+84h]

The guard was not changed when the function was copied from
ITEM_GetDropSound, whose guard and value are one field. An item with a use
sound and no drop sound would be silent when used; a unique or set record with
a drop sound and no use sound would answer 0 instead of the base item's sound.

**What prevents it: the data.** Over the rows that carry an item code --
misc.txt 139, weapons.txt 306, armor.txt 202, UniqueItems.txt 387, SetItems.txt
127 -- **no row has usesound set and dropsound blank**. The reverse is four
misc.txt rows and no unique or set row, and in the items.txt arm it answers 0,
which a guard on usesound would answer too. No unique or set row is built on
any of the 34 useable codes, and the function's two callers,
PANEL_BeltUseItemInSlot and INV_UseItemAtGridCell, are the use paths. A mod that
gives an item a use sound without a drop sound trips it.

Reproduced in `Item.cpp`, with the three offset pairs pinned by
`static_assert` so a transcription cannot swap them.

## Item message 0x01 of packet 0x9C uses the local player without testing it

`0x6FB2AF65`, `Item.cpp`, `ITEM_ItemMsg01_ToCursor`. Found at Item.cpp's
batch 4, verdict at its close, 2026-09-15.

    6FB2AF60  call CUNIT_GetPlayer
    6FB2AF65  mov  edi, [eax+60h]          ; no test of eax
    6FB2AF68  mov  al, [esp+20h+pItemSave.nAnimMode]

The player's inventory is read before the message's mode is even tested, so a
message with any mode dereferences the player, and nothing guards it.

**What prevents it:** D2MOO's D2Game sends action 1 from one place,
`INVENTORY/InvMode.cpp`, and only when `pPlayer == pClientPlayer` -- the item's
owner is the receiving client's own player, whose unit that client has held
since it joined. The argument of packets 0x53 and 0x7E above.

Reproduced in `Item.cpp`.

## The belt-frame assert tests a constant instead of the bound

`0x6FB002B0`, `panel.cpp`. **Found 2026-09-16**, panel.cpp batch 6.

`PANEL_DrawBelt` takes the belt's row and asserts before storing it:

    nBeltType = UNITS_GetBeltType(pBeltUnit);
    if (nBeltType < 0)  ->  FOG_DisplayAssert(..., panel.cpp, 2788); exit(-1)
    gnBeltsTxtIndex = nBeltType;

The assert's own string, 29 characters at 0x6FB7CFC8, is
`"nFrame >= 0 && NUM_PANELBELTS"` — a condition whose second operand is the
COUNT rather than a comparison against it. A non-zero constant is always true,
so the compiler folded it away and only `cmp eax, edi / jl` at 0x6FB002F2
reached the image. **The upper bound the assert was written to enforce is not
tested.**

**And nothing downstream tests it either.** `UNITS_GetBeltType` returns
`pItemsTxtRecord->nBelt` straight out of items.txt, and
`DATATBLS_GetBeltsTxtRecord` indexes
`gpBeltsTxtTable[nIndex + BELT_IDX_COUNT_PER_RESOLUTION * bHigherRes]` with no
bound of its own — both read in D2MOO's D2Common (`Units.cpp` and
`BeltsTbls.cpp`), a separate reconstruction of the same bytes. The value goes
from the data file to the table index untested.

**LATENT: what prevents it is the data.** `gnBeltsTxtIndex` is set from the
`belt` column of items.txt and from nowhere else, and in stock data that
column indexes the belts.txt rows that exist. A row carrying a larger value
reads past the table — and the high-resolution stride is added on top of it.

Reproduced as the bytes have it: one test, not two.

## Two panel draws construct a 100-Unicode array they never read

`0x6FAFF0F4` and `0x6FAFF8CB`, `panel.cpp`. **Found 2026-09-16**, batches 3
and 6.

`PANEL_DrawOrbHoverText` runs the Unicode constructor over THREE arrays of 100
and writes two; the one at `[esp+0x1A4]` is never touched again after its loop
at 0x6FAFF0F4. `PANEL_Draw800BorderFrame` does the same once, at 0x6FAFF8CB:

    CRT_VectorConstructorIterator(pArray, 2, 100, Unicode's constructor)

**LATENT by construction**, the same verdict as the `win2Unicode` above and
for the same reason: it is 100 constructor calls that nothing can observe. The
hover text pays them while the cursor is over a globe, the border frame every
frame it draws. Listed here rather than under REFUTED because the work really
is dead; it is simply harmless.

Both reproduced: the calls are in the bytes.

## The character screen constructs a 50-Unicode array it never reads

`0x6FACFE6D`, `char.cpp`. **Found 2026-09-16**, char.cpp batch 2.
`CHAR_CharPanelDraw` runs the Unicode constructor fifty times over `[esp+35Ch]`
right after reading the cursor, and nothing in the function's 4657 bytes
addresses that slot again -- the database now types it `Unicode[50]
wszUnused`. **LATENT by construction**, the verdict of the entry above: fifty
calls a frame while the character screen is open, and nothing observes them.

Reproduced: `Unicode wszUnused[50];` at the same point in the body.

## A layer-0 weapon-block stat replaces the maximum instead of competing with it

`0x6FAD1010`, `char.cpp`. **Found 2026-09-16**, char.cpp batch 1.
`CHAR_CharPanelGetWeaponBlockPercent` keeps the largest applicable
`STAT_PASSIVE_WEAPONBLOCK`, except that an instance with layer 0 skips the
comparison:

    6FAD107B  test esi, esi         ; the layer, zero-extended from a word
    6FAD107D  jle  6FAD10A9         ; -> mov ebx, edi -- assigned, not compared
    6FAD10A5  cmp  edi, ebx         ; only the item-typed arm compares

so a layer-0 value copied after a larger item-typed one would lower the
answer. **The server has the same code**: D2MOO's `SUNITDMG_GetWeaponBlock`
(D2Game 0x6FCC2850) spells it `nItemType <= 0 || (...) && nValue > nMax`.

**Why LATENT -- the data never makes a layer-0 instance.** The stat's only
stock source is the skill Weapon Block (skills.txt row Id 263:
`passivestat1 = passive_weaponblock`, `passiveitype = h2h`), and a passive
skill's stats are set with the skill's item type as their layer whenever that
type is positive (D2MOO D2Common D2Skills.cpp:579-581). Properties.txt has no
property for the stat, so no item carries it. Every instance therefore has
layer `h2h`, and the unconditional arm never runs. A mod that grants the stat
without an item type would reach it.

Reproduced in `char.cpp`, marked beside the loop.

## The chance-to-hit divisors are never tested

`0x6FAD1269` (`CHAR_CharPanelChanceToHitPercent`) and `0x6FAD0493`
(`CHAR_CharPanelDraw`), `char.cpp`. **Found 2026-09-16**, char.cpp batches 1
and 2. Both finish with

    2 * nPercent * lvl / (clvl + mlvl)       ; idiv with no test before it

where clvl is `STATLIST_UnitGetStatValue(player, STAT_LEVEL, 0)` and mlvl the
reference monster's `nLevel[difficulty]`, read with `movsx`.

**Why LATENT -- the player's level.** The reference is `gnToHitRefMonster` /
`gnToBeHitRefMonster`, 19 at load and rewritten only by
`SELECT_ApplySelectedUnit` (0x6FAB5D28, 0x6FAB5D2D) with the class of a
hovered monster that has valid monstats and monstats2 rows, and by
`SELECT_ResetRefMonsters`; its level word is never
negative in stock data. clvl is a real player's level, at least 1:
`STATLIST_UnitGetStatValue` answers 0 only for a NULL unit (D2Common
0x6FDB7C35), and the only caller of both is the character screen, drawn from
`UIPANELS_DrawInGamePanels` while the local player exists. So the divisor is at
least 1.

Reproduced, the untested divides, in `char.cpp`.

## The character screen hands a NULL class record's name to D2Win

`0x6FAD08F6`, `char.cpp`. **Found 2026-09-16**, char.cpp batch 2.
`CHAR_CharPanelDraw` bounds the player's class against
`nCharStatsTxtRecordCount` and stores NULL when it fails (`xor ebx, ebx`),
then passes `ebx` -- the record, whose `wszClassName` is at +0 -- to
`D2Win_10121_GetTextWidth` and `D2Win_10117_DrawText` without a test.

**Why LATENT -- the class ids.** A player's `dwClassId` is one of the seven
classes, 0..6, and charstats.txt compiles exactly those seven rows (its
`Expansion` line is a marker, not a record). Only a unit whose class lies
outside the table -- a malformed server -- would reach the NULL.

Reproduced in `char.cpp`, marked.

## The defence row reads the unit's inventory without testing the unit

`0x6FAD0D01`, `char.cpp`. **Found 2026-09-16**, char.cpp batch 2. The
`STAT_ARMORCLASS` row of `CHAR_CharPanelDraw` does `mov eax, [ebp+60h]` with ebp
the panel's unit, where every other use of that unit in the function goes
through a D2Common export that tests it (`STATLIST_UnitGetStatValue`,
`UNITS_GetDefense`, the state-mask checks).

**Why LATENT -- the caller.** The unit is `CUNIT_GetPlayer()`, and the
only caller draws the character screen while the local player exists; the
class-name block above already requires `pUnit && dwUnitType == UNIT_PLAYER`
for its own arm, but the stat rows run for any unit.

Reproduced in `char.cpp`, marked.

## The block tooltip's weapon-class test can read the right skill's chance to hit

`0x6FAD0762`, `char.cpp`. **Found 2026-09-16**, char.cpp batch 2.
`COMPOSIT_GetWeaponClassId` writes its out parameter into `[esp+24h]`, a slot
`CHAR_CharPanelDraw` also uses for the right skill's chance to hit, and D2Common
returns WITHOUT writing it for a NULL unit and for a right-arm item that is
not an item unit (D2MOO D2Composit.cpp:420 and :242). The test `cmp slot, 0Dh`
would then compare a percentage with `WEAPONCLASS_HT2`.

**Why LATENT -- neither path is open here.** The call is made only when
`CHAR_CharPanelGetWeaponBlockPercent` returned a positive value, which needs a
non-NULL unit, and an inventory body slot holds item units. The value it
would read is defined, not garbage -- so this is reproduced exactly, by
seeding the C's out variable with that chance to hit.

## The list box's text trim writes before its buffer, and never stops, in a box under 50 wide

`0x6FAF012B`..`0x6FAF0152` in `LISTBOX_DrawColumnText`, `Listbox.cpp`. Found
at the unit's re-check, 2026-09-17.

A line wider than `nWidth - 50` is trimmed one character at a time, and the
store comes BEFORE the width test:

    6FAF012F  call esi                    ; Unicode::strlen
    6FAF0135  mov  [esp+eax*2+0Ah], bx    ; wszText[len - 1] = 0
    6FAF0142  call D2Win_10123            ; the width of what is left
    6FAF0150  cmp  eax, edx / jg          ; still wider than nWidth - 50?

Once the text is empty the store lands on `wszText[-1]`, `[esp+0Ah]`, the
high word of the ebx saved at `0x6FAF00A6`, and the loop cannot end:
D2Win_10123 measures an empty string as 0 -- D2MOO's body leaves its loop
before the first character -- and 0 stays above a negative limit. With a
limit of 0 or more the loop stops the moment the text is empty, before any
store at length 0. So it needs `nWidth < 50`.

**Why LATENT:** the two boxes the game builds are 490 wide (the hire list,
`push 1EAh` at `0x6FAF4FBE`) and the screen width minus 20 (the message log,
`add edx, -14h` at `0x6FB1F0A9`), and `LISTBOX_Construct` has no other
caller.

Reproduced in `Listbox.cpp` with the out-of-bounds store dropped --
undefined in C, and in the original it damages only a register the hung loop
never restores -- and the endless loop kept; marked.

## The list box's middle assert is inverted, so it catches the wrong indices

`0x6FAF1217`..`0x6FAF128F` in `LISTBOX_GetEntryUserData`, `Listbox.cpp`.
Found at the unit's re-check, 2026-09-17.

The fast path returns an entry's user data only for `0 <= index <
wNumEntries` (and `< 128`); every other index runs three asserts with the
game's own texts:

    6FAF121E  cmp dx, 80h / jb            "mNumEntries < LISTBOX_MAX_ENTRIES"
    6FAF1249  cmp edx, ecx / jl           "mNumEntries < Selection"
    6FAF126B  test ecx, ecx / jge         "Selection >= 0"
    6FAF128D  xor eax, eax / retn 4

The second asserts the index is PAST the count, the wrong way round for a
bounds check: it fires for `index == wNumEntries` and for every negative
index, and lets an index beyond `wNumEntries` fall through to `return 0`.
So the third can never fire -- the first or the second has already stopped
every negative index.

**Why LATENT:** the only caller is `NPCMENU_OnHireListConfirmClicked`
through proxy slot 13, and it passes the list's own selection, a real row
whenever that callback runs. OnMouseUp calls it right after HitTestEntry
stored a hit; OnChar only with a selection other than -1; and the sixteen
writes of `nSelection` in the module, all in this unit, keep it between -1
and `wNumEntries - 1` while the list is not empty. The hire list is empty
only if `MONSTERS_HirelingInit` fails on the first offer, and D2MOO's body
(D2Common `0x6FDA4C10`) fails only when `hireling.txt` has no record for the
offer's act and difficulty -- the stock table has records for all three
difficulties of acts 1, 2, 3 and 5 in both versions. On an empty hire list
the wheel's `nSelection = nTopEntry` would give 0, and Enter would end the
game on the second assert.

Reproduced in `Listbox.cpp`, all three asserts as found; marked.

## The attack line's no-newline arm draws the title buffer

`0x6FB15EFB` in `SKILLDESC_DrawSkillInfo`, `SkillDesc.cpp`. **Found
2026-09-17**, SkillDesc.cpp batch 6; verdict at its close.

The attack path formats its line with `Unicode::sprintf` and splits it with
`CHAR_TextCutNextLine`. When the split finds no newline the else arm draws
`wszText` -- the TITLE buffer -- instead of the sprintf output:

    6FB15EFB  lea edx, [esp+2ACh+wszText]   ; pushed as the draw's text
    6FB15F1D  lea edx, [edx+ecx-1E0h]       ; nY - 480

so the arm would draw the title a second time, where the attack line belongs.
The batch-6 log entry gives the address as 0x6FB15EEF; re-measured at the
close, that address is the `mov edi, gnBorderXDifference` that opens the arm,
and the `lea` quoted above is at 0x6FB15EFB.

**Why LATENT -- the table strings.** The two formats the arms use are tbl
4063 `"%s\nAttack Rating"` and tbl 4065 `"%s\nRating"` (`database/string.txt`,
lines 4064 and 4066), and both carry the newline `CHAR_TextCutNextLine` needs, so
with stock data the else arm never runs. A modded `string.tbl` whose formats
lack the newline would draw the title where the attack line belongs.

Reproduced as the original has it, in `SkillDesc.cpp`.

## The skill drawer's damage and attack bounds exceed the tables they index

`0x6FB15CBE`..`0x6FB15CDC` in `SKILLDESC_DrawSkillInfo`, `SkillDesc.cpp`.
**Found 2026-09-17**, SkillDesc.cpp batch 6; verdict at its close.

The drawer dispatches `gpSkillDescDamFuncs[pSkillDesc->wDescDam]` and
`gpSkillDescAttFuncs[pSkillDesc->wDescAtt]` behind two guards, and the guards
are sized for something four times the tables:

    6FB15CBE  mov ax, [ecx+12h]     ; wDescDam
    6FB15CC2  cmp ax, 90h           ; 144 -- the table is 36 entries
    6FB15CD8  mov ax, [ecx+14h]     ; wDescAtt
    6FB15CDC  cmp ax, 48h ; 'H'     ; 72  -- the table is 18 entries

The ratio is exactly four in both (144/36, 72/18), which reads like a guard
written in bytes over tables counted in dwords -- the extents themselves are
in the database comment on the two tables.

**Why LATENT -- the data, and ONLY the data.** Stock SkillDesc.txt, where the
two columns live, carries `descdam` 1..24 on 110 rows and `descatt` 1..5 on 49
(measured 2026-09-18 on the stock install). The null checks do not keep the
overrun theoretical, because the three tables are contiguous in `.data` and
what lies past them is not null (read from the stock DLL, 2026-09-18, the
SkillDesc.cpp re-check's part 11):

    wDescDam 36..53    gpSkillDescAttFuncs -- index 37..41 are the five
                       descatt handlers (0x6FB0A4A0, 0x6FB0A730, 0x6FB0AAB0,
                       0x6FB0ABC0, 0x6FB0A610), called with descdam's
                       arguments
    wDescDam 54..89    gSkillDescPanelLayout -- 36 non-zero coordinates
                       (162, 93, 258, ...), each called as an address
    wDescDam 90..143   the format strings and the unit's own source path
                       from 0x6FB8CE68, called as addresses

and `wDescAtt` 18..53 and 54..71 reach the same layout and strings. Both
bounds are unsigned (`jnb`, 0x6FB15CC6 and 0x6FB15CE0). So a modded
SkillDesc.txt with `descdam` past 36 or `descatt` past 18 does not meet a
null test that saves it: it calls a handler with the wrong arguments or
jumps into data. The sentence this replaces -- "every slot the index reaches
is null-checked ... only the NULL test stands between the index and a stale
pointer", and "Stock skills.txt" for the table the columns are in -- was
written at the close and overturned by the re-check.

Reproduced: both tests and both null checks are in the C unchanged.

**DECLARED DIFFERENCE, 2026-10-12.** Past their ends both tables answer NULL in our C
(SkillDesc.cpp's SKILLDESC_GetDamFuncOrNull and SKILLDESC_GetAttFuncOrNull),
where the bytes read the original's .data and call what they find there; each
caller's NULL test takes its no-function arm -- no damage or attack line,
GetAttackRatingForHand's 0. LATENT, as above.

## The cell geometry answers ZERO for a row or column outside its table

`SKILLTREE_CellBottom` / `SKILLTREE_CellLeft` in `skillstree.cpp`
(the first was `SKILLTREE_CellTop` until 2026-09-18) -- the two switches the
original inlines into both handlers and the icon drawer. Found at the unit's
close, 2026-09-18.

Rows 1..6 and columns 1..3 map to panel coordinates; every other value falls
into a `default` that answers zero:

    switch (nSkillRow)    { case 1: ... height - 418; ... case 6: ... - 77;  default: return 0; }
    switch (nSkillColumn) { case 1: ... - 305; ... case 3: ... - 167;        default: return 0; }

Zero is not a neutral value here. The row coordinate is the cell's BOTTOM --
the tests are `nCellBottom - 48 < y < nCellBottom` -- so a zero row puts the
cell's box at `y` in (-48, 0), entirely above the pixels the cursor can
occupy; a zero column puts it at `x` in (0, 48), against the screen's left
edge. The draws take the pair literally, and the icon goes where the box
goes: `SKILLSTREE_DrawSkillIcon` hands the same pair to
`D2GFX_TEXTURE_CelDrawColor(&pData, nCellLeft, nCellBottom, ...)` (`push ebx`
at `0x6FB16925`), and a cel's Y is its BOTTOM edge:
`GFX_DrawInventoryItem` (`0x6FB5A320`) draws an item whose top is its `nY`
by handing `TEXTURE_CelDraw` the sum `nY + D2CMP_CelGetHeight`
(`add eax, edi` at `0x6FB5A437`), and the tooltip builders take the icon's
register as their `nIconBottom` (`0x6FB16956`). So a zero row draws the icon
above the screen's top edge, out of sight and out of the cursor's reach --
only the level number, drawn at `nCellBottom + 12` (`add ebx, 0Ch` at
`0x6FB16A2E`) when the skill has a level, comes down onto the screen's top
rows -- and a zero column draws it against the left edge at its row's
height, over whatever is there. The names said TOP for the cell's bottom
edge -- `SKILLTREE_CellTop`, `nCellTop`, `nPanelTop` -- until the re-check of
2026-09-18 renamed them, and until then this paragraph read them as the top
and put an out-of-range icon "into the screen's top-left corner, over
whatever HUD is there".

**Why latent -- the data and the page.** Every record the stock game shows is
in range: reading the stock `data\global\excel\skilldesc.txt` (221 records,
2026-09-18), pages 1..3 carry 70 rows each, all of them rows 1..6 and columns
1..3, and every record outside those ranges is page 0's eleven -- all row 0 /
column 0. Page 0 is never shown: `gnSelectedSkillPage` starts at 1 (dword 1
at `0x6FB8CF00` in the image) and the module's only three writers store 3, 2
and 1 (`0x6FB17DA5`, `0x6FB17DC6`, `0x6FB17DE3`, the three tab arms of
`SKILLSTREE_OnLButtonDown`), and every consumer of the helpers sits behind the
same page test -- five `cmp`s, one per walk: `0x6FB16504` and `0x6FB16B66`
(the two draw passes, which call the icon drawer), `0x6FB17BCA` and
`0x6FB17F67` (OnLButtonDown's assign and arm walks) and `0x6FB18301`
(OnLButtonUp's); the arm walk's was missing from this list until the re-check
of 2026-09-18. A mod's `skilldesc.txt` with `SkillRow 0` (or 7 and up) on a
shown page would draw its icon above the screen, with a hover and click box
no cursor can enter -- `y` in (-48, 0); a `SkillColumn` outside 1..3 would
move the icon and its box to the screen's left edge.

Reproduced in `skillstree.cpp`; both helpers keep the zero.

## `PARTY_BuildRowList` keeps walking while `nRow > 8`, with no roster entry left

`0x6FB01D05`, `Party.cpp`. **Argued 2026-09-23** at Party.cpp's
close. **LATENT.**

    6FB01D01  cmp  ebp, ebx               ; pEntry
    6FB01D03  jnz  short loc_6FB01D10     ; another entry: build its row
    6FB01D05  cmp  [esp+1Ch+var_4], 8     ; nRow, signed
    6FB01D0A  jle  loc_6FB01EFE           ; done

The loop runs while `pEntry || nRow > 8`. Past the roster's last entry a row
count above 8 would keep it building rows from a NULL entry, and
`ROSTER_PartyGetNextEntry(NULL)` answers the roster's head (`mov eax,
gpPartyEntries`, `0x6FAAF5BB`), so the walk would start over rather than end.
It reads as a cap written the wrong way round.

**Why latent -- the roster holds at most eight entries.** A roster of N gives
N - 1 rows (the walk starts at the second entry), so the arm needs ten. D2Game
refuses a ninth client (`cmp dword ptr [ebp+8Ch], 8 / jge` in
`CLIENTS_AddToGame`, D2Game `0x6FC32600`); `ROSTER_AddPlayer` merges a second
0x5B for an id or a name it already holds into that entry
(`0x6FAAECC5..0x6FAAECFB`); and `ROSTER_RemovePlayer` unlinks and frees the
entry on 0x5C. So `nRow` is at most 7 when `pEntry` comes back NULL.

Reproduced in `Party.cpp`.

## `PARTY_BuildRowList` copies the display name into 25 bytes with no bound

`0x6FB01E79..0x6FB01E81`, `Party.cpp`. **Argued 2026-09-23.**
**LATENT, two bytes to spare.**

The inlined `strcpy` copies `ROSTER_GetDisplayName(pEntry)` into the row's
25-byte `szName` at +0xA0. The callee answers the entry's 26-byte `szNameEx`
(`lea eax, [esi+66h]`, `0x6FAAF5EE`), which `ROSTER_AddPlayer` builds as
`sprintf("%s {%s}", name, tag)` when the tag is not empty and as a copy of
the name otherwise (`0x6FAAED9F..0x6FAAEDC8`).

**Why latent:** D2Game's 0x5B builder (`0x6FC3DF80`) zeroes the packet (`rep
stosd` of 0x48 dwords and a `stosw`, `0x6FC3DF97..0x6FC3DFA4`), copies the
name from the player's 16-byte `szName` -- which the join refuses unless it
is terminated within 16 (D2MOO, `D2Game/src/GAME/CCmd.cpp`,
`CCMD_IsStringZeroTerminated(szClientName, 16)`) -- and stores the guild tag
as ONE dword at +0x22 (`CLIENTS_GetGuildTag`, `mov eax, [ecx+460h] / mov
[edx], eax` at D2Game `0x6FC344C0`, called at `0x6FC3E01B`). So the display
name is at most 15 + 2 + 4 + 1 = 22 characters, 23 bytes with its
terminator. Even a 25-character one would put its terminator on `nClassId`'s
first byte (+0xB9), which the next store overwrites (`mov [esi+0B9h], eax`,
`0x6FB01E8C`).

Reproduced in `Party.cpp`.

## `PARTY_DrawPartyGroupDividers` fills an eight-slot array with no bound

`0x6FB029FC..0x6FB02A18`, `Party.cpp`. **Argued 2026-09-23.**
**LATENT, margin zero.**

The fill stores one dword per roster entry, from the head, for as long as
`ROSTER_PartyGetNextEntry` answers, into `gnPartyRowPartyIds[8]` at `0x6FBB5EF8`;
the next dword is `gpPartyRowList` (`0x6FBB5F18`). A ninth entry would write
its party id over the head of the row list `PARTY_DrawPanel` walks on the
next frame.

**Why latent:** the roster holds at most eight entries (the entry above on
`PARTY_BuildRowList`'s `nRow > 8`), so the fill writes at most eight dwords --
with nothing to spare.

Reproduced in `Party.cpp`.

## `PARTY_DrawHoverHint` would fault on a NULL roster entry over a row's name

`0x6FB03110`, `Party.cpp`. **Argued 2026-09-23.** **LATENT.**

On a name hit the function hands its roster cursor to `ROSTER_GetDisplayName`
and converts the answer with `Unicode::toUnicode`. For a NULL entry the
callee warns (`FOG_DisplayWarning("ptRoster", ...)`,
`0x6FAAF5D3..0x6FAAF5E6`) and goes on: `FOG_DisplayWarning` is
`FOG_DisplayAssert`'s body with level 7 for 5 (Fog `0x6FF5ED90` against
`0x6FF5ED30`), it writes Fog's error log and returns, and an assert halts
only by the `exit(-1)` its caller writes after it. The callee then answers
`NULL + 0x66`, which `toUnicode` would read and fault on. Batch 4 had read
the warning as an assert; the source says so and is corrected.

**Why latent:** the cursor is NULL on a row only if the rows outnumber the
roster's entries after the first, and the rows are rebuilt by every function
that changes the roster. Thirteen functions write `gpPartyEntries`: the nine roster mutators that call
`PARTY_BuildRowList` -- `ROSTER_AddPlayer` at `0x6FAAEE89`,
`ROSTER_RemovePlayer` by the tail jump at `0x6FAAF4D5` -- three helpers only
those nine call (`ROSTER_PartyExtractPartyMembers`,
`ROSTER_FlattenGroupsIntoEntryList`, `ROSTER_MoveEntryToFront`; the call
graph), and `ROSTER_FreeAll`, which rebuilds nothing and whose one caller is
`GAME_AppModeCloseWindow`.

Reproduced in `Party.cpp`.

## `SELECT_ApplySelectedUnit` hands a tile's room to two asserting exports untested

`0x6FAB5B9C..0x6FAB5BA2` and `0x6FAB5C01..0x6FAB5C07`, `Select.cpp`.
**Argued 2026-09-23** at Select.cpp's close. **LATENT.**

DUNGEON_UpdateWarpRoomDeselect (#10092, 0x6FD8D260) and _Select (#10091,
0x6FD8D220) test the room and on NULL call FOG_DisplayAssert("ptRoom") and
`_exit(-1)` (0x6FD8D268..0x6FD8D281, 0x6FD8D228..0x6FD8D241). The room is
UNITS_GetRoom's answer for a TILE, `pStaticPath->pRoom` (D2Common
0x6FDBE2BC). What prevents a NULL: a tile's static path is filled at creation
with the room CUNIT_InitUnit finds for the tile's own position --
CUNIT_FindRoomForCoords, then UNITS_InitializeStaticPath and
UNITROOM_AddUnitToRoom with that room (CUnit.cpp:485, 532-537) -- and a tile
leaves its room only when it is freed (CUnit.cpp:1349). Both sites reach a
live tile: the new one has passed CUNIT_HitTestUnit, the old one was
re-found by GUID. The assert needs a tile the server spawned (packet 0x09) at
a position no loaded room holds.

Reproduced -- the room is passed untested.

## `SELECT_UpdateSelectedUnit` skips missiles before its corrupt-list guards

`0x6FAB5FFC`, `Select.cpp`. **Argued 2026-09-23** at Select.cpp's
close. **LATENT.**

The room walk has two guards against a corrupt unit list -- a unit that links
back to the one just seen ends the room, and a 30000th unit ends it with
FOG_DisplayWarning, "Far too many units on screen" (Select.cpp line 536) --
but a missile `continue`s at 0x6FAB5FFC before either and is not counted, so
a missile linked to itself would hold the loop. What prevents it: the one
insert that makes such a link, adding a unit to a room whose list already
holds it, is refused in D2Common -- UNITROOM_AddUnitToRoomEx walks the list
and asserts "Unit being added to a room it is already in" with `_exit(-1)`
(0x6FDBD02E..0x6FDBD03D; the string at 0x6FDE5BFC has that one reference).
It needs a list corrupted some other way.

Reproduced -- the missile test is where the bytes put it.

## `SELECT_RosterPetsSetHirelingDisplayState` stores through a record it does not test

`0x6FAB643B..0x6FAB6444`, `Select.cpp`. **Argued 2026-09-23** at
Select.cpp's close. **LATENT.**

ROSTERPETS_GetPetInfo answers NULL for a pet not in its list (`xor eax,
eax` at 0x6FAB18E5), and the store `mov [eax+8], ecx` follows untested.
What prevents it, caller by caller: PLAYER_OnMouseClick
and PLAYER_ReleaseClickAndResetHireling look up the same GUID with
GetPetInfo themselves and read `[eax+8]` untested before they call
(0x6FB49C5D, 0x6FB4B399), so a NULL faults in the caller first;
HIREABLES_HirIconsOnF11 passes the monster whose GUID is 0xFFFFFFFF, looked for only
when the player has no associated hireling -- the value that means "none".
The store needs a live monster carrying that GUID. **The two PLAYER_ callers
are settled at Player.cpp's close**: their GUID is
ROSTERPETS_GetAssociatedUnitGUID's, read off gpRosterPetList, which
ROSTERPETS_GetPetInfo walks for the same id, so the record is found ("The
PLAYER_ family's dead tests").

Reproduced -- the record is written untested.

## `UNITS_GetXPosition` and `UNITS_GetYPosition` read a static path they do not test

`0x6FAB6A82..0x6FAB6A85` and `0x6FAB6AB2..0x6FAB6AB5`, `Select.cpp`.

**Argued 2026-09-23** at Select.cpp's close. **LATENT.**

For OBJECT, ITEM and TILE both read `pStaticPath->tGameCoords` with no NULL
test, where the dynamic arm tests its path; D2Common's own copy,
UNITS_GetXPosition (0x6FD925E0), is the same instructions. What prevents a
NULL: CUNIT_InitUnit allocates the static path for exactly those three types
(UNITS_AllocStaticPath, CUnit.cpp:518, 527, 532), and it is freed only with
the unit (CUnit.cpp's free arms).

Reproduced -- the static path is read untested.

## `VIDEO_RecordCinematicProgress` takes the raw mask 0x22 as the stored rank when the value cannot be read

`0x6FAA159D` (`mov al, 22h`), `Video.cpp`. **Argued 2026-09-23**
at Video.cpp's close. **LATENT.**

The progress is the fourth field of "216.148.246.<mask>" in the registry
value "Aux Battle.net" under "Diablo II". When Storm #422 (SRegLoadString)
fails, the stored rank is 0x22 -- 34, the mask's base, not its decoding,
which would be 1 -- and no rank reaches 34, so nothing is ever written. What
prevents it: **D2Launch creates the value itself.** Its function at
0x6FA1FD10 (eight call sites and one table entry, 0x6FA33960) reads the same
value with the same format, and when the read fails it takes rank 1 (`mov
byte ptr [esp+1Ch], 1`, 0x6FA1FDDE) and writes "216.148.246.34" through
Storm #425 (SRegSaveString, 0x6FA1FDC3..0x6FA1FDFC); a second site does the
same at 0x6FA1D84C..0x6FA1D87F. The launcher runs before any game, so
D2Client finds the value unless the registry refused that write -- in which
case its own write would be refused too. D2Launch's copy of the decoder
also shows the value D2Client's branch should have had: 1. (D2Launch.dll
read with pefile and capstone.)

Reproduced -- the 0x22 is kept.

## The floor passes' `ptRect` assert tests an address that a NULL view never makes zero

`0x6FAA2E95..0x6FAA2E9B` (DFLOOR_DrawFloorTile) and `0x6FAA3CF9..0x6FAA3D06`
(DFLOOR_DrawViewGridFloors), `dFloor.cpp`. **Argued 2026-09-23**
at dFloor.cpp's close. **LATENT.**

`mov eax, [ebx+24h] / add eax, 14h / jnz` -- the assert (line 118) is
`&pView->rcClip != NULL`, which fails only for a pView of -20. A NULL view
passes it and faults on the clip read after. What prevents a NULL from
arriving: both passes dereference the view before they reach the assert --
DFLOOR_DrawRoomFloors reads `pView->nOriginX` filling the context
(0x6FAA33CA), DFLOOR_DrawViewGridFloors at its entry (0x6FAA3BC1) -- so a
NULL faults there first, and their caller hands them the live view.

Reproduced -- the address is what is tested.

## DFLOOR_DrawRoomFloors reads the level's DrawEdges byte before it asserts the record

`0x6FAA3390`, `dFloor.cpp`. **Argued 2026-09-23** at dFloor.cpp's
close. **LATENT.**

`mov cl, [eax+9]` on DATATBLS_GetLevelsTxtRecord's answer, untested; the
same lookup is asserted (`ptLvlData`, line 304) forty lines of bytes later.
What prevents a NULL: the level id is the player's own room's
(DUNGEON_GetLevelIdFromRoom), and a room exists only for a level
levels.txt defines.

Reproduced -- the byte is read untested.

## The edge pass trusts the bounds the floors widened, and fires on the seeds if none did

`0x6FAA3615..0x6FAA3620` and `0x6FAA38CE..0x6FAA38D9`, `dFloor.cpp`.
**Argued 2026-09-23** at dFloor.cpp's close. **LATENT.**

The bounds start at 0x7FFFFFFF (min) and 0x80000000 (max). If no floor tile
passed DFLOOR_DrawFloorTile's tests this frame, the two MIN sides still
fire -- the player's position minus 0x7FFFFFFF is far below either margin
-- and draw their six edge tiles around x or y = 2147483640; the max sides'
differences wrap positive and stay quiet. What prevents it: the edge pass
runs only with bDrawEdges, and the player stands in view on a floor of the
player's own room, one of the rooms the pass has just drawn, so at least one tile
widens the bounds. Where those far tiles would land on screen after
D2Common's subtile conversion was not computed.

Reproduced -- the seeds are compared as they stand.

---

# UNREACHABLE

Code that cannot execute. Reproduced anyway, because the branch is in the image.

## Half of `EDITBOX_UiEditBoxDraw`, and both of the bounds it guards

`0x6FAD7800`, `editbox.cpp`. **Found 2026-09-12**, and the reason all of it
lands in one entry is that one measurement settles the lot.

**THE MODULE BUILDS EXACTLY ONE EDIT BOX.** `EDITBOX_UiEditBoxConstruct` has ONE
caller, `EDITBOX_UiCtrlProxyConstructEditbox`; that has ONE caller,
`UI_OpenGoldEntryDialog` at 0x6FB23DF6; and it passes

    nWidth 100   nLineCount 1   nMaxChars 0x0A   nFont 1

So `nMaxLen` is always 10 and `nLineCount` is always 1, everywhere, for the
life of the process. Three consequences:

* **The multi-line half of the draw pass never runs.** `if (nLineCount == 1)`
  takes the single-line branch, so `D2Win_10199`, the per-line caret search
  and the split-list walk are all dead -- roughly a third of the function.
* **The `> 199` guard inside it cannot fire.** It tests one split line against
  a 200-element local and, when it trips, `return`s from the middle of the
  loop, leaking every remaining node of the split. A ten-character buffer
  cannot produce a two-hundred-character line even if the branch were live.
* **And `EDITBOX_UiEditBoxGetIntValue`'s zero cannot either.** It answers
  `nValue > nMaxValue ? 0 : nValue` -- the ceiling maps to ZERO rather than to
  the ceiling -- and the only way the text can exceed `nMaxValue` is
  `EDITBOX_UiEditBoxSetTextFromInt` or `SetTextFromAscii`, because `EDITBOX_UiEditBoxOnChar`
  rewrites the whole buffer to the ceiling on every keystroke that would pass
  it. Those two are vtable slots 15 and 14 and **nothing in the module indexes
  them**: `UI_OpenGoldEntryDialog` touches the spinner through byte offset 44
  (slot 11, GetIntValue) and offset 0 (the destructor), and no other code
  holds the control at all.

All three are reproduced as written. The first is why the file is longer than
the game needs; the other two are the kind of defect that a second caller
would make live, which is worth knowing before anything constructs a second
edit box.

## The charge-up icon strip

`0x6FB21A00`, `ui_panels.cpp`. The whole body is gated by
`gbChargeUpIconsEnabled` at `0x6FBBA794`, which is **inside `.data`'s virtual
extent but past its raw size** — `ida_bytes.is_loaded` is False and the
`ffffffff` IDA shows is filler, not a value. The loader zero-fills it and the
module's whole reference set for it is one read.

So the charge-up icon strip is unreachable in a stock 1.10 client, and
`gnChargeUpIconStepX` beside it is zero for the same reason. **Whether it is a
defect or a feature left switched off cannot be settled from inside D2Client.**

## The 800x600 fall-through, two of its three copies

`inv.cpp`. The same block appears in `INV_CubeOnLButtonDown`, `INV_StashOnLButtonDown`
and `INV_OnLButtonDown` with a `!sgInventoryMode` conjunct. In the first two it
**cannot hold**, because those functions have already required the mode to be 14
or 12. In `INV_OnLButtonDown` the mode really can be 0, so the third conjunct is
the only one of the three copies that does anything.

## `INV_UseItemAtGridCell` tests an item code that no stock table defines

`0x6FAE8ED4`..`0x6FAE8EE4`, the three compares, `inv.cpp`. **Found 2026-09-11.**

The not-useable branch tests three packed item codes and opens the Scroll of
Inifuss reader for any of them:

    544436852  'trs '   NOT IN ANY STOCK TABLE
    544435042  'bks '   Bark Scroll, misc.txt, quest 5
    543452002  'bkd '   deciphered Bark Scroll, misc.txt, quest 5

`trs` is defined by no `code` column in `weapons.txt`, `armor.txt` or
`misc.txt`. **So that arm cannot run in an
unmodified game** -- and a mod that gives an item the code `trs` gets the
readable-scroll panel with no patching at all.

Whether it was a cut item or a typo for something is not established.
Reproduced.

## `INV_CubeStartQuestItemAnim`'s fall-through arm for an unknown item code

`0x6FAED0D0`, `inv.cpp`. Any item code other than `'hst '` (0x20747368) and
`'qf2 '` (0x20326671) starts the transmute animation with no quest bookkeeping at
all. **Unreachable from the one call site**: `ITEM_PlaceAtStorePage` reads
`pItemsTxt->dwCode` at +0x80 and calls here for those two codes only. The shape
of a switch that lost its other arms.

## `INV_LeavePanelMode5`'s trace arm, in the sense that its siblings assert

`0x6FAE9500`, `inv.cpp`. Not unreachable — listed here for contrast. Its two
siblings `INV_NpcShopLeaveVerifyMode` and `INV_NpcShopCloseAndRestoreCursor` assert and
`exit(-1)` on a wrong mode; this one calls `FOG_Trace("Inventory mode is %d\n")`
and returns. **Whether the difference is deliberate is NOT ESTABLISHED.**

## Two asserts in `Wall2.cpp`, deliberately

Recorded in that file: the conditions cannot hold given the callers, and the
asserts are the author's statement of that.

## `INV_AppendChanceToBlockText`'s class lookup faults on its out-of-range arm

`0x6FAE4F36`, `inv.cpp`. An inlined `DATATBLS_GetCharStatsTxtRecord` honours
its bounds test by producing NULL, and the next instruction reads the record's
`BlockFactor` anyway:

    6FAE4F2D  xor  eax, eax
    6FAE4F36  mov  cl, [eax+49h]          faults at 0x49

The fourth instance of this shape after the SOUND accessors and the two
MonStats lookups. All three decompilers print it. **Reproduced**: the rewrite
compiles to `xor eax,eax / movzx eax, byte ptr [eax+49h]`, checked in the
object file.

**UNREACHABLE**: the unit is the player, whose class is one of the seven
playable ones, and `CharStats.txt` has a row for each.

## `INV_AppendAttackSpeedText` indexes backwards with no player

`0x6FAE561A`, `inv.cpp`. With no player the class id is `-1`
(`or eax, 0FFFFFFFFh`) and the offsets table is read at `2 * -1 + bDistance`
-- index -2 or -1.

**Where it lands is measurable**: `gptAttackSpeedMatrix` is 18 x 5 ints at
0x6FB79360 and ends exactly at 0x6FB794C8, where `gptAttackSpeedClassOffsets`
begins. So the read is `matrix[17][3]` or `matrix[17][4]` -- both 5 --
deterministic, inside the original's own data, and not a fault.

**UNREACHABLE**: an item tooltip is only drawn when there is a player.
Reproduced as written; BFE's port guards it and D2MOO carries it.

2026-10-11, the globals plan's batch P2: both tables are objects of ours
since (inv.h), so an index outside one no longer lands in the other by
layout -- C gives the two no order. The read is written by NAME: a negative
offset index reads `gptAttackSpeedMatrix[17][5 + i]`, and a flat index of
90 or more reads `gptAttackSpeedClassOffsets[(n - 90) / 2][(n - 90) % 2]`
(inv.cpp, INV_AppendAttackSpeedText) -- the same dwords the original's flat
reads land on.

## `INV_BuildItemNameText`'s body-part arm faults on its out-of-range arm

`0x6FADD8CB`, `inv.cpp`. The fifth instance of the shape, and the third on
MonStats: an inlined bounds-checked lookup yields NULL, and the next read has
no test.

    6FADD8BB  xor  esi, esi                 file index out of MonStats' range
    6FADD8BD  mov  cx, [edi+0F4h]           the base name is fetched FIRST
    6FADD8CB  mov  cx, [esi+6]              ...and the monster's name faults

**Reproduced**, and checked in the object file: `xor esi,esi`, the base-name
call, then `mov cx, word ptr [esi+6]` -- the original's order, and its
424-byte stride as `imul esi, eax, 1A8h`.

**UNREACHABLE**: the arm needs item type 40, `ITEMTYPE_BODY_PART` (`push 28h`
at 0x6FADD880), and all twelve items of that type in `Misc.txt` are named
"Not used" with `spawnable` 0; no type names it as an equivalent. BFE's port
guards the read and says so.

## `INV_DrawItemDescription`'s rune letters read the socketed rune's ItemsTxt record unchecked

`0x6FAE3127`, `inv.cpp`. The sixth instance of the shape:

    6FAE3122  call DATATBLS_GetItemsTxtRecord    the socketed item's class
    6FAE3127  mov  eax, [eax+0F0h]               dwGemOffset -- faults at 0xF0

All three decompilers print the unchecked read. **Reproduced**, and checked in
the object file: `call _DATATBLS_GetItemsTxtRecord@4` then
`mov eax, dword ptr [eax+0F0h]`.

**UNREACHABLE**: the record is looked up by the class of an item already in a
socket, and an item unit is created from its ItemsTxt row.

## The player's second pass over the weapon placeholders cannot draw

`INV_UpdateAndDrawPanels`, 0x6FADFA89..0x6FADFB98, `inv.cpp`. **Found
2026-09-12.**

The empty weapon-slot art is drawn by two passes. The first (0x6FADF94D)
reads both hands and the wield type into locals and draws with the swap-set
nudge. The second reads the SAME two slots again and guards every one of its
four draws with the answer:

    0x6FADFB28  no right-hand item, and  pLArm && !Get(LARM)
    0x6FADFAB7  both hands, not two-handed, and  !Get(LARM)
    0x6FADFAEF  ... and then              !Get(RARM)
    0x6FADFB60  a right-hand item, no pLArm, and  !Get(RARM)

`pLArm` IS `Get(LARM)` and `pRArm` IS `Get(RARM)` -- the same calls on the
same inventory, twenty instructions earlier -- and the only calls in between
are `TEXTURE_CelDraw`. Every one of the four conditions is therefore a
contradiction.

**UNREACHABLE**: the pass runs on every frame the panel is open and can
draw nothing at all. Hex-Rays, Ghidra and Binary Ninja all print the
re-reads. Reproduced whole, gotos included. `INV_MercInvDrawEmptySlots` is the
same shape with its tests on the LOCALS, and there they fire.

## `WAYPOINT_BuildActWaypointList` admits an act index one past its table

`0x6FB26480`, `waypoint.cpp`. **Found 2026-09-12** while closing the unit.

    6FB2647B  mov eax, gnWaypointActTab
    6FB26480  cmp eax, 5
    6FB26483  jbe short loc_6FB26492      <-- 5 PASSES
    ...
    6FB26492  lea ecx, [eax+eax*4]        stride 11: ecx = 5 * eax
    6FB26496  lea ecx, [eax+ecx*2]                   ecx = 11 * eax

`gWaypointActTable` is `D2ClientWaypointActStrc[5]` at 0x6FB90778, eleven
bytes a record, so it ends at 0x6FB907AE and a sixth record would start at
0x6FB907AF. The bound should be `< 5` and is `<= 5`.

**What index 5 would touch**, measured off the image rather than reasoned:

    bHasWaypoint      0x6FB907AF   the LAST BYTE of record 4, nLastWaypointNo
    nFirstLevelId     0x6FB907B0   gnWaypointActCount, the constant 5
    nLastLevelId      0x6FB907B4   the zero beside it
    nFirstWaypointNo  0x6FB907B8   the first int of the row COORDINATE table
    nLastWaypointNo   0x6FB907B9

Only the first would be written -- `bHasWaypoint = 1` clobbering record 4's
`nLastWaypointNo` -- because the scan is guarded by
`nFirstLevelId > nLastLevelId`, which reads 5 > 0 and skips, and the build
loop by `nFirstWaypointNo <= nLastWaypointNo`, which reads 17 > 0 and skips
too. So even the worst case is one byte, not the coordinate table.

**UNREACHABLE**, and the proof is a count of writers. `gnWaypointActTab` has
six references in the module and exactly ONE is a write: `WAYPOINT_ClampActTab`
at 0x6FB2641A. That function opens with

    if (nAct >= gnWaypointActCount) nAct = 0;

and `gnWaypointActCount` is the constant **5** in `.data` -- one reference in
the whole image, that reference this read, no writer, no address taken. Every
later arm of the clamp only lowers the value. So the global can hold 0..4 and
nothing else, and `jbe 5` never sees 5.

Reproduced as written, bound included. The database's note on
`gnWaypointActTab` said the writer was `WAYPOINT_OnMouseDown`; that is a read,
and it was corrected the same day.

## `NPCMENU_ToggleResurrectMercRow` inserts one slot past the rows it has

`0x6FAF9CB0`, `npcmenu.cpp`. **Found 2026-09-13.** Both insert arms open a
hole by copying downwards from slot `nRows`. With no
`NPCMENU_DrawHireMercenaryList` row:

    6FAF9D23  lea esi, [ecx-1]                  esi = nRows - 1
    6FAF9D26  cmp esi, esi / jl                 never taken: one copy, always
    6FAF9D3D  mov ebx, [ecx-4] / mov [ecx], ebx pfnHandler[nRows] = pfnHandler[nRows-1]
    6FAF9D60  mov pfnHandler[eax+esi*4], offset NPCMENU_OnResurrectMercClicked

and the hire-row arm at 0x6FAF9D7D starts the same copy at the same slot. The
record has five rows. With `nRows` 5, slot 5 is `pfnHandler[5]` -- the
record's `bEnabled` and the first three bytes of the next record -- and
`wTblIdx[5]`, the low word of `pfnHandler[0]`. With `nRows` 0 the no-hire arm
writes slot -1: `pfnHandler[-1]` is `wTblIdx[3..4]` and `wTblIdx[-1]` the upper
word of `nRows`.

**UNREACHABLE**, measured over the table and its writers. The toggle is called
for five descriptors -- 8, 11, 24, 21 and 43, from `NPCMENU_OpenMainMenu`'s
class switch (0x6FAF9A91..0x6FAF9B0E) and from
`NPCMENU_SetResurrectMercOffer` -- and inserts only when the row is absent. The
shipped table gives the five 3, 2, 4, 2 and 2 rows; the stores that reach them
write 3 (records 11, 21 and 43, and 11 again in OpenMainMenu's level-8 arm) or
2, 3 or 4 through `NPCMENU_SetRowCount2..4`. Nothing writes 0, and the one
store of 5 is record 41's. So an insert sees 2..4 rows.

Reproduced as written, through byte offsets into the record, so the two slots
stay defined C.

## The main menu's heal row, and a test in it that cannot fire

`NPCMENU_BuildMainMenu`, 0x6FAF698D..0x6FAF6A41, `npcmenu.cpp`. **Found
2026-09-14**, batch 10a.

The row builder has an arm for tbl 3337 `NPCHeal`, 'Heal :': skip the row at
full life, price it with `UNITS_GetHealingCost`, floor the price at 1, append
the figure. Then the bytes test the price again:

    6FAF69C9  test edi, edi / jnz        the floor
    6FAF69CD  mov  edi, 1
    ...
    6FAF6A35  test edi, edi / jnz        and the same test, after the strcat
    6FAF6A3D  mov  [esp+510h+a8], edi    clears the row's selectable flag

A price floored at 1 is never zero, so the store cannot run. Hex-Rays drops
it; Ghidra and Binary Ninja print it.

**UNREACHABLE in 1.10**, and the whole arm with it. No shipped record of
`gNpcMenuDescriptors` carries 3337 in any of its five labels (all 48 records), and no writer of `wTblIdx` stores it: the
eight quest unlocks write their own fixed ids, `NPCMENU_EnableIdentifyRow`
4020, `NPCMENU_OpenMainMenu` 3397, `NPCMENU_ToggleResurrectMercRow` 5383 and
the rows it moves, `NPCMENU_SetRowCount2..4` zero, and
`NPCMENU_BeginInteraction` ebx, zeroed at 0x6FAF828A, into record 21.

Reproduced whole, the dead store included; MSVC 2022 folds that store away.

## `NPCMENU_BeginInteraction` ends the last conversation's GUID for an NPC it cannot talk to

`0x6FAF8913..0x6FAF892E`, `npcmenu.cpp`. **Found 2026-09-14**, batch 10b.

With no menu node at all, the arm tests the NPC's monstats record for the
interact bit (`gdwBitMasks[1] & nMonStatsFlags[1]`, 0x6FAF889F..0x6FAF88A9).
Without it the conversation is ended through `mov ecx, gNpcUnitGUID` /
`call NPCMENU_NpcEndInteraction` -- and nothing on the way there has written that
GUID: the four inlined `NPCMENU_SetInteractingNpc` copies (0x6FAF8422,
0x6FAF84D8, 0x6FAF8675, 0x6FAF88B7) and its call (0x6FAF87EA) are all on other
arms. It holds the previous conversation's NPC, which
`NPCMENU_SetInteractingNpc(NULL)` leaves behind, so client packet 0x30 goes out
for that NPC while the one this packet named keeps the 0x2F just sent for it.

**UNREACHABLE** against the server 1.10 ships: D2MOO's only sender of packet
0x28 with a unit, `sub_6FC61C70` (D2Game 0x6FC61C70, MonsterAI.cpp:475),
returns at :489 unless the target's monstats record carries
MONSTATSFLAGINDEX_INTERACT -- the same bit, over the same stock monstats.txt.
Its six other senders pass unit type 6, which returns at the top of the
function.

Reproduced as written.

## `SCMD_HandleSrv17_ApplyRemoteUnitAction` reads through two 16-bit values as addresses

`0x6FAB2A80`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 4. Packet 0x17's
deferred handler.

It takes the words at +8 and +0x0A, zero-extends both, and **reads through each
as a flat address**:

    6FAB2AB8  mov ax,  [esi+8]        ; esi is the packet
    6FAB2ABC  mov dx,  [esi+0Ah]
    6FAB2AC2  mov cx,  [eax]          ; a word read AT that value
    6FAB2ACA  mov ax,  [edx]

with `xor eax, eax` / `xor edx, edx` above them. A value below 0x10000 is never
a mapped address on Windows, so the first read faults. All three decompilers
print the double load.

**Why UNREACHABLE:** packet 0x17 cannot reach the handler. The shipped D2Net's
server-packet size table at `dword_6FC08148` holds **0** for index 0x17
(read in the D2Net database, with 0x18 -> 15 and 0x73 -> 32 beside it as
controls), and `SERVER_GetServerPacketSize` returns that 0 -- its `if (result)`
is false and it ends at `return result`. The router tests that answer and
returns without dispatching:

    if (!D2NET_SERVER_GetServerPacketSize(pBuffer, nSize, &nSizeRead))
        return;

and the only reference to this function anywhere is gSCmdTable row 0x17's
`pfProcessUnit` (database: 1 caller; the source: one row). So no path reaches
the double dereference.

Reproduced as written, as `inv.cpp` reproduces its NULL records.

## The unit-queue drain gives up with the queue still full

`0x6FAB5610`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 1.

`SCMD_ProcessUnitPacketQueue` walks a unit's deferred packets and re-sizes each
one. When a size cannot be read it returns -- `jz loc_6FAB56E9` at 0x6FAB5673 --
**past the memset and past `nUsed = 0`**. The queue is then never drained, the
router keeps appending, and `SCMD_AllocUnitPacketQueue` grows it five slots
every time it fills.

**Why UNREACHABLE:** a packet is only queued after the ROUTER has already sized
it non-zero through the same `SERVER_GetServerPacketSize`, so the drain's read
of the same id cannot answer zero. Of the twenty rows carrying a `pfProcessUnit`,
the only id whose table entry is not a fixed positive size is 0x17, and 0x17 is
turned away by the router (the entry above). Every other queueing id --
0x0C..0x10, 0x4C, 0x4D, 0x67..0x6D and 0x6E..0x72 -- has a fixed size in
`dword_6FC08148`.

Reproduced as written.

## Item message 0x07 pairs a swap-set hand with the active set's other hand

`0x6FB2C6C2`, `Item.cpp`, `ITEM_ItemMsg07_SwapBodyLocItems`. Found at
Item.cpp's batch 6, verdict at its close, 2026-09-15.

The hand whose item is pushed off is chosen by a jump table over body locations
4..12 (`cmp eax, 8` at 0x6FB2C6B9, `jmp ds:jpt_6FB2C6C2[eax*4]`), and it sends
4 (BODYLOC_RARM) and 11 (BODYLOC_SWRARM) to look in 5, and 5 (BODYLOC_LARM)
and 12 (BODYLOC_SWLARM) to look in 4. An item arriving in a swap-set hand would
displace the ACTIVE set's other hand, not 12 or 11.

**Why unreachable:** D2MOO's D2Game sends this action for an item carrying
command flag 0x10000 (`INVENTORY/InvMode.cpp`), and the one function raising
that flag is D2Game 0x6FC45550, which takes the two active hands only:

    6FC4567D  cmp al, 5 / jz        the other hand is 4
    6FC45681  cmp al, 4 / jz        the other hand is 5
    6FC45685  mov ecx, [esp+18h+a6] / ... / mov dword ptr [ecx], 1 / retn 10h

Every other location fails the request before anything is sent, so the 11 and
12 entries are never exercised by a stock server.

Reproduced in `Item.cpp`.

## Item message 0x13 reads the local player's inventory through an untested pointer

`0x6FB2EFA3`, `Item.cpp`, `ITEM_ItemMsg13_ItemInSocket`. Found at Item.cpp's
batch 8, verdict at its close, 2026-09-15.

Unless the decoded flags carry IFLAG_DELETED, the handler clears the local
player's cursor through

    6FB2EF9E  call CUNIT_GetPlayer
    6FB2EFA3  mov  edi, [eax+60h]          ; no test of eax

and only then asserts the inventory as "hCtrlInventory" (line 2828).

**Why unreachable:** the arm needs a 0x13 message WITHOUT IFLAG_DELETED, and a
stock server never sends one. D2Game passes action 0x13 to its 0x9D builder
(0x6FC3E6F0) at exactly two of that builder's twelve call sites, 0x6FC3E6C5 and
0x6FC3E832 -- the socket-contents loops that follow a socketed item's 0x9C and
0x9D -- and both push 8, IFLAG_DELETED, into the flags just before; D2MOO's builder ORs those flags into the item before
serialising it. So the unguarded dereference, and the cursor clear around it,
never run.

Reproduced in `Item.cpp`.

## The list box's page-follow block is dead in ScrollByTrackClick

`0x6FAF09E0`..`0x6FAF0A14` in `LISTBOX_ScrollByTrackClick`, `Listbox.cpp`.
Found at the unit's batch, verdict at its close, 2026-09-17; narrowed to this
copy and argued again at the unit's re-check the same day.

The lines that push `nTopEntry` back onto the selection exist in three
functions -- here, in OnArrowUp and in OnArrowDown -- and here they follow an
assignment of the one to the other:

    6FAF09E0  mov [esi+180730h], eax      ; nTopEntry = nSelection
    6FAF09F7  cmp eax, ecx / jge 06       ; the same two values: never taken
    6FAF09FB  mov [esi+180730h], eax
    6FAF0A0F  cmp eax, edx                ; x >= x + nVisibleRows ...
    6FAF0A11  jl  07                      ; ... needs nVisibleRows <= 0
    6FAF0A13  inc ecx
    6FAF0A14  mov [esi+180730h], ecx

**Why unreachable, for any panel:** the first test compares a value with
itself. The second needs `nVisibleRows <= 0`, and that field is written only
by the constructor (1, `0x6FAEFCB8`) and by ImplUpdateScrollThumb, which
zeroes it (`0x6FAEFDD8`) and increments it (`0x6FAEFE76`), so it means
`nVisibleRows == 0` left by an UpdateScrollThumb pass. That same pass takes
the thumb from a proportion of `(0 << 7) / wNumEntries = 0`, and the thumb
fields come out EQUAL on every arm: both zeroed at entry (`0x6FAEFDCC`,
`0x6FAEFDD2`), and when the thumb is drawn, set together at the end
(`0x6FAEFFCB`, `0x6FAEFFD1`) -- 0 and 0 on the first page, the scaled top
twice in the middle, the bar's end twice past it. Measured over the decoded
instructions of the whole module, each thumb field has three references:
those two stores and ScrollByTrackClick's read. And ScrollByTrackClick
returns 0 on `nThumbTop == nThumbBottom` at `0x6FAF0933`, before the block.

**And the arm is not a no-op if it were reached**: with `nVisibleRows == 0`
the second test is true by equality, and `nTopEntry = nTopEntry + 1` would
scroll the view one row past the selection.

**CORRECTED 2026-09-17.** The entry was first titled "dead in two of its
three copies" and listed OnArrowUp's copy (`0x6FAF0DE2`..`0x6FAF0E0A`) too,
on two premises: that `nVisibleRows` could reach 0 only in a panel shorter
than a row, and that "nothing drives the selection BELOW the window -- a row
click selects a visible row". The second is false. `LISTBOX_HitTestEntry`
has no page bound, so a click can select a row the draw never shows and a
release below the list can select one further down -- exactly what
OnArrowUp's second test needs: REACHABLE, "The list box hit-tests rows it
never draws, and the hire list hires one". The first argument also cited
three addresses that fall inside other instructions: `0x6FAEFCBA` is the
middle of the CONSTRUCTOR's store of 1, not a per-draw zero (that is
`0x6FAEFDD8`); the increment is `0x6FAEFE76`, not `0x6FAEFE78`; and the
wheel's `lea eax, [eax+edx-1]` is at `0x6FAF0A61`, not `0x6FAF0A5E`.

Reproduced in `Listbox.cpp`, the dead block marked.

## The list box thumb's zero clamp cannot fire

`0x6FAEFDB0`, `LISTBOX_ImplUpdateScrollThumb`, `Listbox.cpp`. Found at the
unit's batch, verdict at its close, 2026-09-17.

`nThumbTop` is computed as `nTopEntry * nThumbRows / wNumEntries` and then
clamped at 0. Measured over the stock module: the displacement `0x00180730`
(`nTopEntry`) has 23 references, **eight of them stores** -- the constructor's
0 (`0x6FAEFBE3`), four stores of a selection (`0x6FAF09E0`, `0x6FAF09FB`,
`0x6FAF0DEC`, `0x6FAF0E94`) and three of `nTopEntry + 1` (`0x6FAF0A14`,
`0x6FAF0E0A`, `0x6FAF0EB3`). Each writes a non-negative value: a selection
after ScrollByTrackClick's clamps, a selection one step from a positive one
or from -1, or one more than a top that is itself non-negative. (CORRECTED
2026-09-17: all seven were called selection-derived; re-measured over the
decoded instructions, the counts stand.) The row count in the product is the
constructor's 18 or 28 -- the only two values its field is ever given -- and
this arm runs only with `wNumEntries >= 1`.

**Why unreachable:** the product cannot be negative -- the largest possible
one, 128 * 28 = 3584, cannot even overflow an int. The clamp is defensive code
the values never violate.

Reproduced in `Listbox.cpp` as written.

## `SKILLSTREE_LoadChartImage`'s null guard can never fire

`0x6FB16310`, `skillstree.cpp`. Found by batch 1, 2026-09-17;
settled and written up at the close, 2026-09-18.

The loader builds the argument it is about to pass -- the class's chart name
-- and then tests that ARGUMENT for null:

    6FB1631E  lea     ecx, sgptPlayerSkillTreeChart.szFileName[ecx*2]  ; class*17, doubled; szName
    6FB16325  test    ecx, ecx
    6FB16327  jnz     short loc_6FB16347        ; always taken
    6FB16329  push    88h                       ; the source line
    6FB1632E  push    offset "C:\projects\D2\head\Diablo2\Source..."
    6FB16333  push    offset "sgptPlayerSkillTreeChart[gbPlayerType]...."
    6FB16338  call    FOG_DisplayAssert
    6FB1633D  add     esp, 0Ch
    6FB16340  push    0FFFFFFFFh
    6FB16342  call    _exit                     ; dead: the jnz above never falls through

`ecx` is an address -- `sgptPlayerSkillTreeChart.szFileName` at `0x6FB8CF0C`
plus `class * 34` -- and an address is never zero, so the assert-and-exit arm
cannot run. The `.data` makes the guard moot a second way: the seven records
(`D2ClientSkillTreeChartStrc[7]` at `0x6FB8CF08`, one per class) all carry a
non-empty name, `Spells\skltree_a_back` through `Spells\skltree_i_back`,
21 characters each -- read from the image at the close, 2026-09-18. A name
that WERE empty would reach `UI_LoadImageFromUiDir` (`0x6FB16347`)
unchecked; the check exists, reads as a check, and protects nothing.

All three decompilers read the `lea` as an address (Ghidra `s_..._6fb8cf0c +
class * 0x22 == 0`, Binary Ninja `&(*"Spells\\skltree_a_back")[class *
0x22]`), which is how the shape was settled.

Reproduced as the bytes have it in `skillstree.cpp`; the database
comment's "unrecoverable" sentence is false and was marked so at the close.

## `gbSkillPointSendPending` is never armed, so the guard written for it never fires

`0x6FBB983C`, `skillstree.cpp`. Re-measured at the close, 2026-09-18:
four references in the module, both writes storing zero.

    SKILLSTREE_ClearSkillPointPending   6FB16300  w   mov gbSkillPointSendPending, 0
    SKILLSTREE_DrawAvailableSkills      6FB1642B  r   mov eax, gbSkillPointSendPending
    SKILLSTREE_OnLButtonUp              6FB1851C  r   mov eax, gbSkillPointSendPending
    SKILLSTREE_OnLButtonUp              6FB1852E  w   mov gbSkillPointSendPending, 0

The address is past `.data`'s raw size (the raw section ends at RVA 0xFB000;
this is RVA 0x11983C), so the loader zero-fills the dword -- there is no
value for it in the image at all -- and nothing in the module ever stores
anything but zero.

The send is guarded and then "flags" itself with the wrong constant:

    if ( (!pPlayer || !STATES_CheckState(pPlayer, STATE_UNINTERRUPTABLE))
         && !gbSkillPointSendPending )
    {
        MSG_SendCmdWord(59, nSkillId);
        gbSkillPointSendPending = 0;      /* the arming line */
    }

**Why unreachable:** `!gbSkillPointSendPending` is a constant true, so the
intended "one spend in flight" guard cannot fire; the store writes the value
it already has; and `SKILLSTREE_DrawAvailableSkills`' tint (`||
gbSkillPointSendPending`, the read at `0x6FB1642B`) is decided by the
player's state alone. The clear the server path reaches -- the eleven-byte
function at `0x6FB16300`, the tail of `SCMD_HandleSrv21_UpdateSkills` --
clears a flag nothing sets. Command 59 is the add-skill-point packet. The
database comment on the global already records this as a defect of the
shipped game.

Reproduced as written; the C marks the store "it was already 0 to get here".

## `PARTY_DrawRowButtons`' NULL-player arm follows a call that asserts the player

`0x6FB02809`, `Party.cpp`. **Argued 2026-09-23** at Party.cpp's
close.

    6FB027E0  call D2COMMON_UNITS_GetRoom     ; pPlayer
    ...
    6FB02809  or   ecx, 0FFFFFFFFh            ; the unit id for a NULL player

The hostile box's `PLAYERLIST_RosterTestInfoFlag` call takes `pPlayer ?
pPlayer->dwUnitId : -1`, but the same pointer went to D2Common's
`UNITS_GetRoom` at `0x6FB027E0`, which asserts it and exits
(`FOG_DisplayAssert("ptUnit", ...)` and `_exit`, D2Common
`0x6FDBE278..0x6FDBE291`).

**Why unreachable, twice:** past that assert the pointer is not NULL; and it
is not NULL on entry either. `PARTY_DrawPanel`'s one caller is
`UIPANELS_DrawInGamePanels`, whose one caller is `GAME_DrawGameScene`, and that hands the frame's player to D2Common's
`UNITS_GetClientCoordX` first, which asserts `ptUnit` the same way (D2Common
`0x6FDBDB18..0x6FDBDB31`).

Reproduced as written.

## `PARTY_DrawHoverHint`'s "ERROR PLAYER NAME" copy is never reached

`0x6FB0323D..0x6FB0324F`, `Party.cpp`. **Argued 2026-09-23.**

    6FB03234  call ROSTER_GetDisplayName
    6FB03239  cmp  eax, ebx                   ; NULL?
    6FB0323B  jnz  short loc_6FB03258
    6FB0323D  xor  eax, eax                   ; copy gszErrorPlayerName

When `ROSTER_GetDisplayName` answers NULL the function copies
`gszErrorPlayerName`, "ERROR PLAYER NAME", into a 128-byte local and uses
that. The callee never answers NULL: both its arms end at `lea eax,
[esi+66h]` (`0x6FAAF5EE`) -- the NULL arm after a `FOG_DisplayWarning` that
returns (the LATENT entry on `PARTY_DrawHoverHint` above) -- so the answer is
the entry's `szNameEx`, or `0x66`.

Reproduced as written.

## `SELECT_UpdateSelectedUnit`'s warp-throttle arm tests a flag that is always set

`0x6FAB60A8..0x6FAB60C3`, `Select.cpp`. **Argued 2026-09-23** at
Select.cpp's close. **UNREACHABLE.**

    6FAB5F29  mov  gbSelectionRoomChanged, 1   ; the room differs from the last
    6FAB60A8  mov  eax, gbSelectionRoomChanged ; the arm's test for 0
    6FAB60C3  mov  gbSelectionRoomChanged, 1   ; the arm's own store

The listing above shows the flag under an older name; it is `gbSelectionRoomSeen`.

0x6FBAB9F4 has those three references and no
other, and nothing stores 0; the flag is `.bss`, zero at load.
gpLastSelectionRoom (0x6FBAB9F8, `.bss`, NULL at load) is compared at
0x6FAB5F21 with the player's room, which the function has just required to be
non-NULL -- so the first call that gets that far sets the flag, before the
scan, and every test at 0x6FAB60A8 from then on reads 1. The arm (push the
interact throttle 500 ms ahead, set the flag) never runs; the other arm, taken
while the throttle is pending, does. The database's comment described the
flag as arming that throttle on each room change until the close's
CORRECTED paragraph.

Reproduced -- the dead arm is written and marked.

## `sgWndProcHandlers` is never registered, so its six handlers never run

`0x6FB747C8`, `0x6FAA2460..0x6FAA2549`, `WNDPROC.cpp`. **Argued
2026-09-23** at WNDPROC.cpp's close. **UNREACHABLE.**

The table holds six `{kind, message, handler}` records, the format
WNDPROC_RegisterMessageHandlers walks: WM_DESTROY, WM_ACTIVATEAPP, WM_NCHITTEST,
commands 40100 and 4001, and WM_USER, answered by WNDPROC_OnDestroy,
_OnActivateApp, _OnDefault, _RedirectToClose, _RedirectToUser and
_OnIgnoredMessage. Nothing hands it to a registrar: IDA has no reference to
it, and a scan of every byte offset of the image for a dword in
0x6FB747C4..0x6FB7480F finds none -- the same scan finds the game window's
table, 0x6FB75818, at 5 sites -- and the six functions' addresses occur only
in the table itself. The table is `.data`, not exported, so no other module
reaches it either. WM_ACTIVATEAPP is handled by the tables that ARE
registered -- Game.cpp's three, through GAME_WindowOnActivateApp (Game.cpp:539);
where WM_DESTROY goes was not followed.

Reproduced -- the six are written as the bytes have them.

---

# NOT ESTABLISHED

## `TEXT_PlaceHoverMsg`'s give-up path never restores the font

`0x6FB1B4F0`, `text.cpp`. Batch 4; verdict at the unit's close, 2026-09-15.

Every other exit of the function ends `return D2Win_10127_SetFont(nPrevFont)`,
restoring whatever font the caller had. The exit taken when all 392 placement
probes fail ends instead

    TEXT_FreeParsedLines(pMsg->pLines);
    return FOG_Free(pMsg, text.cpp, 1674, 0);

-- the node is freed, no rectangle is recorded, and **font 13 is left set**.
Whoever draws next gets the chat font until something else sets one.

**What would settle it:** whether ordinary play can fill `gHoverMsgRect`
densely enough that all 392 probes miss. The count of live rectangles is
`sgTextState.nHoverMsgCount`, and the probe is a fixed 400 x 280 keep-out
box -- much larger than the bubble it places -- so the screen does not have to
be crowded in the ordinary sense. Count the rectangles reachable at 640x480
and at 800x600 and try the placement by hand; until then the mechanism is
certain and the arrival is not.

## `TEXT_DrawPopup` dereferences the `pLines` its own guard tested for null

`0x6FB1BE70`, `text.cpp`. Batch 4; verdict at the unit's close, 2026-09-15.

The function jumps to its tail block when the popup has no lines, and the tail
block reads through the pointer the jump was guarding:

    if ( !nScrollPixels || !pLines )   goto LABEL_48;
    ...
    LABEL_48:  pLastLine = pPopup->pLines;
               for ( i = pPopup->pLines->pNext; i; i = i->pNext )

`pNext` is at +4 of `D2ClientTextLineStrc`, so a null `pLines` faults at
address 4. It is a FAULT rather than undefined arithmetic, so the rewrite
reproduces it as written.

**What would settle it:** whether a popup can reach the draw with an empty
line list -- that is, whether `TEXT_ParseLinesAndScrollRate` can return a
record whose `pLines` is null, and whether any caller stores one. Both are in
this unit and both are written; the walk was not done because the batch's job
was the draw.

## The quick-message word wrap assigns its offset instead of advancing it

`0x6FB1CA73`, `text.cpp`, `TEXT_AddQuickMessage`. Batch 5; verdict at the
unit's close, 2026-09-15.

The wrapper shrinks a candidate slice until it fits `gnScreenWidth - 70`,
backs up to a line-break opportunity, copies it, and goes round again. The
going-round is wrong:

    6FB1CA73  mov  edi, esi        nOffset = nLen        (not nOffset += nLen)
    6FB1CA7D  sub  ecx, esi        nTotalLen - nLen
    6FB1CA7F  mov  esi, ecx        nLen = that

`nOffset` is ASSIGNED the line's length and the remainder is taken from that
same length rather than from what has been consumed. For the first line the
two coincide, because `nOffset` was 0, so a one- or two-line message is
correct. **From the third line on the wrap restarts at the second line's
length** and repeats text already shown. The message never runs past the end
only because `nOffset + nLen` still sums to `nTotalLen`.

All four sources agree -- IDA `nOffset = nLen`, Ghidra `iVar11 = iVar10`,
Binary Ninja `i_7 = i`, and the two instructions.

**What would settle it:** the longest text that reaches this function. The
node caps at six lines, and the wrap only reaches a third line for a message
wider than two screens' worth at font 13. The chat path caps a message at 305
characters; measure the wrapped line count for 305 characters at 640x480 and
at 800x600 and the verdict follows.

## The message log's eviction can leave `pHead` pointing at the node it freed

`0x6FB1C830`, `text.cpp`, `TEXT_PushMessageLogEntry`. Batch 5; verdict at the
unit's close, 2026-09-15.

At the cap of 0x80 entries the function evicts the oldest: it walks to the
tail keeping the PREDECESSOR, frees the tail, and unlinks it with
`if (pPrev) pPrev->pNext = nullptr`. **With exactly one node in the list the
walk never runs**, `pPrev` stays null, nothing clears
`MessageLogDisplayList.pHead`, and the very next statement re-reads that head
and links the new entry onto the node just freed.

**What would settle it:** whether `wCount` can reach 0x80 while the list holds
one node. That is an inconsistent state -- the count and the list disagree --
so the question is whether any path adds to the count without linking, or
frees a node without decrementing. Both halves are in this unit and both are
written.

## Chat message type 7 walks the text for a foreign script and tests nothing

`0x6FB1E60E`, `text.cpp`, `TEXT_HandleChatMessage`. Batch 8, 2026-09-15.

The function drops a message whose language is not this client's if the text
holds any character at or above 0x80, so a foreign script never arrives as
mojibake. **Message type 7 -- the recipe scroll -- repeats that walk and its
body is empty:**

    6FB1E617  add  eax, 2
    6FB1E61A  cmp  word ptr [eax], 0
    6FB1E61E  jnz  short loc_6FB1E617

There is no comparison against 0x80 and no exit: the loop runs to the NUL and
falls through to `NEWSTATS_RecipeScrollShow` regardless. So a recipe scroll in a
script the client cannot render is DISPLAYED, where a chat line in the same
script is dropped by the identical-looking walk at the top of the same
function. All three decompilers render it as a bare walk.

**What would settle it:** whether any server sends message type 7 with an
`nLang` other than the client's. D2MOO's senders are the place to look, and
its D2Client tree does not carry this function.

**SETTLED 2026-09-29 at newstats.cpp's close: UNREACHABLE from a stock
server.** D2MOO's D2Game sets nMessageType to 1, 2, 4, 5 and 6 and to nothing
else (PlrMsg.cpp, SUnitMsg.cpp); its 0x26 sender forwards the type it is
handed (D2Game SCmd.cpp:1050), and every caller hands it one of those five. The walk
stays a defect a server of another kind could reach ("NEWSTATS_RecipeScrollShow copies
the recipe text with no bound", below).

Real defects whose reachability was looked at and not settled. Each says what
would settle it.

## `UIPANELS_DrawUnitNameLabel`'s fault at address 0x0D

It needs `dwClassId` outside `[0, 704)`, which nothing in that function can make.
The two steps of the packet path that were measured do not stop it —
`SCMD_HandleSrvAC`'s own bounds check on that SIGNED 16-bit field is inside
`if (BITMANIP_Read(&pBuffer, 1))` at `0x6FAB4C37`, and
`PALSHIFT_EnsureMonsterPalette` null-checks.

**Not reachable in normal play; the open question is a malformed server.** What
would settle it: reading the rest of `SCMD_HandleSrvAC`'s field validation.

**SETTLED 2026-09-21, at CUnit.cpp's close: LATENT, and a malformed server does
not reach it either.** The validation that matters is not HandleSrvAC's but
its callee's: CUNIT_CreateMonsterUnit refuses a class whose MonStats or
MonStats2 lookup fails before it allocates the monster (0x6FB27695..
0x6FB27756), whatever the packet's own bound did, and MONSTER_ChangeMonsterClass
bounds a new class on entry (0x6FB4190B..0x6FB4194B). The argument is the
entry "`CUNIT_HitTestUnit` and `CUNIT_GetUnitName` read a monster's table row
with no NULL test", near the end of this file; the entry stays here so the
question can be followed to its answer.

## The Close tab's cursor value, on screen

See the REACHABLE entry. The stale value is established; what a cursor id of 8
looks like where 0 was intended is not, and needs `CURSOR_SetCursorBuySell`.

## `TEXT_HandleSystemMessage` has no arm for type 14

`0x6FAED1C0`, `inv.cpp`. `INV_TradeRestoreHeldItem` synthesises
`{ 0x5A, 14, 1, 0 }` on the stack and calls the system-message handler with it.
That function's jump table has 19 entries and **cases 12 and 14 both point at
`0x6FB1DD80`**, which IDA labels the default case and which is the function
epilogue — pop, pop, pop, pop, add esp, retn. So the call prints nothing and
changes nothing.

**Whether 14 was a message that was cut or one that never existed is not
established.** Reproduced, because the packet is built and the call happens.

## Both ANVIL close-button handlers break the handler chain TWICE on one path

`0x6FAEAF40` and `0x6FAEAE70`, `inv.cpp`. **Found 2026-09-11.**

    pMsg->bReturn = 1;  pMsg->lResult = 0;
    SMsgBreakHandlerChain(pMsg);        <-- unconditional
    if (outside the 32x32 close box) return 0;
    gbInvCloseBtnArmed = 1;
    pMsg->bReturn = 1;  pMsg->lResult = 0;
    SMsgBreakHandlerChain(pMsg);        <-- again, same path

The second call re-sets two fields that already hold those values. All three
decompilers print both calls.

**It is not a free no-op, and that is why it is here rather than in REFUTED.**
`SMsgBreakHandlerChain` is Storm ordinal 511, a five-byte thunk to
`SEvtBreakHandlerChain` at Storm `0x6FFC2B00`, which enters a critical section,
calls `SMemAlloc(12, "AuBreakCmd", -2, 8)` and **links a new node into a global
list**. Two calls allocate two nodes and insert two entries.

**Whether a duplicate node changes anything downstream is NOT ESTABLISHED** —
that needs Storm's drain side, which this repository has not read. What is
established is the cost: one allocation and one list insertion per press, twice.

Reproduced, both calls, in both handlers.

*Noted in passing, and out of this file's scope by title:*
`SEvtBreakHandlerChain` **dereferences NULL if `SMemAlloc` fails** — it sets
`v2 = nullptr` on that arm and then ends `v2[2] = a1`. That is Storm's defect,
not D2Client's.

## A `win2Unicode` whose output is never read

`0x6FAE3F65`, `inv.cpp` — in `INV_AppendEtherealAndSocketsText`. Found
2026-09-11 while reading ahead, and written out in inv.cpp batch 12 the same
day, where the dead call is reproduced with this reading beside it.

    6FAE3F58  jz   short loc_6FAE3F7F        not ethereal -> skip
    6FAE3F5C  mov  edx, offset aEthereal     "Ethereal"
    6FAE3F65  call win2Unicode               -> the local scratch `src`
    6FAE3F67  mov  ecx, 58D9h                22745
    6FAE3F6C  call D2LANG_GetStringFromTblIndex
    6FAE3F75  call strcat                    <- appends the TBL string, not src

The scratch is filled with the literal `"Ethereal"` and **the append takes the
tbl string 22745 instead**. `src` is not read again on this path: the only
later read is inside the sockets branch, which overwrites it with `", "` first,
and that branch is skipped when the item is not socketed.

So the widen call is dead work on every path — the shape of a line that once
appended the hardcoded literal and was changed to use the string table without
the scratch being removed.

**Corroborated independently.** D2MOO's `ITEMDESC_SocketsAndEthereality`, a
separate reconstruction of the same bytes, carries the same dead call:

    Unicode::win2Unicode(wszBuffer, "Ethereal", ARRAY_SIZE(wszBuffer) - 1);
    Unicode::strcat(pOutBuffer, D2LANG_GetStringFromTblIndex(STR_IDX_22745_...));

**LATENT by construction** — it costs one call per ethereal item hovered and
changes nothing. Listed here rather than under REFUTED because the work really
is dead; it is simply harmless.

## `automap.cpp` and `help.cpp` each carry one

Both are recorded in their own files as "REPRODUCED. Whether it is REACHABLE is
NOT settled here", with the reason in each case.

help.cpp's -- HELP_HelpScreenOnDismissKey does not claim ESC or SPACE -- was
taken as far as the module goes at that unit's close, 2026-09-24, and was
NOT ESTABLISHED until 2026-09-27, when Storm's dispatch order settled it: **LATENT**, and HELP_HelpButtonOnLButtonUp's release beside it
**REACHABLE** -- the two entries "HELP_HelpScreenOnDismissKey does not claim ESC
or SPACE" and "HELP_HelpButtonOnLButtonUp opens the help screen on a release
whose press began elsewhere", below.

## The equipment highlight's colour is a byte stored over a live register

`INV_UpdateAndDrawPanels`, four `GFX_DrawFilledRect` calls, `inv.cpp`. **Found
2026-09-12.**

`GFX_DrawFilledRect` takes `uint32_t dwColor`, and the seven stores that feed
its four call sites are byte stores into a register the function has not set
for this purpose:

    0x6FAE0380  over the edx GFX_DrawInventoryItem left
    0x6FAE047A  over the cursor state (8)          -- small, top bits 0
    0x6FAE0499  over the cursor state              -- small, top bits 0
    0x6FAE0590  over a zero (the flag test failed) -- top bits 0
    0x6FAE05BB  over the last tested result: an ammo or quiver type, a 0,
                or ITEM_IsItemLocked's BOOL
    0x6FAE070F  over `5 * gnHoverBodyLoc`, or the edx INV_CanInsertSocketFiller
                left
    0x6FAE0747  over the edx INV_CanInsertSocketFiller left

So the argument carries 24 bits of something else at three of the seven, and
those bits leave the module: `GFX_DrawFilledRect` hands the dword to
`D2GFX_DrawBox` (D2Gfx #10055), which forwards it to the renderer through
the interface table at `dword_6FA8D258 + 0xB8`.

**NOT ESTABLISHED**: what would settle it is that backend. D2Gfx has one
implementation per renderer and none of them is in this module. **The rewrite
writes the byte and nothing else** -- an uninitialised read is undefined in C
and the standing rule is to take the defined program and declare the
difference; `inv.cpp` says so where the code is.

## `NPCMENU_OnGambleClicked`'s fallback is a smaller teardown than `NPCMENU_OnTradeClicked`'s

`0x6FAF5D56..0x6FAF5E47` against `0x6FAF5BDB..0x6FAF5CBD`, `npcmenu.cpp`.
**Found 2026-09-13**, batch 4.

The two shop doors take the same fallback when the NPC's unit is gone or has
no inventory -- and it is not the same code. A normalised instruction diff
(callee-saved registers renamed) puts NPCMENU_NpcCloseInteraction in Trade's arm and
NPCMENU_NpcEndInteraction alone in Gamble's: Gamble's does not zero
`sgUIState_CurrentState`, does not call `INV_ResetItemDescState`, and turns
UI_NPCMENU off once instead of twice.

What that changes runs through `UI_UnregisterPanelHandlers`: UI_NPCMENU
turning off calls `NPCMENU_EndInteractionByUiState`, which switches on the
state. After Trade's arm it sees 0 and only closes the dialogs (unless a speech
is active). After Gamble's it sees whatever the state was; at 1 it runs its
own full teardown -- a SECOND client packet 0x30 among it -- and that is what
zeroes the state and resets the item description.

**NOT ESTABLISHED**: the arm needs the NPC's unit missing, or inventory-less,
while its main menu is open, and the state's value at that moment is not
measured. What would settle it is the reading of NPCMENU_BeginInteraction and
NPCMENU_OpenMainMenu (what state an open main menu leaves) and of when the
client drops an NPC's unit. Reproduced as written.

**Half of that is read, 2026-09-14** (batch 10a): an open main menu leaves the
state at 1. `NPCMENU_OpenMainMenu` stores it at 0x6FAF9920 and
`NPCMENU_BuildMainMenu` again at 0x6FAF68E1, in front of its rows, so a Gamble
click straight from a freshly built menu takes the full-teardown arm. Still
open: whether anything among the state's 82 references writes it between the
build and the click, and when the client drops an NPC's unit.

**NPCMENU_BeginInteraction is read as well, 2026-09-14** (batch 10b), and
answers neither half: its one store of the state is the interrupt opener's 1
(0x6FAF8793), ahead of the interrupt dialog and not between a main menu's build
and a click, and the only units it removes are a vendor's inventory items
(NPCMENU_NpcResyncVendorInventory, inlined in its not-found arm).

## The main menu's identify row and the identify price read different quest bits

`NPCMENU_BuildMainMenu` (0x6FAF65C0) against `NPCMENU_TransactionCost`
(0x6FAFB200), `npcmenu.cpp`. **Found 2026-09-14**, batch 10a.

The menu's identify row, tbl 4020, leaves the price off when quest 4 --
QUESTSTATEFLAG_A1Q4, Cain's rescue -- has QFLAG_REWARDGRANTED **or**
QFLAG_REWARDPENDING:

    6FAF6A59  push 0 / push 4 / push edx
    6FAF6A5D  call QUESTRECORD_GetQuestState / test eax, eax / jnz
    6FAF6A6B  push 1 / push 4 / push eax
    6FAF6A70  call QUESTRECORD_GetQuestState / test eax, eax / jnz

and the row reads 'Identify Items'; with neither bit it reads 'Identify
Items: ' and 100 per unidentified item. `NPCMENU_TransactionCost` prices
identifying at 0 when QFLAG_REWARDGRANTED is set and at 100 otherwise
(`push 0` as the state at 0x6FAFB318) -- it never asks for the pending bit. So
while only the pending bit is set, the menu row carries no figure and the
price line the belt's hint reaches during a Cain conversation (the REACHABLE
entry above) says 100.

**NOT ESTABLISHED.** In D2MOO's D2Game the pending bit is set when Cain's
gibbet is opened (`A1Q4.cpp:569`) and by two player iterators (`:348`,
`:467`), and Akara's message 118 moves it to granted
(`:1152..1158`) -- so the window exists on the server's side. What would
settle it: whether an identifying Cain can be talked to inside that window,
and which figure the server charges then, which is D2Game's code and not read
from here.

Reproduced as written: each function tests the bits it tests.

## The router reads a unit header off five packets that are one byte long

`0x6FAB50B0`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 1.

gSCmdTable gives ids **0x6E..0x72** an `nCmdSize` of 1 and a `pfProcessUnit`
(`GAME_NullFunc`). A non-null `pfProcessUnit` is the router's whole test, so
for each of them it resolves a target unit before queueing:

    dwUnitGUID = *(unsigned int *)&pBuffer[2];
    nUnitType  = (D2C_UnitTypes)pBuffer[1];

-- **five bytes past a one-byte packet** -- and then queues that byte on
whatever unit the stale GUID happens to find. The drain later calls
`GAME_NullFunc` on it, so the queued byte itself does nothing; the read and
the queue entry are the whole of the effect.

The sizes are not a misreading. The shipped D2Net's `dword_6FC08148` holds
**1** at each of 0x6E..0x72 (0x6D -> 10 and 0x73 -> 32 beside them as controls),
and D2MOO's `D2GSPacketSrv6E`..`Srv72` are each a lone `uint8_t nHeader`.

**What is not established: whether one is ever sent.** D2Net's server half does
build one -- `const uint8_t data[1] = { 0x6F };` in `sub_6FC020E0`
(D2Net 0x6FC020E0), handed to `FOG_10175(gpServer, data, 1, a2)` -- and that
function is registered as the seventh argument of
`FOG_InitializeServer(..., SERVER_ValidateClientPacket, sub_6FC020B0,
sub_6FC020E0, SERVER_ReadPacketFromBufferCallback)`. **What would settle it:**
what Fog calls that seventh callback for, which is Fog's code and was not read
here. Until then the mechanism is complete and the trigger is unread.

**2026-10-13: the trigger read.** FOG_InitializeServer (Fog 0x6FF54150) stores its seventh argument
at the server's +0xBC8 (`mov [esi+0BC8h], eax`, 0x6FF5432A, the argument read as `[esp+1C8h]` under three pushes at
0x6FF542F4). Seven sites in Fog read it, and each calls it as `cb(server, client id, __FILE__, 370)` just before the
client is looked up in the server's table and unlinked (0x6FF54CD5 in QServer_54C20, 0x6FF55041 in QServer_55020,
Fog #10162, #10163, #10165 and #10166, 0x6FF571BD in QServer_570B0; over every
`[reg+0BC8h]`). D2MOO's D2Net wraps #10162 as #10015 and #10165 as #10016; its D2Game calls them to drop a client --
SrvDisconnectClient ends `D2NET_10016(nClientId)` (D2Game Game.cpp:1272). So the server half sends **0x6F** to a
client it disconnects. On the client, D2Net queues it as a packet of one byte (D2MOO Client.cpp,
CLIENT_ReadPacketsFromStream: one record of exactly the packet's size, the game list below 0xAE), and
GAME_UpdateFrame drains that list every frame BEFORE its network-loss test (Game.cpp, the two drains and then
`D2NET_10017()`), into a 512-byte stack buffer never cleared: the router's bytes 1..5 are the previous packet's of
the frame, or the stack's. **Still not established**: whether the one byte reaches the client before the socket
closes (Fog's send against its close was not read), and whether a single-player game's local server runs the
callback at all. The row it then indexes is the subject of "The unit tables' row is taken unchecked".

Reproduced as written, with a MARKED comment in the router.

## Four variable-length handlers size their bit streams as a length byte minus a constant

`SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batches 8, 10 and 11.

Each takes the packet's own declared length and subtracts its header size with
no test, so a length byte below that constant wraps to nearly 4 GB and reaches
`FOG_BITMANIP_Initialize` as the buffer size:

    0x3E  6FAB39FE  sub eax, 2
    0xA8  6FAB47AF  sub eax, 8
    0xAA  6FAB499F  sub edx, 7
    0xAC  6FAB4BFA  sub eax, 0Dh

**What is not established:** whether D2Net delivers such a packet. All four ids
are variable-length, so `SERVER_GetServerPacketSize` takes their length from the
packet itself rather than from the table, and its per-id arms carry minimum
checks of their own (`case 0x16u: if (nBufferSize < 0xD) return 0;` is the
shape). **What would settle it:** reading the arm of that switch for each of
these four ids and seeing whether a length below the subtracted constant is
rejected before it reaches the client.

Reproduced as written, each with a MARKED comment.

## Packet 0x7B keeps a skill id equal to the record count, and passes its slot byte on unbounded

`0x6FAB41D0`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 9.

Two defects in the hotkey packet.

**The bound is `jle`.** At 0x6FAB41F6 the id is compared against
`sgptDataTables->nSkillsTxtRecordCount` and only an id ABOVE the count becomes
-1, so the id one PAST the last record is stored as it came and indexes
`pSkillsTxt` one element too far.

**The slot is not bounded at all.** `mov bl, [ecx+1]` becomes
`SPELLSEL_SetHotkeySlot`'s index into three arrays, each followed 64 bytes on by
another object: `gnSpellSelSkillIds` (0x6FBB9CF0) by `gnSpellSelOwnerGUIDs`
(0x6FBB9D30), that by `byte_6FBB9D70`, and `gnSpellSelHotkeyMode` (0x6FBB9858)
by `gSpellSelPanelX` (0x6FBB9898). A slot above 15 lands in the neighbour.

**What is not established:** whether D2Game sends either. Its senders are in
`D2Game/src/GAME/SCmd.cpp` and were read for the packet's LAYOUT, not for the
ranges it can emit. **What would settle it:** the server's own bounds on the
hotkey slot and on the skill id at the point it fills this packet.

Reproduced as written, both with MARKED comments.

## Packet 0xAC stores its modifier run without a bound

`0x6FAB4BD0`, `SCmd.cpp`. **Found 2026-09-14**, SCmd.cpp batch 11.

Nine bytes are cleared at `[esp+38h]` and the loop writes one byte per non-zero
modifier id with `inc esi` and no limit (0x6FAB4DB7..0x6FAB4DCC), so a run
longer than nine writes on into the frame -- the component bytes at +0x44, the
bit reader at +0x54, the descriptor at +0x68.

**What is not established:** whether a run longer than nine arrives. The ids come
from `monumod.txt` through D2Game, and how many modifiers that server will put
on one monster was not read here. **What would settle it:** D2Game's own cap on
the modifier count as it fills 0xAC.

**NOT reproduced, and the difference is declared.** Writing past an array is
undefined in C++ and the rewrite never writes UB to look faithful, so **ours
stores the first nine and keeps reading the rest** -- the same bits are
consumed and `CUNIT_CreateMonsterUnit` is handed the same nine bytes.
Identical for every run the buffer holds; different, and deliberately, past it.

## The experience bar zeroes an over-full bar instead of clamping it

`0x6FAFFCA0`, `panel.cpp`. **Found 2026-09-16**, panel.cpp batch 4.

    nBarWidth = 119 * nProgress / nLevelSpan;
    if (nBarWidth > 0x77)          // 0x77 IS 119 -- the full bar
        nBarWidth = 0;
    if (nLevel < GetMaxLevel(nClassId) && nBarWidth)   ->   two DrawLine runs

A progress equal to the span gives exactly 119 and draws a full bar; a
progress PAST the span gives more than 119 and draws **nothing**. The clamp a
reader expects would pin it at 119, and the bar would read full instead of
empty.

**The span is why the progress can pass it.** The upper bound is
`DATATBLS_GetLevelThreshold(class, the BASE level stat)` and the lower
`DATATBLS_GetLevelThreshold(class, the CURRENT level - 1)`; they describe the
same band only while `STAT_LEVEL` equals its base. Every comparison in the
function is unsigned, and the `nProgress >= 0x226B902` arm that shifts both
sides right by 7 is careful code, not the defect — it is exactly the point
where `119 * progress` would leave 32 bits.

**What would settle it:** whether the client can hold a `STAT_EXPERIENCE` past
`threshold(base level)` — the ordering of the server's experience and level
updates — or a `STAT_LEVEL` that differs from its base at all. Neither was
read here. The arithmetic is certain and the arrival is not.

Reproduced, the zero and the unsigned compares, in `panel.cpp`.

## `PANEL_GameOnMouseMove`'s left-click early-out is the one exit that writes no `bReturn`

`0x6FB01430`, `panel.cpp`. **Found 2026-09-16**, panel.cpp batch 6.

Three of the four exits write `pMsg->bReturn`; the fourth returns in silence:

    !gbInGame || IsDisconnecting || IsPlayerDeadOrMissing   ->   bReturn = 0
    GAME_GetLeftClickIssuedThisFrame()                    ->   return

so on that path whatever is already in the parameter block stands.

**Why that is not obviously harmless.** Storm presets `bReturn = 0` in
`SMsgDispatchMessage` and `SEvtDispatch` walks every registered handler,
discarding each return; only `SEvtBreakHandlerChain` stops the walk (the
measurement is in `automap.cpp`). So a handler that writes `bReturn = 1`
WITHOUT breaking the chain leaves its claim in the block for the handlers
after it — and `INV_NpcShopOnMouseMove` is exactly that handler: `bReturn = 1` at
two exits, and no chain break anywhere in its body.

**What would settle it:** the order Storm walks the WM_MOUSEMOVE list in —
whether its registration appends or prepends. `UI_GAME` registers
`gGameMessageHandlers` as the game starts and `UI_NPCSHOP` registers its table
when the shop opens, so an APPEND puts GAME first, the standing value is
Storm's own 0, and the defect is INERT; a PREPEND puts the shop's handler
first, its claim survives GAME's silent return, and `WNDPROC_WndProc` returns
`lResult` instead of calling `DefWindowProcA`.

Reproduced: the silent return is in `panel.cpp`.

## The character screen's highlight colour is a global nothing writes

`0x6FBB1A50`, `char.cpp`. **Found 2026-09-16**, char.cpp batch 2.
`CHAR_CharPanelDraw` takes the colour of the chance-to-hit highlight box
(`mov cl, byte ptr` at 0x6FAD057D) and of the defence tooltip (`mov edx,
... / and edx, 0FFh` at 0x6FAD080E) from `gnCharPanelHiliteColor`. **It is
past `.data`'s raw size** -- `ida_bytes.is_loaded` is False, the `ffffffff`
IDA shows is filler -- **and the module has no writer**: it has
two references, both these reads. The loader zero-fills it, so both are
drawn in palette index 0 in every stock 1.10 client.

**What would settle whether that is a defect**: a writer somewhere outside
D2Client, or a build of the game where the global has a value. It is the
same shape as the charge-up icon strip under UNREACHABLE, with the
difference that this code runs; whether the author meant a colour other than
0 cannot be told from inside the module.

Reproduced: both reads go through the original's global.

---

# REFUTED

Things that looked like bugs and are not. **These are the most useful entries in
the file**, because the appearance recurs and each one cost a reading.

## `mov cl, <k>` before a call is usually the ABI, not a partial store

**Found 2026-09-11 in `inv.cpp`, and it is the general rule the INERT entry above
is the exception to.**

    6FAEC675  call UI_SetUIToggle          -- leaves something in ecx
    6FAEC67F  mov cl, 4Fh               -- the low byte only
    6FAEC685  call MSG_SendCmd3Words

That is the identical shape. It is correct, because:

    int __fastcall MSG_SendCmd3Words(char nCmd, ...)
    6FAAD9E8  mov [esp+8+pPacket], cl   -- CL and nothing else

**A `char` argument in `__fastcall` lives in CL. The upper 24 bits of ECX are not
part of the argument, and writing only CL is what the convention asks for.** The
vendor-page instances are defects because THEIR callees take an `int` there.

**So the shape is never the finding — the callee's parameter width is.** This
module has dozens of innocent `mov cl,` sites.

**And this is where the two stock decompilers actively mislead.** With no
prototype, Ghidra prints
`FUN_6faad9e0(CONCAT31((int3)((uint)extraout_ECX >> 8), 0x4f), 0x12, 0, 0)` and
Binary Ninja prints a partially written `int16_t` — and drops the second argument
entirely, having nothing to say there is one. Hex-Rays prints
`MSG_SendCmd3Words(79, 18, 0, 0)` **only because this database gave it
`char nCmd`.**

## `UIPANELS_BuildUnitLabel` does not return an indeterminate value

Recorded as such until 2026-09-09. `Unicode::Personalize` (D2Lang `0x6FC11D50`)
has six exits and every one ends in a `strcpy`/`strcat` whose destination is its
own `wszOut`, with nothing touching eax before the `retn`.

**And it did not matter**: neither caller reads the result, so the real defect
was the DECLARED RETURN TYPE — `struct Unicode *` where the code has `void`.
With that corrected the "unassigned return" disappears, because Hex-Rays only
invented one to satisfy a type that had been declared.

**An unassigned `return` in the pseudocode is evidence about the DECLARED TYPE
first and about the code second.** The database had carried this as "Quirk left
as found" since 2026-08-24 and a rewrite batch repeated and escalated it before
anyone asked what the type was.

## The smith's shop bar is not missing its Close tab by accident

`0x6FAE9391`, `inv.cpp`. **Found 2026-09-11 while checking a different claim.**

`INV_NpcShopBuildTabs` was recorded as writing slot 3 "unconditionally" as the Close
tab, and `INV_NpcShopSetTabRepairAll` as being "overwritten by the Close tab". Both
are wrong: `6FAE938C call INV_NpcShopSetTabRepairAll` is followed by
`6FAE9391 jmp loc_6FAE924D`, and `0x6FAE924D` is `mov gnNpcShopTabCount, 4` — the
four Close stores occupy `0x6FAE922B..0x6FAE924C` and **the smith path skips them
whole**.

That is not a bug. A smith's bar really is **Buy / Sell / Repair / Repair-all**
with no Close tab, against a merchant's **Buy / Sell / (blank) / Close**, which
is what the game shows. Both database comments were corrected.

`gnNpcShopCloseTab = 3` is written on both paths and is not evidence either way:
its three readers use it as `gatButtonsOffsetX[4 * gnNpcShopCloseTab + iTab]`, a
ROW index into the button-offset table, and both bars are four tabs long.

## `INV_GetVendorPageInventory` cycling to a different page is a feature

`0x6FAE9087`, `inv.cpp`. If the requested page is empty it cycles forward **up
to four times**, wrapping at `gnVendorMaxPage` and again at 0xFF, until it finds
a page that has items. Asking for an empty tab lands you on the next occupied
one.

Written as a `do/while` with an explicit trip limit, and the limit is why an NPC
with nothing at all leaves you where you started. **Deliberate.**

## `WALL_AllocDrawNode` was recorded REACHABLE and is not

Downgraded 2026-09-08 — see the LATENT entry. Recorded here as well because the
correction is the interesting part: the claim rested on "anything that later
walks the array would run past the end", and nothing walks the array.

## Packets 0x9C and 0x9D copying a fixed 252 and 257 bytes is not an overrun

`0x6FAB4420` and `0x6FAB4590`, `SCmd.cpp`. **Looked at 2026-09-14**, SCmd.cpp
batch 10, and it is not a bug.

Both item-message handlers copy the packet into a stack buffer with a FIXED
count -- `mov ecx, 3Fh / rep movsd` is 252 bytes for 0x9C, and
`mov ecx, 40h / rep movsd / movsb` is 257 for 0x9D -- whatever the length byte
at +2 says, and D2Net sizes both packets by exactly that byte
(D2Net 0x6FC01B60). A shorter packet's copy therefore takes the rest of its
bytes from the router's buffer, which reads like an overrun.

**It is defined here.** `Game.cpp` hands the router a 512-byte buffer and
D2Net's dequeue copies ONE packet into it per call (measured, SCmd.cpp batch 1),
so the packet starts at the buffer's head and 257 bytes stay inside it. The
copy takes uninitialised stack-buffer bytes, not memory outside the object, and
the handlers read only the fields the packet's own length covers.

The shape that would make it real -- a buffer holding two packets with a 0x9D
starting past offset 255 -- is what D2Net's one-packet-per-dequeue contract
rules out. Recorded here because the fixed count next to a variable length is
exactly what a reader will flag again.

## `SPELLSEL_SkillDescGetSkillQuantity_Always0` makes every tome and scroll read `Quantity : 0`

`0x6FB19B60`, `spellsel.cpp`. **Argued 2026-09-19** at spellsel.cpp's close.
**REACHABLE, and on screen in a stock game.**

The function walks the unit's skill list for a skill with this id and a quantity
above zero, leaves the walk when it finds one, and returns 0 anyway: every exit
passes through `xor eax, eax` at 0x6FB19BA1, and the skill it found is never
read. Hex-Rays, Ghidra and Binary Ninja agree. The intent reads like
`return SKILLS_GetQuantity(pSkill);`.

**What reaches the screen.** Its two call sites are `SKILLDESC_BuildDescLine`'s
desclines 0x35 and 0x36..0x38 (SkillDesc.cpp:4863 and :4870). 0x35 formats the
value into tbl 4184 `Quantity : %d` and draws it framed at
`gnScreenWidth - 128, gnScreenHeight - 52`; 0x36..0x38 hand it to
`SKILLDESC_DrawQuantityHint`, which formats the same string the same way.

**Those four desclines are used, and only by the four skills a player carries
every game.** Measured over the stock `skilldesc.txt`, counting every
`descline1..6` cell: descline function 53 (0x35) is used by exactly one record,
`scroll of identify`; 54 by `scroll of townportal`; 55 by `book of identify`;
56 by `book of townportal`. So the quantity line of every identify and town
portal scroll and tome reads `Quantity : 0` whatever the real count is.

Reproduced and marked in `spellsel.cpp`; the name carries the verdict on
purpose.

## `SPELLSEL_ControlsDrawBoundKeyLabel`'s 16-element copy is one key name short of losing its terminator

`0x6FB189E0`, `spellsel.cpp`. **Argued 2026-09-19** at spellsel.cpp's close.
**LATENT, with a margin of three characters.**

For a key name wider than 40 pixels the function copies it into a 16-element
static buffer with `Unicode::strncpy(gwszBoundKeyLabel, pwszKeyName, 16)` and
then walks back from `Unicode::strlen` of that buffer, blanking one element at
a time until the text fits. D2Lang's `strncpy` (0x6FC11460) is the CRT's: the
copy loop leaves on the count and the pad after it is zero length, so a source
of sixteen characters or more fills all sixteen elements and writes NO
terminator. `gwszBoundKeyLabel` is `Unicode[16]` at 0x6FBB9CC8 and
`gnSpellSelHotkeySlot` is at 0x6FBB9CE8 -- 32 bytes later, immediately after
it -- so the `strlen` would read into that global and the walk back would write
its first terminator inside it.

**No key name that can reach this function is sixteen characters.** The label's
name comes from `CMDTBL_ControlsGetBoundKeyName` (CmdTbl.cpp 0x6FAD56B0), whose
switch answers the SHORTHAND tbl strings for VK 8, 16, 17, 27, 33, 34, 44, 45,
46, 96..109, 111, 144, 145 and 256..260, and falls through to
`CMDTBL_ControlsGetBoundKeyNameFallback` and the long names for every other code.
Measured over the stock `string.tbl` family (indices 3762..3914): the longest shorthand name is three
characters, and the longest long name is `'Mouse Wheel Down'` at exactly
sixteen -- **which is VK 260, a code the shorthand switch intercepts**, so it
is drawn as `'mwd'` and never reaches the buffer. The longest name that does
fall through is `'Right Windows'` (VK 92, tbl 3787) at thirteen characters, and
`strncpy` then leaves three elements and a terminator.

**What would cross it**: a table whose fall-through key name reaches sixteen
characters, or a build that drops `case 260`. BFE's own tables carry the same
strings at these indices and do not cross it (measured the same day).

## `SPELLSEL_DrawPanelSkills` never tests its count against the arrays' sixty entries

`0x6FB19720`, `spellsel.cpp`. **Argued 2026-09-19** at spellsel.cpp's close.
**LATENT, with a measured margin of six entries.**

The registering pass stores into `gSpellSelPanelX/Y/SkillIds/OwnerGUIDs
[gnSpellSelPanelCount]` and increments the count, which is a BYTE, with no
comparison against the arrays' sixty entries anywhere in the function.

**What an overrun would hit is inside the same object, at first.** The four
arrays are contiguous: `gSpellSelPanelX` `int[60]` at 0x6FBB9898,
`gSpellSelPanelY` at 0x6FBB9988, `gSpellSelPanelSkillIds` at 0x6FBB9A78,
`gSpellSelPanelOwnerGUIDs` `D2UnitGUID[60]` at 0x6FBB9B68 -- 240 bytes each --
and `gnSpellSelPanelCount` at 0x6FBB9C58. So `X[60]` is `Y[0]`, `Y[60]` is
`SkillIds[0]`, `SkillIds[60]` is `OwnerGUIDs[0]`, and `OwnerGUIDs[60]` is the
COUNT itself, which `SKILLS_GetSkillInfo` would then write and the next
`nCount = *pnCount` would read back as an index. The 965 bytes
`SPELLSEL_DrawBothPanels` and `SPELLSEL_Init` clear (`0x3C5`) are exactly those
four arrays plus the count plus the four colour bytes after it.

**The stock data cannot reach sixty.** Only ONE panel is drawn per
`SPELLSEL_DrawBothPanels` call -- `gnMiniSkillMode` picks the side -- and the
count is zeroed before the registering pass, so the maximum is what one pass
places. Measured over the stock tables:

* rows 0..3 take non-item skills whose `skilldesc.txt` ListRow is the row.
  Walking each class's thirty skills plus the class-less ones, with the
  `passive` filter and the ListPool dedup applied, the largest class total is
  the Paladin's **35** (ama 29, ass 33, bar 25, dru 34, nec 32, pal 35,
  sor 31).
* row 4 takes item skills. Taking the richest `charged` unique per body slot,
  two rings counted twice, the stock ceiling is **15** (Spellsteel 4, Wisp 3 +
  Nature's Peace 1, Marrowwalk 2, Metalgrid 2, and one each for belt, head and
  torso); one set item carries a single charged prop and no runeword carries
  any.
* row 4 also takes the tomes and scrolls, of which `skilldesc.txt` has **4**
  records -- and ListPool 1 and 2, the only two non-zero pools in the whole
  table, collapse the two scroll kinds to one each.

35 + 15 + 4 = **54 of 60**. A dual-wielding Barbarian trades four more charged
skills for ten fewer tree skills and comes out lower still.

**What would cross it**: a mod adding skills, or charged items, past that
margin -- which is why the count is worth a bound even though the stock game
cannot reach one. The ListPool dedup is no protection: stock `skilldesc.txt`
has 219 of 221 records at pool 0.

## `SPELLSEL_BindHoveredSkillToSlot` hit-tests the empty panel records too

`0x6FB1A020`, `spellsel.cpp`. **Argued 2026-09-19** at spellsel.cpp's close.
**REACHABLE, at exactly one cursor row.**

The bind loop walks all sixty records, not the `gnSpellSelPanelCount` that were
filled, and nothing distinguishes a filled record from an empty one.
`SPELLSEL_DrawBothPanels` clears X, Y and the skill ids to 0 and the owners to
-1 every frame the popup is drawn, so an empty record's box is
`x in [0, 48], y in [-48, 0]`. The cursor's Y is never negative, so the box is
reachable at **y == 0 exactly**, with x at most 48: the screen's top-left
corner. Every empty record then hits, and the slot ends up holding skill id 0
with owner -1 -- Attack -- with command 81 sent for it. The loop does not break
on a hit, so one press sends one command per empty record and the last wins.

Reproduced and marked. The effect is a misbound hotkey, not a fault; the
verdict is REACHABLE because nothing but the cursor's position prevents it, and
a filled record can legitimately sit at a low x (the right panel starts at
`gnScreenWidth - 128` and steps 48 LEFT per icon, so a long row walks down to
x 0), which is why the empty ones cannot be told apart by position alone.

## `SPELLSEL_UnbindEmptyScrollHotkeys` runs on the wrong event, and is gated on the panel it consults

`0x6FB19EF0`, `spellsel.cpp`. **Argued 2026-09-19** at spellsel.cpp's close.
**REACHABLE; an exhausted tome can keep its hotkey.**

The function clears a hotkey slot only when the slot's skill id is ALSO one of
the sixty ids the skill-select popup registered the last time it was drawn, and
only then tests the quantity. Two measured facts make that gate close on its
own target:

* **It is called on a refill, not on an exhaustion.** Its one call site is
  `inv.cpp:7172`, on the path that drops a SCROLL into a matching TOME
  (command 41) -- the moment a tome's count goes UP. Nothing calls it when the
  last scroll is spent.
* **An exhausted tome is not registered on the panel.** A tome reaches row 4
  only through `SPELLSEL_DrawPanelSkills`' promotion, which requires
  `SKILLS_GetQuantity(pSkill) > 0` before it sets the item-skill flag; its
  skilldesc ListRow is 4, so rows 0..3 never take it either. So once the popup
  is drawn again after the tome empties, the memset at the head of
  `SPELLSEL_DrawBothPanels` drops the id from the arrays and the cleanup can no
  longer match it.

So the slot is cleared only in the window between the tome emptying and the
next draw of the popup, and only if the player happens to stack a scroll into
some tome inside that window. Outside it the binding survives with a quantity
of zero. Reproduced as the bytes have it; only the skill id is compared, never
the owner.

## `SPELLSEL_UiSpinnerOnMouseDown`'s down arrow has a dead bottom row where the up arrow does not

`0x6FB1A580`, `spellsel.cpp`. **Argued 2026-09-19** at spellsel.cpp's close.
**REACHABLE, one pixel, cosmetic.**

The two hit tests in the same function do not agree on their bottom edge:

    up    nMouseY > nUpY - nCelHeight   && nMouseY <= nUpY     (`jg`  at 0x6FB1A5BE)
    down  nMouseY > nDownY - nCelHeight && nMouseY <  nDownY   (`jge` at 0x6FB1A602)

so the up arrow's box keeps its bottom row and the down arrow's box is one
pixel shorter. A press landing on that row falls through both tests and the
handler answers 0, having already armed `bMouseDown` -- which it does before
either test, hit or miss.

Recorded as a defect because the asymmetry is in one function over two arms
that are otherwise written the same, which is what makes it a slip rather than
a layout decision; the consequence is one unresponsive pixel row on the down
arrow. Reproduced as the bytes have it.

## `CONFIG_ScrollList` divides by a scroll range that is zero for a short list

`0x6FAFC4C0`, `config.cpp`. **Argued 2026-09-19** at config.cpp's close.
**UNREACHABLE in a stock client, and reproduced.**

The function recomputes the last first row itself -- the row count less up to
fifteen, floored at zero -- and then divides by it:

    6FAFC57D  cdq
    6FAFC57E  idiv  esi        ; esi = that row, 0 when the list fits a page

So a controls list of fifteen rows or fewer faults with a divide error on the
first call, before anything is drawn.

**Nothing in a stock client can produce such a list.** The row count's only
writer is `CONFIG_SelectControlsList`, which stores 62 (classic) or 51
(expansion), and every path into `ScrollList` runs `SelectControlsList` first:
Init, the enter/leave hook and the panel draw all call it at their head, and
the handlers only run while the panel is up. Both of those counts are far
above fifteen, so esi is 47 or 36 and never 0.

Reproduced as the bytes have it -- the same `idiv`, and a fault either way --
because the defined program faults here too; there is no undefined read to
avoid. What would make it real is a mod whose controls list is short.

## The Configure Controls arrow keys walk the row list without a bound

`0x6FAFD470` and its callers, `config.cpp`. **Argued 2026-09-19** at
config.cpp's close. **LATENT, and the third instance of this shape in the
rewrite** -- the ESC menu's two arrow keys are the other two.

Skipping separators is written as a walk with no counter:

    do
        CONFIG_MoveSelection(-1);
    while (gpCfgControlsList[gnCfgSelectedRow].eCmd == HOTKEY_MAX_COUNT);

A list whose every row is a separator -- `eCmd == HOTKEY_MAX_COUNT`, 57 --
spins for ever, because nothing counts the rows already passed and
`MoveSelection` clamps at the ends rather than wrapping.

**The stock lists cannot do it.** Both compiled lists carry real rows at both
ends and their six separators inside: classic at 6, 19, 25, 32, 38, 47 and
expansion at 7, 28, 35, 42, 49, 58, read out of the image. So every walk meets
a bindable row within six steps.

Reproduced. The neighbouring claim that `MoveSelection` cannot reach the last
row was checked at this close and is false -- from `count - 2` the candidate
`count - 1` passes the bound test at 0x6FAFD4D4 -- so the walk's termination
does not depend on an off-by-one that is not there.

## `DLIGHTMAP_AddLight` tests its owner for NULL three times, then hands the NULL to D2Common

`0x6FAA4280`, `dLightMap.cpp`. **Argued 2026-09-20** at dLightMap.cpp's
close. **LATENT: every caller passes the unit it is working on.**

With `pUnit` NULL the body stores dwUnitType 6 (UNIT_TYPES_COUNT), dwUnitId
-1 and bUnitIsAsync 0 -- `test edi, edi` at 0x6FAA42C8, 0x6FAA42CE and
0x6FAA42DC -- and then pushes the same `edi` into `UNITS_GetPrecisionX`
(0x6FAA42FF) and `UNITS_GetPrecisionY` (0x6FAA430B) with no test. The stock
D2Common's `UNITS_GetPrecisionX` (0x6FDBD780, ordinal 10327) opens with
`test ecx, ecx` / `jnz` and asserts `ptUnit` at its line 1726 (`push 6BEh`,
0x6FDBD788) before `exit(-1)`. So the no-owner arms describe a light the
function cannot finish creating -- and the no-owner handling downstream
(DLIGHTMAP_Update's skip, BuildStaticMap's return, the room callback's
`hSrcUnit` assert) serves lights that cannot exist.

**No caller reaches it.** The ten call sites, in nine functions, all load
`ecx` from the register holding the unit that caller is working on, and
eight dereference or test it first: MISSILE_CltDoFunc33 (`[edi+64h]`,
0x6FB3ACA4), MONUNIQUE_ModLight twice (`[esi+20h]`, 0x6FB44D9C),
MISSILE_ClientSpawn (0x6FB3EA0C), MONSTER_UpdateClientLight (0x6FB40C30),
OBJECT_SetLight (0x6FB46939), PLAYER_InitClientUnit (0x6FB488D9) and
SKILLS_CltDoFunc30 (`jz` at 0x6FAC80E1 on MISSILE_ClientSpawn's
result). SKILLS_RunCltStFunc passes the caster it has already handed to two
overlay calls, and GFX_AddOverlay its own unit argument.

Reproduced -- the NULL goes to D2Common in ours too, and the assert is
D2Common's. What would make it real: a caller with no unit, which the
no-owner arms look written for.

## `DLIGHTMAP_RebuildAmbientLight` ORs an unmasked intensity into the packed cell colour

`0x6FAA46B0`, `dLightMap.cpp`. **Argued 2026-09-20** at dLightMap.cpp's
close. **LATENT: no intensity outside 0..255 arrives with a colour.**

Both fills build the cell's four bytes as one dword -- `blue << 24 | green
<< 16 | red << 8`, then `or ecx, eax` (0x6FAA47B0) and `or ebx, eax`
(0x6FAA48E2) with eax the intensity exactly as the colour chain returned
it -- and split them back with `shr` 8, 16 and 24. Only the DUNGEON arm
masks (`and eax, 0FFh` at 0x6FAA4750 and 0x6FAA4881);
QUESTSFX_GetQuestLightOverride's and ENVIRONMENT_GetIntensityFromAct's full
32-bit returns go in as they are, so any bit above bit 7 would land in red,
green or blue.

**What prevents it is the range of those returns while the colour is not
black** -- a black colour sends the chain on and the value is replaced:

* QUESTSFX_GetQuestLightOverride (0x6FAB9FE0) answers 160 for the Diablo
  tint, and 80 or `cos(phase) * 80` for the Den of Evil, with the phase
  `frame * 128 / 30` over frames 0..29 -- under 128 of FOG_10083_Cos_LUT's
  512 a circle, so the cosine stays in [0, 1]. Otherwise it answers
  gnQuestFxIntensity with gnQuestFxLightRed/Green/Blue, which
  QUESTSFX_TickScreenFade writes as `cos(phase) * roomIntensity` with the
  phase in 0..128, or 0; the fade's end clears the whole block with `rep
  stosd` (0x6FABA3AC). At load the block is zero: `ida_bytes.is_loaded` is
  false for 0x6FBAF948..0x6FBAF952, so the `FFFFFFFF` IDA shows there is
  filler, not the -1 the database comment claims.
  roomIntensity itself comes from the DUNGEON arm (masked) or the
  environment.
* ENVIRONMENT_GetIntensityFromAct returns the environment's nIntensity,
  which D2MOO's ENVIRONMENT_UpdateLightIntensity keeps in [0, 255] on every
  arm: act IV's targets 16..128, the eclipse floor of 32, Rocky Summit's
  200, and otherwise a clamp to 255, or 170 in act V.

Reproduced as the bytes compute it -- dLightMap.cpp packs the same dword.
What would make it real: a mod whose environment or quest light leaves
0..255, which would tint every cell of the grid.

## `DLIGHTMAP_AddLightToGrid` divides its colour blend by the saturated total, so overlapping lights wash out

`0x6FAA51D0`, `dLightMap.cpp`. **Argued 2026-09-20** at dLightMap.cpp's
close. **REACHABLE, cosmetic: whenever a cell's light saturates.**

Each channel is `(gnReciprocal16[T] * (nIntensity * new + nOld * old)) >>
16`, with T the SATURATED total `min(nIntensity + nOld, 255)`
(0x6FAA5227..0x6FAA5231, stored to the argument slot at 0x6FAA5240, the
index at 0x6FAA524F), while the weights are the unsaturated nIntensity
(ebx, loaded at 0x6FAA520C before that store) and nOld (edi). Below 255 that
is the intensity-weighted average the database comment calls it. Above 255
every channel is scaled by `(nIntensity + nOld) / 255`, up to twice, then
clamped to 255 on its own (0x6FAA526F, 0x6FAA5298, 0x6FAA52C2) -- and
scaling all three before clamping each moves the colour toward white.

**Ordinary play reaches it.** All ten DLIGHTMAP_AddLight call sites pass
intensity 255 (`push 0FFh` at each), so a light's centre contributes about
255, and DLIGHTMAP_RebuildAmbientLight fills every cell with its room's
intensity first; any lit cell near a light saturates. The index has to stay
inside gnReciprocal16's 256 entries, which is why it is the saturated value;
nothing scaled the weights to match. Whether the washed-out overlap was
wanted cannot be settled from the bytes; the arithmetic is recorded because
the comment above it described an average. Reproduced exactly.

## `DLIGHTMAP_AddLightToGrid`'s monochrome arm: `gbColoredLights` is 1 and nothing writes it

`0x6FAA52E6`, `dLightMap.cpp`. **Argued 2026-09-20** at dLightMap.cpp's
close. **UNREACHABLE in a stock client.**

The arm that stores the intensity alone and zeroes the three colour bytes
runs when `gbColoredLights` (0x6FB74C88) is zero (`jz` at 0x6FAA5244). The
dword is file-backed -- `ida_bytes.is_loaded` true -- and holds 1, and
the module has two references to it, both reading the
value: this function (0x6FAA5236) and GFX_GetLightAtTile (0x6FAA60FD),
whose colour copy it gates the same way. Neither takes its address, so no
pointer to it leaves the module either. Reproduced whole; like the charge-up
icon strip, a switch left on rather than a defect that can be proved one.
## `SHOWITEMS_FitLabelRect`'s left clamp moves the label to x = 6, and the caller then rejects it

`0x6FB09DAF`, `showitems.cpp`. **Argued 2026-09-20** at showitems.cpp's close.
**REACHABLE, and visible in a stock game whenever the LEFT panel is open.**

Three of the four clamps put the edge where the bound is. The fourth does not:

    if (*pnLeft < nMinLeft) { *pnRight += nMinLeft - *pnLeft; *pnLeft = 6; }

`mov dword ptr [ecx], 6` -- the literal six, where the shift applied to the
opposite edge was `nMinLeft - *pnLeft`. IDA, Ghidra and Binary Ninja all read
the 6 (`*arg1 = 6` at 0x6fb09daf in BN's HLIL). The top clamp's own six is
correct, because its bound IS six; the left clamp borrowed the constant from
it and kept it after the bound became a parameter.

**Under `PANEL_LEFT` that drops the label.** `SCROLL_GetUiShiftScreen() == 2`
sets `nMinLeft = gnScreenWidth / 2` (0x6FB0A0FA), and the caller's acceptance
test at 0x6FB0A1E8 is `nLeft >= nMinLeft`. A label whose centre passed the
`nLabelX >= nMinLeft` gate can still have `nLeft = nLabelX - (nWidth >> 1)`
left of the boundary -- at 640x480, a hundred-pixel label on an item at x=340
starts at 290 against a bound of 320. The clamp fires, stores 6, and 6 is not
>= 320, so the record is discarded and the item shows no name at all. The
whole point of the clamp was to keep such a label on screen.

**Under `nMinLeft == 0` it survives, six pixels out.** The clamp fires for
`nLeft < 0`, shifts the right edge by `-nLeft`, and leaves the left at 6 where
the shift assumed 0 -- so the rectangle is six pixels narrower than the text
it is supposed to cover, and the collision box under-reports the label's right
edge by six.

Reproduced as the bytes have it. The database comment read this clamp as
"left to nMinLeft"; corrected at this close.

## `SHOWITEMS_FitLabelRect` can only nudge a label up and to the right

`0x6FB09EAB` and `0x6FB09F10`, `showitems.cpp`. **Argued 2026-09-20** at
showitems.cpp's close. **The `down` arm is UNREACHABLE; the `left` arm needs an
inverted rectangle.**

Each axis picks its direction from a two-part guard:

    vertical:    (*pnBottom - nTop) / 2 != 0  &&  nTop  <= nMaxBottom   -> up
    horizontal:  (*pnRight - nLeft)  / 2 != 0 &&  nLeft >= nMaxRight    -> left

The first half of each is the label's OWN height or width, halved
(`cdq / sub / sar 1`) and tested against zero; the obstacle appears in
neither. The second half compares the label's own edge against the bound the
clamps at the head of the same call have just enforced -- and enforced the
other way:

* the bottom clamp leaves `*pnBottom <= nMaxBottom` (0x6FB09DFE), and the
  caller builds the box as `nTop = nLabelY - textHeight + 4`, `nBottom =
  nLabelY`, so its height is `textHeight - 4`. For any font taller than five
  pixels both halves hold, `nTop < nBottom <= nMaxBottom`, and the `down` arm
  never runs;
* the right clamp leaves `*pnRight < nMaxRight` (0x6FB09DE6), so
  `nLeft >= nMaxRight` demands `nLeft > *pnRight` -- a rectangle whose left
  edge is right of its right edge. Only the left clamp above can produce one,
  and only by firing repeatedly on one label.

So a colliding label walks up and to the right and never down or left. The
consequence is ordinary and visible: labels pile toward the top of the play
area, and when twelve nudges are not enough the label is dropped (below).

Recorded rather than repaired: the guards are written out as the bytes have
them. The database comment read them as "whether the label is past the
midpoint of the obstacle"; corrected at this close.

## `SHOWITEMS_DrawGroundItemLabels` writes into the label record before it has decided to keep it

`0x6FB0A048` and `0x6FB0A1CB`, `showitems.cpp`. **Argued 2026-09-20** at
showitems.cpp's close. **INERT.**

`pLabel = &gShowItemLabels[gnNumShowItems]` is taken and `pLabel->pItem =
pUnit` stored at 0x6FB0A048 -- before `SCALE_CheckPerspectivePosition`
(0x6FB0A05E), before the on-screen bounds test (0x6FB0A11B) and before the
placement loop. The text buffer is filled the same way at 0x6FB0A157. Any of
those can reject the unit, and then the record keeps a unit pointer and a name
that belong to nothing.

Likewise the placement loop: twelve `FitLabelRect` calls and no fit
(0x6FB0A1CB) clears the placed flag, so the record is abandoned rather than
drawn where it last landed.

**Nothing reads it.** The draw pass and every other reader bound themselves by
`gnNumShowItems` (0x6FB0A3FC), which is incremented only on the path that
commits the record (0x6FB0A387); an abandoned record sits at index
`gnNumShowItems` and is overwritten by the next candidate. The list is rebuilt
from zero every frame (0x6FB09FD0), so nothing survives into the next one
either.

Reproduced -- the store is where the original has it.

## `SHOWITEMS_DrawGroundItemLabels`' quest override dereferences an items record it does not check

`0x6FB0A356`, `showitems.cpp`. **Argued 2026-09-20** at showitems.cpp's close.
**LATENT.**

    pItemsTxt = DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);
    if (pItemsTxt->nQuest && pItemsTxt->dwCode != 0x2067656C)

`mov cl, [eax+12Ah]` at 0x6FB0A35B, with nothing between the call and the load.
The callee returns NULL above the record count -- D2MOO's
`DataTbls/ItemsTbls.cpp:399` is `if (nItemId < ...RecordCount) { ...; return
&...[nItemId]; } return NULL;` -- and it does not bound a NEGATIVE id either,
so a negative `dwClassId` indexes below the table and returns a pointer into
whatever precedes it.

**A stock client cannot reach it.** `dwClassId` on an item unit is set from
the server's item packets through D2Common's own item constructors, which take
it from the same items table; an id outside the table would have failed before
the unit existed. The shape that would make it real is a mod whose client
items.txt is shorter than the server's, or a unit whose class id is corrupted
in transit.

Reproduced unguarded, as the bytes have it.

## The Den of Evil's kill count is clamped to 666 above five

`0x6FB06B36..0x6FB06B48`, `QuestLog.cpp` (Quest.cpp until 2026-10-08; `QUESTLOG_QuestOnQuestStatePacket`, the
`& 0x20` arm). **Argued 2026-09-20** at Quest.cpp's close. **REPRODUCED.**

    movsx   eax, word ptr [esi+4]      ; packet 0x5D field_4, the count
    cmp     eax, 5
    mov     gnMonstersRemaining, eax   ; 0x6FB06B3D -- stored FIRST
    jg      short 0x6FB06B48           ; > 5 -> overwrite
    test    eax, eax
    jge     short 0x6FB06B54           ; 0..5 -> the low half falls on
    0x6FB06B48: mov gnMonstersRemaining, 29Ah   ; and < 0 lands here too

0x29A is 666 and it is a literal. A count outside 0..5 is replaced by it.
Both decompilers keep the whole window, by different spellings: Hex-Rays
declares `nQuestArg` UNSIGNED and writes `nQuestArg < 6`, which covers the
negative half exactly, and Ghidra keeps the two tests apart as
`(v < 6) && (-1 < v)`. The bytes have them as two SIGNED jumps (`jg` at
0x6FB06B42, `jge` after the `test` at 0x6FB06B46), which is what the C
writes. CORRECTED 2026-09-20 at the re-check: the first version of this
entry said Hex-Rays "loses the negative half", which it does not. The `== 5`
sub-case (0x6FB06B54, `cmp eax, 5; jz 0x6FB06B7F`) is the quest-23 countdown
arm and is not part of the clamp. 666 also happens to be the tbl index of
`CainAct4IntroGossip1`; the store is a count field and the match is a
coincidence.

**The number is drawn.** QUESTLOG_BuildQuestDisplayInfo takes the row's
colour from `gQuestLogEntryFlags[nDefIndex]` and, when the colour is 3 or 4,
formats this same global with `sprintf "%d"` into the row whose title is
3714 -- the Den of Evil. That flag is written by this very function from the
same packet's `nState`, so the arm that asks for the number is the arm whose
count was clamped. Whenever the row is on screen with one of those two
colours and the count is above five, the log shows 666.

Which colour A1Q1's updates carry is not measured here; the clamp itself
fires on every count above five whatever the colour, and the display branch
switching on 3 and 4 exists for this row.

Reproduced -- the store lands where the bytes have it, 666 and all.

## The quest-id walk writes `gQuestLogEntryFlags[-1]` when the search fails

`0x6FB06B70`/`0x6FB06B77`, `QuestLog.cpp` (Quest.cpp until 2026-10-08). **Argued 2026-09-20** at Quest.cpp's
close. **LATENT.**

    or      ecx, 0FFFFFFFFh              ; 0x6FB06B70 -- index := -1
    mov     dl, [esi+3]                  ; the packet's nState
    mov     gQuestLogEntryFlags[ecx], dl ; 0x6FB06B77 -- stored anyway

The count arm searches `gQuestLogQuestDefs` for the record whose `nQuestId`
(+8, called `nDenseId` until the re-check of 2026-09-20 renamed it) is 1 (0x6FB06B60, `cmp dword ptr [eax], 1`) and, when all 41 records pass
without a hit, sets the index to -1 and stores through it. Base - 1 is
0x6FBB6015, one byte before `gQuestLogEntryFlags`, which IDA places at
0x6FBB6016. The value it would write is the packet's `nState`.

**What that byte IS, measured at QuestLog.cpp's close, 2026-09-21**: it is
`gQuestListPacketCopy`, and it holds the **header byte of server packet
0x52**. QUESTLOG_ApplyQuestListPacket copies 42 bytes from the packet's first
byte to 0x6FBB6015 (`mov ecx, 0Ah` / `rep movsd` / `movsw`, 0x6FB0623E), and
0x6FBB6015 + 42 = 0x6FBB603F -- one past the flags array's last byte. So the
0x52 header lands in that one-byte slot and **the 41 quest status bytes ARE
`gQuestLogEntryFlags`**: the packet's payload is stored in place and there is
no second copy of the quest list anywhere in the module. The verdict does not
change -- the store is still unreachable on the stock table -- but what it
would corrupt is the header of the last quest-list packet, not an unnamed
byte.

**Unreachable on the stock table**, measured at this close straight off the
image (41 rows x 16 bytes at 0x6FB7D928): row 1 carries `nQuestId == 1`, so
the search always stops there. A table edit that removes that row makes the
store live.

Reproduced -- the store is where the bytes have it.

## The quest record is tested for NULL and handed to the callee anyway

`0x6FB06B0E..0x6FB06B2E`, `QuestLog.cpp` (Quest.cpp until 2026-10-08). **Argued 2026-09-20** at Quest.cpp's
close. **LATENT.**

    mov     edi, eax                     ; edi = NPCMENU_QuestGetQuestRecord()
    test    edi, edi
    jz      short 0x6FB06B24             ; NULL -> straight to the 2nd call
    ...
    0x6FB06B24: push 1; push 1; push edi; call QUESTRECORD_GetQuestState

The C is `if ((!pQuestRecord || !GetQuestState(pQuestRecord, 1, 0)) &&
!GetQuestState(pQuestRecord, 1, 1))`. When the record is NULL the first half
short-circuits TRUE and the second call still runs, with the NULL the test
just found -- 0x6FB06B10 jumps directly to 0x6FB06B24 with edi = 0.
D2Common's QUESTRECORD_GetQuestState asserts its record and then
dereferences it (D2MOO `D2QuestRecord.cpp:9`), so the guard does not guard.

**The NULL is a state of the client, but not a reachable one here.**
NPCMENU_QuestGetQuestRecord returns `*gpNpcQuestRecord`, and NPCMENU_Shutdown
zeroes that cell -- 0x6FAF7CB2, after `xor esi, esi` at 0x6FAF7C96, right
after freeing both records. Its two callers are NPCMENU_Reset, which
re-allocates at once, and UI_ShutdownUiSubsystems (0x6FB20306).
SCMD_HandleSrv5D is the only path to this function, so the NULL window and
the packet pump do not overlap. A mod that leaves the cell NULL while 0x5D
still arrives would make it live and fatal at the callee's assert.

Reproduced -- the short-circuit is written as the bytes have it.

## The NULL packet test jumps into a label that dereferences it

`0x6FB06ACF`, `QuestLog.cpp` (Quest.cpp until 2026-10-08). **Argued 2026-09-20** at Quest.cpp's close.
**LATENT.**

    if (!pPacket)
        goto record_state;               ; the test the author wrote

`record_state`'s first acts read the packet: `xor edx, edx; mov dl, [esi+1]`
at 0x6FB06ACF takes `nQuestId` off ESI, and 0x6FB06BE0 does the same on the
label's other arm. With a NULL packet both read address 1. The test cannot
fire from the shipped caller: SCMD_HandleSrv5D (0x6FAB3E1B) copies the
packet to its own stack and passes that address, and it is the only caller.

Reproduced -- the test and the label are both where the bytes have them.

## `QUESTLOG_Draw`'s quest walk reads one record past `gQuestLogQuestDefs`

`0x6FB049EC`, `QuestLog.cpp`. **Argued 2026-09-21** at QuestLog.cpp's close.
**LATENT.**

    6FB049E8  mov eax, [esp+74h+pLines]
    6FB049EC  inc eax                      ; the increment is at the BOTTOM
    6FB049ED  cmp eax, 29h                 ; 41
    6FB049F4  jg  short 6FB049FB           ; -> bDone = 1
    ...
    6FB04A04  add esi, 10h                 ; and the cursor steps anyway

The walk over the 41 quest definitions ends on `++nLines > 41` and the
increment happens after the body, so pass 42 runs with the cursor at
`&gQuestLogQuestDefs[41]` and reads four of its fields -- `nAct`, `bHasIcon`,
`nIconIndex` and the icon image -- four bytes past a 41-record array. The
other terminator, `nEntryCount >= 6`, cuts the walk short only on a page that
fills up.

**What is past the array, read off the image**: 0x6FB7D928 + 41 x 16 =
0x6FB7DBB8, which is `gnQuestLogDoneYOffset` (`FF FF FF FF`),
`gnQuestLogSelectedSocketYOffset` (`FF FF FF FF`), `gnQuestLogTextX` (16) and
`gnQuestLogTitleY` (248). So the phantom record's `nAct` is the byte at
0x6FB7DBBB = **0xFF**, and the walk's FIRST test is `nAct ==
gnQuestLogActTab` with the tab clamped to 0..4 by QUESTLOG_SetActTab. It can
never match, so nothing downstream of the test ever runs on it.

**And the value cannot move**: 0x6FB7DBB8 and 0x6FB7DBBC
have ONE reference each, both reads and both in this same function, so
no writer exists anywhere in the module and the `-1` is the value at rest.
The read is of loaded, named memory -- `ida_bytes.is_loaded` is true there --
so it does not fault; it costs one comparison a frame.

Reproduced -- the loop is written with the increment where the bytes have it,
and the head of QuestLog.cpp carries the measurement.

## `QUESTLOG_BuildQuestDisplayInfo` hands the quest record to an asserting callee untested, and tests the OTHER one

`0x6FB04E0A`, `QuestLog.cpp`. **Argued 2026-09-21** at QuestLog.cpp's close.
**NOT ESTABLISHED.**

    6FB04DD6  call NPCMENU_QuestGetQuestRecord        ; ebx := the main record
    6FB04DDD  call NPCMENU_QuestGetQuestRecordSecondary
    ...
    6FB04E0A  call D2COMMON_QUESTRECORD_GetQuestState   ; ebx passed untested

`NPCMENU_QuestGetQuestRecord` (0x6FAFBD20) returns the global `gpNpcQuestRecord` and
nothing else, so it is NULL until something sets it; D2Common's
`QUESTRECORD_GetQuestState` opens with `D2_ASSERT(pQuestRecord)`
(D2MOO, `source/D2Common/src/D2QuestRecord.cpp:9`). The main record reaches it
three times in the first eight instructions of this function with no test.

**The same function tests the OTHER record.** At 0x6FB05439's arm the
condition includes `|| !pQuestRecord2 ||` before the secondary record is used,
and Quest.cpp's `QUEST_ShouldPlayAreaEntrySpiel` tests both
(`if (!NPCMENU_QuestGetQuestRecord()) return FALSE;`). So the original's own code
treats a NULL record as possible in two places and not in this one.

**Why NOT ESTABLISHED**: whether `gpNpcQuestRecord` can be NULL on a path
that reaches this function is a question about when the client sets it, and
this unit's three callers all run only with the panel up. **What would settle
it**: the writers of gpNpcQuestRecord, and the
order of the writer against `UIPANELS_DrawInGamePanels` in the app-mode sequence.

Reproduced -- the calls are untested because the bytes do not test them, and
the one test the bytes DO make is written out where they make it.

## `CUNIT_InitUnit` and `CUNIT_CreateMonsterUnit` guard ONE statement with the allocation's NULL test

`0x6FB27092` and `0x6FB2776F`, `CUnit.cpp`. **Argued 2026-09-21** at CUnit.cpp's
close. **UNREACHABLE.**

    6FB27092  cmp  esi, edi          ; InitUnit: pUnit against 0
    6FB27094  jz   short 6FB270B2    ; skips the dwFlagEx update only
    6FB270BE  mov  [esi+0Ch], ebx    ; pUnit->dwUnitId -- unconditional

    6FB2776F  cmp  esi, ebx          ; CreateMonsterUnit: the allocation
    6FB27771  jz   short 6FB2777D    ; skips the async flags only
    6FB27783  mov  dword ptr [esi], 1  ; unconditional

Both tests cover one read/modify/write and nothing after it, so a NULL would
be written through at address 0x0C and 0. **Nothing can hand either a NULL.**
The pointer is D2Common's `UNITS_AllocUnit` (0x6FDBD6B0), which puts
FOG_AllocPool's answer straight into `rep stosd` (0x6FDBD6DA) and then writes
`[esi+8]` and `[esi]` with no test between: a NULL faults inside D2Common
before AllocUnit can return it. And CUNIT_InitUnit's four callers --
CUNIT_CreateUnit, CUNIT_CreatePlayerUnit, CUNIT_CreateObjectUnit and
CUNIT_CreateClientOnlyUnit (`test eax, eax` at 0x6FB27DCA) -- all test the
allocation first anyway.

The same measurement disposes of the untested `MONSTER_GetMonUmodArray`
answer at 0x6FB2785D: for UNIT_MONSTER, AllocUnit allocates the 0x60-byte
monster data (0x6FDBD6FB) and stores it at +0x14 (0x6FDBD709), so the array is
always there.

Reproduced -- both guards are written where the bytes have them, and the head
of each function carries the measurement.

## `CUNIT_CreateMonsterUnit` uses `CUNIT_RegisterUnit`'s answer untested, and returns the allocated unit

`0x6FB27B92`, `CUnit.cpp`. **Argued 2026-09-21** at CUnit.cpp's close.
**LATENT.**

    6FB27B8B  call CUNIT_RegisterUnit
    6FB27B90  mov  esi, eax
    6FB27B92  mov  ecx, [esi+0Ch]      ; no test of eax

CUNIT_RegisterUnit answers NULL only after DESTROYING the new unit, so a NULL
here faults at address 0x0C -- and were it not to, everything after would run
on freed memory, since the rest of the function goes back to the ALLOCATED
pointer and returns it.

**When RegisterUnit gives up**, read off its body: CUNIT_AddUnitToTable
refuses a unit whose id and type are already in the table its dwFlags bit 21
selects; RegisterUnit then looks for the squatter in the BY-ID table
UNCONDITIONALLY, evicts it and retries -- so a unit filed in the by-id table
always gets in, and NULL comes back only for a unit filed in the CLIENT-ID
table whose id is taken there. The 0xAC path passes `bAsyncClientUnit` 0
(`SCmd.cpp`, SCMD_HandleSrvAC) and files by id. The asynchronous path is
CUNIT_CreateClientOnlyUnit (`push 1` at 0x6FB27D88), whose id is
gdwNextClientUnitId pre-incremented and wrapped before 0xFFFFFFFF
(0x6FB27D57..0x6FB27D6E): a collision needs some 4.29 billion client-only
ids handed out while the unit that first held the id is still alive.

Reproduced -- the load is untested and the allocated pointer is returned, as
the bytes do.

## `CUNIT_CreateMonsterUnit` indexes the superunique record without a test

`0x6FB27B51`, `CUnit.cpp`. **Argued 2026-09-21** at CUnit.cpp's close.
**LATENT.**

`DATATBLS_GetSuperUniquesTxtRecord(MONSTER_GetBossHcIdx(pMonster))` is
followed by `mov cl, [eax+ebx+28h]` with nothing between, and D2Common's getter
returns NULL outside `[0, nSuperUniquesTxtRecordCount)` (D2MOO,
`DataTbls/MonsterTbls.cpp:1547`). The arm runs only when the monster's type
flag 2 (superunique) is set.

**The id is the server's, and the client does not check it.** The only source
is packet 0xAC: SCMD_HandleSrvAC reads it as `FOG_BITMANIP_ReadSigned(&tBits,
16)`, only under type flag 2, and hands it on with no bound (`SCmd.cpp`). The
other creator, CUNIT_CreateClientOnlyUnit, passes 0 with the type flags 0,
so it never reaches the arm. A stock server sends the index of a row of its
own superuniques.txt, which is the client's; only a server whose table is
longer than the client's, or a malformed packet, faults here.

Reproduced -- the record is indexed untested.

## `CUNIT_HitTestUnit` and `CUNIT_GetUnitName` read a monster's table row with no NULL test -- and `UIPANELS_DrawUnitNameLabel`'s open question with them

`0x6FB28954` and `0x6FB299DB`, `CUnit.cpp`. **Argued 2026-09-21** at CUnit.cpp's
close. **LATENT.**

    6FB2894C  xor  eax, eax            ; HitTestUnit: the MonStats2 lookup's NULL
    6FB28954  mov  dl, [eax+4]         ; read at once: a fault at 4

    6FB299DB  xor  eax, eax            ; GetUnitName: the MonStats lookup's NULL
              mov  cx, [eax+6]         ; a fault at 6

Both are inlined record lookups whose NULL the caller does not test. They fire
only for a monster class outside monstats.txt, or one whose `wMonStatsEx` is
outside monstats2.txt. **Every class a client monster can have is checked
where it is written**:

* the monster is created by CUNIT_CreateMonsterUnit alone -- CUNIT_InitUnit
  asserts `eTOU != UNIT_MONSTER` (the original's CUnit.cpp:512), so packet 0x09 cannot make
  one -- and CreateMonsterUnit refuses a class whose MonStats or MonStats2
  lookup fails before it allocates anything (the inlined pair at
  0x6FB27695..0x6FB27756 returns NULL);
* the class is changed only by MONSTER_ChangeMonsterClass, which bounds the new
  class and its wMonStatsEx on entry (0x6FB4190B..0x6FB4194B) before it writes
  `dwClassId`.

Those are the two writers this close found; a writer outside D2Client was not
looked for. **HitTestUnit's class can also come from D2Common #11013**, which
for a shapeshifted unit substitutes states.txt `gfxclass` (D2MOO,
`D2Skills.cpp:2851`). In the stock tables the four gfxtype-1 states are wolf
(430), bear (431), monsterset (135) and delerium (212), and each is a
monstats.txt row with a monstats2.txt row (a scratch read of the three stock
tables). A mod whose transform state names a class without them reaches the
fault; the stock game does not.

**This settles the NOT ESTABLISHED entry "`UIPANELS_DrawUnitNameLabel`'s fault at
address 0x0D"**: the class that function reads is a monster's `dwClassId`, and
SCMD_HandleSrvAC's conditional bound does not matter, because
CreateMonsterUnit makes the same test unconditionally. See that entry.

Reproduced -- both reads are written untested.

## `CUNIT_GetUnitName` uses the items.txt record untested

`0x6FB299FF`, `CUnit.cpp`. **Argued 2026-09-21** at CUnit.cpp's close.
**LATENT.**

D2Common's `DATATBLS_GetItemsTxtRecord` bounds its index from ABOVE only
(D2MOO, `DataTbls/ItemsTbls.cpp:400`): at or past the count it returns NULL,
and the name read at +0xF4 faults. A negative class returns a pointer BEFORE
the table and reads whatever is there, without a fault.

Item units come from packets 0x9C and 0x9D (ITEM_CreateItemFromItemExMsg and
its twin, `Item.cpp`), whose class is the items.txt link index of the item's
code (D2MOO, `ItemsTbls.cpp:425`, `FOG_GetLinkIndex` -- -1 for a code the
table does not know), and from packet 0x09, which hands the class on raw
(`SCmd.cpp`, SCMD_HandleSrv09_SpawnUnit). A stock server names items from
the same items.txt; only a mismatched or malformed server reaches either
arm.

Reproduced -- the record is used untested.

## `CUNIT_TurnUnitToFacePlayer` does not test the player

`0x6FB29136`, `CUnit.cpp`. **Argued 2026-09-21** at CUnit.cpp's close.
**LATENT.**

gpPlayerUnit is loaded at 0x6FB29105 and its type read at 0x6FB29136 with no
test, where the twin CUNIT_TurnPlayerToFaceUnit returns on a NULL player
first. The one call site is NPCMENU_BeginInteraction (0x6FAF84BD), whose one
caller is the packet handler SCMD_HandleSrv28_NpcMenu (0x6FAB2B8E) -- and
BeginInteraction dereferences the player itself, untested, right after the
call (0x6FAF84C7). The player is NULL only before packet 0x59 assigns it and
after the game is left; an NPC menu packet in either window would fault in
the caller as well.

Reproduced -- the player is used untested.

## `CUNIT_IsUnitSelectableTarget` tests the TARGET's owner type on the SOURCE's side

`0x6FB29EAC`, `CUnit.cpp`. **Argued 2026-09-21** at CUnit.cpp's close.
**NOT ESTABLISHED** then; **REACHABLE** since the STATES_ family's close,
2026-09-28 (the paragraph at the end).

    6FB29E9A  mov  edx, [ebp+0C8h]    ; the SOURCE's dwFlagEx
    6FB29EA0  shr  edx, 0Ah           ; UNITFLAGEX_STOREOWNERINFO
    6FB29EAC  mov  eax, [esi+94h]     ; the TARGET's dwOwnerType -- esi is pTarget
    6FB29EB6  mov  eax, [ebp+98h]     ; the SOURCE's dwOwnerGUID

The target arm reads all three from the target. Hex-Rays, Ghidra and Binary
Ninja print the same instruction the same way.

**What was measured.** 14 call sites (a scratch lister of every code
reference with the loads before it). Two load the local player itself --
CUNIT_HitTestUnit (`mov edi, gpPlayerUnit`, 0x6FB2874A) and
UIPANELS_GetUnitLifeFill128 (CUNIT_GetPlayer, 0x6FB206AD); PLAYER_DispatchClickAction
passes the unit its click record carries at +4 (0x6FB49F41); the three
MISSILE_Is*Target tests pass the unit beside
`D2COMMON_MISSILE_CheckUnitIfOwner(pMissile, pUnit)` -- the missile's owner --
and MISSILE_ClientOnHit, MISSILE_CltDoFunc7 and the skill and state client
functions pass other units again. So a monster source is possible, and the
arm runs. Owner
info on a MONSTER comes from packet 0xAC with owner type 0, UNIT_PLAYER
(`SCmd.cpp`, `D2COMMON_UNITS_StoreOwnerTypeAndGUID(pMonster, 0, ...)`) and
from STATES_SetFunc4_SourceUnit (0x6FB4BE56), which takes the type from the state. Of the
other StoreOwner calls, the ones read here file an owner on a MISSILE
(MONSTER_LinkOwnedUnits' blade creeper arm, MISSILE_ClientSpawn, packet 0x73);
MONSTER_SpawnAttackMissile's and MONSTER_DoClientSplEndEffect's were not read.
With the source's owner type 0, reading the target's instead only
matters against a target whose owner type is non-zero, and then only the
player-picks-an-item-or-object branch comes out differently. With a non-zero
owner type from STATES_SetFunc4_SourceUnit, a target of owner type 0 -- most units --
re-points the source to `(UNIT_PLAYER, its owner's GUID)`, and against a
player target the roster test runs on a GUID that is not a player's.

**What would settle it**: the stock states whose `setfunc` is 4, and the
source type each stores. A NULL target would also fault here, but only with a
monster source outside town, and no call site was seen passing one.

**Settled 2026-09-28**. `setfunc` 4 is stock `sourceunit` (98) alone.
D2Common's UNITS_StoreOwnerInfo turns it on whenever an owner is stored, with
STAT_SOURCE_UNIT_TYPE the owner's own dwUnitType (D2MOO Units.cpp; D2Common
0x6FDBFF40); the row has no `nosend`, and the two stats carry send bits (5 and
32, ItemStatCost.txt), so the state reaches the client with them and
STATES_SetFunc4_SourceUnit stores that type. D2MOO's D2Game stores Baal as his
clone's owner and the clone as Baal's (AiBaal.cpp:1140..1141): owner type 1,
non-zero, on both -- the re-pointing arm above, with either of them as the
source.

Reproduced -- the read is the target's, and the C marks it.

## `CUNIT_ResyncUnitFromServer` guards `dwPrevPathType`, then stores it unconditionally

`0x6FB2A555`, `CUnit.cpp`. **Argued 2026-09-21** at CUnit.cpp's close.
**REACHABLE.**

The path copy keeps the client's `dwPathType` and `dwPrevPathType` when the
server's is a knockback type, 8 or 11 -- and the very next statement stores the
server's `dwPrevPathType` again with no test (0x6FB2A555), so the second guard
does nothing and the client always takes the server's value. The function
runs from CUNIT_UpdateAllUnits for every player and monster on every frame
of a single-player game (`gnGameType` tested at 0x6FB28325), so the store is
reached constantly. What a knockback type in the client's `dwPrevPathType`
changes downstream was not measured. The database comment described the dead
guard before the rewrite did.

Reproduced -- the guard and the store are both written.

## `MSG_SendPacketToServer` asserts its length with a SIGNED compare

`0x6FAAD864`, `Msg.cpp`. **Argued 2026-09-21** at Msg.cpp's close.
**LATENT.**

    6FAAD85B  cmp  ebx, 200h
    6FAAD864  jl   ...              ; signed -- a negative nLen passes

The assert ("nLen < (D2NET_MAX_MESSAGE_SIZE )", the original's Msg.cpp:531 by its `push
213h`) is the only bound before two copies of nLen bytes, into
gLastSentPacket and into the stack buffer D2Net is handed, so a negative
length would pass it and both copies would run over memory. What prevents
it: the function's eleven call sites are the eleven senders, each in tail
position; ten pass a constant (1, 3, 5, 7, 5, 9, 13, 17, 7 and 6 bytes) and
MSG_SendChatMessage computes at least 6 -- three header bytes, the two
terminators and the zero after them.

Reproduced -- the C compares an `int`.

## `MSG_SendChatMessage` tests `pMsg + 259` for NULL

`0x6FAADB85`, `Msg.cpp`. **Argued 2026-09-21** at Msg.cpp's close.
**UNREACHABLE.**

    6FAADB85  lea  eax, [esi+103h]
    6FAADB8B  test eax, eax         ; then jz -- skip the name, write one zero

The shape of `if (pMsg->szName)`, an array tested for truth, which the
original's compiler did not fold. The arm runs only for pMsg = 0xFFFFFEFD,
and both callers hand it a stack block: CHAT_HandleWhisperCommand's at -430h
and CHAT_HandleInputKey's at -218h. Hex-Rays prints it as a comparison
with `(const void *)-259`, Binary Ninja as `esi == 0xfffffefd`.

Reproduced on integers, so a modern compiler cannot fold the dead arm away.

## `MSG_Send_D2CLTSYS_NEWGAME` copies a 24-byte game name into a 16-byte field

`0x6FAAD273`, `Msg.cpp`. **Argued 2026-09-21** at Msg.cpp's close.
**NOT ESTABLISHED.**

    6FAAD263  push 7FFFFFFFh        ; no real bound
    6FAAD273  call STORM_SStrCopy   ; szJoinGameName -> szGameName[16] at +1

    6FAABC18  lea  eax, [ebx+1Ah]   ; GAME_ApplyParamJoinInfo: the launch
    6FAABC21  call esi ; lstrcpyA   ; config's szGameName[24] -> szJoinGameName

szJoinGameName is 24 bytes (D2Vars.h, `// [24]`) and filled with an
unbounded `lstrcpyA` from the launch config's own 24-byte field, so D2Client
admits a name of up to 23 characters. One of 16 to 23 runs on into nGameType
and the fields after it, which are all stored AFTER the copy and overwrite
it: nothing outside the 46-byte packet is written, and szGameName goes out
with no terminator. The server then refuses the game -- D2MOO's
GAME_VerifyCreateNewGame tests `CCMD_IsStringZeroTerminated(szGameName, 16)`,
and its "[HACKLIST]  <D2CLTSYS_CREATEGAME>  PLAYER:%s  ERROR: Invalid game
name" is in the stock D2Game.dll (file offset 0xFCD80).

**What would settle it**: whether D2Launch can hand D2Client a game name
longer than 15 characters. Nothing in D2Client bounds it.

Reproduced -- the copy is written with the same bound.

## `MSG_Send_D2CLTSYS_JOINGAME` calls through the Battle.net interface without testing it

`0x6FAAD4C6`, `Msg.cpp`. **Argued 2026-09-21** at Msg.cpp's close.
**NOT ESTABLISHED.**

    6FAAD4C1  call GAME_GetComInterface
    6FAAD4C6  call dword ptr [eax+5Ch]      ; GetVersion, untested

gpComInterface (0x6FBA7944) is written only when the launch config's
pComInterface is not NULL (`test eax, eax / jz`, 0x6FAABE86), so without an
interface it keeps the loader's zero and this call faults at 0x5C. Every
other reader tests it -- AUTOMAP_DrawVersionLabel skips its label, and
GAME_AppModeGame's LeaveGame call (Game.cpp) is guarded -- and
MSG_Send_D2CLTSYS_ENDGAME tests WINMAIN_GetBnClientInterface's answer before
its call. The packet goes out for BNET, OBNET_JOIN and LAN_JOIN (GAME_AppModeGame,
Game.cpp). The two Battle.net types run on the interface; for LAN_JOIN, a
TCP/IP join, it depends on whether D2Launch hands one over, which cannot be
read from D2Client.

**What would settle it**: D2Launch's setup of pComInterface for a TCP/IP
join, or gpComInterface read live (x64dbg) after one is launched.

Reproduced -- the pointer is called untested.

## `PALSHIFT_ApplyGreenBlood` reads the green-blood remap without testing it

`0x6FAAE374` and `0x6FAAE39B`, `PalShift.cpp`. **Argued 2026-09-22** at
PalShift.cpp's close. **LATENT.**

    6FAAE354  cmp  byte ptr [eax+11Ch], 2   ; monstats2.txt LocalBlood 2
    6FAAE374  mov  esi, gpGreenBloodRemap
    6FAAE39B  mov  al, [eax+esi]            ; nothing tests esi between

PALSHIFT_Init allocates the remap only when GreenBlood.dat -- the second file
it opens -- is there, and this function reads it for every LocalBlood-2 class
PALSHIFT_EnsureMonsterPalette loads, green blood switched on or not. So a data
set without the file faults at the first such monster. **The stock data has
it**: GreenBlood.dat, 256 bytes, the 0x100 read -- measured in the stock
install's `data\global\monsters` at PalShift.cpp's batch 3.
A mod that drops the file reaches it.

Reproduced -- the remap is read untested.

## `PALSHIFT_GetUnitShiftTable` addresses past its record when RandTransforms.dat was not read

`PalShift.cpp`, the record arms of PALSHIFT_GetUnitShiftTable. **Argued
2026-09-22** at PalShift.cpp's close. **LATENT.**

The jump table takes shifts 0..7 (`cmp esi, 7 / ja`, UNSIGNED) and sends every
other shift -- 8 and up, or negative -- to its default arm. There, with
gbRandTransformsLoaded clear, the code falls into the 2..7 arm (0x6FAAE435),
whose address is `pPalShift + 4 + (shift << 8)`, or `+ 0x804` for the green
copy, with no bound: 8..15 read the green copy through the ordinary arm, 16
and up run past the 4104-byte record, a negative shift lands before it.

**The flag is set in the stock game.** PALSHIFT_Init raises it whenever
RandTransforms.dat is read, and the stock data has the file -- 7680 bytes,
the 0x1E00 read, measured at batch 3 -- so the default arm takes the
RandTransforms branch, whose own assert bounds the biased index to 0..29. A
data set without the file reaches the defect for any unit whose palette shift
is 8 or more, and CUNIT_CreateMonsterUnit gives every unique monster it rolls
for a shift of 9..29 (PALSHIFT_RollUniqueShift, 30 and up sent back to 2).

Reproduced -- the C computes the address as `lea` does, in unsigned
arithmetic, instead of an index that would be undefined out of range.

## `PALSHIFT_GetUnitShiftTable` indexes the per-class table with an unchecked class id

`0x6FAAE3D6`..`0x6FAAE3DF`, `PalShift.cpp`. **Argued 2026-09-22** at
PalShift.cpp's close. **LATENT.**

After the UNIT_MONSTER test the unit's dwClassId indexes gpMonPalShift's
8-byte records (`[ecx+eax*8+4]`, 0x6FAAE3DF) with no bound. The table has one
record per monstats.txt row: PALSHIFT_Init allocates `8 *
nMonStatsTxtRecordCount`. **Every class a client monster can have is bounded
where it is written** -- the argument of the entry "`CUNIT_HitTestUnit` and
`CUNIT_GetUnitName` read a monster's table row with no NULL test" above:
CUNIT_CreateMonsterUnit refuses a class whose MonStats lookup fails before it
allocates, and MONSTER_ChangeMonsterClass bounds a new class on entry. The
same caveat holds: a writer outside D2Client was not looked for.

Reproduced -- the index is written unbounded.

## The palette-shift and demo units use FOG_Alloc's answer untested

`PalShift.cpp` and `Record.cpp`, six calls. **Argued 2026-09-22** at the two
units' close. **LATENT.**

    6FAADEC0, 6FAADF70  PALSHIFT_Init                  memset over each at once
    6FAAE216            PALSHIFT_EnsureMonsterPalette  the 4104-byte record
    6FAAE613, 6FAAE699  RECORD_LogEvent                stores at +0x1F40, +0x1F44
    6FAAE9E6            RECORD_Play                    the same stores

Only a failed allocation reaches them -- 8 bytes per monstats.txt row, 256,
4104 and 8012. **Fog does not always stop on one.** FOG_Alloc (Fog
0x6FF58F50, #10042) calls ErrorManager #10028 with code 2 when its allocator
answers NULL (0x6FF58F73). That function logs the error and a stack dump,
calls whatever callback FOG_InitErrorMgr registered (`call eax` at 0x6FF5F258,
dword_6FFA0264) and returns (`retn` 0x6FF5F295), and FOG_Alloc returns the
NULL. Whether the client then stops or faults on the write depends on that
callback -- the executable's, not read here.

Reproduced -- every answer is used as the bytes use it.

## `RECORD_Play` stores every holder's count into the first holder, so a long demo plays past its end

`0x6FAAEA25` and `0x6FAAEA4F`, `Record.cpp`, with RECORD_ReadNextForFrame's
`0x6FAAEC39`. **Argued 2026-09-22** at Record.cpp's close. **REACHABLE.**

    6FAAEA25  mov  gRecordHolderFirst+1F40h, eax   ; the <= 500 arm
    6FAAEA4F  mov  gRecordHolderFirst+1F40h, eax   ; the > 500 arm

Both stores are to a fixed address inside the loop over the file's holders,
and the loop reads the file's TOTAL, nRecords, afresh on every pass
(0x6FAAEA0E) and never reduces it. For a file of more than 500 records every
holder is asked for 500, the last one's short read is not checked, and every
holder after the first keeps the count 0 its allocation gave it. So
RECORD_ReadNextForFrame's end test -- no next holder and the write count equal
to the read cursor (0x6FAAEC0D..0x6FAAEC25) -- never matches in the last
holder: it replays the real records and walks on into slots the file never
filled. A slot whose frame is below the current one stops the game on the
frame assert (the original's Record.cpp:298); one equal to it is dispatched as a window
message; and if the cursor reaches 500 first, gpRecordBlock becomes NULL
(0x6FAAEC39), the reader returns before its end-frame test from then on,
gbRecordPlaybackLoaded stays set, and WNDPROC_WndProc keeps real input from the game
(WNDPROC.cpp).

**How it is reached.** `-r` and `-p` are two of the eight switches
WINMAIN_D2ClientEntrypoint registers with Storm, and D2Client processes them from the
registry's `Default Command Line` value under the `Diablo II` key
(`SCmdProcess`, WINMAIN.cpp -- D2Client imports SCmdProcess and not
SCmdProcessCommandLine). A session recorded with `-r` that logs more than 500
of the ten kept messages, WM_MOUSEMOVE among them, is written as two holders
or more; `-p` then plays it. A file of up to 500 records is one holder and
plays right.

Reproduced -- both stores go to the first holder, and the reader's walk is
written as the bytes have it.

## `RECORD_Play` with no `Record.dr1` still sets D2Game's seed -- to 0

`0x6FAAEA8C`, `Record.cpp`. **Argued 2026-09-22** at Record.cpp's close.
**REACHABLE.**

    6FAAEA87  mov  eax, gRecordFileHeader.nInitSeed   ; 0x6FBA9A30
    6FAAEA8C  push eax
    6FAAEA8D  call D2GAME_GAME_SetInitSeed            ; outside the version test

CreateFileA's answer is not tested and neither is ReadFile's. With no file
both reads fail and the header keeps what it held -- zero on a first load, as
it lies past `.data`'s raw size -- so version 0 clears both playback flags,
and the call still hands D2Game 0. **D2Game's "no seed" is -1, not 0**:
GAME_SetInitSeed (D2Game 0x6FC35920) stores into nLowSeed (0x6FD2CA08), which
the image holds as 0xFFFFFFFF, and GAME_CreateNewEmptyGame (0x6FC36117) and
GAME_SendGameInit (0x6FC36791) compare it with -1 and otherwise seed the new
game from it. So `-p` with no demo file creates every later single-player
game of the process from seed 0. Reached as the entry above is. RECORD_SaveToFile's
untested CreateFileA only writes nothing when the file cannot be created.

Reproduced -- the call is made whatever the reads did.

## `GAME_ApplyParamBattleNetIp` tests the Battle.net address and copies the game server's

`0x6FAABC59` and `0x6FAABC69`, `Game.cpp`. **Argued 2026-09-22** at Game.cpp's
close. **NOT ESTABLISHED.**

    6FAABC59  lea  esi, [eax+4Ah]    ; szBattleNetIP, compared with "0"
    6FAABC69  add  eax, 32h          ; szServerIP, handed to lstrcpyA

So a given Battle.net address is never copied: gszBattleNetIP receives
whatever szServerIP holds. D2MOO's CONFIG_Apply_BattleNetIP copies szServerIP
too, and all three decompilers print it. gszBattleNetIP has two references,
this write and one read -- the push at 0x6FAADD50, behind `cmp eax, 4` on
gnGameType at 0x6FAADD4B, the game-type-4 arm of MSG_AppModeJoinGame. gnGameType is D2ConfigStrc::dwGameType, which Game.exe
fills from `-gametype` and szBattleNetIP from `-bn`, and then hands to D2Launch
-- the only module it starts (D2MOO Game/src/Main.cpp:700; "client" is
excluded from the command-line choice at :651). **What settles it**: whether
D2Launch passes game type 4 through to D2Client, which no database here can
answer.

Reproduced -- the test reads one field and the copy takes the other.

## `GAME_ApplyParamCharacterName` copies a name of 16 or more characters without a terminator

`0x6FAABE18`..`0x6FAABE2C`, `Game.cpp`. **Argued 2026-09-22** at Game.cpp's
close. **NOT ESTABLISHED.**

    6FAABE18  lea  eax, [ecx+1]      ; strlen + 1
    6FAABE1B  cmp  eax, 10h
    6FAABE1E  jle  short loc_6FAABE25
    6FAABE20  mov  eax, 10h          ; capped at 16 -- no room for the NUL
    6FAABE2C  call _strncpy

The buffer is cleared first, so a name of up to 15 characters is terminated;
one of 16 or more is copied as 16 characters and no terminator. The terminator
is never missed: the four bytes after gLocalPlayerName, 0x6FBA7820..0x6FBA7823,
lie past `.data`'s raw size and nothing refers to them (a bridged probe:
`is_loaded` False, no data references), so every reader sees a terminated
string of 16. The damage is the length, one more than the field's 15: both
join packets copy the name with `SStrCopy(..., 0x7FFFFFFF)` into a 16-byte
szClientName (Msg.cpp), so its terminator lands one byte past that field.
D2MOO keeps the 16 under NO_BUG_FIX and copies 15 otherwise. **What settles
it**: whether a name that long reaches D2ConfigStrc::szName, a 24-byte field
Game.exe fills from `-name` and D2Launch from the character it selects.

Reproduced -- the count is capped at 16, as the bytes cap it.

## `GAME_UnregisterMouseInputHandlers` guards the Windows 95 table with another table's message

`0x6FAAC18C`, `Game.cpp`, with the four inlined copies in GAME_AppModeGame
(0x6FAAB627, 0x6FAAB719, 0x6FAAB895, 0x6FAABAD9). **Argued 2026-09-22** at
Game.cpp's close. **INERT.**

    6FAAC18C  mov  eax, gGameWindowMessageHandlers+4   ; 0x6FB7581C
    6FAAC191  test eax, eax
    6FAAC193  jz   short locret_6FAAC1A8

The id the register half keeps is gWheelWin95Handler's, stored at 0x6FAAC2D3
into 0x6FB758D0. 0x6FB7581C is gGameWindowMessageHandlers[0]'s message,
WM_LBUTTONDOWN (0x201) in the image and written by nothing, so the test is
always true. D2MOO reproduces it, and Ghidra and Binary Ninja both print
0x6FB7581C. **Nothing can observe it**: Storm's SEvtUnregisterHandler
(0x6FFC33C0, behind SMsgUnregisterMessage) removes only an entry whose id and
handler both match, and the register half registers this table only after
storing a non-zero id (0x6FAAC2D3, then the register call at 0x6FAAC2E5). An
unregister with the id still 0 finds nothing; one with the id set does what
the intended test would have done. Whether the block runs at all is the next
entry's question.

Reproduced -- the guard reads 0x6FB7581C.

## `GAME_RegisterMouseInputHandlers` handles the MSWheel query message as if it were the roll

`0x6FAAC29C`..`0x6FAAC2E5`, `Game.cpp`. **Argued 2026-09-22** at Game.cpp's
close. **NOT ESTABLISHED.**

    6FAAC29C  push offset String     ; "MSH_WHEELSUPPORT_MSG"
    6FAAC2A1  call ds:RegisterWindowMessageA
    6FAAC2C7  call ds:SendMessageA   ; asked of the "MouseZ" window
    6FAAC2D3  mov  gWheelWin95Handler+4, esi   ; the same id, to be handled

zmouse.h (Windows SDK 10.0.19041.0) names the roll "MSWHEEL_ROLLMSG", "sent to
the foreground window when a wheel roll occurs", and MSH_WHEELSUPPORT_MSG the
"msg to send to query for wheel support". The function asks the driver
correctly and then waits for the query message as if it were the roll.
D2Client.dll holds no "MSWHEEL_ROLLMSG" (a byte search: 0 hits). **Whether it
can run**: the block's first test is `GetSystemMetrics(4Bh)`,
SM_MOUSEWHEELPRESENT (0x6FAAC270; the unregister half's at 0x6FAAC160), and it
wants Windows 95 exactly (platform 1, minor 0). The SDK defines that metric
only under `(WINVER >= 0x0500) || (_WIN32_WINNT >= 0x0400)` -- no Windows 95
target -- and zmouse.h's defines are "required when providing IntelliMouse
wheel support for Windows95 and NT3.51", the wheel being "supported natively
in WinNT4.0". **What settles it**: what Windows 95 answers for metric 75 with
the driver loaded. If 0, both Windows 95 blocks are dead code and this entry
and the one above are UNREACHABLE; if non-zero, the wheel handler never sees a
roll.

**Added 2026-09-22** at Game.cpp's second close: the right message would be
misread too. The handler the table names, GAME_WindowOnMouseWheel, takes the
signed HIGH word of wParam (`movsx eax, word ptr [edi+0Ah]`, 0x6FAA8F35),
WM_MOUSEWHEEL's delta; the MSWheel roll's wParam is the whole rotation --
"wParam = wheel rotation expressed in multiples of WHEEL_DELTA" (zmouse.h) --
whose high word is 0 for +120 and -1 for -120.

Reproduced -- the query message's id is stored as the handler's message.

## `GAME_AppModeGame` writes the progression 5, 9 or 13 when a game ends

`0x6FAAB7BF`..`0x6FAAB7D1`, `Game.cpp`. **Argued 2026-09-22** at Game.cpp's
close. **NOT ESTABLISHED.**

    6FAAB7B8  mov  cx, [esi+1EAh]    ; unpackedCTemp.nCharacterSaveFlags
    6FAAB7BF  shl  eax, 0Ah          ; eax = gnDifficulty's byte
    6FAAB7C2  add  eax, 500h
    6FAAB7C7  and  ecx, 0E0FFh       ; bits 8..12 cleared
    6FAAB7D1  mov  [esi+1EAh], ax

Bits 8..12 are the character's progression, and this stores
(difficulty << 2) + 5: 5, 9 or 13. At difficulties 1 and 2 that is neither
encoding -- the classic 8 and 12 (GAME_MarkClassicCompletion) nor the
expansion 10 and 15 (GAME_ShowDiabloVanquishedMessage's titles, D2MOO's
D2Win_10161 thresholds) -- and it is stored over whatever the field held,
although the menu test just above it reads that field for a completion
already made. It runs whenever gbGameTeardownActive is set: by GAME_AppModeGame
after cinematic 7 (0x6FAAB76E) and by GAME_SetGameTeardownActive, whose one
caller is VIDEO_PlayCinematic, not yet rewritten. The vanquished screen that
follows computes its title from the difficulty and does not read the field.
D2MOO reproduces the write without comment. **What settles it**: the field's
reader once D2Client returns -- D2Launch's character list, which no database
here holds.

Reproduced -- the write is the bytes' shift, add and mask.

## `GAME_WindowOnRButtonUp` passes the player pointer's high half as click flags

`0x6FAA8EC7`..`0x6FAA8EE3`, `Game.cpp`. **Argued 2026-09-22** at Game.cpp's
second close. **INERT.**

    6FAA8EC7  call CUNIT_GetPlayer   ; eax = the player
    6FAA8ED9  mov  ax, [esi+8]          ; LOWORD(wParam) into ax
    6FAA8EE3  push eax                  ; nFlags

The flags handed to PLAYER_OnMouseClick are the player pointer with its low
word replaced by the MK_* keys, where the other three click handlers pass
flags they build. **No consumer can observe it**: PLAYER_OnMouseClick
(0x6FB49920) reads the caller's value from that slot once, as a byte (`mov bl,
byte ptr [esp+34h+...]`, 0x6FB49A29), and tests 8 and 4 (0x6FB49A2D,
0x6FB49A52). Its only other reads of the slot are in its switch's case 1,
after D2Common's UNITS_GetRunAndWalkSpeedForPlayer has written the walk speed
into it (0x6FB49B61..0x6FB49B6D), and this call passes button mask 5, which
takes case 5 (`jmp ds:jpt_6FB49A6A[esi*4]`, 0x6FB49A6A).

Reproduced -- the push is eax as the bytes build it.

## `GAME_SGameDataAllocAndLinkNode` links the list's terminator as the node when the allocation fails

`0x6FAACB28`..`0x6FAACBB4`, `Game.cpp`. **Argued 2026-09-22** at Game.cpp's
second close. **UNREACHABLE.**

    6FAACB28  xor  eax, eax          ; no node
    6FAACB3E  lea  ecx, [ebx+4]      ; the link is the terminator's own
    6FAACB76  mov  edi, [esi+4]      ; tail insert: esi is 0, reads address 4
    6FAACBB4  mov  [esi], ecx        ; head insert: esi is nLinkOffset

Storm's TSList::NewNode over LinkNode (squall storm/list/TSList.hpp): with no
node, Link(NULL) is the terminator, which is unlinked from its own list -- its
links zero afterwards -- and linked back as the node, so either insert faults
at a near-null address instead of returning NULL. **The arm cannot run**:
Storm's SMemAlloc never returns NULL. Its allocator (called at Storm.0x6FFCB225)
tests the heap's block (`test edi, edi`, Storm.0x6FFCB4E4) and on failure
calls Storm.0x6FFCB280, which is SErrDisplayError and then ExitProcess(1)
(Storm.0x6FFCB28D, 0x6FFCB294); SMemAlloc's own early `xor eax, eax / retn`
(Storm.0x6FFCB14D) follows a call to the same function. The container's
batch 2 wrote it LATENT; no data or caller is needed to stop it.

Reproduced -- the NULL arm links the terminator, as the bytes do.

## `GAME_SGameDataListDestroyAll` stores 0 in the bucket mask where Storm's Destroy stores -1

`0x6FAAC72B`, `Game.cpp`. **Argued 2026-09-22** at Game.cpp's second close.
**UNREACHABLE.**

    6FAAC6A5  xor  ebp, ebp
    6FAAC72B  mov  [esi+24h], ebp    ; nBucketMask = 0

squall's TSHashTable::Destroy (storm/hash/TSHashTable.hpp) stores -1, the
value its Initialized() reads as "never initialised"; with 0, a destroyed
table would count as initialised with no buckets left. **Nothing reaches it**:
its only reference is slot 3 of gVtblSGameData0 (0x6FB6E248), and that table
is in the object only between two stores of CRT_GAMEDATA_ConstructTable
(0x6FAAC366, 0x6FAAC3F9) and after the last vtable store of each destructor
(0x6FAA8A77, 0x6FAAC542, 0x6FAAC90C, 0x6FAACD8F) -- stretches that make no
virtual call. The live table carries gVtblSGameData2, whose slot 3 is
GAME_SGameDataHashDestroyAll. The container's batch 3 wrote it LATENT.
[2026-10-08, the globals plan's unit 14, part B: gGameData is ours, and our
construction stores gVtblSGameData2 alone (Game.cpp, "gGameData'S
CONSTRUCTION") -- so in ours gVtblSGameData0 is in the object only after a
destructor's store, and the verdict stands.]

Reproduced -- the store is 0, as the bytes store it.

## `WINMAIN_CmdArgPreCache` touches the pages of an allocation it never tests

`0x6FAA1E21`..`0x6FAA1E32`, `WINMAIN.cpp`. **Argued 2026-09-22** at
WINMAIN.cpp's close. **UNREACHABLE.**

    6FAA1E1C  mov  ecx, 6000000h
    6FAA1E21  call FOG_Alloc
    6FAA1E26  lea  ecx, [eax+6000000h]
    6FAA1E32  mov  byte ptr [edx], 0      ; edx starts at eax

The `-c` switch's handler zeroes one byte of every 4 KB page of 96 MB with no
test of FOG_Alloc's answer, so a NULL would make the first store fault at
address 0. FOG_Alloc (Fog 0x6FF58F50) does pass a NULL through, after its
error report (Fog #10028), when its allocator (Fog 0x6FF59310) returns one.
**That allocator returns NULL only from its pool arm** (`xor eax, eax` at
Fog 0x6FF594BC, after a FOG_DisplayWarning), **and this request cannot reach
it**: the size class is `bsr(size - 1) + 1 - 4` = 23 for 0x6000000, and a
class at or past the pool's count (`cmp eax, [ebx+1Ch]` / `jl`, Fog
0x6FF5937A) goes to `_malloc` instead, whose failure is FOG_DisplayHalt and
`_exit(-1)` (Fog 0x6FF593B6, 0x6FF593C0). FOG_Alloc hands the allocator the
global pool (`xor ecx, ecx`, Fog 0x6FF58F5D), whose count
FOG_CreateNewPoolSystem computes the same way from its third argument
(Fog 0x6FF5A392..0x6FF5A3C5). The global pool is the one created with a NULL
first argument, and both calls that pass one -- FOG_InitErrorMgr's (Fog
0x6FF5E26F) and the allocator's own lazy one (Fog 0x6FF5933C) -- pass
0x40000: 15 classes. The two other callers create pools of their own
(GAME_InitGame's ghClientPoolSystem, 0x6FAA949D; D2Game's `lea eax,
[esi+1Ch]` at 0x6FC3600A).

Reproduced -- the loop runs from FOG_Alloc's answer, as the bytes do.

## The command-line help box compares an untested `GetUserNameA` buffer

`0x6FAA1DC8`, `0x6FAA1EF2`, `0x6FAA22F8`, `0x6FAA2368`, `WINMAIN.cpp`.
**Argued 2026-09-22** at WINMAIN.cpp's close. **INERT.**

    6FAA1DC0  mov  [esp+1Ch+pcbBuffer], 0Fh
    6FAA1DC8  call ds:GetUserNameA        ; the answer is not read
    6FAA1DDD  call STORM_SStrCmp          ; buffer against "rseis"

All four copies of the help-box block (WINMAIN_CmdArgDisplayHelp, WINMAIN_CmdArgMode's
no-match arm, WINMAIN_SCmdDisplayHelpOnExtraArg, WINMAIN_SCmdDisplayHelp)
call GetUserNameA with a 16-byte stack buffer and a length of 15 and never
test the call. A login of 15 characters or more fails it and leaves the
buffer as the stack held it. **Nothing can observe the difference**: the
buffer's one reader is SStrCmp against "rseis" (0x6FB74748), whose only
effect is whether the box is shown; on bytes that are not the name it
answers "different", which is what the long name itself would answer, and
the box is shown either way. The one exception is stack bytes that happen
to spell "rseis" and its NUL, which would hide the box from that user once.

Reproduced -- the buffer is compared as the bytes compare it, untested.

## `SCROLL_UpdateViewportOrigin` does not test the player

`0x6FAB599E..0x6FAB59AE`, `scroll.cpp`. **Argued 2026-09-23** at scroll.cpp's
close. **LATENT.**

    6FAB599E  call CUNIT_GetPlayer
    6FAB59A5  push edi                        ; untested
    6FAB59A6  call D2COMMON_UNITS_GetClientCoordX
    6FAB59AE  call D2COMMON_UNITS_GetClientCoordY

D2Common #10333 and #10334 assert their argument and exit on NULL
(`FOG_DisplayAssert` at 0x6FDBDB27 / 0x6FDBDB77, `push -1 / call _exit` at
0x6FDBDB2F / 0x6FDBDB7F). **What prevents it is the caller.** The one call
site is GAME_DrawGameScene at 0x6FAA97B9, which loads the same
gpPlayerUnit at 0x6FAA9659 and passes it to #10333 itself, untested, at
0x6FAA967F -- so a frame with no player halts there first. Nothing between
the two calls writes gpPlayerUnit: its two writers, CUNIT_DestroyUnit
(0x6FB27FF6) and CUNIT_SetPlayerUnit (0x6FB283C4), are reached from
GAME_DrawGameScene only through UIPANELS_DrawInGamePanels, which it calls
later, at 0x6FAA97D9 (every one of 42, 36 and 38
paths to DestroyUnit, SetPlayerUnit and FreePlayerUnit through that one
callee). Indirect calls and calls into other modules were not followed.

Reproduced -- the player is used untested.

## `gnUiShiftOffsetX` is computed every frame and never read

`0x6FAB596B`, `0x6FAB5986`, `0x6FAB5992`, `scroll.cpp`. **Argued 2026-09-23**
at scroll.cpp's close. **INERT.**

    6FAB596B  mov  gnUiShiftOffsetX, 0FFFFFF60h   ; -160
    6FAB5986  mov  gnUiShiftOffsetX, eax          ; PANEL_LEFT: width / 2 - 160
    6FAB5992  mov  gnUiShiftOffsetX, 0            ; PANEL_BOTH

SCROLL_UpdateViewportOrigin recomputes gnUiShiftOffsetX (0x6FBAB9C0) from
gUiShiftScreen every frame, and **nothing reads it**: it has
these three stores and no other reference, and a scan of the whole
image for the address as a little-endian dword finds the same three. The
global is scroll.cpp's `.bss`, and no instruction or table takes its address,
so no other module can reach it. The database called it the nudge an open
panel APPLIES until the close's CORRECTED paragraphs.

Reproduced -- the three stores are written as the bytes make them.

## `DLIGHTMAP_DebugDrawLightGrid` reads the first light without testing the list

`0x6FAA6130..0x6FAA6138`, `dLightMap.cpp`. **Argued 2026-09-23** at the
close of dLightMap.cpp's run A. **UNREACHABLE** in a stock 1.10 client.

    6FAA6130  mov  eax, gpDynamicLights       ; the list head, untested
    6FAA6135  mov  ecx, [eax+14h]              ; faults here on an empty list
    6FAA6138  mov  edx, [eax+10h]

With no light in gpDynamicLights the debug view would fault reading 0x14.
**What prevents it is the gate.** The one caller,
VIEWEFFECTS_EnvUpdateViewAndLevelEffects, runs it only when gbDebugDrawLightGrid is set
(`mov eax, gbDebugDrawLightGrid` at 0x6FAB7563), and that dword,
0x6FBAF8FC, lies past `.data`'s raw size -- `ida_bytes.is_loaded` is False,
so the loader zero-fills it -- with one reference in the module, that read.
Nothing writes it (a scratch probe of every reference to the item), and it
is D2Client's own `.bss`, which no other module can name. The same shape and
verdict as `UIPANELS_DrawChargeUpIcons`' gbChargeUpIconsEnabled.

Reproduced -- the head is dereferenced untested.

## `DLIGHTMAP_EnvUpdateAmbientLightCache` hands gpAct to D2Common untested

`0x6FAA62B1`, `0x6FAA62CB`, `dLightMap.cpp`. **Argued 2026-09-23** at the
close of dLightMap.cpp's run A. **INERT.**

    6FAA62B1  mov  eax, gpAct                 ; -> #10927, untested
    6FAA62CB  mov  ecx, gpAct                 ; -> #10926, untested

D2Common #10927 and #10926 go through DUNGEON_GetEnvironmentFromAct, which
asserts its act (D2MOO, `D2Dungeon.cpp:954`). **What prevents it is the
caller's caller.** The function's one caller is VIEWEFFECTS_EnvUpdateViewAndLevelEffects
(0x6FAB7311), whose one caller, GAME_DrawGameScene, hands the same gpAct
to the same #10927, untested, at 0x6FAA972B..0x6FAA9738 -- and every path to
its call of VIEWEFFECTS_EnvUpdateViewAndLevelEffects (0x6FAA97CE) passes there first
(its branches, a scratch probe). A NULL act would assert there, earlier in
the same frame. gpAct's two writers are GAME_PreloadActData (0x6FAA9342),
reached only from packet 0x03's handler, and GAME_AppModeGame's teardown after
the game loop (`xor esi, esi` / `mov gpAct, esi`, 0x6FAAB90A..0x6FAAB90C);
no call path from GAME_DrawGameScene, twelve deep, reaches
neither. Indirect calls and calls into other modules were not followed.

Reproduced -- the act is passed untested.

## Six quest effects hand D2Common's neighbour search an untested room

`0x6FAB9292`, `0x6FAB973A`, `0x6FAB9911`, `0x6FAB9A50`, `0x6FAB9E91`,
`0x6FAB9F1C`, `QuestsFX.cpp`. **Argued 2026-09-23** at QuestsFX.cpp's close.
**NOT ESTABLISHED.**

    6FAB9292  call D2COMMON_UNITS_GetRoom                   ; TriggerTaintedSun
    6FAB9297  push 95h                                      ; no test of eax
    6FAB929F  call D2COMMON_UNITFINDS_FindUnitInNeighboredRooms

D2Common #11087 asserts its room and exits: `push offset aHroom / call
FOG_DisplayAssert / push -1 / call _exit` at 0x6FDBC72D..0x6FDBC743 (D2MOO,
`UnitFinds.cpp:48`). The unit makes twelve UNITS_GetRoom calls. Six test
the answer first, and all six are triggers searching the player's room
(`test eax, eax` at 0x6FAB9017, 0x6FAB9167, 0x6FAB9350, 0x6FAB93F8,
0x6FAB9587, 0x6FAB9637). Six do not:

    TriggerTaintedSun        0x6FAB9292   the player's room (packet 0x89)
    OnBaalDeath              0x6FAB9E91   the player's room (a monster's death)
    OnAncientDeath           0x6FAB973A   the dying monster's room
    SpawnTowerChestEffect    0x6FAB9911   the dying monster's room
    OnCompellingOrbDeath     0x6FAB9A50   the dying monster's room
    OnTyrael3Created         0x6FAB9F1C   the new tyrael3's room

A NULL answer is possible. D2Common's UNITS_GetRoom returns 0 for a
dynamic-path unit with no path (0x6FDBE2B7), and otherwise the path's
room. A client monster's path room is whatever CUNIT_CreateMonsterUnit
got from CUNIT_FindRoomForCoords (0x6FB277A6). That function tests the
answer only to skip seeding from it (0x6FB277B7), then hands it through
MONSTER_InitClientUnit to PATH_AllocDynamicPath (0x6FB40F06), which stores
it as is (D2MOO, `Path.cpp:710`). **What would settle it** is whether a
stock server can announce a monster at coordinates outside every room the
client has loaded, or send packet 0x89 or a quest monster's death while the
local player has no room. Neither was traced. The authors' own six tests
of the same answer say they thought it possible.

Reproduced -- the six rooms are passed untested.

## `QUESTSFX_OnBaalDeath` hands UNITS_GetRoom an untested player

`0x6FAB9E82..0x6FAB9E91`, `QuestsFX.cpp`. **Argued 2026-09-23** at
QuestsFX.cpp's close. **LATENT.**

    6FAB9E82  call CUNIT_GetPlayer        ; gpPlayerUnit, one load
    6FAB9E91  call D2COMMON_UNITS_GetRoom     ; untested

D2Common #10342 asserts `ptUnit` and exits (0x6FDBE278..0x6FDBE291).
**What prevents it is the window the player lives in**, the same as in
`CUNIT_TurnUnitToFacePlayer`'s entry: gpPlayerUnit is NULL only before
packet 0x59 assigns it and after the game is left. Every frame in between
hands it untested to D2Common #10333 in GAME_DrawGameScene
(0x6FAA967F, `SCROLL_UpdateViewportOrigin`'s entry). This arm runs when a
baalcrab dies (MONUNIQUE_MonUModCltQuestComplete, class 544, mode 0). Whether that
can happen before packet 0x59 was not traced. The same function's untested
room is in the entry above.

Reproduced -- the player is passed untested.

## `QUESTSFX_TriggerCairnStones` reads the missiles.txt row without testing it

`0x6FAB90BF`, `QuestsFX.cpp`. **Argued 2026-09-23** at QuestsFX.cpp's close.
**LATENT.**

    6FAB90AE  jle  short loc_6FAB90BD        ; count <= 288: the NULL arm
    6FAB90BF  mov  edx, [eax+38h]             ; dwParam[0], untested

An inlined DATATBLS_GetMissilesTxtRecord(288): NULL when the table holds
288 rows or fewer, and the fade length is read from it at once, a fault at
0x38. **What prevents it is the data**: the stock Missiles.txt has 684 rows
and no `Expansion` marker.
Only a mod that cut the table would fault, and it would fault when the
Cairn Stones trigger fires.

Reproduced -- the row is read untested.

## REFUTED: three quest-death arms spawn with an untested player

`0x6FAB9CEF`, `0x6FAB9D6F`, `0x6FAB9DEF`, `QuestsFX.cpp`
(QUESTSFX_OnRadamentDeath, _OnMaggotQueenDeath, _OnDurielDeath). **Argued
2026-09-23** at QuestsFX.cpp's close. **REFUTED.**

Each hands CUNIT_GetPlayer's answer untested to MISSILE_ClientSpawnSimple
as the missile's owner. **The callee tests it**: `test esi, esi / jz` at
0x6FB3EAF8..0x6FB3EAFA returns 0 for no owner, before anything reads it.
Nothing else in the three arms touches the player. An untested argument
that the callee accepts as NULL is not a defect.

Reproduced as it is.

## REFUTED: `QUESTSFX_OnBloodRavenDeath` reuses a spawn record without clearing it

`0x6FAB98E3`, `QuestsFX.cpp`. **Argued 2026-09-23** at QuestsFX.cpp's close.
**REFUTED.**

    6FAB98E3  mov  [esp+64h+var_4C], 125h    ; nMissile = 293, nothing else

The third spawn changes only nMissile in the record the second spawn used.
That would carry whatever MISSILE_ClientSpawn left in it, **and it leaves
nothing**: every use of pParams in its body (0x6FB3E070) is a load, and
each of its seven stores through a register goes through esi. From
0x6FB3E63D on, esi holds the new unit CUNIT_CreateClientOnlyUnit returned
(a scan of the disassembly for stores through a register). So the third
record is exactly a cleared one for missile 293, with the owner, the origin
and level 1.

Reproduced as it is.

## Packet 0x5E is indexed by D2Game's quest-init row, and the client reads it by quest id

`0x6FAB8F3A` (`mov cl, gQuestStates[eax]`), `Quests.cpp`, against D2Game's
`0x6FC9521F`. **Argued 2026-09-23** at Quests.cpp's close. **REACHABLE.**

**The two modules disagree on what the index means.** D2Client asks
QUESTS_GetQuestState for a D2Quests id. D2Game's QUESTS_SequenceCycler
(0x6FC94DB0) builds the payload from its quest-init table instead:

    6FC95219  mov  al, [eax+9]               ; the row's quest, +9 (bNotIntro)
    6FC9521C  add  esi, 18h                  ; next row
    6FC9521F  mov  [esp+ecx+0C0h+var_8B], al ; payload[ecx], ecx = the row

The table is 36 rows of 0x18 bytes from 0x6FD31278. The count,
dword_6FD315D8, is loaded and 36. The rows' quest ids (+0x10), read off
the stock image (input md5 matches the install's):
* rows 0..13 hold quests 0..13;
* row 14 holds 26, 15 holds 27, 16..22 hold 14..20, 23 holds 28;
* rows 24..27 hold 21..24, 28 holds 25, 29 holds 29;
* rows 30..35 hold 31..36.

So for every id from 14 up but 29 the client reads another quest's byte.
Byte 36 is never written: the frame names no slot in the buffer but the header's and that
store, and nothing clears the buffer. D2MOO's sender (D2Game Quests.cpp:1234)
indexes by row too, but zero-initialises the packet.

**Which client calls are affected.** A scratch probe of the pushes before
all seven code calls:
* five pass 1;
* UIPANELS_DrawInGamePanels passes its title table's ids, 1 and 5;
* QUEST_ShouldPlayAreaEntrySpiel passes its area-entry record's key.

The fourteen gpAreaEntryQuestLines records hold the keys 1, 2, 3, 5, 6, 12,
13, 31, 34, 35 and 36. Only the four Act 5 keys are 14 or more:

    key 31  level 110 (Bloody Foothills)     reads quest 32's byte
    key 34  level 121 (Nihlathak's Temple)   reads quest 35's byte
    key 35  level 120 (Arreat Summit)        reads quest 36's byte
    key 36  level 132 (Worldstone Chamber)   reads byte 36, never written

**What the byte holds.** 1 when the game creates its quest data (D2MOO
Quests.cpp:353). The cycler clears it to 0 once per game (bPickedSet), for
every row whose +0x0C is 0, when that player's record has flag 0 or 15:
D2MOO's QFLAG_REWARDGRANTED and QFLAG_COMPLETEDBEFORE. All six Act 5 rows
have +0x0C 0 (read off the image). The clear runs while byte_6FD31270 is
set. It is 1 in the image. Its one writer, QUESTS_DisableSequences_0,
stores 0, and its one reference is a jump from QUESTS_DisableSequences
(0x6FCCD410). That function's one reference is D2Game's export table
(0x6FD2BB08). D2MOO writes
the byte elsewhere only in Act 2 and 3 quests.

**Why REACHABLE.** QUEST_ShouldPlayAreaEntrySpiel returns FALSE when the
byte is 0 (Quest.cpp). The test that follows is the client's own record for
the intended quest: bits 13, 1, 0 and 14 must be clear. A neighbour's byte
of 0 therefore suppresses a line the intended byte would allow. Example:
the clear ran for a player who has finished the Ancients (35) but not
Nihlathak (34). Entering Nihlathak's Temple then reads 0, and the line,
sound event 77, does not play. A neighbour's byte of 1 lets the gate pass
wrongly. The record test then usually stops it, since it tests bit 0.
Bit 15 alone it does not test. Key 36's byte is stack residue, so the
Worldstone Chamber's gate depends on what was on D2Game's stack.

Reproduced -- the index is the quest id, as the client has it.

## `QUESTS_GetQuestState` bounds the index from above only, and signed

`0x6FAB8F14..0x6FAB8F1B`, `Quests.cpp`. **Argued 2026-09-23** at
Quests.cpp's close. **LATENT.**

    6FAB8F14  mov  eax, [esp+nQuest]
    6FAB8F18  cmp  eax, 25h
    6FAB8F1B  jl   short loc_6FAB8F38    ; signed: every negative index passes
    6FAB8F3A  mov  cl, gQuestStates[eax]

A negative index passes the assert "idx < QUEST_NUMCLASS" and reads below
gQuestStates. **What prevents it is the callers.** The seven code calls
in D2Client pass 1, the title table's 1 or 5, or an area-entry key from 1
to 36 (the entry above). The function is also export #10002, but no stock
module imports D2Client.dll statically (a pefile scan of the install). A
caller through GetProcAddress was not looked for.

Reproduced -- the bound is one-sided and signed.

## `ROSTER_AddPlayer` copies the guild tag and the guild name with no bound

`0x6FAAED7D`, `0x6FAAED92` (an entry already on the roster) and `0x6FAAEF44`,
`0x6FAAEF59` (a new one), `Roster.cpp`. **Argued 2026-09-24** at
Roster.cpp's close. **LATENT, margin zero.**

Both paths copy the packet's guild tag into `szGuildTag[4]` (+0x46) and the
guild name after it into `szGuildName[28]` (+0x4A) with inlined `strcpy`s.
A tag of four characters would put its terminator on the name's first byte,
and the name's own copy rewrites that byte next. A name of 28 or more would
run into `szNameEx` (+0x66) and, past 53, into `pNext` (+0x80).

**Why latent:** the stock server bounds both before they reach the packet.
D2Game fills a client's guild info in one place, the save loader
`0x6FC8A780`. There the tag's fourth byte is zeroed (`mov byte ptr
[esp+0B8h+a2+3], 0`, `0x6FC8AA4F`) and so is the last byte of the name's
28-byte buffer, copied in seven dwords (`mov [esp+0B8h+var_1], 0`,
`0x6FC8AA54`), before `CLIENTS_SetGuildTag` and `CLIENTS_SetGuildName`
store them. That loader is the one caller of each
setter; SetGuildName's second reference, from 0x6FC344FF, lies in bytes
inside `CLIENTS_GetGuildName`'s end and is not a call site. The 0x5B builder
(`0x6FC3DF80`) copies the tag as one dword and the name from
`CLIENTS_GetGuildName`, the name after the tag's terminator
(`0x6FC3E02E`). So a tag has at most three characters and a name at most
27, and both copies fit exactly. A malformed server was not considered.

Reproduced in `Roster.cpp` -- both copies unbounded, as the bytes
have them.

## Roster.cpp uses FOG_Alloc's answer untested at seven sites

`Roster.cpp`. **Argued 2026-09-24** at the unit's close.
**LATENT.**

    6FAAEEE0  ROSTER_AddPlayer            line 213  `rep stosd` over it (0x6FAAEEF0)
    6FAAF76A  ROSTER_FindOrAddListNode    line 505  three stores
    6FAAF374  ROSTER_RemovePlayer         line 505  the same, inlined
    6FAAFBE4  ROSTER_OnPartyUpdate        line 505  the same, inlined
    6FAB0175  ROSTER_PartySendButtonCommand     line 505  the same, inlined
    6FAB0942  ROSTER_UpdateLifeAndLevel   line 505  the same, inlined
    6FAB0E6D  ROSTER_SetPartyId           line 505  the same, inlined

A scratch probe listed every FOG_Alloc call in the unit's span: nine, and
the two in `ROSTER_FindCorpseEntry` (lines 1660 and 1677) test the answer
and assert. The seven above do not. Only a failed allocation of 132 or 12
bytes reaches them, and that is the Fog path argued in "The palette-shift
and demo units use FOG_Alloc's answer untested": FOG_Alloc reports the
error and returns the NULL, and what happens next depends on the
executable's error callback, which was not read.

Reproduced -- every answer is used as the bytes use it.

## `ROSTER_UpdatePartyState` skips an entry whose own GUID is also one of its corpses'

`0x6FAAF070`, `Roster.cpp`. **Argued 2026-09-24** at the unit's
close. **UNREACHABLE with a stock server.**

Packet 0x75's update finds the entry by GUID or by corpse, then walks that
entry's corpses again and returns on a hit (ROSTER_EntryHasCorpse inlined).
So a corpse's GUID changes nothing, as intended. An entry whose own
`dwUnitId` is also on its corpse list would be skipped too.

**Why unreachable:** a corpse id reaches the list only through packet 0x8E,
and every sender in D2MOO's D2Game (`SCmd.cpp:2269`, called from
`Player.cpp:483`, `PlrModes.cpp:648` and `728`, `PlrSave.cpp:1653`,
`PlrSave2.cpp:1201`, and the join loop in `Game.cpp:812`) pairs a player
with a DIFFERENT unit: its dead body, a player unit of its own
(`SUNIT_AllocUnitData(UNIT_PLAYER, ...)`), or a body with the owner its
inventory names. A new unit takes the next GUID of its type
(`dwLastUsedUnitGUID[nUnitType] + 1`, D2MOO `SUnit.cpp:494`), so the two
differ. Read in D2MOO's D2Game, not measured in the stock bytes.

Reproduced -- the second walk returns, as the bytes do.

## `ROSTER_FindCorpseEntry` adds to the pending list and never searches it

`0x6FAB1096..0x6FAB10B5` against `0x6FAB1104..0x6FAB110F`,
`Roster.cpp`. **Argued 2026-09-24** at the unit's close.
**LATENT, and harmless if hit.**

A corpse for a player who is not on `gpPartyEntries` makes a new entry on
`gpPendingRosterEntries`, and `ROSTER_AddPlayer` takes a pending entry back
by GUID when packet 0x5B arrives. The lookup walks `gpPartyEntries` only, so
a second corpse for a player still pending makes a second pending entry,
and AddPlayer takes the newest.

**Why latent:** the character load sends one 0x8E per saved corpse to the
players already in the game, before they have the newcomer's 0x5B. D2MOO's
`PLRSAVE2_WriteCorpsesSection` writes a count of 0 or 1, so the load sends
at most one per player. A second one in that window would need another
sender to fire for the same player -- the body placement at D2MOO
`Player.cpp:483` is the candidate, and whether it runs before the 0x5B was
not established. **Even then nothing is lost:** that sender names the same
body, so the older entry keeps a copy of a corpse id the adopted entry also
holds; no lookup walks the pending list, and `ROSTER_FreeAll` frees it
with the rest.

Reproduced -- the lookup walks gpPartyEntries only.

## `ROSTERPETS_FindOrAddPet` evicts by the monstats id where it means the pet type

`0x6FAB150E`, and `0x6FAB15F1` and `0x6FAB164D` in `ROSTERPETS_OnPetPacket81`,
`RosterPets.cpp`. **Argued 2026-09-24** at the unit's close.
**REACHABLE in stock 1.10.**

When a record of pet type 7 (PetType.txt `hireable`) arrives, the block at
the original's RosterPets.cpp:89 frees the first record whose `nMonStatsId` (+0x00) is 7
under another unit id: `cmp dword ptr [ecx], 7`, where the type is at +0x04
(`mov [edx+4], ebp`, 0x6FAB1585). The unit's two other tests of a record
against 7 read +0x04 (0x6FAB17BF, 0x6FAB1A71). Three inlined copies of one
function, one mistake. The database comment on FindOrAddPet has said so since
2026-09-04, and called it unreachable.

**What it hits:** MonStats.txt's 7 is `zombie3`, the Ghoul -- enabled, levels
12, 43 and 73 -- and MonStats2.txt gives it `revive` 1.
D2MOO's `SKILLS_SrvDo058_Revive` (`SkillNec.cpp:1490`) revives a dead monster
whose MonStats2 row has that flag, keeping its class, and adds it as a pet
(`sub_6FC7D7A0`); the pet packets 0x7A and 0x81 carry `nPetClass =
pPet->dwClassId` (`sub_6FC7DBF0`) and go to every player's client
(`PLAYERPETS_PlayerIterate_UpdateClient`). So a Necromancer's revived Ghoul
has a record with monstats id 7 on every client, and the next record of pet
type 7 that client receives -- a hireling added to anyone -- frees it. Until
a packet announces the Ghoul again, its ROSTERPETS_GetOwnerId answers -1 and
ROSTERPETS_GetAutomapClass 0, and the target and label tests that ask who
owns a monster (CUnit.cpp, ui_panels.cpp) treat it as nobody's pet.

**What it misses:** no hireling's class is 7 -- Hireling.txt's `Class` column
holds 271, 338, 359 and 561 -- so the eviction the code means never happens.
What an extra hireling record changes was not followed through every reader:
the head-first walks find the newest record, which is the one pushed last.

Read in D2MOO's D2Game and the stock tables, not in a running game.

Reproduced -- the three tests read +0x00, marked in RosterPets.cpp.

## RosterPets.cpp uses FOG_Alloc's answer untested at two sites

`RosterPets.cpp`. **Argued 2026-09-24** at the unit's close.
**LATENT.**

    6FAB1569  ROSTERPETS_FindOrAddPet     line 122  `rep stosd` over it (0x6FAB1579)
    6FAB16A8  ROSTERPETS_OnPetPacket81    line 122  the same, inlined (0x6FAB16B8)

The unit's only two FOG_Alloc calls (line 122 twice). Only a failed allocation of 52 bytes reaches
them, the Fog path argued in "Roster.cpp uses FOG_Alloc's answer untested at
seven sites".

Reproduced -- the answer is cleared and filled untested, as the bytes do.

## `HELP_CenterTextX` copies a tbl string into a `Unicode[100]` with no bound

`help.cpp`. **Argued 2026-09-24** at the unit's close. **LATENT.**

    6FAD8292  mov edi, 64h               the array's hundred constructions
    6FAD82B2  call Unicode::strcpy       into it, from D2LANG_GetStringFromTblIndex

Its only callers are the two label passes, HELP_DrawLabelsWide and
HELP_DrawLabels: five calls in the bytes and 37 copies of the body inlined
(each the same unbounded copy), over 20 tbl indices -- 3986, 3987, 4144,
4198 and 4207..4222. Over the stock install's language
packs: eng, CHI, ESP, ITA, JPN and KOR hold the English keys at these
indices, and the longest string among them is 78 bytes (CHI, tbl 4198) -- at
most 78 units after D2Lang's conversion; English's longest is 17. DEU and
FRA in this install carry other keys at these indices and POL, POR and RUS
have no string.tbl, so those five are not measured.

Reproduced -- the copy is written unbounded, as the bytes do.

## `HELP_DimGameScreen` fills with a colour nothing writes

`help.cpp`, `mov al, gnHelpDimColor` at 0x6FAD82EB. **Argued
2026-09-24** at the unit's close. **HARMLESS.**

gnHelpDimColor (0x6FBB365C) has one reference in the module, that read, and lies past `.data`'s raw data
(`ida_bytes.is_loaded` False), so it is 0 from load on. PANEL_SetBoxFillColorBlack,
called just before, stores MixRGB(0, 0, 0) into gnUiBoxFillColor, which the
function does not read. Entry 0 of `pal.dat` in ACT1..ACT5, Menu0 and STATIC
of the stock install is 000000, so the wash is black whichever index the mix
would have given.

Reproduced both ways -- the call and the unwritten byte.

## help.cpp draws its three cels untested

`help.cpp`. **Argued 2026-09-24** at the unit's close. **LATENT.**

ARCHIVE_CellFileLoad fails softly: ARCHIVE_LoadFile answers NULL for a file that will
not open (Archive.cpp). HELP_DrawScreen, HELP_DrawBackground,
HELP_DrawKeyBulletCels and HELP_DrawLeaderLines hand gpHelpBackgroundCel,
gpHelpPointerCel and gpHelpBulletCel to D2GFX_TEXTURE_CelDraw untested. All
four files HELP_LoadImages names are in the stock install --
data\global\ui\MENU\800helpborder.dc6, helpborder.dc6, helpwhitebullet.dc6
and helpyellowbullet.dc6 -- so only a damaged install reaches it. Every UI
image in the module is loaded the same way; this records the unit's own.

Reproduced.

## CUnitEvent.cpp clears FOG_AllocPool's answer untested

`CUnitEvent.cpp`, CUNITEVENT_AddEvent. **Argued 2026-09-24** at the
unit's close. **LATENT.**

    6FB2A763  call FOG_AllocPool   line 72, 32 bytes from the unit's pool
    6FB2A773  rep stosd            over the answer, untested

Fog's AllocPool (#10045, 0x6FF58FF0) is FOG_Alloc's shape with a pool:
the same allocator (0x6FF59310), and on NULL the same
`ErrorManager_10028(2, ...)` report (0x6FF5901F), then `mov eax, esi` and
the NULL returned (0x6FF59027). So this is the path argued in "The
palette-shift and demo units use FOG_Alloc's answer untested": what follows a
failed allocation depends on the executable's error callback, which was not
read. Only a failed allocation of 32 bytes reaches it.

Reproduced -- the answer is cleared and filled untested, as the bytes do.

## `CHAT_HandleInputKey`'s conversion buffer can be overrun

`chat.cpp`, CHAT_HandleInputKey. **Argued 2026-09-24** at the
unit's close. **REACHABLE; the original can crash.**

The "/players" reply is written back into the 260-byte stack buffer the line
was converted into, and a long enough line carries it past the buffer's end.
The chat box's own limits -- the 255-character count and the three-line width
gate, at the stock chat font's widths (`fontingamechat.tbl`) and D2MOO's
reading of D2Win's splitter -- do not prevent it. Read in the bytes, the stock
font table and D2MOO, not in a running game. Two quieter over-reads of the
same buffer (the 255-byte message copy, past the text's NUL and never sent;
an unterminated conversion, possible only in a double-byte client) are
described in the source.

Not reproduced -- undefined behaviour: ours is 512 bytes, zeroed, and the
difference is declared in chat.cpp.

## The "/help" usage line tests a squelch against the length of the line typed

`CHAT_RunBattleNetCommand`, `chat.cpp`. **Argued 2026-09-24** at the
unit's close. **REACHABLE.**

The usage line goes to TEXT_HandleChatMessage as a D2GSPacketSrv26 of which
only the type, the colour and the text are written. The holes are not random:
CHAT_HandleInputKey calls this function and CHAT_ParseCommandPrefix from one
stack depth, and the packet's bytes 0..7 are the two registers the parser's
prologue pushed there (0x6FAD2DB6, 0x6FAD2DB7) -- the handler's count of
leading whitespace and its Unicode::strlen(gChatbuff) (0x6FAD29E6,
0x6FAD28BC), neither written again on the Enter path. So nLang and nUnitType
are 0, and the GUID TEXT_HandleChatMessage hands ROSTER_IsUnitNotSquelched is
the line's length: the line is dropped when the roster member with that GUID
is squelched. Player GUIDs start at 1 and count up (D2MOO, SUnit.cpp:494), so
they are ordinary line lengths.

CHAT_HandleWhisperCommand's usage line lies four bytes lower and its holes are
the parser's saved esi and ebp: nUnitType is the top byte of D2Lang's
Unicode::strlen address, not 0, so no squelch test is made and it always
prints. **HARMLESS.**

Reproduced by a defined route: ours clears the packet and stores the line's
length measured again (gChatbuff is not written in between); the whisper
matcher's zeroed packet tests GUID 0, never issued, and prints as the
original does.

## `CHAT_HandleInputKey`'s wide-character scans test the terminator

`0x6FAD2C1E` and `0x6FAD2CB0`, `chat.cpp`. **Argued 2026-09-24** at
the unit's close. **REACHABLE, on every Enter in an IME language.**

`mov dx, word ptr gChatbuff[eax*2]` with eax the length reads the terminator
on every pass, so neither exit is ever taken. Meant: a line holding a
character above 0xFF skips the prefix parser, and a "/" line holding one is
sent as chat rather than run as a command. Done: every such line is parsed
and run like any other -- "!", "@name" and "/w name" work with CJK text.

This is also why the packet's name is always written:
the first scan's exit is the only path to the send that skips
CHAT_ParseCommandPrefix, which clears the name on entry (0x6FAD2DCD).

Reproduced -- both loops index by the length.

## `CHAT_HandleInputKey` keeps a character it does not count

`0x6FAD2988`, `chat.cpp`. **Argued 2026-09-24** at the unit's close.
**REACHABLE, cosmetic.**

The width is measured again after the store and only the count is withheld,
so a character that makes the third line too wide stays in gChatbuff one past
gnChatInputLength: it is drawn and sent with the line, and one Backspace
removes it with the character before it. One that wraps onto a fourth line is
kept and counted (the recheck asks only `lines != 3 || width < ...`); it is
sent, and CHAT_DrawInputLine, which draws three lines, never shows it.

Reproduced.

## `CHAT_DrawInputLine`'s Chinese IME label is always empty

`0x6FAD218B`, `chat.cpp`. **Argued 2026-09-24** at the unit's close.
**REACHABLE in the two Chinese languages.**

The arm for gnChatImeMode 4 and 5 stores U+6F22 into element 1 and the
terminator into element 2, and nothing into element 0, which D2Lang's
constructor has zeroed (`??_FUnicode@@QAEXXZ`, #10017, `mov word ptr [ecx],
0`). So D2Win is handed an empty string. The writers set 4 and 5 under the
Chinese languages (CHAT_OnImeNotify, CHAT_OnImeStartComposition), so a
Chinese client shows no label for its IME mode.

Reproduced.

## `CHAT_DrawInputLine` tells MultiByteToWideChar 257 for 256 constructed

`0x6FAD228F` against `0x6FAD22A9`, `chat.cpp`. **Argued 2026-09-24**
at the unit's close. **HARMLESS.**

The candidate buffer is constructed 256 wide and the conversion told 257. Its
frame slot is 516 bytes, shared with the text path's 257-wide buffer, so a
257th character lands inside the frame. Ours is declared 257 so that the call
stays defined; the one extra construction stores a zero nothing reads.

## `CHAT_OnImeComposition` measures a full composition past its buffer

`chat.cpp`. **Argued 2026-09-24** at the unit's close. **REACHABLE,
HARMLESS.**

MultiByteToWideChar handed a composition longer than its 32 wide characters
writes all 32 with no terminator and fails (122; a ctypes probe, code pages
1251, 936 and 932), so the Unicode::strlen that follows reads on into the
multibyte buffer above the local, terminated at [199]. The length is only
compared with 31 and 32, and every value from 32 up takes the same arm: the
strncpy of 31, the terminator by hand and the beep. The database's comment
called that arm unreachable; corrected at this close.

Reproduced; in ours the strlen reads on into our own frame, and the same arm
runs.

## `CHAT_OnImeNotify` uses FOG_Alloc's answer untested

`0x6FAD1BE7` and `0x6FAD1CF7`, `chat.cpp`. **Argued 2026-09-24** at
the unit's close. **LATENT.**

The candidate-list refill (lines 229, 231 and 236, inlined twice) hands
FOG_Alloc's answer to ImmGetCandidateListA and reads `[ecx+4]` from it. The
size is the IME's own count, tested non-zero first, so only a failed
allocation reaches it -- the Fog path argued in "Roster.cpp uses FOG_Alloc's
answer untested at seven sites".

Reproduced.

## Two chat.cpp copies run past their buffers and do no harm

`CHAT_Update` and `CHAT_HandleWhisperCommand`, `chat.cpp`. **Argued
2026-09-24** at the unit's close. **UNREACHABLE with a stock server, and
harmless if hit.**

CHAT_Update copies the sender's name without bound into two 340-byte
buffers; a Battle.net name is a character and an account of at most fifteen
characters each, and even a 463-character name runs only into wszName and
wszLine, which are written again before they are read. CHAT_HandleWhisperCommand
copies the target name to +259 of a 276-byte block, and a name of 17
characters or more runs into the buffer it is read from, 17 bytes behind the
read; that path is taken only when the name matches a party member's, which
is at most fifteen characters.

Written with room for both, declared.

## `CHAT_Update`'s name comparison always matches

`0x6FAD3138..0x6FAD3161`, `chat.cpp`. **Argued 2026-09-24** at the
unit's close. **HARMLESS.**

The comparison runs only when the name cut at its first '*' begins with '*',
which happens only when the whole name was copied, so both sides are the text
after the same star of the same string. "*account" prints as it is,
"char*account" as "char (*account)", and a name with no star as
"name (*name)".

Reproduced.

## `HIREABLES_PanelLoadHirelingIcons`' expansion loop is bounded by the names, not the icons

`0x6FADA86D..0x6FADA898`, `Hireables.cpp`. **Argued 2026-09-24** at
the unit's close. **UNREACHABLE.**

The second loop starts at row 5 of a `char [12][32]` name table and at
`&gpClassIcons[5]`, and runs while its count is below 7 (`cmp ebx, 7 / jl`,
0x6FADA895): the bound fits the table's twelve rows, not the icon array's
seven slots, and index 7 would store a handle over gnPartyPortraitMode
(0x6FBB3690). What stops it is the empty-name break (`repne scasb`, `dec ecx
/ jz`, 0x6FADA87D..0x6FADA882), and row 7 is always empty: the function
builds the table itself from seven literals and zeroes rows 7..11 (`rep
stosd` of 28h dwords, 0x6FADA835). Two loads, every time.

Reproduced.

## `HIREABLES_PanelLoadHirelingIcons` clears FOG_AllocPool's answer untested

`Hireables.cpp`. **Argued 2026-09-24** at the unit's close.
**LATENT.**

    6FADA8BF  call FOG_AllocPool   no pool, 20 * nPetTypeTxtRecordCount bytes, line 150
    6FADA8D5  rep stosd            over the answer, untested

The path argued in "CUnitEvent.cpp clears FOG_AllocPool's answer untested":
Fog reports a failed allocation and returns the NULL, and what follows
depends on the executable's error callback. Only a failed allocation of 400
bytes (the stock table's 20 rows) at the UI's initialisation reaches it.

Reproduced.

## `HIREABLES_PanelGetHirelingIcon` reads the pet-icon cache untested

`0x6FADAAA2..0x6FADAAE5`, `Hireables.cpp`. **Argued 2026-09-24** at
the unit's close. **LATENT.**

gpPetIconCache is read and indexed with no NULL test, both for the lookup and
for the answer. It is read only after ROSTERPETS_GetAssociatedUnitGUID has found a
pet of the type for the unit (0x6FADAA45), so it needs a pet record while no
cache exists: the cache is allocated by HIREABLES_PanelLoadHirelingIcons
(UI_InitUiSubsystems, reached from SCMD_HandleSrv01_GameFlags) and freed
by HIREABLES_PanelFreeHirelingIcons (UI_ShutdownUiSubsystems, from GAME_AppModeGame),
and the pet list is filled by the pet packet (ROSTERPETS_OnPetPacket81) and
emptied by ROSTERPETS_FreeAll. That no frame draws the strip outside that
window is not established here; a failed allocation would already have
faulted in HIREABLES_PanelLoadHirelingIcons.

Reproduced.

## `HIREABLES_PanelDrawIconNumberClipped` measures its count before it sets its font

`0x6FADB565..0x6FADB58A`, `Hireables.cpp`. **Argued 2026-09-24** at
the unit's close. **LATENT: it needs font 42 current and a count of 20 or
more.**

The cut to 46 pixels runs in whatever font is current, and
D2WIN_10127_SetFont(6) comes after it; the unit's three other label drawers
set font 6 first. UIPANELS_DrawInGamePanels sets no font before the strip
(ui_panels.cpp), so the font is the one the last drawer left. The digits'
widths in the stock fonts (`data\local\font\latin\*.tbl`, read by D2MOO's
D2WinFont.h layout -- the width is the sum of each character's byte at +3):
a two-digit count is at most 14 pixels in font 6 and 44 in font 30, and
passes 46 only in font 42, whose widest digit is 29 -- "20" is 50 pixels
and loses its last digit. A counted pet kind reaches 20 (stock Skills.txt:
Revive's petmax is `lvl`), but the two places the rewritten sources set font 42 are
the disconnect screen and the Diablo-killed screen (Game.cpp), drawn in place
of the game; the unwritten code was not searched.

Reproduced -- the cut before the font, as the bytes have it.

## `SKILLS_ClientCastAtTarget` leaves a corpse's STATE_CORPSE_NOSELECT off when the start refuses

`0x6FABA706 -> 0x6FABA790`, `Skills.cpp`. **Argued 2026-09-25** at
the unit's close. **NOT ESTABLISHED.**

The replay of a cast (server packets 0x99 and 0x9A) lifts
STATE_CORPSE_NOSELECT from a dead target around the skill's start function,
for a skill with Skills.txt flag 34, ItemCltCheckStart. When the start
answers 0 the `jz` at 0x6FABA706 goes to 0x6FABA790, past the second
STATES_ToggleState (0x6FABA710..0x6FABA715): the path target and the
UNITFLAG_SKSRVDOFUNC bit are restored, the state is not.

In stock 1.10 the flag is set on two rows only, Corpse Explosion (74) and
Poison Explosion (83), both `cltstfunc` 21 (a scan of Skills.txt). That start
(0x6FAC8530) answers 0 with no used skill, with no path target, or, for the
local player, on a target D2Common #11021 refuses -- D2MOO's duplicate of
SKILLS_CanUnitCorpseBeSelected: a monster in MONMODE_DEAD, without the UDEAD
state mask, whose MonStats2 row carries CorpseSel. SKILLS_RunCltStFunc can
also answer 0 before the start, at its line-of-sight, corpse, ally and town
gates. **What would settle it**: whether D2Game replays either skill (its
item-skill casts, the only senders of 0x99/0x9A) onto a corpse whose client
copy fails one of those tests while it carries STATE_CORPSE_NOSELECT.

Reproduced.

## `SKILLS_ClientCastAtTarget` hands the caster's `pDynamicPath` to D2Common untested

`0x6FABA660`, `Skills.cpp`. **Argued 2026-09-25** at the unit's
close. **LATENT.**

#10175, #10176 and #10170 get `pUnit->pDynamicPath` for whatever unit type the
packet named; for an object, an item or a tile the slot holds the static path.
D2Game queues 0x99 and 0x9A only in D2MOO's SKILLITEM_CastSkillOnTarget and
SKILLITEM_CastSkillOnPosition (D2Game SKILLS/SkillItem.cpp, the callers of
sub_6FCC60D0 and sub_6FCC6150), and only after SKILLITEM_HandleItemEffectSkill
-- the server's side of this function -- has run on the same caster and handed
its pDynamicPath to #10175 and #10176 (SkillItem.cpp:1959, 1964).

Reproduced.

## `SKILLS_GetPathTargetPos` hands `pDynamicPath` to D2Common untested for any unit

`0x6FABA9D3`, `Skills.cpp`. **Argued 2026-09-25** at the unit's
close. **NOT ESTABLISHED.**

With no path target the path's first point is read through #10175 and #10176
from `pUnit->pDynamicPath`, whatever the unit's type -- after
CUNIT_GetPathTargetUnit has read the same slot (CUnit.cpp). Every site in
Skills.cpp, called or inlined, hands it a skill's caster or, from
SKILLS_SpawnClientEffectScatter's two hit functions, a missile. **What would
settle it**: its 41 call sites in 40 functions outside the unit (a bridged
xref list), all in units not yet rewritten -- among them
CLIENT_SetUnitRoomAndPos (0x6FB4B68F), which lets a unit through by its
animation mode (2, 3 or 6, `[esi+10h]`) and not by its type. (It is
PLAYER_SetUnitRoomAndPos since Player.cpp's batch 5, 2026-09-28, which
wrote it. **Settled for that caller at Player.cpp's close**: its one
caller, UNITMODE_SyncUnitPosition, hands it players only -- any other type
goes to #10230 (UnitMode.cpp) -- and a player holds its dynamic path from
PLAYER_InitClientUnit to PLAYER_FreeClientUnit. The other call sites stand
as above.)

Reproduced.

## `SKILLS_GetPosInFrontOfUnit` does not bound its direction index

`0x6FABA878`, `Skills.cpp`. **Argued 2026-09-25** at the unit's
close. **LATENT.**

`UNITS_GetDirection(pUnit) & 0FFh`, shifted right by 3, indexes two
eight-entry step tables on the stack with no bound: a direction of 64 or more
reads past the X table into the Y table and past that. In D2MOO's D2Common
every store into a dynamic path's direction goes through
PATH_NormalizeDirection, `% PATH_NB_DIRECTIONS` (Path.cpp:804..858, 1079),
PATH_GetDirection answers 0 with no path, and the one store into a static
path's direction is 0 (Units.cpp:586). The index stays in 0..7. The eight
functions of Skills.cpp that inline the table carry the same read (a bridged
scan for UNITS_GetDirection followed by `shr 3`).

Reproduced.

## Ten Skills.cpp functions test `pUnit` for NULL, or leave it untested, where no caller can pass NULL

`Skills.cpp`. **Argued 2026-09-25** at the unit's close.
**UNREACHABLE** -- the NULL arms cannot run.

    SKILLS_UnitMatchesTargetMask       0x6FABB3C7  its one caller returns on a NULL unit (0x6FABB5A5)
    SKILLS_SpawnCltMissileStraight     0x6FABAFDD  read untested; its one caller is RunClientDoFunc
    SKILLS_CltStFunc3                  0x6FABBA0C  after its own CUNIT_GetPathTargetUnit
    SKILLS_CltDoFunc1, CltDoFunc2      the flag    only RunClientDoFunc reaches slots 1 and 2
    SKILLS_RunCltStFunc                0x6FABC2CD..0x6FABC53C  after 0x6FABC01B, on every path
    SKILLS_RunClientDoFunc             0x6FABC8B9, 0x6FABC911  neither caller passes NULL
    SKILLS_CltStartSkillOnUnit         0x6FABD2B0  behind `&&` on UNITS_CanDualWield
    SKILLS_PostSkillStatWithRemoveCallback 0x6FABDD24  a `cltstfunc` handler
    SKILLS_ApplyAttackRateStat         0x6FABE205  a `cltstfunc` handler

The arguments. CUNIT_GetPathTargetUnit (CUnit.cpp) reads `pUnit->pDynamicPath`
with no test, so a NULL unit faults there before any later test.
gpSkillCltStFuncs is read by SKILLS_RunCltStFunc alone (0x6FABC41D), which
has made that read at 0x6FABC01B. Slots 1 and 2 of gpSkillCltDoFuncs are
indexed in stock 1.10 by Skills.txt `cltdofunc` alone (no `cltprgfunc`,
ItemCltEffect or States.txt `cltactivefunc` holds 1 or 2 -- a scan), so only
SKILLS_RunClientDoFunc reaches them, and of its two callers
SKILLS_RunUsedSkillClientDoFunc hands the unit first to UNITS_GetUsedSkill,
which asserts it (D2MOO, Units.cpp:75), and SKILLS_ClientCastAtTarget to
CUNIT_GetPathTargetUnit (0x6FABA657), after its wrappers have tested it.
UNITS_CanDualWield answers FALSE for NULL (D2MOO, Units.cpp).

Reproduced -- each test where the bytes make it.

## `SKILLS_ForEachNearbyTarget`'s room box cannot reject a room

`0x6FABB6D1..0x6FABB6FC`, `Skills.cpp`. **Argued 2026-09-25** at
the unit's close. **INERT.**

Each axis skips a room only when the centre is more than nRadius before the
room's subtile rectangle AND more than nRadius past its end (`jge` past the
second compare, then `jg`), which no rectangle of non-negative width allows:
`||` where an overlap test needs `&&`. The test passes every adjacent room,
and every unit of every room is then held to the squared radius
(PATH_ComputeSquaredDistance) before the filter or the visitor sees it -- the
per-unit test decides alone. The cost is the walk of rooms that test empties.

Reproduced.

## `SKILLS_RunCltStFunc` reads a MonStats2 row and a missiles.txt row with no NULL test

`0x6FABC351`, `0x6FABC62E`, `Skills.cpp`. **Argued 2026-09-25** at
the unit's close. **LATENT.**

A player's Attack on a monster reads `nHeight` of DATATBLS_GetMonStats2TxtRecord's
answer with no test (address 0x0B for NULL), and the cast light reads `nLight`
of `cltmissile`'s missiles.txt row with no test (address 0x130 for NULL). In
stock 1.10 all 704 monstats.txt rows with an hcIdx name a MonStats2.txt row in
MonStatsEx (605 ids), and every `cltmissile` of Skills.txt names a
Missiles.txt row (scans; the monster's class comes from the server).

Reproduced.

## `SKILLS_CltAlternateDualWieldHand` hands the Attack skill to asserting getters untested

`0x6FABD3AE..0x6FABD3B6`, `Skills.cpp`. **Argued 2026-09-25** at
the unit's close. **LATENT.**

SKILLS_GetSkillById(pUnit, SKILL_ATTACK, -1) goes untested to SKILLS_GetParam4
and SKILLS_SetParam4, which assert their skill (D2MOO, D2Skills.cpp:2193,
2225). Both callers ask UNITS_CanDualWield first, which passes barbarians,
assassins, Shadow Warriors and Shadow Masters only (D2MOO, Units.cpp). A
player gets Attack when its skill list is made: the client calls
SKILLS_InitSkillList at 0x6FB488E0, and D2MOO's body adds skill 0 for any
class in CharStats. Both shadows carry Attack in monstats.txt's Skill7; that
the client's copy of a shadow is given its monstats skills was not read.

Reproduced.

## `SKILLS_ApplyClientSkillEffect` indexes the `cltprgfunc` columns with an unbounded charge count

`0x6FABCF44`, `Skills.cpp`. **Argued 2026-09-25** at the unit's
close. **LATENT.**

nCharges comes from packet 0xA3 as a byte (0..255), and nCharges - 1 picks a
`cltprgfunc` column with no bound: 0 reads the column at -1, wCltDoFunc, and
4 reads wStSound, each as a gpSkillCltDoFuncs index; the PrgStack loop indexes
the table with the lower columns, each tested only for a NULL slot. D2Game's
one sender of a progressive skill, sub_6FCF77E0 (SKILLS/SkillAss.cpp), clamps
the count to 1..3, and the six PROGRESSIVE rows of stock Skills.txt are all
its. The three PrgStack rows -- Fists of Fire, Claws of Thunder, Blades of Ice
-- carry `cltprgfunc` 9, 10 and 11 or none (a scan).

Reproduced.

**DECLARED DIFFERENCE, 2026-10-12.** A column value below 0 or past the table's 130 slots answers
NULL in our C (Skills.cpp's SKILLS_GetCltDoFuncOrNull), where the bytes read
the original's .data and call what they find: the PrgStack loop calls nothing
for that charge, as for an empty slot. LATENT, as above.

## `SKILLS_SpawnMissileArray` lays more missiles than asked for most counts

`0x6FABD6E3`, `Skills.cpp`. **Argued 2026-09-25** at the unit's
close. **LATENT.**

The ring loop leaves only at a count of exactly 0 (`test ebp, ebp / jz`), the
quadrant loop at 0 or below (`jle`, 0x6FABD6ED); a ring that takes the count
below 0 stops its quadrants, and every ring after it still lays its axis
missile. Of the counts the assert allows only 1, 10, 19, 28 and 37 lay as
many as asked: 2 lays 10, 18 lays 24, 40 lays 44 (a scratch transcription of
the branches, its controls 1 and 10). D2Game's twin has the same `jz` (D2Game
0x6FD142C3). The callers: MISSILE_CltHitFunc46 takes calc4 of the missile's
skill, evaluated and floored at 1; its one stock missile, catapultchargedball
(Id 407), is named only by Catapult Charged Ball (Skills.txt 287), whose calc4
is 0xFFFFFFFF in skills.bin -- D2MOO's SKILLS_EvaluateSkillFormula answers 0
for it, so the count is 1. MISSILE_CltHitFunc51 takes its missile's
`cHitPar1`: 10 for lightingtrailingjavalin (431) and 18 for
advlighttrailingjav (438), which no column of Skills.txt or Missiles.txt
names, and whose enum names no D2MOO source uses.

Reproduced.

## `SKILLS_SpawnScatteredMissiles` gives each missile the next corner's facing

`0x6FABD936`, `0x6FABDA0E`, `Skills.cpp`. **Argued 2026-09-25** at
the unit's close. **NOT ESTABLISHED.**

The corner is advanced (`inc ebp`) before it is multiplied by 16 for the
init callback's argument, so the missile sent along corner d gets the facing
16 * ((d + 1) % 4); by D2MOO's PATH_GetDirectionVector the four offsets point
at directions 32, 15, 0 and 47, and the facings given are 16, 32, 48 and 0.
It runs on every call: five call sites, counts 1 and 5
(STATES_CltActiveFunc63, Skills.txt `cltdofunc` 63) and 3, 2 and 3
(SKILLS_SpawnCorpseExplosionEffect). **What would settle it**: the missiles
those callers put in the block, how many directions each draws, and whether
anything re-aims a missile after MISSILE_ClientSpawn's init callback.

Reproduced.

## `SKILLS_AddLevelsToSkillPage` loops for ever on a skill it cannot resolve

`0x6FABDEDB..0x6FABDF38`, `Skills.cpp`. **Argued 2026-09-25** at
the unit's close. **LATENT.**

The next skill is taken at the top (0x6FABDEBE) and moved into ebx only at
0x6FABDFCB; the id's two bounds, the skills.txt row's NULL test, the skilldesc
id's two bounds and its row's NULL test jump to 0x6FABDFCF, past that move, and
`test ebx, ebx / jnz` goes round with the same skill. SCMD_HandleSrv93 looks
the unit up as a player only. A player's list holds the default skills (0..5
and 217..220), its class's -- all 210 class rows of stock Skills.txt name a
SkillDesc.txt row -- and its items': every property that carries a skill, the
event ones included, in UniqueItems, SetItems, Sets, Runes, MagicPrefix and
MagicSuffix names a skill that has one (scans).

Reproduced.

## `SKILLS_CltDoFunc17`, 18 and 20 refuse on an answer SKILLS_GetPathTargetPos never gives

`0x6FABE8B7`, `0x6FABEA81`, `0x6FABEE95`, `SkillsAma.cpp`. **Argued
2026-09-25** at the unit's close. **UNREACHABLE** -- the three `return 0` arms
cannot run.

Each tests SKILLS_GetPathTargetPos' answer (`test eax, eax / jz`) and refuses
on 0. That function answers 1 on every path: `mov eax, 1` at its early exit
(0x6FABAB09), and ebx, loaded with 1 for the step table (0x6FABAA33), at the
other three (Skills.cpp). Binary Ninja leaves all three tests out of its HLIL.

Reproduced.

## `SKILLS_CltDoFunc17` and 18 test `pUnit` for NULL where no caller can pass NULL

`SkillsAma.cpp`. **Argued 2026-09-25** at the unit's close.
**UNREACHABLE** -- the NULL arms cannot run.

    SKILLS_CltDoFunc17   0x6FABE7F8  guards the flag; the rest takes pUnit untested
    SKILLS_CltDoFunc18   0x6FABEA5D  guards the flag; the rest takes pUnit untested

Slots 16..22 of gpSkillCltDoFuncs are indexed in stock 1.10 by Skills.txt
`cltdofunc` alone: no `cltprgfunc1..3` or `ItemCltEffect` value is 16..22, and
States.txt's `cltactivefunc` holds only 85..90 (a scan of the stock tables).
So only SKILLS_RunClientDoFunc reaches them, and neither of its callers can
pass NULL ("Ten Skills.cpp functions ...", above). SKILLS_CltDoFunc16 and
19..22 read pUnit untested on the same argument.

Reproduced -- each test where the bytes make it.

## `SKILLS_CltDoFunc21` tests for NULL a target it has already refused as NULL

`0x6FABF180..0x6FABF184`, `SkillsAma.cpp`. **Argued 2026-09-25** at
the unit's close. **UNREACHABLE.**

The step's target reaches `test esi, esi` (0x6FABF180) only past a refusal of
NULL: the stored target is tested at 0x6FABF133, before UNITS_IsInMeleeRange,
and the searched one at 0x6FABF15E. So `or esi, 0FFFFFFFFh` (0x6FABF184), the
id -1 for a missing target, never runs. D2Game's SKILLS_SrvDo013_Fend_Zeal_Fury
keeps the same test behind the same refusal (0x6FCF4889, 0x6FCF498E).
SKILLS_CltDoFunc20's same test (0x6FABEFAE) is live: there the shot's target
can be NULL.

Reproduced.

## Five SkillsAss.cpp functions test `pUnit` for NULL where no caller can pass NULL

`SkillsAss.cpp`. **Argued 2026-09-25** at the unit's close.
**UNREACHABLE** -- the NULL arms cannot run.

    SKILLS_CltDoFunc3         0x6FABF55E  guards the flag; pUnit went to CUNIT_GetPathTargetUnit first
    SKILLS_CltDoFunc5         0x6FABF893  guards the flag; four calls after it take pUnit untested
    SKILLS_CltDoFunc6         0x6FABFB72  guards the flag's clear; three calls before it
    SKILLS_CltDoFunc7         0x6FABFD16  guards the flag; three calls before it
    SKILLS_StartChannelState  0x6FABFA11  arms the no-unit type and id; STATES_CheckState before it

Slots 3..8 of gpSkillCltDoFuncs are indexed in stock 1.10 by Skills.txt
`cltdofunc` alone: no `cltprgfunc1..3` or `ItemCltEffect` value is 3..8
(`ItemCltEffect` holds 10 and 96), and States.txt's `cltactivefunc` holds
only 85..90 (scans of the stock tables). So only SKILLS_RunClientDoFunc
reaches them, and neither of its callers can pass NULL ("Ten Skills.cpp
functions ...", above). SKILLS_StartChannelState is reached only through
SKILLS_CltStFunc8's `jmp`, slot 8 of gpSkillCltStFuncs, which
SKILLS_RunCltStFunc alone reads, after its own use of pUnit (the same entry).

Reproduced -- each test where the bytes make it.

## `SKILLS_CltDoFunc6`, 8, `SKILLS_CltDoFunc9`, 14 and 15 refuse on an answer SKILLS_GetPathTargetPos never gives

`0x6FABFB96`, `0x6FABFD93`, `0x6FAC01DD`, `0x6FAC0B49`, `0x6FAC0BEB`,
`SkillsAss.cpp`. **Argued 2026-09-25** at the unit's close.
**UNREACHABLE** -- the five `return 0` arms cannot run.

Each tests SKILLS_GetPathTargetPos' answer (`test eax, eax / jnz`, the refusal
falling through) and refuses on 0, and that function answers 1 on every path
("`SKILLS_CltDoFunc17`, 18 and 20 refuse ...", above). Binary Ninja leaves
the test out of its HLIL in 6, 8, 14 and 15.

Reproduced.

## `SKILLS_StartChannelState` would drop the list it has just allocated on a unit without the skill

`0x6FABFA4B`, `SkillsAss.cpp`. **Argued 2026-09-25** at the unit's
close. **UNREACHABLE.**

A new channel allocates its stat list (STATLIST_AllocStatList, filter 2,
expire frame 14h) and only then asks D2Common's
SKILLS_GetHighestLevelSkillFromUnitAndId for the unit's skill of this id;
when that answers NULL (`test eax, eax / jnz`, 0x6FABFA47) the function
returns 0 at 0x6FABFA4B without posting or freeing the list. It cannot answer
NULL there. The function is reached only as Blade Fury's start (`cltstfunc`
8), from SKILLS_RunCltStFunc, whose step 4 makes the skill of nSkillId the
unit's used skill -- found in its list or added to it -- and answers 0 before
any start function when it cannot (Skills.cpp); and D2MOO's
SKILLS_GetHighestLevelSkillFromSkillId (D2Common D2Skills.cpp) takes the
first skill of the list with that id. D2Game's twin, SKILLS_SrvSt26_BladeFury,
asks for the skill before it allocates (0x6FCF9408).

Reproduced.

## `SKILLS_CltDoFunc11` and 83 never leave their ring loop on a step of 0 or less

`0x6FAC0461..0x6FAC0469`, `0x6FAC06C1..0x6FAC06C9`, `SkillsAss.cpp`.
**Argued 2026-09-25** at the unit's close. **LATENT.**

The loop's one exit is the byte offset reaching 100h (`add esi, edi / cmp
esi, 100h / jl`), stepped by four times the step: a step of 0 keeps it at 0
for ever and hangs the client, a negative one walks back through the memory
before the table. The step is SKILLS_EvaluateProgressiveSkillCalc's, or the
row's `aurarangecalc` when that is 0 or less. In stock 1.10 slot 11 is named
only by Claws of Thunder's `cltprgfunc3` and slot 83 only by Royal Strike's
`cltprgfunc2` (scans of `cltdofunc`, `cltprgfunc1..3` and `ItemCltEffect`).
Claws of Thunder is PrgStack, so SKILLS_ApplyClientSkillEffect sets its
charge stat to 3 before it calls the third column (Skills.cpp), and the step
is `prgcalc3`, `par1` = 4. Its `prgcalc1`, `prgcalc2` and `aurarangecalc` are
all empty, so a count read as anything but 3 would take an empty calc for the
step -- 0, if it is compiled to the -1 the SKILLS_SpawnMissileArray entry
above measured, which D2MOO's SKILLS_EvaluateSkillFormula answers 0 for.
Royal Strike is not PrgStack: its count is the one the server last sent, and
whichever calc the ladder picks -- `prgcalc1` empty, `prgcalc2` `par6` = 10,
`prgcalc3` `par5` = 16 -- a result of 0 or less falls back to
`aurarangecalc`, `par1` = 8.

Reproduced.

## `SKILLS_CltDoFunc11` and 83 hand MISSILE_SetTargetX a missile they did not test

`0x6FAC045B`, `0x6FAC06BB`, `SkillsAss.cpp`. **Argued 2026-09-25**
at the unit's close. **REFUTED.**

MISSILE_ClientSpawn's answer is pushed to D2Common's MISSILE_SetTargetX with
no test between. The callee makes the test itself: it returns on a NULL unit
(`test eax, eax / jz`, 0x6FDBA494), on a unit that is not a missile (`cmp
dword ptr [eax], 3`, 0x6FDBA498) and on one without missile data
(0x6FDBA4A0), and only then stores (0x6FDBA4A8). A spawn that fails costs the
call and nothing else.

Reproduced.

## Nine SkillsBar.cpp sites in eight functions test `pUnit` for NULL where no caller can pass NULL

`SkillsBar.cpp`. **Argued 2026-09-26** at the unit's close.
**UNREACHABLE** -- the NULL arms cannot run.

    SKILLS_CltDoFunc38           0x6FAC0D60  guards the flag; CUNIT_GetPathTargetUnit read pUnit first
    SKILLS_CltDoFunc40           0x6FAC1043  the same
    SKILLS_CltDoFunc41           0x6FAC10DC  the same
    SKILLS_CltStFunc29           0x6FAC1384  after UNITS_GetUsedSkill, which asserts it
    SKILLS_CltStFunc30           0x6FAC2238  after D2Common's skill lookup; NULL would hear sound 13h
    SKILLS_CltDoFunc43           0x6FAC1E19  after D2Common's skill lookup
    SKILLS_CltDoFuncLeap         0x6FAC1E67, 0x6FAC1E84  both callers have read through it
    SKILLS_ApplyDefaultCollision  0x6FAC1CE9  both callers have read through it

Slots 38..45 of gpSkillCltDoFuncs are named in stock 1.10 by Skills.txt
`cltdofunc` alone (a scan of `cltdofunc`, `cltprgfunc1..3` and
`ItemCltEffect`; States.txt `cltactivefunc` holds only 85..90), so only
SKILLS_RunClientDoFunc reaches them, and neither of its callers can pass NULL;
gpSkillCltStFuncs is read by SKILLS_RunCltStFunc alone, after its own read of
the unit ("Ten Skills.cpp functions ...", above). D2Common's
SKILLS_GetHighestLevelSkillFromUnitAndId reads `pUnit->pSkills` first (D2MOO's
SKILLS_GetHighestLevelSkillFromSkillId, D2Skills.cpp), and UNITS_GetUsedSkill
asserts `hUnit` and exits (0x6FDBD674..0x6FDBD691). SKILLS_CltDoFuncLeap's
callers are `cltdofunc` 43 and 44, each after that lookup;
SKILLS_ApplyDefaultCollision's are SKILLS_RestoreCollisionFootprint, which reads
`pUnit->pDynamicPath` untested first (0x6FAC1C04), and SKILLS_CltDoFuncLeap,
whose own callers have read through the unit.

Reproduced -- each test where the bytes make it.

## `SKILLS_CltDoFunc39` tests the unit's path after CUNIT_GetPathTargetUnit has asserted it

`0x6FAC0E33`, `0x6FAC0EC3`, `SkillsBar.cpp`. **Argued 2026-09-26** at
the unit's close. **UNREACHABLE.**

Both arms test `pUnit->pDynamicPath` for NULL after CUNIT_GetPathTargetUnit
(CUnit.cpp) has been handed the unit, and that function asserts the path
("hPath") and exits without one. The tests cannot fail.

Reproduced.

## `SKILLS_ResolveClientTargetPoint` keeps an arm for a try count that cannot occur

`0x6FAC19D6..0x6FAC19D8`, `SkillsBar.cpp`. **Argued 2026-09-26** at
the unit's close. **UNREACHABLE.**

The count is set to 3 (0x6FAC1865) and decremented only by the loop, so it
runs 3, 2, 1, 0; the arm for any other value -- back to the room lookup
without storing the point (`test edx, edx / jnz`, to 0x6FAC1875) -- cannot run.
D2Game's twin, SKILLS_FindLeapTargetPosition, carries the same arm
(0x6FCFD242..0x6FCFD244), which D2MOO's `while (nCounter)` keeps.

Reproduced -- the C writes the arm as the loop going round again with the
point it already holds.

## `SKILLS_CltDoFunc42` refuses the one kind of item the server throws besides the throwable ones

`0x6FAC0F7E..0x6FAC0F8B`, `SkillsBar.cpp`. **Argued 2026-09-26** at
the unit's close. **LATENT.**

Double Throw's do function refuses an item ITEMS_CheckIfThrowable calls not
throwable that carries stat 125 -- ItemStatCost.txt's `item_throwable`,
STAT_ITEM_THROWABLE (0x7D) in D2C_ItemStats. Everywhere else the stat makes
an item usable as a throwable: the client's SKILLS_HasAmmoForShot accepts an
item for either (Skills.cpp), and so does the server at every test D2MOO has
(`ITEMS_CheckIfThrowable(...) || STAT_ITEM_THROWABLE`: ItemMode.cpp:1065,
PlrModes.cpp:950, Skills.cpp:2248 and 2306). So such an item would be thrown
by the server and drawn by no client missile.

**Why LATENT -- nothing in stock data grants the stat.** It is given only
through the property whose function D2MOO's ItemMods.cpp:330 maps to it,
Properties.txt `throw`, and no row of any stock excel table uses `throw` (a
scan of every cell of every table; the only hits are the `throw` skilldesc of
Skills.txt and SkillDesc.txt). A mod that adds the property reaches it.

Reproduced.

## Leap's charge move and Whirlwind's start assume a player's class is in charstats.txt

`0x6FAC1AF3` (SKILLS_StartChargeMove) and `0x6FAC25E3..0x6FAC2608`
(SKILLS_CltStFunc31), `SkillsBar.cpp`. **Argued 2026-09-26** at the
unit's close. **LATENT.**

The charge move reads the charstats record's `RunVelocity` at once after an
inlined lookup that answers NULL for a class outside the table -- a fault at
41h -- where its monster arm tests its record; D2Game's twin,
SKILLS_SetVelocityForLeap, reads it the same way (0x6FCFD48D). Whirlwind's
start, for the same class, sets no velocity at all, where the server's velocity
function, D2MOO's sub_6FD15500, answers 600h (0x6FD15579).

**Why LATENT -- the class ids.** The charge move's arm is "anything but a
monster", and only players and monsters use Leap and Leap Attack; Whirlwind's
start takes players and monsters only (0x6FAC24D3..0x6FAC24DC). A player's
`dwClassId` is one of the seven classes, 0..6, which charstats.txt compiles
("The character screen hands a NULL class record's name to D2Win", above).

Reproduced.

## `SKILLS_CltStFunc31` reads a monster's monstats record through NULL

`0x6FAC2647`, `SkillsBar.cpp`. **Argued 2026-09-26** at the unit's
close. **LATENT.**

Whirlwind's start takes a monster's velocity from an inlined
DATATBLS_GetMonStatsTxtRecord (0x6FAC261C..0x6FAC2645, routed in the C) and
reads `Velocity` from the answer with no test -- a fault at 32h for a class
outside monstats.txt -- where the player arm tests its record and the
server's sub_6FD15500 tests both (0x6FD1556D).

**Why LATENT**: every class a client monster can have is checked where it is
written ("`CUNIT_HitTestUnit` and `CUNIT_GetUnitName` read a monster's table
row with no NULL test ...", above).

Reproduced.

## A monster's Leap or Leap Attack leaves COLLIDE_PLAYER at its landing subtile

`0x6FAC165D`, `0x6FAC2279`, `0x6FAC1D2B`, `SkillsBar.cpp`. **Argued
2026-09-26** at the unit's close. **REACHABLE.**

Leap's and Leap Attack's starts mark the unit's collision pattern at the
landing point with COLLIDE_PLAYER whatever the unit (`push 80h`, `cltstfunc` 29
at 0x6FAC165D and 30 at 0x6FAC2279). The landing, SKILLS_ApplyDefaultCollision
from SKILLS_CltDoFuncLeap, clears the footprint mask of the unit's kind --
COLLIDE_MONSTER for a monster (0x6FAC1D2B). So a monster's landing point
keeps the 80h. Whirlwind does not have the defect: its start marks and its end
clears the unit's own mask (0x6FAC259A..0x6FAC25B2, 0x6FAC2735..0x6FAC273C).
The server does exactly what the client does: SKILLS_SrvSt40_Leap and
SKILLS_SrvSt41_LeapAttack mark with 80h (0x6FCFCEC1, 0x6FCFD913), and
SKILLS_Leap's landing writes a monster's values (0x6FCFD5E7..0x6FCFD61C).

**Why REACHABLE**: stock MonStats.txt gives Leap to sandleaper1..7 (78..82,
629, 630) and Leap Attack to ancientbarb3 (542), and every such leap that lands
leaves the bit, on both sides. What reads a stale COLLIDE_PLAYER at that
subtile was not traced.

Reproduced.

## `SKILLS_CltDoFunc44` faces and hears a target the server need not strike

`0x6FAC2344..0x6FAC2397`, `SkillsBar.cpp`. **Argued 2026-09-26** at
the unit's close. **REACHABLE** -- a divergence from the server.

When Leap Attack lands, the client takes param3/param4's unit (the start's
path target), else SKILLS_FindNextNearbyTargetById around the unit -- radius
3, mask 0, the smallest unit id in range (0x6FAC2356..0x6FAC2364) -- makes it
the unit's target (#10344), faces it and plays its hit class's sound. The
server's SKILLS_FindLeapAttackTarget (0x6FCFDC40) takes its own target first
when it is in melee range, then param3/param4's (`cmp ebx, 6`, 0x6FCFDC85),
then a search with D2MOO's sub_6FD15460 radius and mask 20003h
(0x6FCFDC9F..0x6FCFDCB8), writes the one it found back into param3 and
param4, and strikes it.

**Why REACHABLE**: a player's Leap Attack whose start target is gone by the
landing, with more than one unit near, is searched differently on the two
sides; the client's facing and sound can then belong to a unit the server
does not hit. The client computes no damage, so only the facing and the sound
differ. Stock ancientbarb3 uses the skill too.

Reproduced.

## Leap Attack's and Whirlwind's do functions answer otherwise than the server's

`0x6FAC232E`, `0x6FAC27E5`, `SkillsBar.cpp`. **Argued 2026-09-26** at
the unit's close. **INERT.**

`cltdofunc` 44 answers 0 while the leap is in flight, where
SKILLS_SrvDo078_LeapAttack answers 1 (0x6FCFDC25); `cltdofunc` 45 answers 1
without bit 0 and for a dead unit, where SKILLS_SrvDo076_Whirlwind answers 0
(0x6FCFC57C, 0x6FCFC584).

**Why INERT**: the answer reaches only SKILLS_RunClientDoFunc's `bHandled`,
which gates `dosound`, `tgtsound`, `tgtoverlay` and the local player's
`delay` (Skills.cpp) -- all four empty in stock Skills.txt for Leap Attack
(143) and Whirlwind (151) -- and the dispatcher's own answer is discarded by
every caller: SCMD_HandleSrv99 and SCMD_HandleSrv9A through
SKILLS_ClientCastAtUnit and SKILLS_ClientCastAtPosition (SCmd.cpp), and
Skills.cpp's other call.

Reproduced.

## A whirling monster starts its sequence at another frame than on the server

`0x6FAC27D3..0x6FAC27DB`, `SkillsBar.cpp`. **Argued 2026-09-26** at
the unit's close. **REACHABLE** -- a divergence from the server.

With bit 0 alone, `cltdofunc` 45 sets every unit to sequence frame 3 with the
frame counter at 500h. SKILLS_SrvDo076_Whirlwind rewinds a player's skill to
frame 3 (`push 3`, 0x6FCFC59E) and sets every other unit to frame 0 with the
counter at 400h (0x6FCFC5AB..0x6FCFC5B3). D2Common's UNITS_SetAnimationFrame
writes the sequence frame (D2MOO Units.cpp: `dwSeqFrame = (nFrame << 8) +
(dwSeqFrame & 0xFF)`).

**Why REACHABLE**: stock MonStats.txt gives Whirlwind to ancientbarb1 (540);
each time its do function runs with bit 0 alone, the client puts it at frame 3
where the server puts it at 0.

Reproduced.

## `SKILLS_CltDoFunc45`'s end clears the whirl's mark at a point it does not test

`0x6FAC277E`, `SkillsBar.cpp`. **Argued 2026-09-26** at the unit's
close. **LATENT.**

Whirlwind's end (bits 0 and 1) clears the unit's footprint mask at param1 and
param2 with no test of the point, where the server's
SKILLS_RemoveWhirlwindStats clears it only when both coordinates are non-zero
(0x6FCFC404..0x6FCFC412).

**Why LATENT**: on the client, bit 0 is set only by SKILLS_CltStFunc31, in the
same step that stores the whirl path's last point into param1 and param2
(0x6FAC26B5..0x6FAC26CE), and SKILLS_StopWhirlwindSound adds bit 1 only to a
skill with bit 0 (`test al, 1`, 0x6FAC2815). So the point is always a point
PATH_GetPathPoints returned; a 0 would need a path through the map's first row
or column of subtiles.

Reproduced.

---

## `SKILLS_CltStFunc45` refuses Delerium Change, whose row carries no overlay

`0x6FAC2896`, `SkillsDru.cpp`. **Argued 2026-09-26** at the unit's
close. **LATENT.**

`cltstfunc` 45 serves Wearwolf, Wearbear and Delerium Change, and answers 0
when the row's `cltoverlaya` is negative (`test cx, cx / jl`). The stock
compiled table gives Delerium Change -1 in both overlay words, where Wearwolf
has 198 and 199 and Wearbear 200 and 201 (`data\global\excel\skills.bin`, 357
records of 572 bytes after its count, the size checked against the file's).
A start that answers 0 makes SKILLS_RunCltStFunc clear the unit's used skill
and, for a player or a monster, set mode 1 (Skills.cpp), while the server's
do, SKILLS_SrvDo116_Wearwolf_Wearbear, would apply the `delerium` state.

**Why LATENT**: the start runs when a unit starts the skill as an action --
PLAYER_DoClientAction and MONSTER_DoClientAction reach SKILLS_RunCltStFunc -- or on an item cast whose row sets
`ItemCltCheckStart` (SKILLS_ClientCastAtTarget, Skills.cpp). In stock 1.10
the name Delerium Change appears in one other cell of every table, runes.txt
Runeword22's (Delirium) `gethit-skill`: an item cast, and the row does not set
`ItemCltCheckStart`, where Corpse Explosion's and Poison Explosion's do. No
monster and no class has the skill to start.

Reproduced.

## `SKILLS_CltStFunc45` picks the shape's overlay by one state where the server undoes the whole group

`0x6FAC28BD`, `SkillsDru.cpp`. **Argued 2026-09-26** at the unit's
close. **REACHABLE.**

The client plays `cltoverlayb` (wolf_undo, bear_undo) when the unit has the
row's own `aurastate` and `cltoverlaya` (wolf_into, bear_into) otherwise. The
server's do, SKILLS_SrvDo116_Wearwolf_Wearbear, first calls sub_6FD11C90
(D2Game 0x6FCFE3A0), which switches off every state of the aura state's
States.txt `group` the unit has -- the group word at +1Eh of each states.txt
record compared in a walk over all of them (0x6FD11CD8, 0x6FD11D23..
0x6FD11D2B) -- and when it found one, calls sub_6FD11BA0 (D2MOO: the
skill's delay) and answers 0 without applying the new shape
(0x6FCFE3A5..0x6FCFE3BF). In stock States.txt
wolf, bear and delerium share group 3 with maul, feralrage and monsterset.

So a werewolf casting Wearbear is turned back to the druid on the server
while the client, finding no `bear` state, plays bear_into; and a unit under
the Delirium runeword's `delerium` casting Wearwolf loses `delerium` on the
server and plays wolf_into on the client. The client's start runs for every
player starting the skill (PLAYER_DoClientAction). An overlay, nothing more: the states that follow come
from the server.

Reproduced.

## `SKILLS_SpawnSeededMissileVolley`, `SKILLS_CltDoFunc92` and 91 refuse on an answer SKILLS_GetPathTargetPos never gives

`0x6FAC2918`, `0x6FAC310C`, `0x6FAC324D`, `SkillsDru.cpp`. **Argued
2026-09-26** at the unit's close. **UNREACHABLE** -- the three refusals cannot
run.

Each tests SKILLS_GetPathTargetPos' answer (`test eax, eax` / `jnz`) and
returns 0 on 0. That function answers 1 on every path: `mov eax, 1` at its
early exit (0x6FABAB09), and at its other three exits `mov eax, ebx` with ebx
loaded with 1 for the step table (0x6FABAA33) -- re-read at this close. Binary
Ninja leaves all three tests out of its HLIL.

Reproduced.

## `SKILLS_SpawnSeededMissileVolley` fires at a point the server's volley refuses

`0x6FAC2913`, `SkillsDru.cpp`. **Argued 2026-09-26** at the unit's
close. **NOT ESTABLISHED.**

The client takes the point from SKILLS_GetPathTargetPos, which falls back to
the step in front of the unit when either coordinate of the target unit's
position or the path's point is 0, and fires the whole volley there. The
server's twin, sub_6FCFE4C0 (D2Game 0x6FCFE4C0), takes it from
D2GAME_GetXAndYFromTargetUnit_6FD14020 -- the same two sources, no fallback --
which answers 0 when either coordinate is 0 (`test eax, eax / jz`, `cmp dword
ptr [ecx], 0 / jz`, 0x6FD140AC..0x6FD140B3), and then fires nothing
(0x6FCFE5B9); Firestorm's aimed missile goes out first on the server
(0x6FCFE4ED) and is the only one. So with a zero coordinate the client shows
a volley in front of the caster that the server never made.

**What would settle it**: whether a cast of Firestorm, Twister or Tornado can
reach the server with a point or a target on world coordinate 0. Players cast
them at a point or a unit; in stock data only items cast them otherwise --
Flamebellow, Windhammer, Carrion Wind, Horizon's Tornado and Stormlash, each
a `hit-skill` (uniqueitems.txt), cast from the hit event at the unit struck
or at the wielder's path point (D2MOO's SKILLITEM_CastSkillOnTarget and
..OnPosition).
Whether any act places a room at X or Y 0 is D2Common's DRLG's to say, and
nothing in D2Client or the stock tables does.

Reproduced.

## `SKILLS_CltDoFunc67`, 68 and 69 test `pUnit` for NULL where no caller can pass NULL

`0x6FAC2ADB`, `0x6FAC2B8B`, `0x6FAC2C3B`, `SkillsDru.cpp`. **Argued
2026-09-26** at the unit's close. **UNREACHABLE** -- the NULL arms cannot run.

Each tests pUnit before setting UNITFLAG_SKSRVDOFUNC and then hands it
untested to D2Common's #11276 and to SKILLS_SpawnSeededMissileVolley, whose
SKILLS_GetPathTargetPos reads `pUnit->pDynamicPath` through
CUNIT_GetPathTargetUnit (CUnit.cpp). Slots 67..69 of gpSkillCltDoFuncs are named
in stock 1.10 by Skills.txt `cltdofunc` alone -- Firestorm, Twister, Tornado
(a scan of every "clt" column of Skills.txt and States.txt; `ItemCltEffect`
holds only 10 and 96, States.txt `cltactivefunc` only 85..90) -- so only
SKILLS_RunClientDoFunc reaches them, and neither of its callers can pass NULL
("Ten Skills.cpp functions ...", above). SKILLS_GetClientMissileForSkill,
before the test, reads nothing of pUnit for a row without the flag
PROGRESSIVE, and none of the three rows has it (stock Skills.txt, against
Tiger Strike's 1).

Reproduced -- each test where the bytes make it.

## The hurricane state's handler draws three debris per unit of `cltcalc1`

`0x6FAC2D4C`, `SkillsDru.cpp` (SKILLS_CltDoFunc90; States.cpp
until 2026-10-08). **Argued 2026-09-26** at the unit's close. **INERT.**

For each unit of `cltcalc1` the loop calls SKILLS_StatesSpawnHurricaneCltMissile
three times (0x6FAC2D4C..0x6FAC2D76), nine debris a pass for Hurricane's stock
3, where the row's `*cltcalc1 desc` column calls the value "debris/frame".

**Why INERT**: the description column is not data the game reads -- D2MOO's
skills.txt field list (SkillsTbls.cpp) loads `cltcalc1..3` at +114h..+11Ch
and no `*` column -- and the value's only reader is this loop, so the note can
disagree with no behaviour. Whether the designer meant three calls a unit or
one is not something the image records; the game draws nine.

Reproduced.

## Hurricane's debris missiles are made with no skill

`0x6FAC2EDB`, `SkillsDru.cpp` (SKILLS_StatesSpawnHurricaneCltMissile;
States.cpp until 2026-10-08). **Argued 2026-09-26** at the unit's close. **INERT.**

The record is cleared and given the missile, flags 3, the offset, the owner,
the start and the level (0x6FAC2EDB..0x6FAC2F05) and never the skill, so
MISSILE_ClientSpawn hands D2Common's MISSILE_SetSkill a 0 (0x6FB3E8D7..
0x6FB3E8DC) -- where the volley and `cltprgfunc1` 91 and 92 set theirs.

**Why INERT**: Hurricane's three client missiles, hurricaneswoosh,
hurricanerock and hurricanetree, have no client function in stock
Missiles.txt -- no `CltDoFunc`, no `CltHitFunc`, no `Skill` -- only a velocity,
a range and a hit shift, so nothing they run asks for their skill.

Reproduced.

## Hurricane's debris rolls the caster's own seed

`0x6FAC2DA2`, `SkillsDru.cpp` (SKILLS_StatesSpawnHurricaneCltMissile;
States.cpp until 2026-10-08). **Argued 2026-09-26** at the unit's close. **INERT.**

Every piece rolls `pUnit->pSeed` up to three times -- the point
(SEED_RollRandomNumber, 0x6FAC2DA2), the pick when `cltmissileb` is set, and
the height when a missile was made and its formula is above 0 -- nine pieces
a pass for stock Hurricane, and no server function advances that seed for
Hurricane.

**Why INERT**: the client's copy of a unit's seed is not kept equal to the
server's by anything. The server advances its own for rolls the client never
repeats -- a chance to cast (`ITEMS_RollRandomNumber(&pAttacker->pSeed)`,
D2MOO's SKILLITEM_EventFunc21_SkillOnGetHit), Volcano's target byte (SrvDo123,
0x6FCFF7C1..0x6FCFF7FA) -- and where the client needs the server's randomness
the server sends it: 0xA3's seed byte, which SKILLS_CltDoFunc91 reads from
dwInitSeed.

Reproduced.

## `SKILLS_CltDoFunc70`, 71, 72, 76, 81, `SKILLS_CltStFunc49` and `SKILLS_CltDoFunc80` refuse on an answer SKILLS_GetPathTargetPos never gives

`0x6FAC3386`, `0x6FAC35B3`, `0x6FAC383A`, `0x6FAC3EDA`, `0x6FAC4110`,
`0x6FAC4D19`, `0x6FAC4F36`, `SkillsEMon.cpp`. **Argued 2026-09-26**
at the unit's close. **UNREACHABLE** -- the seven refusals cannot run.

Each tests SKILLS_GetPathTargetPos' answer (`test eax, eax` and a `jz`, or
76's `jnz` past its refusal) and on 0 returns 0 -- or, in `cltstfunc` 49,
skips the fan. That function answers 1 on every path, re-read at this close:
`mov eax, 1` at its early exit (0x6FABAB09), and at its other three exits
`mov eax, ebx` (0x6FABAAC2, 0x6FABAADC, 0x6FABAAFA) with ebx loaded with 1
at 0x6FABAA33 and not written again before them. Binary Ninja leaves the
seven tests out of its HLIL.

Reproduced.

## `SKILLS_CltDoFunc70`, 72, 77 and `SKILLS_CltStFunc49` test `pUnit` for NULL where no caller can pass NULL

`0x6FAC338C`, `0x6FAC3840`, `0x6FAC4512`, `0x6FAC3D90`,
`SkillsEMon.cpp`. **Argued 2026-09-26** at the unit's close.
**UNREACHABLE** -- the NULL arms cannot run.

Each tests pUnit before setting or clearing UNITFLAG_SKSRVDOFUNC. The three
do functions have already handed pUnit on untested -- 70 and 72 to
SKILLS_GetPathTargetPos, whose CUNIT_GetPathTargetUnit reads
`pUnit->pDynamicPath` (CUnit.cpp), 77 to D2Common's UNITS_GetUsedSkill, to
SKILLS_SpawnInfernoStream and to #11276 -- and their slots are named in
stock 1.10 by Skills.txt `cltdofunc` alone (Wake Of Destruction Sentry, Imp
Fireball, mon inferno sentry: a scan of every "clt" column of Skills.txt and
States.txt; `ItemCltEffect` holds only 10 and 96, States.txt
`cltactivefunc` only 85..90, and no `cltprgfunc` column holds 70, 72 or
77), so only SKILLS_RunClientDoFunc reaches them, and neither of its callers
can pass NULL ("Ten Skills.cpp functions ...", above). `cltstfunc` 49 tests
before any use, but gpSkillCltStFuncs has one reader, SKILLS_RunCltStFunc
(0x6FABC41D), and every path to that call first hands
pUnit to CUNIT_GetPathTargetUnit, which reads its path with no test
(Skills.cpp, that function's head).

Reproduced -- each test where the bytes make it.

## `SKILLS_CltDoFunc70` and `SKILLS_CltStFunc49` aim at a point their server twins refuse

`0x6FAC337F`, `0x6FAC3ED3`, `SkillsEMon.cpp`. **Argued 2026-09-26**
at the unit's close. **NOT ESTABLISHED.**

The client takes the point from SKILLS_GetPathTargetPos, which falls back to
the step in front of the unit when either coordinate is 0. The server's
twins, SKILLS_SrvDo125_WakeOfDestruction (D2Game 0x6FD004E0) and
SKILLS_SrvDo130_VineAttack (0x6FD01360), take it from
D2GAME_GetXAndYFromTargetUnit_6FD14020, which answers 0 when either
coordinate is 0, and then make nothing (`test eax, eax / jz`,
0x6FD00560..0x6FD00562 and 0x6FD01435..0x6FD01437). So on a zero coordinate
the client draws a wake, or a fan of vines, that the server never made.

**What would settle it**: the volley's question ("`SKILLS_SpawnSeededMissileVolley`
fires at a point ...", above) -- whether a target unit or a path point can
stand on world X or Y 0. Here the casters are monsters aiming at a unit:
wakeofdestruction (MonStats.txt, hcIdx 410) and the druid's three vine
summons, plaguepoppy, cycleoflife and vinecreature (425..427).

Reproduced.

## Imp Inferno's drawn flame is not the server's: two missiles and a spread speed against one missile at `Vel`

`0x6FAC36CE`, `0x6FAC3702`, `SkillsEMon.cpp` (SKILLS_CltDoFunc71).
**Argued 2026-09-26** at the unit's close. **REACHABLE.**

The client makes `cltmissilea`, impinfernoflame1, or on an odd roll of the
unit's seed `cltmissileb`, impinfernoflame2 (0x6FAC36CE..0x6FAC36F8), at the
missile's `Vel` plus gnImpInfernoSpread[roll % 5] -- -2, -1, 0, 1 or 2, from
a second roll (0x6FAC3702..0x6FAC373A). The server's twin,
SKILLS_SrvDo126_ImpInferno (D2Game 0x6FD00710), makes `srvmissilea`,
impinfernoflame1, alone, at `Vel` plus `VelLev` times the level over 8
(0x6FD008C4..0x6FD008DB). Stock impinfernoflame1 and 2 carry `Vel` 20 and
no `VelLev` (Missiles.txt), so the server's flame moves at 20 and the
client's at 18 to 22, and about half of the client's flames are the other
missile.

**Why REACHABLE**: every stock imp has the skill (MonStats.txt: imp1..imp7,
hcIdx 492..496, 688 and 689), and the client's do function runs from a
monster's animation -- MONSTER_UpdateAnimMode, SKILLS_RunUsedSkillClientDoFunc,
SKILLS_RunClientDoFunc. Only what is drawn differs;
the server's flame is the one it moves.

Reproduced.

## Vine Attack's drawn vines fan out in eight directions and to `cltcalc1`, the server's in four and to `calc1`

`0x6FAC3EE4`, `SkillsEMon.cpp` (SKILLS_CltStFunc49). **Argued
2026-09-26** at the unit's close. **REACHABLE** from the skill's level 3.

The client makes `cltcalc1`, min(12,ln12), missiles of `cltmissileb`
(vines), missile i aimed along gnDir8SignX/Y[i % 8] -- (0,1), (0,-1), (1,0),
(-1,0), then the four diagonals (0x6FAC3EE4..0x6FAC3EEF). The server's twin,
SKILLS_SrvDo130_VineAttack (D2Game 0x6FD01360), makes `calc1`, min(24,ln12),
missiles of `srvmissilea` (plague vines) along i % 4 (`and eax, 80000003h`,
0x6FD01441) into D2Game's own two tables, the same four axes. ln12 is
`Param1` 3 plus `Param2` 1 per level above the first, so at levels 1 and 2
both sides make 3 or 4 missiles along the same axes; from level 3 the
client's fifth and later vines go diagonally where the server's repeat the
axes, and from level 11 the server makes more than the client's twelve.

**Why REACHABLE**: Vine Attack is the summoned skill of the druid's three
vines -- Skills.txt rows Plague Poppy, Cycle of Life and Vines (`charclass`
dru), `sumskill1` Vine Attack at `sumsk1calc` lvl, the druid's own skill
level -- which the server gives the pet in
D2GAME_SetSummonPassiveStats_6FD0C530 (D2MOO, SkillNec.cpp), called by
SKILLS_SrvDo115_Vines (SkillDruid.cpp). A druid with any of the three at
level 3 or more sees it.

Reproduced.

## `SKILLS_CltDoFunc82` is reached by no stock row

`0x6FAC4FE0`, `SkillsEMon.cpp`. **Argued 2026-09-26** at the
unit's close. **UNREACHABLE.**

No column that indexes gpSkillCltDoFuncs holds 82 -- Skills.txt `cltdofunc`,
`cltprgfunc1..3` and `ItemCltEffect`, States.txt `cltactivefunc` (a scan of
every "clt" column) -- and the table's readers are SKILLS_RunClientDoFunc,
the two halves of packet 0xA3 and SKILLS_StatesRunActiveStateFuncs, seven indexed
references in all, each indexing by one of those
columns. The body reads Skills.txt row 285, Baal Corpse Explode, by
constant; that row names `cltstfunc` 82, a column of the other table,
gpSkillCltStFuncs, whose count is 70 (0x6FB775A0), and SKILLS_RunCltStFunc
calls no start function at or past the count and goes on as after one that
answered 1 (Skills.cpp). So baalthrone's Baal Corpse Explode (MonStats.txt,
hcIdx 543) draws nothing through this slot, and its work is
SKILLS_SrvDo141_BaalCorpseExplode's on the server (D2Game 0x6FD02950).

Reproduced.

## The cyclers' client missile is owned by the corpse, the server's by the vine's master

`0x6FAC4C73`, `SkillsEMon.cpp` (SKILLS_CltDoFunc93). **Argued
2026-09-26** at the unit's close. **INERT** -- by design.

`cltprgfunc1` 93 makes `cltmissileb`, recycler delay, with the path target
-- the corpse -- as owner and origin (`mov [esp+6Ch+var_58], ebx` and
`..var_54`, 0x6FAC4C73, 0x6FAC4C77: +4 and +8 of D2MissileStrc). The
server's start, SKILLS_SrvSt63_Corpse_VineCycler (D2Game 0x6FD024A0), makes
`srvmissilea` with the corpse as origin and the vine's master
(AIGENERAL_GetMinionOwner) as owner (D2MOO; SkillsEMon.cpp's batch 2).

**Why INERT**: the two missiles are two units run by two codes, and each
owner is read only on its own side. The client's is read by recycler
delay's client tick, MISSILE_CltDoFunc51 (`pCltDoFunc` 51, stock
Missiles.txt): at `CltParam1` (10) frames it makes `CltSubMissile1`,
recycler vine, owned by the owner (CUNIT_GetOwnerUnit, 0x6FB354AB), and at
`CltParam2` (23) it hides the owner -- UNITFLAGEX_NODRAW, `or ebp, 40000h`
(0x6FB355E9..0x6FB35600) -- and makes `CltSubMissile2`, recycler explosion,
owned by it. With the corpse as owner that hides the corpse the vine
consumes; the server's owner would have hidden the vine creature. The field
offsets are D2MOO's (`wCltSubMissile[3]` at +1Eh, `dwCltParam[5]` at +58h,
MissilesTbls.h).

Reproduced.

## `SKILLS_CltDoFunc48`, 53, 54, 56, 58, 60 and `SKILLS_CltDoFunc65` refuse on an answer SKILLS_GetPathTargetPos never gives

`0x6FAC565C`, `0x6FAC5E63`, `0x6FAC5F48`, `0x6FAC6585`, `0x6FAC6A75`,
`0x6FAC6E3F`, `0x6FAC76BD`, `SkillsMon.cpp`. **Argued 2026-09-26**
at the unit's close. **UNREACHABLE** -- the seven refusals cannot run.

Each tests SKILLS_GetPathTargetPos' answer and on 0 returns 0. That function
answers 1 on every path: `mov eax, 1` at its early exit (0x6FABAB09) and
`mov eax, ebx` at its other three (0x6FABAAC2, 0x6FABAADC, 0x6FABAAFA), ebx
loaded with 1 at 0x6FABAA33 -- read at SkillsEMon.cpp's close for its seven
("`SKILLS_CltDoFunc70`, 71, 72, 76, 81, ...", above).

Reproduced.

## Sixteen SkillsMon.cpp functions test `pUnit` for NULL where no caller can pass NULL

`SKILLS_CltStFunc32` `0x6FAC5233`, `SKILLS_CltStFunc33` `0x6FAC5443`,
`SKILLS_CltStFunc39` `0x6FAC6270`, `SKILLS_CltStFunc42` `0x6FAC725B`, and
`SKILLS_CltDoFunc51` `0x6FAC5BB5`, 53 `0x6FAC5E41`, 55 `0x6FAC630D`, 57
`0x6FAC6719`, 58 `0x6FAC69BB`, 60 `0x6FAC6E1D`, 61 `0x6FAC708A`, 63
`0x6FAC74BD`, 66 `0x6FAC7C6B`, 94 `0x6FAC7A4D` and `0x6FAC7AA1`, 95
`0x6FAC7B6D` and `0x6FAC7BC1`, and `SKILLS_CltDoFunc65` `0x6FAC76C5`,
`SkillsMon.cpp`. **Argued 2026-09-26** at the unit's close.
**UNREACHABLE** -- the NULL arms cannot run.

The do functions test pUnit only for UNITFLAG_SKSRVDOFUNC (94 and 95 also
for the monster's component byte) and hand it on untested before or after.
Their slots are named in stock 1.10 by Skills.txt `cltdofunc` alone -- a
scan of every "clt" column of Skills.txt and States.txt at this close:
`cltprgfunc1..3` and `ItemCltEffect` hold none of the numbers, States.txt
`cltactivefunc` only 85..90 -- so only SKILLS_RunClientDoFunc reaches them,
and neither of its callers can pass NULL ("Ten Skills.cpp functions ...",
above). Slot 65, which tests pUnit after SKILLS_GetPathTargetPos has read it,
is named by no stock row at all.

The four starts sit in gpSkillCltStFuncs, whose one reader is
SKILLS_RunCltStFunc (0x6FABC41D), and every path to that call first hands
pUnit to CUNIT_GetPathTargetUnit, which reads its path with no test
(Skills.cpp) -- as SkillsEMon.cpp's close argued for `cltstfunc` 49. `cltstfunc`
39 and 42 test pUnit after their own CUNIT_GetPathTargetUnit has read it,
so their NULL arms are dead inside the function as well.

Reproduced -- each test where the bytes make it.

## `SKILLS_CltDoFunc48`, 53, 54, 56, 58 and 60 aim at a point their server twins refuse

`SkillsMon.cpp`, their SKILLS_GetPathTargetPos calls. **Argued
2026-09-26** at the unit's close. **NOT ESTABLISHED.**

The client takes the point from SKILLS_GetPathTargetPos, which falls back to
the step in front of the unit when either coordinate is 0. Each server
twin takes it from D2GAME_GetXAndYFromTargetUnit_6FD14020, which answers 0
when either coordinate is 0, and then makes nothing (`test eax, eax` and a
jump, D2Game): SKILLS_SrvDo088_AndrialSpray 0x6FD06BBF..0x6FD06BC1,
SKILLS_SrvDo111_FetishAura 0x6FD0AE2E..0x6FD0AE30, SKILLS_SrvDo095_MonInferno
0x6FD07F83..0x6FD07F85, SKILLS_SrvDo152_DiabLight 0x6FD08423..0x6FD08425
(its zero arm resets the mode's end event and answers 0, 0x6FD084B3),
SKILLS_SrvDo102_DiabWall 0x6FD093D2..0x6FD093D5 and
SKILLS_SrvDo105_DesertTurret 0x6FD09A04..0x6FD09A06. So on a zero
coordinate the client draws a spray, an aura, an inferno, a bolt, a wall
or a row of fireballs the server never made.

**What would settle it**: the volley's question ("`SKILLS_SpawnSeededMissileVolley`
fires at a point ...", above), as for SkillsEMon.cpp's two -- whether a
target unit or a path point can stand on world X or Y 0. The casters are
monsters aiming at a unit (MonStats.txt): andariel; fetishshaman1..8 (the
aura and their inferno); baalcrab, baalclone, megademon1..5, venomlord,
frozenhorror1..5 and nihlathakboss for the other infernos; diablo and
diabloclone; turret1..3 and firetower.

Reproduced.

## `SKILLS_CltStFunc36` sets the nest's collision in a NULL room when none holds the point

`0x6FAC5A20`, `0x6FAC5A33`, `SkillsMon.cpp`. **Argued 2026-09-26**
at the unit's close. **INERT.**

With no room for the unit, or none near it holding the point, the room is
NULL (`xor eax, eax`, 0x6FAC5A20) and goes to D2Common's
COLLISION_SetMaskWithPattern. That function reads nothing through it: for
SMALL_UNIT_PRESENCE it hands the room straight to COLLISION_SetCollisionMask
for each subtile (D2Common 0x6FD4370B..0x6FD4372D), which returns at once
for a NULL room (`cmp ecx, ebx / jz`, D2Common 0x6FD434B7..0x6FD434BB). So
nothing is set, and `cltdofunc` 50's reset later finds the same NULL. The
server's twin, SKILLS_SrvSt49_Nest_EvilHutSpawner, sets the presence in the
room its own search finds, NULL alike (D2MOO).

Reproduced.

## `SKILLS_CltDoFunc55`'s second ring wraps its stride for a step of 2^29 or more

`0x6FAC641B`, `SkillsMon.cpp`. **Argued 2026-09-26** at the
unit's close. **LATENT.**

The step is `calc2` evaluated, at least 1; the second pass steps a byte
offset by the step times four (`shl edi, 2`, 0x6FAC641B) while it is below
60, SIGNED. A step at or above 2^29 wraps the stride -- to 0 at 2^30, a
pass that never ends; elsewhere to an offset that reads off
gnDir16SquareX/Y. The server's twin, SKILLS_SrvDo099_PrimePoisonNova, walks
the same way (`shl edi, 2`, D2Game 0x6FD08E62). The one stock row with
`cltdofunc` 55, PrimePoisonNova (mephisto and trap-poisoncloud, MonStats.txt),
has `calc2` the constant 2: no stock data reaches it.

Reproduced.

## `SKILLS_MissileStartSeededPathOrStall` hands its missile's path to D2Common's #10175 untested

`0x6FAC6AE7`, `0x6FAC6AEF`, `SkillsMon.cpp`. **Argued 2026-09-26**
at the unit's close. **UNREACHABLE.**

The path is loaded (`mov ebx, [edi+2Ch]`, 0x6FAC6AE7) and handed to #10175,
which reads it with no test (`mov ax, [ecx+10h]`, D2Common 0x6FDA9DE6). The
function is the init function of `cltdofunc` 58's block, and
MISSILE_ClientSpawn calls it (`call eax` with the block's +54h, +58h in
edx, 0x6FB3EA86..0x6FB3EA92) only after testing the new missile's dynamic
path and removing a missile without one (`mov edi, [esi+2Ch] / test edi,
edi / jnz`, 0x6FB3E771..0x6FB3E776, then CUNIT_RemoveUnitByClientId and
0). Nothing between clears the path. The server's
SKILLS_MissileInit_DiabWall is the same body.

Reproduced.

## `SKILLS_CltDoFunc64` hands SKILLS_GetParam1 a skill it never tested

`0x6FAC75C4`, `0x6FAC75CC`, `SkillsMon.cpp`. **Argued 2026-09-26**
at the unit's close. **NOT ESTABLISHED.**

The skill is D2Common's SKILLS_GetHighestLevelSkillFromUnitAndId for the
dispatched id, and goes untested into SKILLS_GetParam1, which asserts it
(D2MOO, D2Skills.cpp, `D2_ASSERT(pSkill)`). Through
SKILLS_RunUsedSkillClientDoFunc the id is the used skill's own, so the
lookup finds at least that skill. Through SKILLS_ClientCastAtTarget -- the
server's cast packets -- nothing looks the skill up in the unit before the
do (Skills.cpp: the record only).

**What would settle it**: whether the server can send a maggot queen
(maggotqueen1..5, MonStats.txt) a cast of QueenDeath its client unit does
not hold.

Reproduced.

## `SKILLS_CltStFunc44` tests one Hell Meteor launch missile and uses the other

`0x6FAC79BD`, `0x6FAC79CA`, `SkillsMon.cpp`. **Argued 2026-09-26**
at the unit's close. **NOT ESTABLISHED.**

`hellmeteorlaunch2`'s answer is tested (`test edi, edi / jz`,
0x6FAC79C1..0x6FAC79C3) and `hellmeteorlaunch1`'s, never tested, goes to
D2Common's UNITS_SetAnimationSpeed first (0x6FAC79CA), whose three stores
to `[ecx+4Ch]` have no test of the unit (D2Common 0x6FDBF8DE, 0x6FDBF8F0,
0x6FDBF900). A NULL launch1 beside a live launch2 writes at 0x4C.

**What would settle it**: whether MISSILE_ClientSpawn can refuse
hellmeteorlaunch1 (333) and make hellmeteorlaunch2 (334), made one after
the other at the same position for the same owner, skill and level. Its
refusals were not read for this. The one stock caster is hellmeteor
(MonStats.txt).

Reproduced.

## `SKILLS_CltStFunc32` makes the maggot targetable before its move, the server after

`0x6FAC5237`, `SkillsMon.cpp`. **Argued 2026-09-26** at the
unit's close. **NOT ESTABLISHED.**

The client sets UNITFLAG_TARGETABLE, _CANBEATTACKED and _ISVALIDTARGET
(`or al, 0Eh`, 0x6FAC5237..0x6FAC523F) before it moves the maggot, and keeps
them when the move fails and the start answers 0. The server's twin,
SKILLS_SrvSt44_MaggotUp (D2Game 0x6FD06590), moves first (0x6FD065FE) and
sets the flags only when that succeeds (0x6FD0660D..0x6FD0661B).

**What would settle it**: whether the client's move -- D2Common's
COLLISION_GetFreeCoordinates, then #10231 -- can fail where the server's
succeeded. The casters are sandmaggot1..6 and maggotqueen1..5
(MonStats.txt).

Reproduced.

## Self-resurrect's client start sets the three flags its server start clears

`0x6FAC6244`, `SkillsMon.cpp` (SKILLS_CltStFunc38). **Argued
2026-09-26** at the unit's close. **REACHABLE.**

`cltstfunc` 38 serves Emerge and Self-resurrect, and sets UNITFLAG_TARGETABLE,
_CANBEATTACKED and _ISVALIDTARGET (`or al, 0Eh`, 0x6FAC6244..0x6FAC624C).
Emerge's server start does the same; Self-resurrect's,
SKILLS_SrvSt61_SelfResurrect (D2Game 0x6FD01320), resurrects the unit and
CLEARS the three (`and al, 0F1h`, 0x6FD01341). Every stock reanimated horde
has the skill (MonStats.txt: reanimatedhorde1..6), so each one that rises
is targetable on the client and not on the server.

Reproduced.

## The gargoyle trap turns to face its shot on the client alone

`0x6FAC5D73`, `0x6FAC5D87`, `SkillsMon.cpp` (SKILLS_CltDoFunc51).
**Argued 2026-09-26** at the unit's close. **REACHABLE.**

The client sets the trap's own path direction through gnDir64ToSector4 and
gnSector4Direction and D2Common's PATH_SetDirection (0x6FAC5D73..0x6FAC5D87);
the server's twin, SKILLS_SrvDo093_GargoyleTrap (D2Game 0x6FD07A30), read
in its bytes, sets none. The stock gargoyletrap (MonStats.txt) has the
skill; only its drawn facing differs.

[2026-10-10, the globals plan's batch 2, unit 30: the server turns the
trap too -- in its AI, not in the skill. The stock gargoyletrap's AI is
GargoyleTrap, monai.txt 63, and AITHINK_Fn063_GargoyleTrap
(D2Game 0x6FCDBCE0) takes UNITS_GetDirectionToCoords toward the target's
position on the longer axis (0x6FCDBE18), looks it up in the same sector
table (0x6FD29400, byte for byte gnDir64ToSector4) and in four facings of
its own, 31, 49, 0, 17 (0x6FD29440), and calls PATH_SetDirection on the
trap's own path (0x6FCDBE3D) -- read in the stock D2Game.dll's bytes, as
D2MOO writes it (AiThink.cpp:5602). "On the client alone" does not hold.
What differs is the facing: per sector the client's 49, 31, 17, 0
against the server's 31, 49, 0, 17, each pair swapped, from points
computed apart; whether one shot ends on one facing on both sides is not
looked for.]

Reproduced.

## The infernos and Diablo's lightning are drawn at other speeds and counts than the server makes them

`0x6FAC5F66`, `0x6FAC6026`, `0x6FAC611E`, `0x6FAC666D`,
`SkillsMon.cpp` (SKILLS_CltDoFunc54, SKILLS_MissileCltSpawnWithSkillRange,
SKILLS_CltDoFunc56). **Argued 2026-09-26** at the unit's close.
**REACHABLE.**

`cltdofunc` 54 makes TWO missiles each do (the counter 2, 0x6FAC5F66), each,
with a `cltmissileb`, picking it over `cltmissilea` on a seeded coin; the
spawn's flags are
0x4025 on a seed roll (0x6FAC6026..0x6FAC6034) and its speed `Vel` plus
gMissileVelJitter, -2..2 (0x6FAC611E..0x6FAC6124). The server's twin,
SKILLS_SrvDo095_MonInferno (D2Game 0x6FD07E30), makes ONE `srvmissilea`
with flags 0x25 at `Vel` plus `VelLev` times the level over 8. No stock
inferno missile has `VelLev` (Missiles.txt: fetishinferno1 and 2,
megademoninferno and frozenhorror arcticblast1 `Vel` 12, baal inferno 20),
so the server's go at `Vel` and the client's at `Vel` - 2 to `Vel` + 2.
MegademonInferno's client columns name fetishinferno1 and 2 where its
`srvmissilea` is megademoninferno -- another missile, drawn, than the one
made (Skills.txt).

`cltdofunc` 56 draws diablight at `Vel` plus gnDiabLightVelJitter, -3..0,
where SKILLS_SrvDo152_DiabLight (D2Game 0x6FD082D0) makes it at `Vel` 20;
and it sets the current frame one frame before the server's (`shl edx, 8 /
sub edx, 100h`, 0x6FAC666D..0x6FAC6670, against D2Game
0x6FD08580..0x6FD08588).

The casters are fetishshaman1..8, baalcrab, baalclone, megademon1..5,
venomlord, frozenhorror1..5 and nihlathakboss, and diablo and diabloclone
(MonStats.txt).

Reproduced.

## The finger mage's spider carries the skill level in its missile's target X on the client alone

`0x6FAC6939`, `SkillsMon.cpp` (SKILLS_CltDoFunc57). **Argued
2026-09-26** at the unit's close. **REACHABLE.**

The client hands a missile it made to D2Common's MISSILE_SetTargetX with
the SKILL LEVEL (`push edi`, 0x6FAC6939); the server's twin,
SKILLS_SrvDo101_FingerMageSpider (D2Game 0x6FD09000), ends at
MISSILES_CreateMissileFromParams (0x6FD0928D) and sets none. The value is
read: fingermagespider's client tick, MISSILE_CltDoFunc15 (Missiles.txt
`pCltDoFunc` 15), treats the target X as a countdown -- it ends the missile
once the value is at or below minus `CltParam2` (+5Ch) and decrements it
only while it is 0 or less (0x6FB31E43..0x6FB31E6F) -- so with a level of 1
or more there the countdown never starts. fingermage1..5 have the skill
(MonStats.txt).

Reproduced.

## Diablo's run stops beside its target and after `Param4` on the client alone

`0x6FAC6B50`, `0x6FAC6BE1`, `0x6FAC6C15`, `SkillsMon.cpp`
(SKILLS_CltStFunc41, SKILLS_CltDoFunc59). **Argued 2026-09-26** at the
unit's close. **REACHABLE.**

The client's start keeps the path's target unit; the server's,
SKILLS_SrvSt54_DiabRun (D2Game 0x6FD094B0), clears it (PATH_SetTargetUnit,
0x6FD094FD) and aims the path at the target's position (#10170,
0x6FD0955E). The client's do then stops the charge when that target is
within distance 1 (`cmp eax, 1 / jg`, 0x6FAC6C15..0x6FAC6C18) and answers 0
past `Param4` (0x6FAC6BE1..0x6FAC6BE3); SKILLS_SrvDo103_DiabRun (D2Game
0x6FD09570) has neither. So the client's Diablo can end his run beside the
target while the server's charges on. diablo and diabloclone have the skill
(MonStats.txt).

Reproduced.

## The desert turret's server row starts each fireball's Y from the X offset

`0x6FAC6FB1`, `0x6FAC6FCD`, `SkillsMon.cpp` (SKILLS_CltDoFunc60).
**Argued 2026-09-26** at the unit's close. **REACHABLE.**

The client starts each fireball a sixth of its offset from the unit on
each axis (0x6FAC6FB1, 0x6FAC6FCD). The server's twin,
SKILLS_SrvDo105_DesertTurret (D2Game 0x6FD09970), divides only the X offset
and adds it to the X and the Y start (`lea ecx, [edx+eax]`, `add edx, eax`,
0x6FD09B8A..0x6FD09B91); D2MOO keeps the line with "TODO: Maybe
nOffsetY?". The server's defect, not the client's: the drawn row starts
where the server's does not, for turret1..3 and firetower (MonStats.txt).

Reproduced.

## The mosquito's client zeroes the bite count the server keeps when the target is gone

`0x6FAC7392`, `SkillsMon.cpp` (SKILLS_CltDoFunc62). **Argued
2026-09-26** at the unit's close. **NOT ESTABLISHED.**

With the target not found by its id and type, the client sets `Param1` to
0 and answers 0; the server's twin, SKILLS_SrvDo107_Mosquito (D2Game
0x6FD09DE0), leaves it -- its one SKILLS_SetParam1 is the countdown's
(0x6FD0A158).

**What would settle it**: whether a bitten target can leave the client's
unit list and come back within one drain; the client's next bite would then
find 0 and end the drain the server continues. mosquito1..4 have the skill
(MonStats.txt).

Reproduced.

## The doom knight's, the necromage's and the hirelings' missiles need a target unit on the server alone

`SkillsMon.cpp` (SKILLS_CltDoFunc94, 95 and 66). **Argued
2026-09-26** at the unit's close. **NOT ESTABLISHED.**

The client spawns toward its path target with no target unit asked for.
The server's twins refuse a unit with no target (SUNIT_GetTargetUnit):
SKILLS_SrvDo148_DoomKnightMissile at 0x6FD0A625,
SKILLS_SrvDo149_NecromageMissile at 0x6FD0A765 and
SKILLS_SrvDo110_Hireable_RogueMissile at 0x6FD0A8A2 (D2Game).

**What would settle it**: whether doomknight2 and 3, dkmag1 and 2,
necromage or roguehire (MonStats.txt) ever cast these skills with no target
unit.

Reproduced.

## The spider's web: faster on the client, and in town on the server alone

`0x6FAC7F32`, `SkillsMon.cpp` (SKILLS_LaySpiderGooTrail). **Argued
2026-09-26** at the unit's close. **REACHABLE.**

The client's web is made with flags 25h and velocity 3 (0x6FAC7F32).
MISSILE_ClientSpawn takes the block's velocity when the flags hold 4 (`test
al, 4`, 0x6FB3E2EE) and missiles.txt's `Vel` plus `VelLev` otherwise, as D2MOO's
D2Game does (MISSILES/Missiles.cpp, `!(dwFlags & 4)`). The server's twin,
SKILLS_CreateSpiderLayMissile (D2Game 0x6FD0AC20), uses flags 21h
(0x6FD0ADAA), so its web moves at spidergoolay's `Vel` 1 (Missiles.txt)
where the client's moves at 3; it names SKILL_SPIDERLAY as the skill
(0x6FD0ADE5) where the client leaves 0, and plays no sound. Every arach
(arach1..6, MonStats.txt) lays webs.

The server also has no town test: every exit of its room search reaches
the spawn (0x6FD0AD95). Whether an arach can walk a town level is NOT
ESTABLISHED.

Reproduced.

## `SKILLS_CltDoFunc30`, `SKILLS_CltStFunc19` and 22 refuse on an answer SKILLS_GetPathTargetPos never gives

`0x6FAC8070`, `0x6FAC81BE`, `0x6FAC8B97`, `SkillsNec.cpp`. **Argued
2026-09-26** at the unit's close. **UNREACHABLE** -- the three refusals
cannot run.

Each tests SKILLS_GetPathTargetPos' answer and on 0 returns 0. That function
answers 1 on every path ("`SKILLS_CltDoFunc48`, 53, 54, 56, 58, 60 and
`SKILLS_CltDoFunc65` refuse ...", above, with its addresses). Binary
Ninja prints none of the three tests.

Reproduced.

## `SKILLS_CltDoFunc30` and the two corpse-explosion effects test `pUnit` for NULL where no caller can pass NULL

`SKILLS_CltDoFunc30` `0x6FAC804E`, `SKILLS_SpawnCorpseExplosionEffect`
`0x6FAC86C5`, `SKILLS_SpawnPoisonExplosionEffect` `0x6FAC89EA`,
`SkillsNec.cpp`. **Argued 2026-09-26** at the unit's close.
**UNREACHABLE** -- the NULL arms cannot run.

Each tests pUnit only for UNITFLAG_SKSRVDOFUNC and hands it on untested:
30 to SKILLS_GetPathTargetPos, SKILLS_EvaluateSkillFormula and the block;
the corpse explosion's effect reads the unit's seed (0x6FAC86E5); the poison
explosion's makes it the block's owner and hands it to
CUNIT_GetPathTargetUnit. Slot 30 is named in stock 1.10 by Skills.txt
`cltdofunc` alone -- the scan of every "clt" column of Skills.txt and
States.txt at this unit's batch 1 found no `cltprgfunc1..3`, `ItemCltEffect`
or `cltactivefunc` value in 30..33 -- so only SKILLS_RunClientDoFunc reaches
it, and neither of its callers can pass NULL ("Ten Skills.cpp functions
...", above). The two effects each have one caller, `cltdofunc` 32 and 33,
reached the same way, which hand them the unit they were given after
UNITS_GetUsedSkill has taken it.

Reproduced -- each test where the bytes make it.

## `SKILLS_CltStFunc19` hands its new missile's path to D2Common's #10160 untested

`0x6FAC8347`..`0x6FAC834C`, `SkillsNec.cpp`. **Argued 2026-09-26**
at the unit's close. **UNREACHABLE**.

The cast missile's path is loaded (`mov edx, [edi+2Ch]`, 0x6FAC8347) and
handed to #10160, which reads it with no test (`mov edx, [ecx+30h]`,
D2Common 0x6FDA9C4C). MISSILE_ClientSpawn answers a missile only after
testing its dynamic path and removing one without it (0x6FB3E771..0x6FB3E776
-- "`SKILLS_MissileStartSeededPathOrStall` hands ...", above); between its answer
and the call only D2Common's UNITS_GetDirectionToCoords runs, on the caster.

Reproduced.

## `SKILLS_SpawnCorpseExplosionEffect` reads its sub-missile's missiles.txt row through a NULL arm

`0x6FAC87CC`/`0x6FAC87CE` and `0x6FAC8819`/`0x6FAC881B`,
`SkillsNec.cpp`. **Argued 2026-09-26** at the unit's close.
**UNREACHABLE** -- the arm cannot run.

The LARGE and the neither arm look `cltmissilea`'s row up with an inlined
lookup that answers NULL past the count (`xor eax, eax`) and read
`SubMissile1` or `SubMissile2` off the answer with no test -- at address 18h
or 1Ah for NULL. The id is `cltmissilea` itself, which the function's head
bounded below the same count (0x6FAC8643..0x6FAC8649), and nothing between
writes the table count: the flag, the block, two seed rolls and one
SKILLS_SpawnScatteredMissiles call.

Reproduced, as Skills.cpp reproduces the same lookup's unchecked read.

## Corpse Explosion's client jitter never reaches a missile

`0x6FAC86E5`..`0x6FAC8776`, `SKILLS_SpawnCorpseExplosionEffect`,
`SkillsNec.cpp`. **Argued 2026-09-26** at the unit's close.
**INERT**.

Two rolls of the caster's seed, each taken modulo 3 less 1, are stored as the
block's nTargetX and nTargetY (0x6FAC872B, 0x6FAC876E) -- the point itself
is the position. Every arm then calls SKILLS_SpawnScatteredMissiles
(0x6FAC879C, 0x6FAC87E8, 0x6FAC8839), which writes nTargetX and nTargetY for
each piece before its MISSILE_ClientSpawn (Skills.cpp), and after the
scatter only nX and nY are put back (0x6FAC8846, 0x6FAC884A). The rolled
target reaches no missile; the two rolls still advance the seed, which the
C reproduces.

Reproduced.

## Corpse Explosion's last two client missiles inherit the scatter's target and facing

`0x6FAC8846`..`0x6FAC88A5`, `SKILLS_SpawnCorpseExplosionEffect`,
`SkillsNec.cpp`. **Argued 2026-09-26** at the unit's close.
**INERT** for the stock rows.

`cltmissileb` and `cltmissilec` are spawned from the block the last scatter
left: its last piece's nTargetX and nTargetY, and SKILLS_SetUnitDirection as
pInitFunc with that piece's facing in pInitArgs (Skills.cpp), since only the
position is restored. Both stock rows, Corpse Explosion and
NihlathakCorpseExplosion, name explodingarrowexp and redlightmissile, which
Missiles.txt gives no `Vel` and `NumDirections` 1: a missile that does not
move and draws one direction. A row naming a moving or directional missile
there would show the inherited target and facing.

Reproduced.

## Raise Skeleton's and the corpse explosions' client starts have no town test, and the explosions test the corpse for the local player alone

`SKILLS_CltStFunc20` (0x6FAC8370), `SKILLS_CltStFunc21` `0x6FAC855A`..`0x6FAC856B`,
`SkillsNec.cpp`. **Argued 2026-09-26** at the unit's close. **NOT
ESTABLISHED**.

The server's starts refuse a corpse whose room is in town --
SKILLS_SrvSt15_RaiseSkeleton_Mage (D2Game 0x6FD0AF42..0x6FD0AF50) and
SKILLS_SrvSt17_Poison_CorpseExplosion (0x6FD0B0C2..0x6FD0B0D0) -- and the
second asks #11021 (SKILLS_CanUnitCorpseBeSelected, by D2Common's export
table) for every caster (0x6FD0B0D9). The client's start for Raise Skeleton
and Raise Skeletal Mage has no town test, and the explosions' asks #11021
only when the caster is the local player (0x6FAC855A..0x6FAC856B).
SKILLS_RunCltStFunc's own town gate tests the caster's room (Skills.cpp), so
the difference shows only for a corpse in a town room while its caster
stands outside one: the client starts, writes the corpse's position into
the skill and its do draws the burst there, where the server refuses.
Whether a stock monster leaves a corpse inside a town room -- one that
#11017 or #11021 accepts -- was not established, nor whether another unit's
start can reach the client without the server's start having tested the
corpse.

Reproduced.

## Bone Wall's and Bone Prison's client start judges the town by the point's room

`SKILLS_CltStFunc22` `0x6FAC8BA5`..`0x6FAC8C69`, `SkillsNec.cpp`.
**Argued 2026-09-26** at the unit's close. **NOT ESTABLISHED**.

The client finds the room holding the point the caster heads for -- its own
or one adjacent to it -- and answers 0 without one, and, for a row without
InTown (both stock rows), 0 when that room is in town. Bone Wall has no
server start; Bone Prison's, SKILLS_SrvSt19_BonePrison (D2Game 0x6FD0B0F0),
answers whether the TARGET unit's room is out of town (0x6FD0B0FE..0x6FD0B10F)
and has no room search. So the two sides differ for a Bone Wall point in a
town room, which only the client refuses, and for a point beyond the
caster's room and its neighbours, which only the client refuses. Whether a
caster outside town can aim a wall into a town room, and whether either
skill's range reaches past the adjacent rooms, was not read.

Reproduced.

## Iron Golem's client start refuses an item with no room, which the server does not

`SKILLS_CltStFunc23` `0x6FAC8CD2`..`0x6FAC8CDA`, `SkillsNec.cpp`.
**Argued 2026-09-26** at the unit's close. **NOT ESTABLISHED**.

The client asks D2Common's UNITS_GetRoom of the metal item and answers 0
without a room; SKILLS_SrvSt20_IronGolem (D2Game 0x6FD0B120..0x6FD0B182)
makes the other five tests -- an item, on the ground, metal, identified,
unowned -- and none on the room. The difference needs an item in
IMODE_ONGROUND with no room on the client; whether the client ever holds
one was not read.

Reproduced.

## `SKILLS_CltDoFunc34`, 35 and 36 test `pUnit` for NULL where no caller can pass NULL

`0x6FAC8F45`, `0x6FAC9145`, `0x6FAC9259`, `SkillsPal.cpp`.
**Argued 2026-09-26** at the unit's close. **UNREACHABLE** -- the NULL arms
cannot run.

Each tests pUnit only for UNITFLAG_SKSRVDOFUNC and hands it on untested: 34
and 36 to CUnit.cpp's CUNIT_GetPathTargetUnit, which reads it, 35 to
SKILLS_GetPathTargetPos and the missile block. Slots 34..36 of gpSkillCltDoFuncs
are named in stock 1.10 by Skills.txt `cltdofunc` alone -- the scan of every
"clt" column of Skills.txt and States.txt at this unit's batch 1 found no
`cltprgfunc1..3`, `ItemCltEffect` or `cltactivefunc` value in 34..36 -- so
only SKILLS_RunClientDoFunc reaches them, and neither of its callers can pass
NULL ("Ten Skills.cpp functions ...", above).

Reproduced -- each test where the bytes make it.

## `SKILLS_CltDoFunc34` and 35 refuse on answers their callees never give

`0x6FAC8F69`..`0x6FAC8F6B` and `0x6FAC9167`..`0x6FAC9169`,
`SkillsPal.cpp`. **Argued 2026-09-26** at the unit's close.
**UNREACHABLE** -- the two refusals cannot run.

34 returns 0 on a NULL dynamic path right after CUnit.cpp's
CUNIT_GetPathTargetUnit, which asserts that same path ("hPath") and exits on
NULL. 35 returns 0 when SKILLS_GetPathTargetPos answers 0, which it never does
("`SKILLS_CltDoFunc30`, `SKILLS_CltStFunc19` and 22 refuse ...", above);
Binary Ninja prints no test there.

Reproduced.

## `SKILLS_CltDoFunc37` tests `pUnit` for NULL after D2Common has asserted it

`0x6FAC9BBA` and `0x6FAC9CC1`, `SkillsPal.cpp`. **Argued
2026-09-26** at the unit's close. **UNREACHABLE**.

Its first call is D2Common's UNITS_GetUsedSkill(pUnit), which asserts the unit
and exits on NULL (FOG_DisplayAssert, then `_exit`, D2Common
0x6FDBD687..0x6FDBD691). The second test's NULL arm would go on to write the
unit's dwFrameCountPrecise (0x6FAC9D23).

Reproduced.

## Charge's do walks one sequence record where it means the sequence -- the client and the server alike

`SKILLS_CltDoFunc37` `0x6FAC9BA6`..`0x6FAC9BB0`, `SkillsPal.cpp`;
`SKILLS_SrvDo067_Charge` D2Game `0x6FD19814`..`0x6FD19822`. **Argued
2026-09-26** at the unit's close. **REACHABLE** for Duriel, the reanimated
horde and the siege beasts; **INERT** for the player's Charge and the claw
vipers'.

The walk for the charge's last run frame loads the first record's event once
(`mov al, [eax+5]`, 0x6FAC9BA6) and loops on that byte: the back edge (`jl`,
0x6FAC9BB0) returns to `test al, al` (0x6FAC9BA9), and the record pointer never
moves. It answers -1 when the first record has no event and 7 otherwise. The
server's twin is the same loop (`mov bl, [ebx+5]`, 0x6FD19814), which D2MOO
writes with "TODO: Why is this check inside a loop?". Walking the records --
the first with ANIMSEQ_EVENT_NONE, less one -- gives, from stock monseq.txt and
D2MOO's gPlayerSequenceCharge:

    seq_durielcharge       duriel                   7 here, 15 walked
    seq_reanimatedcharge   reanimatedhorde1..6      7 here, 10 walked
    seq_siegebeastcharge   siegebeast1..5           7 here, 13 walked
    seq_serpentcharge      clawviper1..10           7 here,  7 walked
    seq_pinheadsmite       flyingscimitar          -1 here, -1 walked
    the player's Charge    8 run frames             7 here,  7 walked

So Duriel, the horde and the siege beasts loop their run back to frame 0 after
frame 7 (`jl`, 0x6FAC9D45), and on reaching the target jump to frame 8, still
inside their run, instead of to their attack. A monster's sequence is its
MonStats.txt `Sk{N}mode` for the skill's slot (D2MOO's
SKILLS_GetSeqNumFromSkill); the monsters are the MonStats.txt rows that have
Charge or SerpentCharge as a skill.

Reproduced -- the loop over one record, as the bytes have it.

## `SKILLS_DrawMotionTrail` clears the fraction of the unit's frame on every draw

`0x6FAC96DB` and `0x6FAC9802`, `SkillsPal.cpp`. **Argued 2026-09-26**
at the unit's close. **REACHABLE**; what it costs is **NOT ESTABLISHED**.

The frame is saved as `nSeqCurrentFramePrecise >> 8` (`sar ebp, 8`) and put
back `<< 8` (`shl ebp, 8`, `mov [esi+44h], ebp`) by every call that passes the
direction test, Param3 0 included, so the field's low 8 bits are zeroed.
D2Common advances the field by the unit's wAnimSpeed each tick (D2MOO's
Units.cpp, `nSeqCurrentFramePrecise += pUnit->wAnimSpeed`), so the fraction a
charging unit carries between ticks is lost at each draw: at a wAnimSpeed that
is a multiple of 256 nothing changes, below 256 the frame can stall.

**What would settle it**: the wAnimSpeed of a charging player and of each
charging monster in the SEQUENCE mode, and how many draws fall between two
ticks.

Reproduced.

## `SKILLS_DrawMotionTrail`'s behind pass gives its farthest ghost the newest frame

`0x6FAC9728`..`0x6FAC9745` and `0x6FAC979A`..`0x6FAC979E`,
`SkillsPal.cpp`. **Argued 2026-09-26** at the unit's close.
**REACHABLE**, cosmetic.

Each pass counts the ghosts' frames back from the first ghost it paints: the
unit's frame less one, then less two. The front pass paints outward from one
step, so the ghost nearest the unit is one frame back and the next two; the
behind pass paints inward from Param3 steps, so the FARTHEST ghost is one frame
back and the nearest Param3 frames back -- the trail's frames run the wrong way
while the unit moves down-screen. Param3 reaches 2 on the second do of a charge
(SKILLS_CltDoFunc37), so the two ghosts swap frames.

Reproduced.

## The sanctuary state sets each bolt's direction twice

`SKILLS_CltDoFunc86` `0x6FAC95E6`, `SkillsPal.cpp` (States.cpp
until 2026-10-08). **Argued 2026-09-26** at the unit's close. **INERT**.

The block's flags 2001h make MISSILE_ClientSpawn turn the new missile to a
random one of 64 directions from the missile's own seed (`test ah, 20h`,
0x6FB3E826); the handler then sets it again from the caster's seed
(PATH_SetDirection, 0x6FAC95E6), before anything draws it. The missile is
sanctuarybolt, Sanctuary's `cltmissilea`, with NumDirections 1 and no `Vel`
(Missiles.txt), so neither direction shows. The first roll also leaves the
missile's own seed one step further on; nothing this close read uses it.

Reproduced.

## Blessed Hammer's and Fist of the Heavens' client dos against their server twins

`SKILLS_CltDoFunc35`, `SKILLS_CltDoFunc36`, `SkillsPal.cpp`; D2Game
`SKILLS_SrvDo073_BlessedHammer` 0x6FD1A200, `SKILLS_SrvDo080_FistOfTheHeavens`
0x6FD1AA50. **Argued 2026-09-26** at the unit's close. **INERT**, but for Fist
of the Heavens' flag and overlay, **NOT ESTABLISHED**.

- Both servers want their missile above 0 (`jg`, 0x6FD1A254, 0x6FD1AAAB); the
  clients take 0 too. The stock rows' missiles are blessedhammer (92) and
  fistoftheheavensdelay (233).
- Blessed Hammer's server sets UNITFLAG_SKSRVDOFUNC after its target point
  (0x6FD1A283); the client sets it before SKILLS_GetPathTargetPos, which never
  fails.
- The client tests the hammer's path (0x6FAC91B7..0x6FAC91BC), the server does
  not (0x6FD1A2EF); MISSILE_ClientSpawn answers only a missile with a path
  (0x6FB3E771..0x6FB3E776).
- Fist of the Heavens sets UNITFLAG_SKSRVDOFUNC on the client alone, and its
  server puts the row's `srvoverlay` on the target (D2MOO), which the client's
  do does not. Whether the client shows that overlay by another road, and what
  the flag does after the do on each of SKILLS_RunClientDoFunc's two callers,
  was not read.

Reproduced.

## Charge's start and do against their server twins

`SKILLS_CltStFunc25`, `SKILLS_CltDoFunc37`, `SkillsPal.cpp`; D2Game
`SKILLS_SrvSt31_Charge` 0x6FD19460, `SKILLS_SrvDo067_Charge` 0x6FD19780.
**Argued 2026-09-26** at the unit's close. **NOT ESTABLISHED**.

- A monster already in melee range of its target: the server puts it into an
  attack mode (MONMODE_ATTACK2 for the base ids clawviper1 and Duriel,
  ATTACK1 otherwise -- D2MOO); the client's start charges it, and only a
  player goes to SKILLS_DispatchUnitAction22.
- A charging monster's path: distance 20 after PATH_ClearPoint2 on the server
  (0x6FD1968C..0x6FD19694), 30 with no clear on the client (0x6FAC9A11).
- A charging monster told to end (flags bit 1) is in melee range at 3 on the
  server (D2MOO), at 0 on the client (`push 0`, 0x6FAC9C25).
- The client's alone, by design: the refusal of a charge the local player
  already runs, the sound, Param3 (the motion trail), the facing, and the
  attack sound at action frame 3.

**What would settle it**: whether the server sends a charge start for a
monster it has put into an attack mode instead, and how far a monster's path
at distance 30 strays from the server's at 20.

Reproduced.

## `SKILLS_CltDoFunc23`, 25, 27, 28, 29 and 96 test `pUnit` for NULL where no caller can pass NULL

`0x6FAC9F47`, `0x6FACA5CA`, `0x6FACA95A`, `0x6FACAC5D`, `0x6FACAE3B` and
`0x6FACAA2B` (96), `SkillsSor.cpp`. **Argued 2026-09-26** at the
unit's close. **UNREACHABLE** -- the NULL arms cannot run.

Each tests pUnit only for UNITFLAG_SKSRVDOFUNC, after pUnit has gone to
SKILLS_GetClientMissileForSkill, and hands it on untested -- to #11276, the
ring, the launch test, the spawn or CUNIT_GetPathTargetUnit. Slots 23, 25,
27, 28 and 29 of gpSkillCltDoFuncs are named in stock 1.10 by Skills.txt
`cltdofunc` alone, and 96 by `ItemCltEffect` alone,
so only SKILLS_RunClientDoFunc reaches them -- the one dispatcher that reads
those two columns -- and neither of its callers can pass NULL ("Ten
Skills.cpp functions ...", above). The table's other readers index
`cltprgfunc1..3` and `cltactivefunc`, which hold none of the six.

Reproduced -- each test where the bytes make it.

## `SKILLS_CltDoFunc23`, 26, 96, `SKILLS_CltStFunc17` and `SKILLS_SpawnInfernoStream` refuse on an answer SKILLS_GetPathTargetPos never gives

`0x6FACA00E`..`0x6FACA011`, `0x6FACA6E4`..`0x6FACA6E6`,
`0x6FACABC0`..`0x6FACABC2`, `0x6FACACD3`..`0x6FACACD5` and
`0x6FACA25A`..`0x6FACA25C`, `SkillsSor.cpp`. **Argued 2026-09-26**
at the unit's close. **UNREACHABLE** -- the five refusals cannot run.

Each tests SKILLS_GetPathTargetPos' answer and on 0 returns 0, NULL for the
stream. That function answers 1 on every path ("`SKILLS_CltDoFunc48`, 53,
54, 56, 58, 60 and `SKILLS_CltDoFunc65` refuse ...", above, with its
addresses). Binary Ninja prints none of the five tests.

Reproduced.

## `SKILLS_CltStFunc15` makes a no-unit owner no caller can need

`0x6FACA0EA`, `SkillsSor.cpp`. **Argued 2026-09-26** at the unit's
close. **UNREACHABLE**.

`test edi, edi` picks UNIT_TYPES_COUNT and -1 as the new stat list's owner
for a NULL pUnit, after pUnit has gone to STATES_CheckState and
STATLIST_GetStatListFromUnitAndState. gpSkillCltStFuncs is read by
SKILLS_RunCltStFunc alone, which has read the unit at 0x6FABC01B ("Ten
Skills.cpp functions ...", above). SkillsAss.cpp's SKILLS_StartChannelState
has the same arm.

Reproduced.

## `SKILLS_CltStFunc16` is dispatched by no stock row, and differs from its server twin

`0x6FACA3D0`, `SkillsSor.cpp`; D2Game
`SKILLS_SrvSt12_Telekinesis_DragonFlight` 0x6FD15B40. **Argued 2026-09-26** at
the unit's close. **UNREACHABLE** in stock 1.10.

No stock Skills.txt row carries `cltstfunc` 16: Telekinesis (43) leaves it
empty and Dragon Flight (275) carries 5, though both carry `srvstfunc` 12 (a
scan of the column). So SKILLS_RunCltStFunc never runs the slot. Its body is
Telekinesis's client start, and against the server's it has the same range,
positions, squared compare and player-or-monster gate in another order -- the
server takes the target before the record and asks its selectable test first
-- and one test more: the server hands UNITS_GetRoom's answer to
DUNGEON_IsRoomInTown untested (0x6FD15CA9..0x6FD15CB0,
0x6FD15CC2..0x6FD15CC9), where the client refuses a NULL room. A row given
`cltstfunc` 16 would meet both.

Reproduced.

## Fire Wall is cast at a point in no room found, as out of town -- the client and the server alike

`SKILLS_CltDoFunc26` `0x6FACA7B3`..`0x6FACA7C5`, `SkillsSor.cpp`;
D2Game `SKILLS_SrvDo024_FireWall` `0x6FD16EFB`..`0x6FD16F05`. **Argued
2026-09-26** at the unit's close. **INERT**.

Both sides search the caster's room and its neighbours for the room holding
the point and hand a point in none of them to DUNGEON_IsRoomInTown as NULL --
the client from its routed search, the server by `xor eax, eax`
(0x6FD16EFB). D2Common answers 0 for NULL (`test eax, eax / jnz`,
0x6FD8D0E4..0x6FD8D0E8), so a row without InTown casts the wall there, as
out of town, on both sides: the client draws what the server makes. Hydra's
client start, SKILLS_CltStFunc17, refuses the same NULL.

Reproduced.

## Blaze's trail bounds `cltmissileb` and `cltmissilec` by their sign alone

`SKILLS_LayBlazeTrail` `0x6FACAFD4`..`0x6FACAFDC` and
`0x6FACAFEA`..`0x6FACAFF4`, `SkillsSor.cpp`. **Argued 2026-09-26**
at the unit's close. **LATENT**.

`cltmissilea` is tested inside [0, missiles.txt's count); B and C only for a
negative word (`cmp word ptr [ebp+0ECh], 0 / jl`; `cmp [ebp+0EEh], di / jl`)
before one of them goes to MISSILE_ClientSpawn. The trail takes its skill
from STATE_BLAZE's stat list, and two stock rows name that state
(over stock Skills.txt): Blaze (46), whose three
are firewall (69), firesmall (104) and firemedium (105), all in
Missiles.txt, and PrimeBlaze (187), whose three client missiles are empty.
An out-of-range B or C needs a changed table.

Reproduced.

## `SKILLS_CltDoFunc84` tests `pUnit` for NULL after CUNIT_GetPathTargetUnit has read it

`0x6FACB0E5`, `SkillsSor.cpp`. **Argued 2026-09-26** at the unit's
close. **UNREACHABLE**.

`test ebx, ebx` refuses a NULL pUnit after pUnit has gone to CUnit.cpp's
CUNIT_GetPathTargetUnit, which reads `pUnit->pDynamicPath` with no test ("Ten
Skills.cpp functions ...", above). Slot 84 is Thunder Storm's `cltprgfunc1`
alone, run by SKILLS_ApplyClientSkillEffectNonProgressive, which does
nothing for a NULL unit (Skills.cpp).

Reproduced.

## Chilling Armor's retaliation reads a NULL missiles.txt row for a class outside the table -- the client and the server alike

`SKILLS_StatesCltEventFunc1` `0x6FACB26C`/`0x6FACB26E`, `SkillsSor.cpp`
(States.cpp until 2026-10-08); D2Game `SKILLS_EventFunc01_ChillingArmor`
`0x6FD1801B`/`0x6FD18023`. **Argued 2026-09-26** at the unit's close.
**LATENT**.

The RETURNFIRE test looks the missile's row up in place, answers NULL for a
class outside [0, missiles.txt's count) (`xor eax, eax`) and reads byte 1 of
the flags off it with no test -- a fault at address 5. D2Game's twin has the
same slip; D2MOO's source tests the record. The missile is the one that hit
the unit, and a missile unit outside the table gets neither missile data nor
a path on the client: MISSILE_InitClientUnit refuses its class before
MISSILE_AllocMissileData and PATH_AllocDynamicPath (`cmp ecx, [edx+0B6Ch] /
jge`, 0x6FB39771..0x6FB39777). Its class would have to come from a malformed
server packet; whether such a unit can still reach MISSILE_ClientOnHit was
not read.

Reproduced, as inv.cpp's and the SOUND accessors' are.

## `SKILLS_CltDoFunc87` tests the address of the unit's seed for NULL

`0x6FACB4C7`..`0x6FACB4CC`, `SkillsSor.cpp` (States.cpp until 2026-10-08).
**Argued 2026-09-26** at the unit's close. **UNREACHABLE**.

`lea esi, [ebx+20h] / test esi, esi / jz` is NULL only for pUnit ==
(D2UnitStrc *)-32, and pUnit has been read before it, by
SKILLS_GetClientMissileForSkill and D2Common's #10950 and #11276. Hex-Rays
prints the test as `pUnit == (D2UnitStrc *)-32`.

Reproduced.

## MONUNIQUE_MonUModCltElementalDeath lets a NULL unit through to the missile ring

`0x6FB456ED`..`0x6FB45717`, MONUNIQUE_MonUModCltElementalDeath (not written when argued;
`MonUnique.cpp` batch 2 wrote it, 2026-10-02; the ring,
SKILLS_SpawnMissileRingFromUnit, is `Skills.cpp`'s). **Argued
2026-09-27**. **UNREACHABLE**.

The arm for nModId 2Ah spawns when `pUnit == NULL || pUnit->dwAnimMode == 0`:
`test esi, esi / jz` goes straight to the spawn (0x6FB456ED -> 0x6FB456F8),
and the ring reads `pUnit->dwUnitType` with no test (0x6FABD4EA) -- a fault
for a NULL unit. No NULL gets there. The handler's only entries are three
slots of gpMonUModCltFuncs, rows 10, 18 and 42 of the five-column table, all
in column 2 (slots 52, 92 and 212, read off the image); the table's only
reader is MONUNIQUE_MonUModRunCltFuncsForUnit (2 references,
both indexed, both there), which returns at once for a NULL, non-monster or
data-less unit (`test ecx, ecx / jz`, `cmp edx, 1 / jnz`, 0x6FB461C7..
0x6FB461E7) and hands columns 0 to 3 that tested unit (`mov ecx,
[esp+18h+var_8]`, 0x6FB4622C). Only column 4 gets the dispatcher's
untested second argument (0x6FB46256), and no slot of column 4 holds this
handler. The NULL test is the inlined mode accessor's own (`pUnit ?
pUnit->dwAnimMode : 0`), dead on the one path that reaches it.

Reproduced where it lives: the ring's head in Skills.cpp carries the
reading, and the handler in MonUnique.cpp writes its NULL arm as the bytes
have it.

## HELP_HelpScreenOnDismissKey does not claim ESC or SPACE

`0x6FADA2D0`, `help.cpp`. **Argued 2026-09-27**.
**LATENT**.

The only dismiss handler of six that leaves the key unclaimed. What the
unclaimed key can reach is decided by Storm, read in Storm's database:

* **Order.** SMsgDispatchMessage (Storm 0x6FFCD200) dispatches the MESSAGE
  list first -- `SEvtDispatch('SMSG', hWnd, uMsg)`, 0x6FFCD26A -- and for a
  WM_KEYDOWN the KEY list after it -- 'SMSI', 0x6FFCD28D..0x6FFCD2B7 --
  with no test of bReturn between, then repeats both for hWnd 0 when the
  window is Storm's default.
* **A break does not cross lists.** SEvtBreakHandlerChain (0x6FFC2B00)
  queues a record keyed by the params pointer (0x6FFC2B81); SEvtDispatch
  stops when it meets one (0x6FFC2D10) and removes every one with that key
  when it returns (0x6FFC2E6A..).
* **Newest first.** SEvtRegisterHandler stamps each node with a rising
  count (0x6FFC336E..0x6FFC337C) and SEvtDispatch takes the highest below
  the last (`cmp eax, edx / jb`, 0x6FFC2DA2).

So CMDTBL_OnKeyDown, on the message list (gKeyCommandHandlers), runs
BEFORE this handler, while the help screen is still on. The compiled-in
bindings (gDefaultCommandTable, 0x6FB78500, read off the image) put ESC on
command 56, CMDTBL_ActionEscMenu, and SPACE on 38, GAME_CmdTblActionClearScreen,
and both call UI_CloseAllPanels -- ClearScreen always in a game
(0x6FAA9240), EscMenu unless an NPC dialog or a popup is up -- which
closes every open closeable window, UI_HELPSCREEN among them
(gbUiWindowCloseable[33] = 1), and so unregisters this handler before
Storm walks the key list. The unclaimed key can matter only when neither
command closes the screen first, and then only to a handler of the same
key registered before the help screen's: UI_SetUIToggle refuses the help
screen over a popup and over the NPC menu, the anvil, the shop and the
trade screen (gaUiToggleRules[8], [14], [12], [23][33] = 2), and its draw
pass closes the closeable panels. What is left -- a rebound key, or a
dialog or disc prompt left under the screen with ESC's command refusing --
was not traced further.

Reproduced.

## HELP_HelpButtonOnLButtonUp opens the help screen on a release whose press began elsewhere

`0x6FADA660`, `help.cpp`. **Argued 2026-09-27**.
**REACHABLE** while the button's hint is not suppressed.

The release does not test gbHelpButtonDown, as the help screen's own close
button does, so a release inside the box opens the screen whatever the
press was. Nothing claims it first: UI_InitUiSubsystems, run from
SCMD_HandleSrv01_GameFlags after GAME_AppModeGame has registered
gGameWindowMessageHandlers at its entry, turns UI_GAME on and then
UI_HELPBUTTON, so the button's WM_LBUTTONUP record is newer than
PANEL_GameOnLButtonUp's and GAME_WindowOnLButtonUp's, and Storm runs the newest
first (the entry above). A hit claims the release, so a press begun on the
world and released over the button opens the help screen and neither older
handler sees that release -- GAME_WindowOnLButtonUp's clear of gbLeftButtonDown
among what does not happen. Panels opened later are newer still; whether
any claims a release over the button's box was not read.

Reproduced.

## Charged Bolt's and Inferno's client functions against their server twins

`SKILLS_CltDoFunc23`, `SKILLS_CltStFunc15`, `SKILLS_SpawnInfernoStream`,
`SkillsSor.cpp`; D2Game `SKILLS_SrvDo017_ChargedBolt_BoltSentry`
0x6FD15EB0, `SKILLS_SrvSt11_Inferno_ArcticBlast` 0x6FD15AB0 with
`SKILLS_StartInferno` 0x6FD15940, `SKILLS_DoInferno` 0x6FD156F0. **Argued
2026-09-26** at the unit's close. **NOT ESTABLISHED**.

- Charged Bolt: the server sets UNITFLAG_SKSRVDOFUNC before the record
  (0x6FD15EC0..0x6FD15EC8), the client after the missile test; the server's
  missile is SKILLS_GetProgressiveSkillMissileId's (0x6FD15F0A), the
  client's the row's `cltmissilea` -- no Charged Bolt row is progressive; and
  the server takes the target for every bolt and skips a bolt whose target X
  or Y is 0 (0x6FD15FF5..0x6FD1600C), where the client takes it once and
  spawns every bolt.
- Inferno's start: the server refuses without the mana (`startmana` << 8) or
  a `srvmissilea` in the table (0x6FD15AE4..0x6FD15B1D), gives a monster's
  used skill a deadline in Param1 (0x6FD15982..0x6FD159B4), writes its expire
  frames as dwGameFrame + 20 and + 6 where the client writes a bare 15h and
  7, hangs another remove callback (0x6FCF9550, against the client's
  ITEM_OnUninterruptableStateRemoved), and plays no cue.
- Inferno's stream: the server makes the row's `srvmissilea` alone, where
  the client alternates `cltmissilea` and `cltmissileb` on a seed bit; it
  spawns nothing on a channel's first do (Param1 0, 0x6FD15782..0x6FD1578A);
  it sets no animation speed; and for a monster it writes
  nSeqCurrentFramePrecise B00h (0x6FD1583C), where the client holds the
  caster's pose.

**What would settle it**: whether D2Common's #10476 and the new stat list
take the client's bare 15h and 7 as counts or as frames, and whether the
stream the client draws on a channel's first do answers anything the server
sent.

Reproduced.

## The novas, Fire Wall, Chain Lightning's item effect and Hydra against their server twins

`SKILLS_CltDoFunc25`, 26, 96 and `SKILLS_CltStFunc17`,
`SkillsSor.cpp`; D2Game `SKILLS_SrvDo022_NovaAttack` 0x6FD169A0,
`SKILLS_SrvDo024_FireWall` 0x6FD16D70, `SKILLS_SrvDo151_Unused` 0x6FD172B0,
`SKILLS_SrvSt14_Hydra` 0x6FD15D50. **Argued 2026-09-26** at the unit's close.
**INERT** for the novas' velocity; **NOT ESTABLISHED** for the rest.

- The novas: the server's velocity adds `calc1` (+138h, 0x6FD16A24) where
  the client adds `cltcalc1` (+114h) -- the same formula in Howl's row and
  empty in the others (stock Skills.txt), so the rings agree; the server
  sets the flag first and takes the progressive missile.
- Fire Wall: the client refuses the whole cast when the maker's
  `SubMissile1` is missing, where the server spawns its third missile,
  `srvmissileb`, only when it is in the table (0x6FD16FEE..0x6FD17004); the
  server refuses a point with X or Y 0 (0x6FD16E69..0x6FD16E75), the client
  casts it; the client refuses a NULL caster room, the server goes on with
  it (0x6FD16E81..0x6FD16E83); and the INTOWN flag is read on the client
  alone.
- Chain Lightning from an item: the client starts the bolt at the target's
  position and takes the path point with its zero test; the server hands
  its spawn (sub_6FD11420, 0x6FD17417) the offset from the caster to its
  target, with no point and no test of one.
- Hydra's start: the client looks the record up and reads INTOWN; the server
  has neither and answers whether the point's room is out of town
  (0x6FD15E31..0x6FD15E3E).
- Chain Lightning's do and the centres' (`cltdofunc` 27 and 28) have the
  shape of SrvDo026 and SrvDo028.

**What would settle it**: for Fire Wall, whether the server ever sends the
cast for a point with a zero coordinate, and whether a maker can lack
`SubMissile1`; for the item's Chain Lightning, where the server's bolt
starts.

Reproduced.

## Blaze's trail, Chilling Armor's bolt and Thunder Storm's strike against the server

`SKILLS_LayBlazeTrail`, `SKILLS_CltDoFunc84`, `SkillsSor.cpp`;
`SKILLS_StatesCltEventFunc1`, `SkillsSor.cpp`; D2Game
`SKILLS_CreateBlazeMissile` 0x6FD16C00, `SKILLS_EventFunc01_ChillingArmor`
0x6FD17F40, `SKILLS_SrvDo029_ThunderStorm` 0x6FD17570. **Argued 2026-09-26**
at the unit's close. **INERT**, but for Chilling Armor's level, **NOT
ESTABLISHED**.

- Blaze: the server makes one missile a step, the row's `srvmissilea` (blaze,
  0x6FD16C98), with no pick; the client draws one of three flames by a seed
  roll -- its picture of the burning ground.
- Chilling Armor: the client's bolt is `cltmissilea` where the server's is
  `srvmissilea` (0x6FD17FCC), and the client adds the `cltoverlaya` hit
  flash. The server turns a negative level into 1 (0x6FD17FE6..0x6FD17FEA);
  the client hands the level on as it is.
- Thunder Storm: the server picks the target, refuses in town, makes the
  bolt, deals its hit and removes it (0x6FD17671..0x6FD17738), then messages
  the client (SUnitMsg_C6300, 0x6FD1774D); the client's strike draws the
  falling bolt, the flash and the overlay. The message was not read.

**What would settle it**: whether a Chilling Armor stat list can carry a
level below 1.

Reproduced.

## The client do functions' NULL-missile arms, and what each would do with one

`Missile.cpp`, the MISSILE_ family's batches 1 to 9. **Argued
2026-09-27** at the family's close. **UNREACHABLE.**

Twenty-eight pCltDoFunc handlers hand a NULL missile on: 23 to
MISSILE_DestroyClientMissile (5, 7..10, 12..17, 19..22, 24..30, 67), two to
MISSILE_CltDoFunc1 (4 and 18), two to
CUNIT_RemoveUnitByClientId(-1, 6) (44 and 46: type 6 is one row past the six
of gpUnitTableByClientId's [6][128], `mov edx, 6 / or ecx, 0FFFFFFFFh` at
0x6FB348DA..0x6FB348DF and 0x6FB34AEE..0x6FB34AF3), and one to
CUNIT_GetOwnerUnit's assert (43).

None can be handed NULL. Of the 28, every reference is
in gpMissileCltDoFuncs (0x6FB91260) but one, MISSILE_CltDoFunc53's call of 7
(0x6FB35822) with the missile it was itself handed; the table is jumped into
only by MISSILE_ClientUpdate (`jmp gpMissileCltDoFuncs[eax*4]`, 0x6FB3D865),
after its own test of the missile (`test ebx, ebx / jz`, 0x6FB3D6D8..0x6FB3D6DE).
So what each arm would do is never asked: MISSILE_DestroyClientMissile tests
NULL itself and returns (0x6FB3DBB3..0x6FB3DBB5); MISSILE_CltDoFunc1 would
read the class id; the removal would read bucket 127 of a seventh row; the
assert would exit.

Reproduced.

## The owner asked and handed untested to MISSILE_ClientSpawn

`Missile.cpp`: pCltDoFunc 19, 25, 30, 33, 35, 36, 42, 51, 53, 55 and
56; pCltHitFunc 9, 37, 38, 51, 53 and 60; MISSILE_SpawnFallingSubMissile.
**Argued 2026-09-27** at the family's close. **REFUTED** as a defect.

Each asks CUNIT_GetOwnerUnit and stores the answer in the block's pOwner with
no test, and CUNIT_GetOwnerUnit can answer NULL: with no
UNITFLAGEX_STOREOWNERINFO, or an owner the client does not hold (CUnit.cpp).
But MISSILE_ClientSpawn tests pOwner at every read -- the room (the local
player's with none), the slow-missiles test, the point in front, the owner
stored and pierce, the bow sound and the monster modifiers (Missile.cpp's
head of it, batch 25) -- and hands it untested only to D2Common's
MISSILE_SetOwner, which tests it (D2MOO, D2Common/src/Units/Missile.cpp:218).
A NULL owner makes a missile with no owner, which is what a spawn with no
owner is meant to make.

## The owner asked and handed untested to SKILLS_EvaluateSkillFormula

`Missile.cpp`: pCltDoFunc 9, 13 (twice) and 50; pCltHitFunc 12 (its
two formulas). **Argued 2026-09-27** at the family's close. **NOT
ESTABLISHED.**

The owner can be NULL (the entry above). D2Common's evaluator stores the unit
in its calc record and hands it to every unit term -- SKILLS_GetSpecialParamValue
passes it to the elemental-damage lookups and D2Common_11032 (D2MOO,
D2Common/src/D2Skills.cpp). Whether each term the formulas of these rows use
tests the unit was not read. **What settles it**: the rows' calc columns read
term by term against the terms' own NULL tests.

## Record lookups that keep NULL and read it

`Missile.cpp`. **Argued 2026-09-27** at the family's close.
**LATENT.**

- MISSILE_SpawnOverArea keeps NULL for an id out of range (0x6FB35D57) and
  reads RandStart through it (0x6FB35D9B); MISSILE_SpawnFromMissile reads the
  first velocity through it (0x6FB360BE). Their ids come from missiles.txt and
  skills.txt columns, and a column naming no row of the 684 is what it takes.
- MISSILE_LaunchCatapultShot reads its record with no test; its one caller,
  pCltDoFunc 61, passes 416 or 422.
- Three constants: hfcontrol (373) in pCltHitFunc 43 (0x6FB39DA5..0x6FB39DBB,
  read at 0x6FB39E0D), hffragment1 (374) in pCltDoFunc 38 (0x6FB3A091), izual
  control (379) in pCltDoFunc 42 (0x6FB3A236) -- in range while missiles.txt
  has more than 379 rows.
- The missile's own class looked up a second time, its NULL kept and read:
  MISSILE_ClientSpawn's pierce lookup (0x6FB3E951), MISSILE_ClientUpdate's
  second, MISSILE_ClientOnHit's second (0x6FB3DDC4) -- each after the same
  function's first lookup has found that class in range.
- MISSILE_ClientOnHit reads the row's ExplosionMissile before any test
  (0x6FB3DCA3): a NULL missile or a class out of range reads 16h. Its callers
  hand it a missile they hold and have tested -- the do and hit functions
  their own, SkillsEMon.cpp's vine beast (States.cpp's until 2026-10-08) the one D2Common #10407 found for it --
  and a class out of range never reaches a hit: MISSILE_ClientUpdate destroys
  such a missile first.

What prevents all of them is the data. Reproduced.

## Fist of the Heavens' bolts: the aura filter's test inverted, and the cap never stored

`MISSILE_CltHitFunc26`, `Missile.cpp`; D2Game
MISSMODE_SrvHit22_FistOfTheHeavensDelay 0x6FC5C4D0. **Argued 2026-09-27** at
the family's close. **REACHABLE.**

- `test eax, eax / jz` (0x6FB3799C..0x6FB379A0) keeps a ZERO AuraFilter and
  replaces any other with A683h; the server keeps a non-zero one (`jnz`,
  0x6FC5C712..0x6FC5C719). The stock row, fistoftheheavensdelay (233), carries
  42375 (A587h), so every Fist of the Heavens' client bolts search with A683h:
  USELOS added, FINDUNDEAD and IGNINTOWN gone.
- With cHitPar2 not above 0 the skill's Calc4 is evaluated and dropped (eax
  overwritten, 0x6FB37995) and five of the six fields stored
  (0x6FB379AD..0x6FB379B9, 0x6FB379D6): nMaxTargets (+10h) keeps whatever the
  stack held. The stock row's cHitPar2 is 0, so every cast takes
  that arm. Ours leaves the field unwritten too and our stack holds other
  garbage -- a difference declared in Missile.cpp.
- The gate reads the server's HitSubMissile1 and spawns CltHitSubMissile1
  (0x6FB37853, 0x6FB379BD): **INERT**, the stock row sets both to
  fistoftheheavensbolt.

Reproduced, the cap as the declared difference.

## Grim Ward's next stage carries the level as its skill

`MISSILE_CltHitFunc29`, `Missile.cpp`; D2Game
MISSMODE_SrvHit26_GrimWardStart 0x6FC5CA50. **Argued 2026-09-27** at the
family's close. **REACHABLE.**

nSkill and nSkillLevel are both stored from ebx, the level (0x6FB37CC0,
0x6FB37CC4); the skill id was in esi, which the missile took at 0x6FB37C92.
The server stores the skill (0x6FC5CB62). Six stock rows take pCltHitFunc 29
(249, 250, 252, 253, 255, 256: the three -start rows and the three the starts
spawn), each spawning the next stage,
so every Grim Ward's client stages carry the wrong skill -- and the next stage
of this handler looks its skill record up by it, whose Calc1 is the range
while cHitPar1 is blank, as it is on all six.

Its `jnz` where the server has `jg` (0x6FB37C6A against 0x6FC5CB13) takes a
negative cHitPar1 as a range: **LATENT**, no stock row sets one.

Reproduced.

## hfcontrol's fragment ranges from signed offsets

`MISSILE_CltDoFunc38`, `Missile.cpp`. **Argued 2026-09-27** at the
family's close. **REACHABLE.**

The rough length compares the SIGNED offsets (`cmp eax, ecx / jge`,
0x6FB3A081), not their magnitudes, and takes |x| + 2|y| when x >= y -- the
other way round from D2Common's UNITS_GetDistanceToCoordinates, which takes it
when |x| <= |y| (D2MOO, Units.cpp). hfcontrol (373) is the stock row with
pCltDoFunc 38 and its fourteen offsets are built on the stack
(0x6FB39F38..0x6FB39FF5), negative ones among them, so each fragment's range
is taken from this length whenever hfcontrol runs. Which offsets land on the
wrong arm was not tabulated. The row has no server hit function.

Reproduced.

## Baal's death: two stage records are never chosen

`MISSILE_CltDoFunc64`, `Missile.cpp`. **Argued 2026-09-27** at the
family's close. **REACHABLE.**

The stage records from 0x6FB70B68 are {420, 64, 0} {435, 20, 1} {470, 8, 1}
{270, 5, 1} and {0, 64, 0} (read off the image, batch 21), and the walk stops
at the first whose bound the frame is above. The bounds do not descend: any
frame above 420 stops at record 0, so records 1 and 2 would need a frame of at
most 420 above 435 or 470 -- never. Every baalfx control (625, the stock row
with pCltDoFunc 64) runs stages 0, 3 and 4 only.

Reproduced.

## NULL tests made after the pointer has been used

`Missile.cpp`. **Argued 2026-09-27** at the family's close.
**UNREACHABLE** -- each test can never fire.

- pCltHitFunc 39 tests the glob for NULL twice after its seed at +20h has
  been rolled (0x6FB3B3B6); pCltHitFunc 41 tests the rock (0x6FB3B75F) after
  SKILLS_SpawnClientEffectScatter has seeded from it; pCltDoFunc 64 tests the
  control (0x6FB3C16D) after its seed was used.
- MISSILE_ClientOnHit tests the missile after CUNIT_GetOwnerUnit, which
  asserts a NULL unit and exits (CUnit.cpp).
- pCltHitFunc 13's second lookup answering 0: its class was just found.
- The tests of an ADDRESS: `&pMissile->pSeed` in the spray
  (0x6FB2FF42..0x6FB2FF49) and pCltDoFunc 58 -- NULL only for a unit at
  (D2UnitStrc *)-32.
- MISSILE_ClientSpawnSkillEffect' and MISSILE_ClientSpawnAtPathTarget's tests
  of SKILLS_GetPathTargetPos's answer (0x6FB3EC2F, 0x6FB3ED8C): it answers 1 on
  every path (Skills.cpp).
- The target predicates' pCtx and pCtx->pMissilesTxt: MISSILE_CltDoFunc1
  builds the context from the missile and its row, both tested before.
- pCltHitFunc 54's and 56's paths, read untested: MISSILE_ClientSpawn answers
  only a missile with a path, and MISSILE_InitClientUnit leaves a server's
  missile pathless only for a class out of range (0x6FB39771..0x6FB39777),
  which MISSILE_ClientUpdate destroys before any hit.

Reproduced.

## The family's arms no stock row reaches, and its divergences from D2Game

`Missile.cpp`. **Argued 2026-09-27** at the family's close.

**UNREACHABLE in stock 1.10** (a scan of the column, in each batch):
pCltDoFunc 12 (no row; its wrap), pCltHitFunc 64 (no row; its missing id --
every piece is missile 0, the block's id never stored, 0x6FB361C7..0x6FB361E6),
pCltHitFunc 18's fourth run from a negative cHitPar2 (no row sets one),
pCltHitFunc 31's -1 to the spawn (every stock row is in its switch), pCltDoFunc
34's phase 4 (pCltDoFunc 33 never writes it), MISSILE_SpawnRingOfMissiles'
step not above 0 (its caller floors it). **LATENT**: MISSILE_SpawnBoulderImpactDebris
ignores nFlyingRockMissile for 456 (0x6FB38A7A), and both stock callers pass
456; pCltHitFunc 37's re-roll has no bound but the rolls (a bolt at the centre
is 1 in 400 per try).

**INERT** against the server: pCltHitFunc 12 evaluates the SERVER's SHitCalc1
(+88h, 0x6FB36834) and pCltHitFunc 46 gates on the server's column -- the stock
rows set both columns alike; pCltDoFunc 48 tests collision in the missile's own
room for a point that may lie in the next, as the server does.

**NOT ESTABLISHED** -- what settles each is named:

- pCltDoFunc 29's stage leaps from 17 to 12 between F 84 and 85: the design or
  a slip; there is no server twin to compare.
- pCltDoFunc 57 goes on with the missile after pCltDoFunc 43's inlined body
  may have made it hit (0x6FB35A71); what MISSILE_ClientOnHit leaves of a unit
  it removes settles it. Its TargetY is raised and reset, a count that never
  passes 1.
- pCltDoFunc 15's mark: what sets a parent's TargetX above 0.
- pCltDoFunc 24 dereferences UNITS_GetRoom's answer untested (0x6FB32E2F):
  whether a cairnstones missile can be roomless.
- pCltDoFunc 68 with sucfireball's blank CltSubMissile1: what the compiled
  table holds for a blank cell.
- MISSILE_FreeUnitData's untested unit; MISSILE_ClientOnHit's
  CUNIT_GetOwnerUnit before its test (above).
- MISSILE_ClientSpawnSkillEffect hands SKILLS_GetPathTargetPos the UNIT,
  whatever its type -- MISSILE_CltDoFunc50 hands it a missile -- where
  Skills.cpp's untested dynamic path is itself NOT ESTABLISHED.
- Client-only or client-different, against D2MOO's server: pCltHitFunc 16's
  MISSILE_SetOwner; 30's burst on any hit where the server's waits for its
  countdown; 53's bounce, 54's and 56's spawns and 60's height; 55 testing
  each sub-missile column alone where SrvHit51 returns on the first; 63
  asking for no owner where SrvHit59 does; MISSILE_ClientSpawn extending the
  range by the slow factor, which the server does not, testing TravelSound
  `!= 0` where MISSILE_DestroyClientMissile and MISSILE_ClientOnHit test
  `>= 0`, and gating the light on the block's nLightRadius while taking the
  row's radius; MISSILE_ClientSpawnSimple taking a (0, 0) or far point that
  D2GAME_CreateMissile_6FD115E0 replaces or refuses. Each is settled by the
  two sides' rows and the unit that sees the difference.

Reproduced.

## `STATES_SetFunc5_ChangeClass` faces a unit changed into MONSTER_BAALTHRONE, and nothing changes a unit into it

`States.cpp`, 0x6FB4BEC8. **Argued 2026-09-28** at the STATES_
family's close. **UNREACHABLE** in a stock game.

After MONSTER_ChangeMonsterClass the arm `cmp ebx, 21Fh` tests the NEW class,
the state's STAT_SHORTPARAM1, for 543 and turns the unit to face a point ten
subtiles along Y. The server writes that stat in two places only (a grep of
D2MOO's D2Game for STATE_CHANGECLASS): Baal leaving his throne, into
MONSTER_BAALCRABSTAIRS 559 (AiThink.cpp near 16684), and a druid summon's
chain class (SkillDruid.cpp near 1805). D2Game names MONSTER_BAALTHRONE only
in MonsterMode.cpp and SUnitInactive.cpp, never as a class to change into. The
dynamic path the arm reads untested would be asserted first by D2Common's
PATH_GetYPosition inside the routed UNITS_GetYPosition (D2MOO Path.cpp:1095).

Reproduced.

## Cyclone Armor's removals test two ids against 0 and the first tier's id against 1

`STATES_SetFunc8_CycloneArmor`, `States.cpp`. **Argued 2026-09-28** at
the STATES_ family's close.

- `test ebx, ebx / jz` (0x6FB4C737, 0x6FB4C797) where the add half tests -1:
  an empty `overlay` column (-1) passes, the remover's own -1 test skips its
  group, and id + 1 = 0 takes overlay 0's group off the unit. **LATENT**: the
  stock row's three columns are 208, 210 and 212 (Overlay.txt, batch 2).
- `cmp ebp, 1 / jg` on the first id (0x6FB4C7F3..0x6FB4C7FE) where the tier
  was meant: the block runs for an id of 1 or a negative one, never for 208.
  **INERT**: the tier is at least 1 while the state is on, so the tier test
  could not fire either, and in both readings the first tier's pair stays on
  until STATES_RemFunc5_CycloneArmor takes every overlay off.

Reproduced.

## Maul's and Feral Rage's second overlay range divides by the first range's count

`STATES_SetFunc16_SkillCharges`, `States.cpp`. **Argued 2026-09-28**
at the STATES_ family's close. **LATENT.**

The most charges and the charges are clamped to the FIRST range's count
(0x6FB4CE7E..0x6FB4CE8A) before either range is tested, and the second range
divides by the clamped most (`cdq / idiv ebp`, 0x6FB4CF0A). A row whose
`overlay2` is one below `overlay1` gives a count of 0, skips the first range as
empty and divides the second by zero. The two stock rows, maul (117) and
feralrage (120), give five and five (maul1..maul5, feralrage1..5).

Reproduced.

## `PLRSKILLS_SetStateStat` reports the old alignment as UNASSIGNED unless it fetched the list itself

`PlrSkills.cpp`, 0x6FB4D859. **Argued 2026-09-28** at the STATES_
family's close. **LATENT.**

The old value starts at 0, becomes UNIT_ALIGNMENT_UNASSIGNED (4) for
STAT_ALIGNMENT, and is read from the list only on the arm that fetches the
list (0x6FB4D8A6). A caller that passes a list in, or a list made here,
reports 4 to CUNIT_OnUnitAlignmentChanged, which then cannot see a GOOD
monster leave GOOD and leaves the room's ally count raised. The callers pass a
list only from the second stat of one state on (SCMD_HandleSrvA8, and
SCMD_HandleSrvAA per state), and the server writes STATE_ALIGNMENT's list
with STAT_ALIGNMENT alone (D2MOO's D2Game sub_6FCBDD30, SUnit.cpp) -- so the
alignment is the first stat of its state and its list is fetched.

Reproduced.

## The STATES_ family's dead tests and inert divergences

`States.cpp`, `PlrSkills.cpp`. **Argued 2026-09-28** at the
family's close.

- **UNREACHABLE** -- unit tests that cannot fail: STATES_SetFunc15_Revive's
  between two uses (0x6FB4CD7D) and STATES_SetFunc10_Redeemed's after the spawn
  (0x6FB4CA1F), both behind PLRSKILLS_StatesRunStateSetFunc's own test (0x6FB4D926);
  PLRSKILLS_SkillsDrawUnitEffectBehind's and InFront's null-safe type and mode reads,
  behind D2Common's UNITS_GetUsedSkill, which asserts `hUnit` and exits
  (D2Common 0x6FDBD674..0x6FDBD691); PLRSKILLS_EndStateAndSkill's -1 tests
  (0x6FB4D6F2, 0x6FB4D765, 0x6FB4D79A) on ids its `jl` has made non-negative.
- **INERT** -- STATES_RemFunc1_Progressive's `stat` tested and never read (the
  six stock rows each name one); STATES_RefreshUnitColorShift taking a state
  0 winner for none (`test ebx, ebx / jle`, 0x6FB4D58C; stock row 0, `none`,
  has no `colorpri` and cannot win).
- **NOT ESTABLISHED** -- STATES_SetFunc3_Blaze lacks the server's
  negative-level test (D2Game SKILLS_CreateBlazeMissile takes a negative level
  as 1). It shows only for a negative MODIFIERLIST_LEVEL on the `blaze`
  state's list; the server code that writes that list was not read, and
  reading it settles this.
- **NOT A DEFECT** -- STATES_ToggleStateWithSound's monster arm pushes eax,
  the unit's type, as bSet (0x6FB4D3FC); `cmp eax, 1 / jnz` has made it 1.

Reproduced -- each as its bytes have it.

## `UNITMODE_MustMoveToTarget` walks to an Inferno or Arctic Blast target in range and casts at one out of range

`UnitMode.cpp`, 0x6FB4F155. **Argued 2026-09-28** at UnitMode.cpp's
close. **REACHABLE.**

For a queued command 6 or 13 whose skill has Skills.txt `srvdofunc` 19 --
Inferno (41) and Arctic Blast (230) in stock, both with `aurarangecalc`
`ln12/4-2` (a scratch read of the stock Skills.txt) -- the answer, TRUE for
"walk first", is PLAYER_IsUnitInAuraRange's, and that function answers
`nDistance <= nRange` (`setle cl`, 0x6FB498FA): TRUE when the target is IN
range. PLAYER_DispatchSkillClick tests the same two skills with the same
formula (`cmp word ptr [eax+2Eh], 13h` at 0x6FB4A7A0, dwAuraRangeCalc at
+64h) and reads it the other way: in range it sends the cast (0x6FB4A87D),
out of range it queues 6 or 13 (PLAYER_SetQueuedCommand, 0x6FB4A8A3 and
0x6FB4A8BF) and walks toward the range's edge
(PLAYER_SendMoveToReachablePoint, 0x6FB4A962).

The queue reaches this function. UNITMODE_RunQueuedCommand runs on every
game tick of a game (GAME_UpdateFrame, Game.cpp), and three routes put a 6
or 13 there with one of the two skills behind it:

- DispatchSkillClick's first queue, when the walk after it does not run:
  PLAYER_SendMoveToReachablePoint returns before its call to
  UNITMODE_PerformClientCommand when PATH_StartUnitMove plans no route
  (0x6FB4A6D5), and PerformClientCommand returns before its queue clear
  (0x6FB4ED3A..0x6FB4ED64) at its interrupt gate (0x6FB4EAF8..0x6FB4EB03).
  When the walk does run, that clear empties the queue the click has just
  armed.
- DispatchSkillClick's second queue, armed AFTER its command
  (0x6FB4A9C3..0x6FB4AA29) for a skill that is not `srvdofunc` 19 at the
  click, once the left or right skill is changed to one of the two before a
  tick runs it.
- This function's own TRUE: UNITMODE_MoveToTargetAndQueueCommand arms the
  queue again after its walk (0x6FB4F20E), so the next tick asks again.

So a target within the skill's range is walked to -- WalkToEntity or
RunToEntity, the command queued again -- on every tick it stays in range,
and one beyond it is cast at: the reverse of the two arms below it, which
answer TRUE when the unit is NOT close enough to an object or for melee.

Reproduced.

## `UNITMODE_SyncUnitPosition` hands any unit that is not a player to #10230 as a dynamic path

`UnitMode.cpp`, 0x6FB4E334. **Argued 2026-09-28** at UnitMode.cpp's
close. **UNREACHABLE from the stock server.**

The warp's last arm passes `pUnit->pDynamicPath` to D2Common #10230 for every
type but UNIT_PLAYER, and +2Ch is the STATIC path of an object, an item or a
tile. None of the three reaches it. Of SCmd.cpp's ten calls, the three in
SCMD_HandleSrv18_PlayerAttributes, SCMD_HandleSrv95 and
SCMD_HandleSrv96 pass CUNIT_GetPlayer(), whose arm returns first
(0x6FB4E2D6, 0x6FB4E2EC, 0x6FB4E306); rows 0x0F and 0x10 name their unit by
the packet's type byte (SCmd.cpp's router) and 0x16 by each update's, and
D2MOO's D2Game writes UNIT_PLAYER into all three -- `bTOU` 0 in each of the
three sends of 0x0F and 0x10 (PLAYER/PlrMsg.cpp) and players alone in 0x16's
list (CLIENTS_RefreshUnitsUpdateList, GAME/Clients.cpp); rows 0x68, 0x6A, 0x6B
and 0x6D are monster-only forms, whose type the router sets to UNIT_MONSTER.
The other callers pass players: PLAYER_DoClientAction's arms -- every direct
caller of PLAYER_DoClientAction hands it a player (the entry below) -- and
UNITMODE_MoveUnitToCoords, which PLAYER_DoClientAction alone calls. So the
arm gets monsters, and a monster holds its dynamic path from
MONSTER_InitClientUnit (0x6FB40F06) until CUNIT_DestroyUnit frees it.

A server that named an object, an item or a tile in 0x0F, 0x10 or 0x16 would
hand #10230 the static path. That is a malformed server, not argued further.

Reproduced.

## The UNITMODE_ family's dead tests, its untested paths and the server's memory probe

`UnitMode.cpp`. **Argued 2026-09-28** at UnitMode.cpp's close.

- **UNREACHABLE** -- UNITMODE_UpdateUnit's NULL-safe reads of the id, the
  type and the async bit (0x6FB4E737..0x6FB4E755) and its `pUnit &&`: its
  one caller, CUNIT_UpdateAllUnits, calls it eight times, each inside a
  `while (pUnit)` walk (CUnit.cpp). UNITMODE_PerformClientCommand's unit
  tests after UNITS_GetUsedSkill: D2Common's UNITS_GetUsedSkill asserts
  `hUnit` and exits (D2Common 0x6FDBD674..0x6FDBD691).
  UNITMODE_InteractWithUnit's local player, used untested: its callers are
  the cube panel's INV_CubeOnDismissKey, INV_CubeOnLButtonUp and INV_CubeOnPanelClose
  (inv.cpp) and PLAYER_DoClientAction's case 2 (0x6FB49703), and only
  PerformClientCommand makes action 2, from command 19 on the unit it is
  handed. None of the 28 calls to PLAYER_DoClientAction and the dispatch
  loads a constant 2 -- CUNIT_WarpUnit and
  GFX_StopIfAlive pass 7 behind a UNIT_PLAYER test (CUnit.cpp, 0x6FB5DA50),
  PLAYER_AdvanceAnimMode 6, 7 and 1, the packet rows the server's byte --
  and D2MOO's D2Game's player-action table has none (PLAYER/PlrMsg.cpp,
  stru_6FD30708, twenty rows). Case 2 does not test that its unit is the
  local player: the interaction is always the local player's.
- **NOT A DEFECT** -- the dynamic path passed untested by
  UNITMODE_RecacheUnitRoom, StartMoveToLocation, StartMoveToUnit and
  MoveUnitToCoords' PATH_SetPathingFlag. Their callers are PLAYER_ and
  MONSTER_DoClientAction, and a player or a monster
  holds its path from creation to its free: PLAYER_InitClientUnit
  (0x6FB488BB) and MONSTER_InitClientUnit (0x6FB40F06) call
  PATH_AllocDynamicPath, which stores it untested (D2MOO's D2Common,
  Path/Path.cpp), and only PLAYER_FreeClientUnit and MONSTER_FreeUnitData,
  from CUNIT_DestroyUnit, free it. And MoveUnitToCoords' action block leaves
  [2], [6] and, for a player, [3] unwritten; no consumer of actions 1 and 23
  reads them (that function's head). And UNITMODE_SetAnimMode and
  SetAnimModeSilent test the unit for their flag clear alone and hand it on
  to D2Common's UNITS_ChangeAnimMode, which tests it itself -- NULL and
  UNIT_TILE jump to one exit (D2Common 0x6FDBEA68, 0x6FDBEA6F), FALSE in
  D2MOO's Units.cpp -- so a NULL unit changes nothing.
- **NOT A DEFECT** -- UNITMODE_UpdateUnit's first arm, for a unit with
  dwFlags bit 23 set, runs: D2Common's DUNGEON_FreeRoom sets the bit on
  every unit of a room it frees (`or ecx, 800000h`, 0x6FD8CE6E, stored at
  0x6FD8CE75, the walk over the room's unit list at 0x6FD8CE80..0x6FD8CE85)
  and dwFlagEx bit 5 on each one without UNITFLAG_ISCLIENTUNIT (bit 22,
  `shr edx, 16h`, 0x6FD8CE5C; `or [eax+0C8h], esi` with esi 20h,
  0x6FD8CE68) -- D2MOO's D2Common has the same, D2Dungeon.cpp's
  DUNGEON_FreeRoom. So a server unit left in a freed room asks the server
  for itself again (packet 0x4B) and an async one is removed. The batch had
  found that immediate and not followed the register it went into. No other
  0x800000 in D2Client or D2Common is a unit's (D2Client's one is this arm's clear, D2Common's other eight are in DRLG
  room code, a global and ITEMS_SerializeItem), the 35 writes to +0C8h in
  D2Client and the 16 in D2Common set other bits, and so do the masks handed to D2Client's NPCMENU_SetUnitFlag and
  MONSTER_SetFlagEx. The two bits are named
  UNITFLAG_ROOMFREED and UNITFLAGEX_REQUESTUPDATE, in D2C_UnitFlags and
  D2C_UnitFlagsEx. D2Game has no reader or writer of either.
- **DECLARED** (a difference of the rewrite, not a defect of the original)
  -- UNITMODE_FlushServerMemoryProbe reads the two addresses the server names in
  OUR process, where the original D2Client.dll still holds its code and every
  global D2Vars.h resolves into, but not what the rewrite has taken into its
  own storage (the globals gate). It answers only in a Battle.net
  game, and D2MOO's server sends no 0xAD and reads no 0x64 -- their sizes in
  D2Net's Server.cpp are its only mentions -- so it is INERT against any
  server but Battle.net's.

Reproduced -- each as its bytes have it.

## An Inferno or Arctic Blast click beyond range queues its cast, then its own walk clears it

`Player.cpp`, PLAYER_DispatchSkillClick, 0x6FB4A8A3..0x6FB4A962.
**Argued 2026-09-28** at Player.cpp's close. **REACHABLE.**

For a skill with Skills.txt `srvdofunc` 19 -- Inferno (41) and Arctic Blast
(230) in stock -- whose target is beyond its aura range, the click queues the
cast, command 6 or 13 (PLAYER_SetQueuedCommand, 0x6FB4A8A3, 0x6FB4A8BF), and
THEN sends the walk toward the range's edge (PLAYER_SendMoveToReachablePoint,
0x6FB4A962), which goes out through UNITMODE_PerformClientCommand (0x6FB4A6F4)
-- and that function's last act clears the queue (0x6FB4ED3A..0x6FB4ED64,
UnitMode.cpp). Every other queue of the unit is armed after its command:
PLAYER_DispatchClickAction's two, PLAYER_DispatchSkillClick's other two,
PLAYER_InteractOrApproachTarget's five, UNITMODE_MoveToTargetAndQueueCommand's.

So a single click on a target out of range walks and does not cast when the
walk arrives: nothing is queued for UNITMODE_RunQueuedCommand to run. A HELD
button casts: the held click (8) passes PLAYER_ShouldHandleClick only while the
player stands (mode not 2, 3 or 6), so once the walk ends the next held click
reaches PLAYER_DispatchSkillClick in range and sends the cast. The queue
survives only where the walk is not sent -- no route (0x6FB4A6D5) or
PerformClientCommand's interrupt gate -- and there the queued command meets
UNITMODE_MustMoveToTarget's reversed test (the entry above). Any Sorceress with
Inferno or Arctic Blast on a button reaches it.

Reproduced.

## PLAYER_DispatchClickAction takes #10399's -1 as far, and a player standing beside you in town is walked to

`Player.cpp`, 0x6FB4A359. **Argued 2026-09-28** at Player.cpp's
close. **REACHABLE.**

D2Common's #10399 answers the distance class of two units, from a table for
units within 8 subtiles on both axes and under size 4 -- and the table's
cells for dx 0..2 at dy 0, dx 0..1 at dy 1 and dx 0 at dy 2 are -1 (D2MOO's
Units.cpp, D2Common_10399). A player is size 2 (UNITS_GetUnitSizeX answers
COLLISION_UNIT_SIZE_SMALL, D2MOO), so two players side by side are -1.
PLAYER_InteractOrApproachTarget compares the answer signed (`jg`/`jle`,
0x6FB4AD5F, 0x6FB4AC77, 0x6FB4AE15, 0x6FB4B00A) and takes -1 as close;
PLAYER_DispatchClickAction's player arm -- both players in town -- compares it
UNSIGNED (`ja`, 0x6FB4A359), so -1 is far. There a click on another player
standing beside the local one sends a walk to that player (the record's
nMoveToUnitCmd) and queues command 19 on it (PLAYER_SetQueuedCommand, inlined,
0x6FB4A3A2) instead of sending 19 at once. The queue survives the walk (it is
armed after it), and on the next tick UNITMODE_RunQueuedCommand finds the target
in melee range (UNITMODE_MustMoveToTarget) and sends the 19. So the
interaction goes out one tick late, after a walk command the server did not
need. D2Game's own handler of the 19 acts on a player within #10399 8, signed
(sub_6FC828D0, PLAYER/PlrMsg.cpp).

Reproduced.

## A command of the local player's own that arrives in a knockback is sent and not carried out

`Player.cpp`, PLAYER_DoClientAction, 0x6FB49119. **Argued
2026-09-28** at Player.cpp's close. **REACHABLE.**

A player in PLRMODE_KNOCKBACK takes an action only when bInterruptKnockback is
set; otherwise PLAYER_DoClientAction answers TRUE at once (0x6FB49119).
UNITMODE_PerformClientCommand -- the path of every command the local player
issues, with bInterruptKnockback FALSE -- sends the command to the server only
when that answer is TRUE (its inlined dispatch, UnitMode.cpp). So a click while
the local player is being knocked back is sent to the server and not carried
out on the client. Any monster attack with knockback reaches it. What the
server does with the command, and how the two sides come back together
(CUNIT_ResyncUnitFromServer, CUnit.cpp), was not argued.

Reproduced.

## gbLastRoomInTown is written by every player the client builds, and the NPC reset on leaving town reads it

`Player.cpp`, 0x6FB4891E (PLAYER_InitClientUnit) and
0x6FB48EB6..0x6FB48ED1 (PLAYER_AdvanceAnimMode). **Argued 2026-09-28** at
Player.cpp's close. **REACHABLE** in a game with other players.

The global has three references: PLAYER_AdvanceAnimMode
reads and writes it for the LOCAL player on a frame its room has changed
(D2Common's UNITS_IsCurrentRoomInvalid, 0x6FB48E45, then the local-player and
room tests), and on a step from a town room into one that is not calls
NPCMENU_AbortOnPlayerAction (npcmenu.cpp), which marks every NPC speech entry
aborted and rerolls the lines flagged for it. PLAYER_InitClientUnit writes it
for EVERY player unit the client builds -- the local player's and each other
player that comes into view -- from the room that player is built in. So
another player built in a town room while the local player is outside makes
the local player's next change of room fire the reset, though no town is left;
one built outside while the local player is in town, before the local player
walks out, hides that step and the reset does not run. Both need another player
built between two of the local player's room changes; the effect is the NPC
speech state's, not argued further. The name was gbLastInitRoomInTown until
this close: it is written by more than the initialiser.

Reproduced.

## The PLAYER_ family's dead tests, untested reads that cannot fail, and two quirks

`Player.cpp`. **Argued 2026-09-28** at Player.cpp's close.

- **UNREACHABLE** -- a player's mode of 20 or more, which
  PLAYER_AdvanceAnimMode would read past gnAnimModeControl's twenty rows with
  (0x6FB48B79, no bound). A player's mode comes from PLAYER_DoClientAction's
  arms, whose switch rejects any action above 19h (`ja`) and sets PLRMODE_
  constants; from CUNIT_ResyncUnitFromServer, the in-process server's own
  mode (CUnit.cpp:3977); and from a skill's mode (SKILLS_RunCltStFunc's
  SKILLS_GetSkillModeFromUnit or PLRMODE_ATTACK1/2, Skills.cpp) -- none above
  19 with the stock server and data. D2Common's UNITS_ChangeAnimMode stores what
  it is given (D2MOO's Units.cpp); CUNIT_CreateMonsterUnit's packet mode
  (CUnit.cpp:943) is a monster's.
- **UNREACHABLE** -- PLAYER_FreeClientUnit's cast-light removal: its one
  caller, CUNIT_DestroyUnit, has taken the light with the same #10958 and
  removed it before (CUnit.cpp), so the second #10958 answers NULL.
- **UNREACHABLE** -- PLAYER_InitClientUnit's corpse arm (0x6FB4893A): the
  mode has just been set to TOWNNEUTRAL by D2Common's UNITS_ChangeAnimMode,
  which stores it for any unit that is not a tile (D2MOO), so it is never
  DEAD or DEATH there.
- **UNREACHABLE** -- PLAYER_AdvanceAnimMode's stamina arm's UNIT_TYPES_COUNT
  and -1: CUNIT_GetPathTargetUnit answers D2Common's PATH_GetTargetUnit on the
  same path (CUnit.cpp), and the arm takes them only when a second
  PATH_GetTargetUnit, with nothing between, answers a unit.
- **UNREACHABLE** -- PLAYER_DoClientAction's `!pUnit` arm (0x6FB490E3), after
  D2Common's UNITS_GetUsedSkill has asserted the unit (the UNITMODE_ family's
  entry above); the weapon class slot COMPOSIT_GetWeaponClassId leaves
  unwritten for a right-arm "item" that is not an item -- the body location
  holds items only; a NULL pParams outside CLTACTION_NEUTRAL -- every caller
  that passes NULL passes NEUTRAL (CUNIT_WarpUnit, Skills.cpp:1711,
  PLAYER_AdvanceAnimMode, GFX_StopIfAlive).
- **UNREACHABLE** -- PLAYER_CanUseSkill's swap finding no Attack: D2Common's
  SKILLS_InitSkillList adds skill 0 to every player (D2MOO's D2Skills.cpp),
  and PLAYER_InitClientUnit calls it; and a negative use state below sgSnds:
  PLRSKILLS_SkillsGetUseStateForClient answers D2Common's SKILLS_GetUseState, whose
  every answer is a SKILLUSTATE_ member from 0 up (D2MOO), or BLOCKED.
- **UNREACHABLE** -- the skills.txt rows read untested after a lookup that
  answers NULL out of range: PLAYER_OnMouseClick's right press (+14h,
  0x6FB49CEC), PLAYER_IssueRightSkillCommand's (+14h, 0x6FB4B332) and
  PLAYER_DispatchClickAction's `InTown` test (+5): each id is a skill's from
  the unit's own skill list, which D2Common builds from rows of the table.
  PLAYER_DispatchClickAction's `!pSkillsTxt` arm (0x6FB49F7C) is dead: the
  row is NULL there only for a NULL skill, which has returned (0x6FB49EDB).
- **UNREACHABLE** -- the asserts "ptObjectData" (PLAYER_DispatchClickAction,
  line 1220): D2Common's DATATBLS_GetObjectsTxtRecord asserts the id's bounds
  itself and never answers NULL (D2MOO's ObjectsTbls.cpp), which also makes
  PLAYER_InteractOrApproachTarget's dropped lookup (0x6FB4AA9F) a bounds check
  and nothing more; "nDR > 0" (1352), reached only when
  PLAYER_IsUnitInAuraRange returns without writing, which after the lookups
  the site has made means no local player -- the click is the local player's;
  and "nDist" (1353), which needs a target at distance 0 with a negative range:
  `aurarangecalc` is `ln12/4-2` for both skills, Param1 20 and 35, Param2 3 and
  2 (the stock skills.txt), 3 and 6 at level 1 and never below 2.
- **UNREACHABLE** -- PLAYER_OnMouseClick's held arm's unit test (0x6FB49B50),
  after CUNIT_CanPlayerIssueCommand has read the unit; PLAYER_ShouldHandleClick's
  own tests are live. PLAYER_InteractOrApproachTarget's NULL target handed to
  #10399 (0x6FB4B001): both callers pass a resolved target.
- **UNREACHABLE** -- PLAYER_ReleaseClickAndResetHireling's GetPetInfo record
  read untested (0x6FB4B399), and PLAYER_OnMouseClick's copy of it
  (0x6FB49C5D): the GUID is ROSTERPETS_GetAssociatedUnitGUID's, a dwUnitId read
  off gpRosterPetList, and ROSTERPETS_GetPetInfo walks the same list for the
  same id (RosterPets.cpp) with nothing between -- so the Select.cpp entry's
  first two callers are settled. PLAYER_AssignCorpse's late NULL test of the
  item (0x6FB4B5C5): the inventory's items are items, and
  INVENTORY_UnitIsItem answers NULL only for a unit that is not one (D2MOO's
  D2Inventory.cpp). GFX_StopIfAlive's NULL test after CUNIT_IsUnitDead
  (0x6FB5DA4C): each of its 13 callers in Item.cpp hands it a unit it has
  tested or already read.
- **NOT A DEFECT** -- the path pointer PLAYER_AdvanceAnimMode saves and hands
  untested to the knockback's two calls and PATH_AdjustDirection: a player
  holds its path from PLAYER_InitClientUnit to PLAYER_FreeClientUnit (the
  UNITMODE_ family's entry). PLAYER_IsUnitInAuraRange measures the LOCAL
  player whoever owns the skill, and both callers hand it the local player's
  skill (PLAYER_DispatchSkillClick's click, and UNITMODE_MustMoveToTarget under
  UNITMODE_RunQueuedCommand, which GAME_UpdateFrame runs for the local
  player). The 100h stored into the flag copy (0x6FB49FA1) is read by
  nothing. A queued 19 whose target is gone stays queued past
  PLAYER_DispatchClickAction, and UNITMODE_RunQueuedCommand clears it on the
  next tick.
- **REACHABLE, harmless** -- PLAYER_OnMouseClick's once-a-frame guard lets a
  second click at (0, 0) through (0x6FB4994C..0x6FB4995C): its callers pass
  the cursor's position (Game.cpp), so two button events in one frame with the
  cursor on the screen's top-left pixel are both handled.

Reproduced -- each as its bytes have it.

**DECLARED DIFFERENCE, 2026-10-12.** A player's mode of 20 or more reads a row of zeros in our C
(Player.cpp's PLAYER_GetAnimModeControlRow), and a negative use state plays no
sound (PLAYER_CanUseSkill), where the bytes read the original's .data past
gnAnimModeControl and before sgSnds. Both UNREACHABLE, as above.

## PLAYER_ReleaseClickAndResetHireling tests a display state for 2, and nothing writes 2

`0x6FB4B399` (`cmp dword ptr [eax+8], 2`) in PLAYER_ReleaseClickAndResetHireling, and its routed copy in
PLAYER_OnMouseClick (`0x6FB49C5D`), `Player.cpp`. **Argued 2026-10-13.** **UNREACHABLE.**

The dword is the roster pet record's +0x2C, the third of the block ROSTERPETS_GetPetInfo hands out --
D2ClientPetInfoStrc's nDisplayState. Every writer, read off the image:

    6FAB1746  mov [eax+2Ch], edx     ROSTERPETS_OnPetPacket81's copy of the block's third dword, which its one caller,
                                     SCMD_HandleSrv81, stores as a literal: mov dword ptr [esp+14h], 0 at 0x6FAB41B9
    6FAB1579  rep stosd              the record's allocation zeroed whole, in ROSTERPETS_FindOrAddPet
    6FAB16B8  rep stosd              the same, in OnPetPacket81's inlined copy of it
    6FAB6444  mov [eax+8], ecx       SELECT_RosterPetsSetHirelingDisplayState, whose three callers push
                                     1 (0x6FADB9DF, HIREABLES_HirIconsOnF11) and 0 (0x6FB49C67, 0x6FB4B3A2)

gpRosterPetList's references are all RosterPets.cpp's, and of GetPetInfo's five callers only the setter writes through
the answer (measured 2026-09-24). So the value is 0 or 1, the `== 2` is never true, and the reset behind
it -- the setter called with 0 -- never runs from these two sites. Reproduced as the bytes have it.

## At 800x600 a dialog's click-away asks the red buttons about boxes where neither is drawn

`newstats.cpp`, NEWSTATS_IsPointOverButton (0x6FAEECC0) and
NEWSTATS_NewSkillsIsPointOverButton (0x6FAEED20), and `dialog.cpp`,
DIALOG_OnMouseDown (the calls at 0x6FAD5FFA and 0x6FAD6033). **Argued
2026-09-29** at newstats.cpp's close. **REACHABLE.**

A click outside a dialog whose pfnCallback2 answers non-zero -- the three NPC
dialogs' click-away callbacks answer 1 (npcmenu.cpp, THE DIALOG CALLBACKS) --
is consumed by DIALOG_OnMouseDown only if one of the two hit tests places the
message's point on a lit button; otherwise it returns without consuming
(dialog.cpp). Neither test has a resolution arm. Every bound strict, they take
x 41..73 (or w/2+41..w/2+73 with the left panel open), y h-138..h-103 for the
stats button and x w-72..w-41 (or edge-72..edge-41 with the right one),
y h-137..h-103 for the skills button -- the 640x480-style boxes. At 800x600
the lit buttons are drawn in the control panel's strip at (206, h - 8) and
(563, h - 8) (NEWSTATS_PanelDrawNewStatsButton 0x6FAEF656, NEWSTATS_PanelDrawNewSkillButton
0x6FAEFA2C) and pressed there by the press handlers' 800x600 arms. So at
800x600, with an NPC dialog open and a button lit:

- a click on the drawn button is not consumed by the dialog and goes on down
  the handler chain;
- a click in the phantom box -- x 41..73, y 462..497 (33 x 36) for the stats
  button with no left panel, x 728..759, y 463..497 (32 x 35) for the skills
  button with no right panel, at w 800 and h 600 -- is consumed after the
  click-away has run, where a click one pixel outside it is not.

Levelling up, talking to an NPC and clicking there at 800x600 reaches it; the
click that is lost is whatever that spot would have taken. What lies under the
two boxes on screen was not measured.

Reproduced -- the hit tests have no 800x600 arm, as the bytes have them.

## The new-skills button tests its points signed, the new-stats button tests for zero

`newstats.cpp`, NEWSTATS_UpdateButtonVisibility (0x6FAEEC00).
**Argued 2026-09-29** at newstats.cpp's close. **INERT.**

STAT_STATPTS is tested `test eax, eax / jnz` to put the button up and `jz` to
take it down (0x6FAEEC34, 0x6FAEEC44); STAT_SKILLPTS `jg` and `jle`
(0x6FAEEC7C, 0x6FAEEC98). The two differ only on a negative count, and
neither stat holds one: ItemStatCost.txt's rows 4 and 5 (the stock file) have
`Signed` and `CSvSigned` empty, `Send Bits` 9 and `CSvBits` 10 and 8; and the
server spends both guarded -- D2MOO's PlayerStats.cpp:136 returns before
taking a stat point when the count is 0, D2Game Skills.cpp:2855 takes skill points
only when the count is at least the cost. So the two tests agree on every count
the stock server produces.

Reproduced -- each test as its bytes have it.

## NEWSTATS_RecipeScrollShow copies the recipe text with no bound

`newstats.cpp`, NEWSTATS_RecipeScrollShow (Unicode::strcpy at 0x6FAEE997).
**Argued 2026-09-29** at newstats.cpp's close. **LATENT.**

The destination, gwszRecipeScrollText, holds 4096 characters
(CRT_StaticInit10's `mov edi, 1000h`, 0x6FAEE977), and the copy runs to the
source's NUL. The callers prevent an overrun. The one caller,
TEXT_HandleChatMessage's message type 7 (text.cpp), copies from a
256-character stack buffer filled by `MultiByteToWideChar(..., szMessage, 256,
..., 256)`, and szMessage is terminated within 256 bytes: SCMD_HandleSrv26
takes it off the wire with `STORM_SStrCopy(..., 256)` (SCmd.cpp), which writes
at most 255 characters and a NUL (Msg.cpp, THE LENGTH), and the explicit length
converts that NUL too. So at most 256 characters, the NUL among them, reach a
4096-character buffer. And no stock server sends type 7: D2MOO's D2Game sets
nMessageType to 1, 2, 4, 5 and 6 and to nothing else (PlrMsg.cpp,
SUnitMsg.cpp), its 0x26 sender forwarding the type it is handed and asserting
the text under 256 characters (D2Game SCmd.cpp:1050).

Reproduced -- the unbounded copy is written as the bytes have it.

## The four button draws use the level-up cels untested

`newstats.cpp`, the four PANEL_DrawNew* (0x6FAEF2A0..0x6FAEFA3E).
**Argued 2026-09-29** at newstats.cpp's close. **LATENT.**

NEWSTATS_LoadImages stores UI_LoadImageFromUiDir's answers for `Panel\Level`
and `Panel\Levelsocket` untested, and the four draws hand them to D2CMP and
D2Gfx untested, where NEWSTATS_RecipeScrollDraw tests its image. A missing file makes
the handle 0: the archive read answers NULL for a file it cannot open (D2MOO's
Archive.cpp, ARCHIVE_AllocateBufferAndReadFile), D2CMP's CelFileNormalize
(#10024, 0x6FDF1EA0) writes its out-pointer only for a non-NULL file (`if
(a1)`), and D2Client's ARCHIVE_CellFileLoad (0x6FAA1000) returns the 0 it set before the
call. The lit draws' first CelGetHandle (#10036, the calls at 0x6FAEF421 and
0x6FAEF7DD) then asserts `ValidCelContext( pCelContext )` and exits
(0x6FDF2585..0x6FDF25A0, CelCmp.cpp:1379) -- a halt, not a stray draw; the
grey draws hand the 0 to D2Gfx #10072, which was not followed. The data
prevents it: both cels ship with the game -- the button is seen at every
level-up in stock play, and levelsocket.dc6 lies loose in this install
(data\global\ui\PANEL) -- and CHAR_CharPanelDraw, HELP_DrawHelpHint and
QUESTLOG_DrawFrameAndTitle draw the same two handles.

Reproduced -- no test is added.

## The four button handlers' escape-menu early-outs write no bReturn

`newstats.cpp`, NEWSTATS_OnLButtonDown (0x6FAEED80, the `jnz short
loc_6FAEEDF7`), NEWSTATS_OnLButtonUp (0x6FAEEFD0), NEWSTATS_NewSkillsOnLButtonDown
(0x6FAEEEA0, `jnz short loc_6FAEEF12`) and NEWSTATS_NewSkillsOnLButtonUp (0x6FAEF130,
`jnz short loc_6FAEF1B5`). **Argued 2026-09-29** at newstats.cpp's close.
**NOT ESTABLISHED.**

At 800x600 with the escape menu open each returns having written nothing,
where every other rejection stores bReturn 0. It is the shape of
`PANEL_GameOnMouseMove`'s entry above, and its question: Storm presets bReturn to
0 (SMsgDispatchMessage, `v10 = 0` at 0x6FFCD254, read 2026-09-29 in the Storm
database) and walks every registered handler, so the silence shows only if a
handler earlier in the WM_LBUTTONDOWN or WM_LBUTTONUP walk writes 1 without
breaking the chain. **What would settle it:** that walk's order, and whether
such a handler is registered with these.

Reproduced -- the four silent returns.

## The button handlers' 800x600 arms test the cursor, the other arms the message

`newstats.cpp`, the four button handlers (0x6FAEED80, 0x6FAEEEA0,
0x6FAEEFD0, 0x6FAEF130). **Argued 2026-09-29** at newstats.cpp's close.
**NOT ESTABLISHED.**

Each handler's 800x600 arm tests GetMouseX and GetMouseY; its other two arms
test the message's nXpos and nYpos (the heads). The draws test the cursor on
every arm. The two can differ only when the cursor position the client keeps
is not where the click landed -- the press handlers hand the message to
CURSOR_UpdateCursorPosInGame only after the test. **What would settle it:**
whether every button message that reaches these handlers follows the mouse move
that stores the same point (CURSOR_OnMouseMove, Cursor.cpp).

Reproduced -- each arm reads what its bytes read.

## With both panels open the press handlers take boxes the draws have hidden

`newstats.cpp`, NEWSTATS_OnLButtonDown and NEWSTATS_NewSkillsOnLButtonDown
against NEWSTATS_PanelDrawNewStatsButton and NEWSTATS_PanelDrawNewSkillButton. **Argued
2026-09-29** at newstats.cpp's close. **NOT ESTABLISHED.**

Outside 800x600 each draw hides its button when both panels are open
(`cmp ebp, 3`, 0x6FAEF377 and 0x6FAEF747) and clears its pressed flag every
such frame. The press handlers test only their own side's state -- PANEL_LEFT
for stats, PANEL_RIGHT for skills -- so with both panels open they take the
edge boxes, x 41..73 and x w-72..w-41, which then lie under the left and the
right panel. A press there arms the flag, plays sound 4 and consumes the
message; the next drawn frame clears the flag, so the release finds it 0 and
toggles nothing. **What would settle it:** whether the open panels' own
handlers see such a click first and take it -- Storm's walk order for the
WM_LBUTTONDOWN list, the question of `PANEL_GameOnMouseMove`'s entry.

Reproduced.

## At 800x600 releasing a red button opens its sheet and leaves the button lit

`newstats.cpp`, NEWSTATS_OnLButtonUp and NEWSTATS_NewSkillsOnLButtonUp.
**REFUTED 2026-09-29** at newstats.cpp's close.

Each release's 800x600 arm issues one toggle, the sheet on, where the other
arm issues two, the button off and then the sheet on; the heads left open how
the button closes at 800x600. The draws answer it. At 800x600 the lit draw
skips its whole hiding set (the `jz` at 0x6FAEF375 and 0x6FAEF745) and draws
the button in the control panel's strip, below every panel, so it stays lit
with the sheet open, and NEWSTATS_UpdateButtonVisibility takes it down when the
points run out -- the only thing that takes an open button down there.
Outside 800x600 the button would be hidden by the sheet it opens (UI_STATSCREEN
and UI_SKILLTREE are in the hiding sets), and the release closes it with the
second toggle. The one toggle is the 800x600 layout's, not a slip.

Reproduced -- one toggle and two, as the bytes have them.

## With the belt's rows pinned and the inventory open, the mini panel's clicks land 118 pixels left of its buttons

`minipanel.cpp`, MINIPANEL_OnLButtonDown (0x6FB099F0) and
MINIPANEL_OnLButtonUp (0x6FB09730) against MINIPANEL_Draw (0x6FB08DE0).
**Argued 2026-09-29** at minipanel.cpp's close. **REACHABLE.**

The handlers place their strip from three terms: -118 when
PANEL_InvHintPhaseSuppressed() and the belt has more than one row (0x6FB09A79,
0x6FB097ED), -118 more when gbMiniPanelInvSideOpen (0x6FB09A87,
0x6FB097FB), +119 when gbMiniPanelWindowOpen. MINIPANEL_Draw has one layout
for the inventory side, layout 1, whichever of the inventory, the skill tree
or the belt's rows put it there, and draws its first button at w/2 - 202. So
with the inventory or the skill tree up WHILE the belt's rows show, the
handlers test the strip (w/2 - 320, w/2 - 147) around buttons drawn from
w/2 - 202 to w/2 - 35 (- 56 in single player): the first two answer, the
third on its left twelve columns (w/2 - 159..w/2 - 148), and none from the
fourth on -- in multiplayer the party screen, the automap, the message log,
the quest log and the game menu; in single player the automap onwards. A
press there misses the strip, arms nothing and is not consumed, and the
release returns on gbMiniPanelButtonDown clear. The hint follows the drawn
layout, so it names the button the click then misses. What the press does
further down the chain was not measured.

Both halves are reachable. The belt's rows stay up with the cursor away
from the belt only when pinned: the hover that pops them is cleared as the
cursor leaves the belt unless UI_BELTROWS is on (PANEL_GameOnMouseMove,
panel.cpp), and the Show Belt command (CMDTBL_ActionShowBelt, CmdTbl.cpp)
turns it on with a belt equipped; PANEL_BeltShowPanel then sets
gbBeltPanelVisible every frame and PANEL_DrawBelt sets the flag for any belt
of more than one row. gaUiToggleRules (0x6FB90310, read off the image)
lets the rows and the inventory coexist in either order -- row 1, column 31
and row 31, column 1 are 0 -- and the rows be pinned over the skill tree
(row 4, column 31 is 0; opening the tree over pinned rows closes them, row
31, column 4 is 1). Control: row 4, column 1 is 1, the inventory closing
the tree.

Reproduced -- the handlers' terms and the draw's layout as their bytes have
them.

## While the waypoint panel is up, its step-aside box misses the mini panel's last button and bottom rows

`minipanel.cpp`, MINIPANEL_WaypointIsMouseOverMiniPanel (0x6FB09CC0),
against MINIPANEL_LayOutButtons (0x6FB092B0) and the two click handlers, and
`waypoint.cpp`, WAYPOINT_OnMouseDown and WAYPOINT_OnMouseUp.
**Argued 2026-09-29** at minipanel.cpp's close. **REACHABLE.**

With the waypoint panel open MINIPANEL_Draw puts the mini panel in layout 3,
right of centre: the first button at w/2 + 57 in single player (0x6FB09308)
and w/2 + 35 otherwise, 21 apart, each answering x in (button, button + 20)
inside the handlers' strip, y in (h - 69, h - 47). The waypoint handlers ask
MINIPANEL_WaypointIsMouseOverMiniPanel first and step aside on a yes; its box is x in
(w/2 + 56 or + 35, that + 20 * count), y in (h - 76, h - 50)
(0x6FB09D21..0x6FB09D38). So:

- the last button's right seven columns -- w/2 + 195..w/2 + 201 in
  multiplayer, w/2 + 196..w/2 + 202 in single player -- and the strip's
  bottom rows h - 50..h - 48 under every button lie outside the box. There
  WAYPOINT_OnMouseDown takes the press as off its panel (x > w/2), sends
  the close, turns UI_WAYPOINT off and consumes nothing; over the rest of
  the row it steps aside and the panel stays open;
- the other way, the box's rows h - 75..h - 69 above the strip, the
  one-column gaps between buttons and, in single player, column w/2 + 57
  step the waypoint panel aside for a press no mini-panel button takes.

Storm runs the newest registration first, and the waypoint
panel's handlers are registered when it opens, after the mini panel's,
which UI_InitUiSubsystems turns on at the game's start -- unless the
mini panel was switched on while the waypoint panel was up. Whether
MINIPANEL_OnLButtonDown then sees a press the waypoint handler passed on,
after that handler unregistered itself in the walk, was not read; the
release finds the waypoint's handlers gone, and acts only if no frame has
redrawn the panel centred in between (the strip then lies at
w/2 - 84..w/2 + 89). Either way a click on those columns and rows closes the
waypoint panel, where one column left or one row up it does not. The last
button is the game menu in both modes.

Reproduced -- the box and the buttons as their bytes have them.

## With the mini panel hidden, the waypoint panel still steps aside for its box

`minipanel.cpp`, MINIPANEL_WaypointIsMouseOverMiniPanel (0x6FB09CC0).
**Argued 2026-09-29** at minipanel.cpp's close. **REACHABLE.**

The box asks UI_WAYPOINT and UI_MINIPANEL (0x6FB09CCE, 0x6FB09CDE) and
nothing about gbMiniPanelHidden. MINIPANEL_Draw hides the panel when a
window of its first chain -- the waypoint panel is one -- and the inventory
side are open together, and both happen with the waypoint panel up.
gaUiToggleRules lets the waypoint panel open over the inventory or the skill
tree (row 1, column 20 and row 4, column 20 are 0; opening either over the
waypoint panel closes it, row 20, columns 1 and 4 are 1), and the belt's
pinned rows and the waypoint panel coexist in either order (row 20, column
31 and row 31, column 20 are 0). So with nothing of the mini panel drawn, a
press in x w/2 + 36..w/2 + 194, y h - 75..h - 51 in multiplayer
(w/2 + 57..w/2 + 195 in single player) leaves the waypoint panel open and
goes on down the chain, and a press right of centre outside it closes the
panel (waypoint.cpp). What lies there -- the inventory's bottom or the
belt's rows -- was not measured.

Reproduced.

## Under a dialog, the mini panel's row test is single player's centre row

`minipanel.cpp`, MINIPANEL_IsPointOverMiniPanelRow (0x6FB09C50), and
`dialog.cpp`, DIALOG_OnMouseDown and DIALOG_OnMouseUp. **Argued
2026-09-29** at minipanel.cpp's close. **REACHABLE** in multiplayer.

A dialog lets a click through to the mini panel when UI_MINIPANEL is on and
this test holds: x in (w/2 - 74, w/2 + 71), y in (h - 69, h - 50), every
bound strict (0x6FB09C62..0x6FB09C7C), and no layout offset. Those are the
single-player centre row's buttons -- MINIPANEL_LayOutButtons' first at
w/2 - 74, seven 21 apart, each answering x in (button, button + 20) --
less the last button's last column, w/2 + 71; and in both modes the strip's
bottom rows h - 50..h - 48 lie outside it. In multiplayer the centre row
starts at w/2 - 84 and runs eight buttons, answering w/2 - 83..w/2 + 82, so
the first button's left ten columns and the last button's right twelve lie
outside the test. There, with an NPC dialog open, DIALOG_OnMouseDown runs
the click-away callback, which closes the dialog and answers 1 (npcmenu.cpp,
THE DIALOG CALLBACKS), and does not consume the press; inside the test the
dialog stays open and the press goes straight on. So in multiplayer a click
on the character sheet button's left ten columns, or on the game menu
button's right twelve, also ends the NPC conversation, where the rest of
those buttons leave it open. The other two layouts are not tested at all;
whether the mini panel can be in either under a dialog was not read.

Reproduced -- the rectangle as the bytes have it.

## The mini panel's two images are drawn untested

`minipanel.cpp`, MINIPANEL_LoadImages (0x6FB09B80), against
MINIPANEL_DrawBackgroundSegment, MINIPANEL_DrawButtonCel and the copies in
MINIPANEL_Draw and MINIPANEL_DrawButtons. **Argued 2026-09-29** at
minipanel.cpp's close. **LATENT.**

MINIPANEL_LoadImages stores UI_LoadImageFromUiDir's answers for
`Panel\minipanel` or `Panel\minipanel_s` and `Panel\minipanelbtn`
untested, and every draw puts gpImgMiniPanel or gpImgMiniPanelButtons into
a D2GfxDataStrc and hands it to D2Gfx #10072 untested, where
MINIPANEL_SaveAndUnload tests both before unloading them. A missing file
makes the handle 0 (the entry "The four button draws use the level-up cels
untested", above); #10072 forwards to the renderer's pfCelDraw (D2MOO's
D2Gfx Texture.cpp:13), which was not followed. The data prevents it: the
three files lie loose in this install (data\global\ui\PANEL) and the mini
panel is drawn in every stock game. The loads run in
UI_InitUiSubsystems before the first draw and the unload in
UI_ShutdownUiSubsystems, on GAME_AppModeGame's way out, so no draw meets the NULL the unload writes.

Reproduced -- no test is added.

## The mini-panel hint copies and appends its label with no bound

`minipanel.cpp`, MINIPANEL_DrawButtonHint (0x6FB09460): a
Unicode::strcpy and two Unicode::strcat into a 200-unit label (`0C8h` at
0x6FB095C7). **Argued 2026-09-29** at minipanel.cpp's close. **LATENT.**

The label is the button's tbl string, then up to two key suffixes. Each
suffix is written by Unicode::sprintf with the count 30 into its own
30-unit buffer, cleared first; D2MOO's reconstruction of that function
(#10033, D2Lang 0x6FC11670; D2UnicodeStr.cpp) stops at the count -- its
`%s` arm through a strncat of count - (written + 1), which the clearing
makes safe -- so each suffix appends at most 29 units. The binary's body was
not read. The string is the rest: over the stock install's language packs
that carry a string.tbl (CHI DEU ESP FRA ITA JPN KOR eng --
POL, POR and RUS carry none, and DEU's and FRA's hold other keys at these
indices), the longest of tbl 4169..4176 and 4178 is 21 bytes (ITA's 4171,
CHI's 4176, ESP's 4175), so a label reaches at most 21 + 29 + 29 = 79 units
and its NUL. A tbl string of more than 141 units would overrun it; none in
the stock data comes near.

Reproduced -- the unbounded copies as the bytes have them.

## In an expansion install the play prompt asks for the Expansion Disc

`InsertDisc.cpp`, INSERTDISC_DrawInsertDiscPrompt (0x6FADBF50) and its
frame INSERTDISC_DrawInsertDiscPromptScene (0x6FADC180). **Argued 2026-09-29** at
InsertDisc.cpp's close. **REACHABLE; whether the disc it names is the wrong
one NOT ESTABLISHED.**

The second line is tbl 21880 "Expansion Disc" when FOG_IsExpansion()
answers non-zero and 5169 "Play Disc" otherwise, whichever Show function is
up (0x6FADC054, 0x6FADC27F). D2Win's ARCHIVE_LoadExpansionArchives (#10171,
D2Win 0x6F8A5E80, D2MOO's D2WinArchive.cpp line for line) asks the same Fog
#10227: non-zero, it hands the play prompt to the loads of d2char.mpq and
d2music.mpq (`push esi`, 0x6F8A5ECA, 0x6F8A5EEC) and the expansion prompt
to d2Xmusic, d2Xtalk and d2Xvideo.mpq (`push edi`, 0x6F8A5F12..0x6F8A5F56);
zero, the play prompt to the same two (0x6F8A602C, 0x6F8A604E). The loader,
ARCHIVE_LoadMPQFile (0x6F8B2399), calls the prompt only when the archive
cannot be opened (D2MOO's ARCHIVE_FindAndOpenArchive: the working
directory, the executable's, the root of every CD-ROM drive; not read in
the bytes). So when d2char.mpq or d2music.mpq cannot be found, an expansion
install asks for the "Expansion Disc" and a classic one for the "Play Disc"
-- one pair of archives under two disc names. Which retail disc carries
them is in no image read here, so whether the expansion install's text
names the wrong disc is not settled. The prompt's other use is right: for
videos 6 and 7 VIDEO_PlayCinematic hands ARCHIVE_LoadVideoArchives (#10172)
the play prompt with d2Xvideo.mpq (0x6F8A60CD), the archive #10171 asks for
with the expansion prompt. FOG_UseDirect (#10117) drops the prompts at the
top of both loaders.

Reproduced -- the text as the bytes choose it.

## Slots 1 and 2 of gafnMessageCallbacks are never called

`InsertDisc.cpp`, INSERTDISC_DrawInsertDiscPromptScene (0x6FADC180) and
INSERTDISC_DrawInsertCinematicsDiscPromptScene (0x6FADC970); `Game.cpp`,
gafnMessageCallbacks and GAME_SetActiveMessageCallback. **Argued
2026-09-29** at InsertDisc.cpp's close. **UNREACHABLE** -- two functions,
525 and 605 bytes, that never run.

The three Show functions install slot 1 or 2 with
GAME_SetActiveMessageCallback just before their nested
WNDPROC_WindowRunMessagePump, and slot 0 just after it. gpfnActiveMessageCallback,
where the slot goes, is read at two places, 0x6FAA9B78 and 0x6FAAA2D8, both
calls in GAME_UpdateFrame (four references, those
two and the writes of GAME_SetActiveMessageCallback and
GAME_InitGame). GAME_UpdateFrame's one referencer is GAME_AppModeGame,
which hands it to the OUTER WNDPROC_WindowRunMessagePump as its idle function, and the pump calls only its own idle function
(WNDPROC.cpp). While slot 1 or 2 is installed the prompt's nested pump is
running, with the prompt's own idle function, and slot 0 is back before the
outer pump runs again. So neither slot's function is ever called, and the
database's names for them, "Offline" and "Selectable", rested on nothing in
the code; both are named ...Scene
after what each draws and slot 0's GAME_DrawGameScene. Each is also the
body its prompt's idle function inlines (0x6FADBF58..0x6FADC15A,
0x6FADC6E8..0x6FADC93C), which does run.

Reproduced -- the table carries both (Game.cpp) and both are written.

## The cinematics prompt's "Expansion Disc" line is never drawn

`InsertDisc.cpp`, INSERTDISC_DrawInsertCinematicsDiscPrompt (0x6FADC6E0)
and INSERTDISC_DrawInsertCinematicsDiscPromptScene (0x6FADC970). **Argued 2026-09-29** at
InsertDisc.cpp's close. **UNREACHABLE.**

The second line is tbl 21880 when gnDiscPromptKind is non-zero and 5170
"Cinematics Disc" when it is 0 (`test eax, eax`, 0x6FADC7EE, 0x6FADCA79).
The kind's one writer is INSERTDISC_SetDiscPromptKind (three
references, the two reads and that write), called twice, both in
VIDEO_PlayCinematic (0x6FAA183F, 0x6FAA1A43). The first
stores bExpansionDisc, and the cinematics prompt is handed to
ARCHIVE_LoadVideoArchives only when that is FALSE (Video.cpp); the second
stores TRUE and precedes the play prompt, which asks FOG_IsExpansion()
instead. VIDEO_PlayCinematic is the cinematics prompt's one referencer
(0x6FAA1861). So the prompt always reads "Cinematics Disc".

Reproduced.

## In a game, a key pressed under a disc prompt runs its bound command

`InsertDisc.cpp`, INSERTDISC_OnEscape (0x6FADC6D0) and
INSERTDISC_CineDiscOnEscape (0x6FADCEF0), with the prompts' Enter records. **Argued
2026-09-29** at InsertDisc.cpp's close. **REACHABLE** in a game.

Both Escape handlers store one flag and return -- no bReturn, no
SMsgBreakHandlerChain -- and that matters less than the key's order. Storm
dispatches a WM_KEYDOWN to the MESSAGE list before the KEY list, and a
break does not cross from one list to the other (the entry
"HELP_HelpScreenOnDismissKey does not claim ESC or SPACE", from Storm's bytes).
The prompts' Escape and Enter records are KEY records
(D2_WINPROC_KEYDOWN); in a game the MESSAGE list holds CMDTBL_OnKeyDown
(gKeyCommandHandlers, registered by CMDTBL_SetKeyHookMode), which tests
nothing about a prompt or a cinematic (CmdTbl.cpp). A prompt comes up in a
game through SCMD_HandleSrv61 -> QUESTS_PlayCinematicFromPacket ->
VIDEO_PlayCinematic, which turns no key handler off before D2Win's loader
calls the prompt -- the cinematics prompt when an act's cinematic finds no
d2video.mpq, the play prompt when videos 6 and 7 find no d2Xvideo.mpq. So
with the keys hooked for commands (gnKeyHookMode 1), every bound key
pressed under a prompt runs its command first: ESC's, CMDTBL_ActionEscMenu,
closes the game menu if it is open, else the open panels, else opens the
menu (nothing under an NPC dialog or a popup), and the other keys whatever
they are bound to. What that leaves on screen is drawn when the prompt
ends. The prompt's own Escape runs after it: the play prompt's clears
gbDiscPromptDone and cancels nothing, the cinematics prompt's cancels. What
the unclaimed Escape reaches after that -- an older KEY record for
VK_ESCAPE -- was not traced.

Reproduced.

## A disc prompt's button stays armed after a missed release

`InsertDisc.cpp`, INSERTDISC_OnLButtonDown and _OnLButtonUp
(0x6FADC550, 0x6FADC5F0), INSERTDISC_CineDiscOnLButtonDown and _OnLButtonUp
(0x6FADCCC0, 0x6FADCDB0), and gbDiscPromptButtonDown (0x6FBB36C0). **Argued
2026-09-29** at InsertDisc.cpp's close. **REACHABLE, benign.**

gbDiscPromptButtonDown has nine references in six handlers and none in a Show
function: a press on a button sets it (0x6FADC5A5,
0x6FADCD5D), a release on a button clears it (0x6FADC64E, 0x6FADCE12,
0x6FADCE61) and so does Enter (0x6FADC69A, 0x6FADCEBA), and the releases
read it (0x6FADC5F0, 0x6FADCDB0). A press on a button released off every
button leaves it 1: the play prompt's release resets the mouse state and
does not clear it, and the cinematics prompt's has no miss arm at all when
armed -- it claims the message (0x6FADCE72, 0x6FADCE79) with no reset and
no disarm. A later release on a button then fires whatever press began it:
on the play prompt an OK; on the cinematics prompt whichever button the
release lands on -- there the release decides even without a miss, one flag
arming both, so a press on OK released on CANCEL cancels and the cinematic
is skipped. The flag is shared and never cleared on entry, so the armed
state also carries into the next prompt of either kind. Every outcome is
one a click could have chosen.

Reproduced.

## An OK and an Escape in one pass of a disc prompt's pump

`InsertDisc.cpp`, INSERTDISC_OnEscape (0x6FADC6D0),
INSERTDISC_CineDiscOnEscape (0x6FADCEF0) and INSERTDISC_ShowInsertCinematicsDiscPrompt
(0x6FADCBD0). **Argued 2026-09-29** at InsertDisc.cpp's close.
**REACHABLE, benign.**

WNDPROC_WindowRunMessagePump calls its idle function only when no message is
waiting (WNDPROC.cpp), so two messages that arrive together are both
handled before the prompt's draw looks at the flags. On the play prompt an
OK or Enter sets gbDiscPromptDone and an Escape after it clears it: the
prompt stays, and the OK has to be given again. On the cinematics prompt OK
sets gbDiscPromptAccepted and Escape gbDiscPromptCancelled, and the Show
function answers gbDiscPromptCancelled == 0 (0x6FADCC4D): the cancel wins
whichever came first, and the cinematic is skipped.

Reproduced.

## The disc prompts draw their cels untested

`InsertDisc.cpp`, INSERTDISC_ShowInsertPlayDiscPrompt,
INSERTDISC_ShowInsertExpansionDiscPrompt and INSERTDISC_ShowInsertCinematicsDiscPrompt
(0x6FADC390, 0x6FADC470, 0x6FADCBD0) against the four draws. **Argued
2026-09-29** at InsertDisc.cpp's close. **LATENT.**

Each Show function stores ARCHIVE_CellFileLoad's answers for popupok (twice) and
okcancelbtn untested, and every draw hands them to D2Gfx #10072 untested,
where the Show function tests each before unloading it. ARCHIVE_CellFileLoad
(0x6FAA1000) answers 0 for a file that will not open: its handle starts at
0 and D2CMP_CelFileNormalize (#10024, D2CMP 0x6FDF1EA0) does nothing with a
NULL file. #10072 forwards to the renderer, not followed (as for the mini
panel's images, above). The data prevents it: both files lie loose in this
install, data\global\ui\BIGMENU\popupok.dc6 and
data\global\ui\MENU\okcancelbtn.dc6.

Reproduced -- no test is added.

## LANG_SubstituteArg reads the word below its buffer when a placeholder opens the text

`Shared.cpp`, LANG_SubstituteArg (0x6FADCF10). **Argued
2026-09-29** at InsertDisc.cpp's close. **REACHABLE, and the same answer; a
DECLARED DIFFERENCE in the C.**

The space test reads `[esp+eax*2+12h]` (0x6FADCFA4), eax the scratch
buffer's length, with no test for 0; at 0 that is the word below the
buffer, the top half of the digit's dword, `(nIndex + '0') >> 16`. The
buffer is empty when the placeholder opens the text and the value is "". A
magic item with no prefix does that: INV_BuildItemNameText empties
wszPrefixPart and passes it as `%0` (inv.cpp), and MagicFormat opens with
`%0` in the eng, CHI, JPN and KOR packs ("%0 %1 %2"). nIndex is 0 there,
the word 0, the test fails, and the tail starts at the character after the
placeholder -- the answer the C gives by testing the length first. The two
would differ only for an nIndex whose `+ '0'` has 0x20 in its top half; the
stock call sites pass 0 to 2.

Reproduced as a declared difference: reading below an array is not C.

## The item-name formatter's buffers have no bound

`Shared.cpp`, LANG_FormatString (0x6FADCFE0) and
LANG_SubstituteArg (0x6FADCF10). **Argued 2026-09-29** at InsertDisc.cpp's
close. **LATENT.**

Four copies with no bound: the arguments into ten 200-character slots (an
eleventh argument overruns them), a form into a 100-character buffer, the
text through a 400-character scratch buffer, and the result into the
caller's wszOut. Measured over the stock install's language packs whose
tables hold the formats at this build's indices -- eng, CHI, ESP, ITA, JPN
and KOR; DEU's and FRA's string.tbl hold affix names at 1711..1718, and
POL, POR and RUS carry no full set -- reading every string through its
table's index array:

* the eleven call sites clean 10h, 14h or 18h (InsertDisc.cpp batch 3):
  one to three arguments and the NULL;
* the longest argument, over the strings named by the name columns of
  weapons, armor and misc, the magic and rare affixes, sets, set items,
  uniques, monstats, SuperUniques and the monster affixes, is 88
  characters, ESP's "Wyrm's" -- four gendered forms in one string; a
  hireling's description (tbl 3371..11090, D2MOO's
  MONSTERS_GetHirelingDescString) is 16 at most (ITA);
* a kept form is one piece of such a string;
* a magic name is 76 at most, the prefix, base and suffix columns' longest
  added -- the base over weapons and armor, the misc bases a magic item can
  have (rings, amulets, jewels, charms) being 11 at most -- (ESP; eng 65),
  against INV_BuildItemNameText's smallest nMaxLen,
  128, which that function also asserts before the call
  (inv.cpp's line 2243); the rare name 39 at most (ESP) into 200, the set
  line 31 (ITA) into 100, a hireling's description 16 into npcmenu.cpp's
  150, and a monster's name 73 at most (ESP) into
  MONUNIQUE_BuildMonsterNameText's buffer, which was not read (unwritten).

None comes near. A table string that did would overrun.

Reproduced.

## The formatter's directive takes its slot digits unchecked

`Shared.cpp`, LANG_FormatString (0x6FADCFE0). **Argued
2026-09-29** at InsertDisc.cpp's close. **LATENT.**

With a ':' in the format, the FIRST 'a' and the first 'n' of the whole
format are taken if they lie before it (`jnb`, 0x6FADD0B3, 0x6FADD0D9), and
the character after each is the slot, `ch - '0'`, never tested: a non-digit
indexes outside the ten slots, '/' makes -1 and reads as absent, and an 'a'
in text that merely contains a ':' is taken for a directive. And 'n' alone
strips four characters from its slot whether or not it opens with a tag.
The stock formats: the only ones with a ':' are ESP's and ITA's "a0n1:..."
-- the item formats 1711..1718 and the monster formats 1721, 1722 and
11072 (MONUNIQUE_BuildMonsterNameText, 0x6FB44BA8, 0x6FB44C69, 0x6FB44CBF)
-- and npcmenu.cpp's own "a0:%0"; every digit is 0 or 1, and no format has
an 'n' without an 'a'.

Reproduced.

## Three language packs write the aura line's format with the name first

`mods.cpp`, MODS_FormatPropertyLine (0x6FAF21C0), descfunc 16.
**Argued 2026-09-30** at mods.cpp's close. **REACHABLE in the retail DEU,
ESP and POL clients of 1.10 -- the three strings were corrected in the
tables by 1.14d.** Settled the same day: at the close the verdict was
REACHABLE with the install's tables and their origin NOT ESTABLISHED.

The format of a descfunc's sprintf is a string-table entry, and D2Lang's
Unicode::sprintf (0x6FC11670) takes its arguments strictly in order. It
knows four specifiers and each takes one: %d and %u (itoa / ultoa), %s, and
%% -- case 37 (0x6FC11839) writes the '%' and falls into the tail at
0x6FC1185C that steps the argument pointer (`add eax, 4`) like the others;
anything else is "Unknown format specifier" and `exit` (0x6FC1191E).
Descfunc 16 pushes the value, then the skill's name (`push eax`, `push ebp`,
0x6FAF2968..0x6FAF2969). Every patchstring.tbl in the install is 1.10's --
1143 records, as eng's, with 1125 to 1143 of eng's keys at eng's index -- and three write ModitemAura (item_aura, tbl
10976) with the name first: DEU '%s-Aura Level %d wenn verwendet', ESP 'Aura
%s en el nivel %d cuando ...', POL '... (%s) na poziomie %d'.
Through them %s takes the level for a string pointer, and D2Lang's `s` arm
(0x6FC117DF) reads the word at that address: an access violation for any
level above 0. Six stock rows carry item_aura -- Azurewrath and the five
runewords Runes.txt names Bramble, Doomsayer, Exile's Path, Hand of Justice
and The Beast, all `complete`.

The tables are the retail ones, extracted from 1.10's Patch_D2.mpq; 1.14d's
patch archive was extracted the same way to compare. 1.14d's patchstring.tbl appends 36 records and keeps
the key at every one of 1.10's 1143 indices (eng, CHI, DEU, ESP, POL
checked), and over the nine languages both carry, matched by key, the one
string whose specifiers changed -- the placeholder `x` rows aside, 15 more
of them -- is ModitemAura in DEU, ESP and POL, each rewritten level first,
the order eng, FRA, ITA, CHI, JPN and KOR always had: DEU 'Stufe %d der
%s-Aura, wenn getragen.', ESP 'Nivel %d del aura %s cuando ...', POL '...
zapewnia %d poz. ... %s'. So Blizzard fixed the text, not the push order, and which patch
between 1.10 and 1.14d did it is not measured.

The census behind it covers every sprintf whose format the tables supply
(descfunc 11, 14, 15, 16, 19, 24 and MODS_FormatElementalDamageText's
fourteen fixed indices). eng: 62 strings, each
consuming exactly what the bytes push -- descfunc 15's literal 0 is its
"%%"'s argument (tools/d2client/propline/README.md, "The `%%` trap": BFE's
port crashed on it, its sprintf being D2MOO's, which takes nothing for
"%%"). CHI, ITA and JPN: no finding. KOR's 3620 leaves its second
argument unread, which changes nothing. ESP's three empty strings are the
next entry. DEU's and FRA's string.tbl are older than 1.10 -- 5322 records,
the keys at 3600..3630 sixty-nine below eng's -- so their
fixed-index findings are the files', not the game's.

Reproduced: the C pushes what the bytes push, to D2Lang's sprintf.

## A property line can come back empty and still be joined

`mods.cpp`, MODS_FormatPropertyLine (0x6FAF21C0) and
MODS_BuildStatsText (0x6FAF19C0). **Argued 2026-09-30** at mods.cpp's
close. **REACHABLE in the retail ESP client, a blank line; every other
route UNREACHABLE with stock data.**

The line starts empty (0x6FAF21E2), and a case that reaches the end answers
1 whatever it built, so the builder appends the empty line and its joiner:
a blank line when bAppendNewline is set, a stray ", " when it is clear. The
routes: ESP's descfunc-15 strings for kill, death and level-up (tbl 10994,
10995, 10996) are empty -- 1.10's Patch_D2.mpq (previous entry), and still
empty in 1.14d's -- and Executioner's Justice, Breath of the Dying,
Medusa's Gaze and Rainbow Facet carry those stats. The others cannot run on
stock data: no described stat has descval 0 under descfunc 1, 6, 12, 13, 25
or 26; nothing carries descfunc 22's stats (the
NULL-table arm); and the exits of 24, 27 and 28 test answers that cannot
come -- D2LANG_GetStringFromTblIndex never answers NULL (its lookup asserts
"pUnicode[lpDefine[usStr]] != NULL" and exits, 0x6FC12F26, and takes string
500 for an index past the table, 0x6FC12E94), a zero value is never passed
(the builder skips it), and the stock `oskill` parameters name skills.

Reproduced.

## MODS_AppendPropertySetText reads below wszOut when it is empty

`mods.cpp`, MODS_AppendPropertySetText (0x6FAF13C0). **Argued
2026-09-30** at mods.cpp's close. **UNREACHABLE; a DECLARED DIFFERENCE in
the C.**

`cmp word ptr [esi+eax*2-2], 0Ah` (0x6FAF1452) reads the word before wszOut
when its length is 0. It is never 0 there: each of the four calls, all in
MODS_BuildSocketFillerText, follows a newline appended to wszOut
(0x6FAF1831, 0x6FAF188B, 0x6FAF18DF, 0x6FAF1933), and the function only
appends. The C tests the length first, the defined program.

## The property text is appended with no bound

`mods.cpp`, all nine functions. **Argued 2026-09-30** at
mods.cpp's close. **One line: LATENT. The whole list: NOT ESTABLISHED.**

nMaxLen is read by none of them: the builders strcat into the callers'
buffers, and the lengths passed on bound nothing. One property line goes
into MODS_BuildStatsText's 128-Unicode local, through sprintfs bounded at
256 or 128 and strcats bounded at nothing. An upper bound on a line, every
number taken at eleven characters and every name at its pack's longest: ITA 121, KOR 119, JPN 112, FRA 107, DEU 104, POL
95, eng 89, ESP 85, CHI 84 -- under 127 by 6 at worst, and the CJK figures
are bytes of multibyte text, above the characters they become. In the
original an overrun would land in the undead line's local, dead by then.

The whole list is built into MODS_AppendMagicModsText's 1024 Unicodes, or
straight into the set appenders' caller's buffers. Settling it means
rendering every stock unique, set item with its set bonuses, runeword with
its runes and affix pair through the description columns; the one part
measured is the socket filler's, 241 of 256 (inv.cpp's entry above).

Reproduced.

## Tests in the property list that cannot fail

`mods.cpp`. **Argued 2026-09-30** at mods.cpp's close.
**UNREACHABLE** -- kept as the bytes make them, none a defect:

* MODS_BuildStatsText's three pUnit NULL tests (0x6FAF1C61, 0x6FAF1CAD,
  0x6FAF20A1): ITEMS_GetItemType has asserted the item and exited first
  (D2Common 0x6FD995D4), and for an item its items.txt record too
  (0x6FD9960D), so the durability tail's untested record is that one;
* the elixir half's two asserts of "0" (lines 321 and 317): the six stock
  records hold 1 and 2;
* MODS_FormatPropertyLine's NULL tests after D2LANG_GetStringFromTblIndex,
  in 15, 16, 24, 27 and 28: it never answers NULL (previous entries);
* 27's `nSkillClass >= 7`: SKILLS_IsPlayerClassSkill answers
  `*pPlayerClass != 7` (D2MOO D2Skills.cpp, #11010).

## Four description paths no stock stat takes

`mods.cpp`, MODS_FormatPropertyLine (0x6FAF21C0). **Argued
2026-09-30** at mods.cpp's close. **UNREACHABLE with stock data; a mod
reaches each**:

* descfunc 22 bounds nLayer by monprop.txt's record count (+0C34h,
  0x6FAF2B96) and indexes montype.txt's table (+0C3Ch). monprop has 13
  records and montype 58, so no read leaves the table, and a montype from 13
  up would print montype 0's plural -- but no stock row carries
  attack_vs_montype or damage_vs_montype;
* descfunc 25 and 26 negate the value after it is rendered (`neg ebp`,
  0x6FAF2DE7), so -5 prints "+-5": no stat has either;
* 17 and 18 print no number for a negative value of a non-negative stat
  (0x6FAF2A6D..0x6FAF2A9B): the one stock carrier, `ac/time`, is on the "of
  Dawn" and "of Sunlight" suffixes, and neither is spawnable;
* the group scan compares every member at parameter 0 with the stat's own
  value at nLayer: the grouped stats -- the four attributes (dgrp 1) and the
  four resistances (dgrp 2) -- take no parameter.

Reproduced.

## Three untested pointers the callee tests

`mods.cpp`. **Argued 2026-09-30** at mods.cpp's close.
**REFUTED** -- each callee answers for NULL, measured in D2Common's bytes:

* MODS_AppendSetBonusText hands pPlayer to ITEMS_GetSetItemsMask untested
  (0x6FAF32FE, 0x6FAF3351): a NULL player answers 0 (0x6FD9FB57 ->
  0x6FD9FD74), no bit, nothing rendered;
* MODS_AppendStateModsText hands pItem to ITEMS_GetItemQuality untested: a
  NULL item answers 2, not a set item (0x6FD98554 -> 0x6FD98569);
* MODS_FormatPropertyLine's op scale hands CUNIT_GetPlayer's answer to
  STATLIST_UnitGetStatValue untested: a NULL unit answers 0 (0x6FDB7C35 ->
  0x6FDB7C3A). D2MOO's copy dereferences it (D2StatList.cpp:1362); the
  bytes test it.

## SKILLDESC_FormatChargesText widens with a count twice its buffer

`SkillDesc.cpp`, SKILLDESC_FormatChargesText (0x6FB13E70), written
2026-09-30. **LATENT.**

`Unicode::toUnicode(wszNumber, szNumber, 32)` (`push 20h`, 0x6FB13EE6) into
a local constructed as 16 Unicodes (`mov ebx, 10h`, 0x6FB13EB7). What it
converts is wsprintfA's "%d" of nCharges: eleven characters at most, and the
terminator, twelve of the sixteen. The count would matter only for a longer
source, which "%d" cannot make.

Reproduced.

## An object's start frame stored raw, and overwritten before anything reads it

`Object.cpp`: OBJECT_FinishDoorAnim (0x6FB47030),
OBJECT_SetClientMode (0x6FB478D0), OBJECT_ProcessSkeletonSpawn (0x6FB47FA0),
OBJECT_ProcessIceBubbles (0x6FB48050), and the drinker's, gesturer's and
turner's stores on the way back to NEUTRAL. **Argued 2026-10-01** at
Object.cpp's close. **INERT** -- nine dead stores.

Each puts objects.txt's nStart byte into nSeqCurrentFramePrecise
zero-extended -- the counter's FRACTION byte, where OBJECT_AdvanceAnim shifts
it into the frame byte (`mov ch`/`mov dh`). The store instructions:
0x6FB4704F (FinishDoorAnim, both arms, the OPERATING one jumping back from
0x6FB47082), 0x6FB47935, 0x6FB47A22 and 0x6FB47B2E (SetClientMode),
0x6FB47FDA and 0x6FB4808A (the skeleton spawn, the bubbles), 0x6FB47DE3,
0x6FB47EC1 and 0x6FB47F76 (the drinker, the gesturer, the turner); a scratch
scan of the seven bodies for `mov [reg+44h], reg` found these and three more
(the next entry). D2Common's UNITS_SetAnimStartFrame then writes the counter
again, as nStart[mode] << 8 on every path for an object (`xor ecx, ecx` /
`mov ch, [eax+edi+129h]` / `mov [esi+44h], ecx`, D2Common
0x6FDBE87C..0x6FDBE890; its D2COMMON_11013_ConvertMode returns at once for a
unit that is not shapeshifted, D2MOO D2Skills.cpp:2853):

* FinishDoorAnim calls it right after GFX_RebuildUnitAnimForMode
  (0x6FB4705C);
* the other eight stores come before UNITS_ChangeAnimMode into a different
  mode -- OPENED to SPECIAL1, NEUTRAL to OPERATING, SPECIAL1 or above to
  NEUTRAL --
  which calls it on a change (D2Common 0x6FDBEA78..0x6FDBEA9B); except
  SetClientMode's last two, which OBJECT_DoClientAction follows with its own
  call after the return (0x6FB47826), UNITSND_UpdateUnitModeSound between.

Nothing between reads the counter. A scratch walk of GFX_RebuildUnitAnimForMode's
and UNITSND_UpdateUnitModeSound's direct calls, ten deep into D2Client and
D2Common, met `+44h` only on stack slots, an item's data
(ITEMS_GetBodyLocation) and the CRT heap; UNITS_ChangeAnimMode's
UNITROOM_RefreshUnit and STATLIST_DeactivateTemporaryStates met it in seven
statlist functions, where D2MOO has D2StatListExStrc's pOwner
(D2StatList.h:489), and the CRT heap. Indirect calls, and the Fog, Storm,
D2Sound and D2Win imports, were not walked. So a door starts each mode at
that mode's nStart after all, and the stock rows show what that decides: of
the 25 doors (a scratch read of objects.txt) none has a Start0, one a Start2
-- 75, Door by Dock, Act 2, 1 -- and 17 a Start3 (1, 3 or 8), the frame a
closing door steps back from. Had SetClientMode's first store, nStart[0] --
0 on every door -- survived, each closing would sit at frame 0, where
OBJECT_AdvanceDoorAnim ends it at once (`nFrame <= 0`).

Reproduced.

## Three timed objects keep a raw start frame when their timer fires in the mode it asks for

`Object.cpp`: OBJECT_ProcessDrinker (0x6FB47D50),
OBJECT_ProcessGesturer (0x6FB47E00), OBJECT_ProcessTurner (0x6FB47EE0).
**Argued 2026-10-01** at Object.cpp's close. **LATENT** -- the data keeps it
from showing.

Each stores nStart[5] raw (0x6FB47DA7, 0x6FB47E5C, 0x6FB47F37) and then asks
UNITS_ChangeAnimMode for mode 3 (the gesturer 3 or 4 on a coin). When the
unit is already in that mode, UNITS_ChangeAnimMode changes nothing
(D2Common 0x6FDBEA7A, `jz`) and the raw byte stays in the counter's fraction.
A timer of 7000..11999, 23000..32999 and 20000..29999 ms against an animation
of a few seconds makes that rare, and the three stock rows -- 110 drinker,
112 gesturer, 114 turner, each named "not used" -- have every Start column 0,
so the byte and the shifted value are the same 0. A mod with a nonzero
Start5 on such a row would see a mode restart one 256th of a frame in.

Reproduced.

## A special chest keeps its gleam once opened

`Object.cpp`: OBJECT_AdvanceAnim (0x6FB46D50),
OBJECT_DoClientAction (0x6FB47660), OBJECT_AddIdleOverlay (0x6FB46BA0).
**Argued 2026-10-01** at Object.cpp's close. **REACHABLE.**

OBJECT_AddIdleOverlay gives the MULTIGLEAM overlay to eight chests while they
are NEUTRAL; OBJECT_AdvanceAnim takes it off seven when OPERATING ends
(0x6FB46E4F..0x6FB46E79), and OBJECT_DoClientAction puts it back on the same
seven -- 14 of 15 instructions equal. The eighth,
OBJECT_CHEST_SPECIALCHEST (455; stock row `chest`, "specialchest", Overlay
1, CycleAnim1 0, FrameCnt1 6, not a door), is in neither list.

**Why reachable:** eleven act 5 DS1 files place it -- a scratch parse of all
2276 stock DS1 files through D2MOO's preset table (D2Common
DrlgPreset.cpp:65, act 5 slot 59), among them iceEtheme, iceNtheme,
tempNEdown, NorthHouse8 and WestHouse4a, which LvlPrest.txt names -- and
D2Game makes a preset object id below 573 as itself (D2MOO SUnit.cpp:1024).
Opened in view, its OPERATING runs to the end in OBJECT_AdvanceAnim, which
does not take the gleam off. Whether anything outside Object.cpp removes the
overlay -- GFX_RebuildUnitAnimForMode, the overlay code -- was not read, and
it was not watched in a running game.

Reproduced.

## The Shrine of Enirhs copies the name onto itself

`Object.cpp`, OBJECT_ShrineOfEnirhsEffect (0x6FB470D0). **Argued
2026-10-01** at Object.cpp's close. **UNREACHABLE** from a stock server; a
DECLARED difference in ours.

`strncpy(szName, _strrev(szName), 16)` (0x6FB470DD..0x6FB470EA): _strrev
returns its argument, so the copy overlaps itself, which C leaves undefined.
The image's own strncpy (0x6FB60A50) leaves the reversed name and zeroes the
bytes after its terminator up to 16; ours writes that out instead.

**Why unreachable:** the effect is chart row 16, and D2MOO's D2Game never
gives a shrine code 16. OBJECTS_InitFunction01_Shrine turns 16 into 18
before storing it (Objects.cpp:537..544), OBJECTS_SpawnShrine's six ranges
end at 14 (Objects.cpp:71..97), and the byte's other writers in D2Game's
OBJECTS directory serve other objects (the well's regenerate counter,
ObjMode.cpp:273, queued from :3800; a town portal's level, :3696) -- not
each traced. shrines.txt marks the row "(OUT)".

## OBJECT_ApplyShrineOverlays tests the shrine for NULL after using it

`Object.cpp`, OBJECT_ApplyShrineOverlays (0x6FB47520). **Argued
2026-10-01** at Object.cpp's close. **UNREACHABLE.**

`!pObject || pObject->dwAnimMode == 0` guards each GFX_AddOverlay, after
pObject has gone to GFX_RemoveOverlay; both callees test the unit
themselves (0x6FB5BEC4, 0x6FB5BAD3). The test cannot fire: the function is
reached through the shrine chart's pfnApplyOverlays alone, and both of the
chart's callers have read the unit's object data first -- OBJECT_InitShrine
(0x6FB475D9) and OBJECT_DoClientAction's action-3 arm.

Reproduced.

## A shrine's action 21 reads its record untested

`Object.cpp`, OBJECT_DoClientAction (0x6FB47660). **Argued
2026-10-01** at Object.cpp's close. **LATENT.**

For a shrine the action-21 arm takes `UNITS_GetShrineTxtRecordFromObject`'s
answer and reads its code at once (`mov al, [ebp+0]`, 0x6FB476A8), where the
action-3 arm asserts it (the original's Object.cpp:886). The record is there: OBJECT_InitShrine
sets it, reads it back and asserts it, and CUNIT_CreateObjectUnit runs it
for every object UNITS_IsShrine accepts (CUnit.cpp); OBJECT_InitShrine is
the one caller of the setter in D2Client, and OBJECT_InitClientUnit zeroes
the object data it lives in.

Reproduced.

## A non-shrine's action 21 would end at OBJECT_PlayShrineSound's assert

`Object.cpp`, OBJECT_DoClientAction (0x6FB47660) and
OBJECT_PlayShrineSound (0x6FB47B70). **Argued 2026-10-01** at Object.cpp's
close. **UNREACHABLE** from a stock server.

For an object UNITS_IsShrine refuses, the arm takes the code from
objects.txt's ShrineFunction -- 0 on every stock row, so no effect runs --
and then calls OBJECT_PlayShrineSound, whose first assert, `ptShrineData`
(the original's Object.cpp:841), stops the client: such an object has no shrine record.

**Why unreachable:** D2MOO's D2Game sends an object action 21 only as packet
0x4D from the object's state refresh, and only when its SubClass has bit 0
(ObjMode.cpp:640..653) -- the bit UNITS_IsShrine tests, `test [ecx+167h],
al` with al 1 (D2Common 0x6FDBFCA3). Its one object sender of the action
packets 0x0C..0x10 is 0x0E with action 3 (ObjMode.cpp:631), and 0x9A goes
to SKILLS_ClientCastAtPosition, not to this function (SCmd.cpp).

Reproduced.

## OBJECT_ProcessClientSmoke hands the player to D2Common untested

`Object.cpp`, OBJECT_ProcessClientSmoke (0x6FB48110). **Argued
2026-10-01** at Object.cpp's close. **LATENT.**

`UNITS_GetDistanceToOtherUnit(pUnit, GetPlayer())` with no test of the
player. D2MOO goes through UNITS_GetCoords, which asserts; D2Common's bytes
inline the reads and start with the second unit's type, `mov ebp,
[esp+8+pUnit2]` / `mov eax, [ebp+0]` (0x6FDC2E42, 0x6FDC2E48), so a NULL
player would fault there.

The object is the client smoke, objects.txt 478, which only
MONSTER_UpdateAnimMode makes -- when an evil hut (monstats 528) is in mode 0
with dwFlags bit 0 set, CUNIT_CreateClientOnlyUnit(478, ...), `mov ecx,
1DEh` at 0x6FB423D3, the one `1DEh` immediate in the module (a scratch
scan, control 0x13 at 0x6FB48442). The body runs only from
CUNIT_UpdateAllUnits, inside the tick
GAME_UpdateFrame runs while gbInGame is set;
SCMD_HandleSrv04_LoadComplete, the one write site that sets it, halts
unless the player's room is found (the original's SCmd.cpp:1319), and the seven others clear it.
Whether the server can
remove the player while gbInGame is set was not looked for.

Reproduced.

## A critter's player walk tests a monster's dead mode on a player

`0x6FB3F394`, `0x6FB3F398`, `Monster.cpp`, MONSTER_CritterTickAmbient
(0x6FB3F350). **Argued 2026-10-02** at Monster.cpp's close. **REACHABLE.**

The walk over gapClientPlayers skips a player whose mode is 0 or 12 (`test
ecx, ecx`, `cmp ecx, 0Ch`). For a player 0 is PLRMODE_DEATH, but 12 is
PLRMODE_KICK: a dead player stands in PLRMODE_DEAD, 17, and 12 is
MONMODE_DEAD, a monster's. MONSTER_CritterPickRandomCorpseCallback, written beside it,
tells the two apart by the unit's type (`cmp ecx, 11h`, 0x6FB40312; `cmp ecx,
0Ch`, 0x6FB40322).

**Why REACHABLE**: a player killed near a critter lies in mode 17 and stays
the nearest player it measures, and a player in the kick animation is passed
over for that tick. Stock Skills.txt gives the kick animation (`anim` KK) to
Kick (1), Dragon Talon (255) and Dragon Tail (270), and levels.txt's
cmon1..cmon4 name a critter in 183 cells -- rats, bats, scorpions, snakes,
parrots, bunnies, spiders, chickens, bugs, larvae (both read by a scratch
script). Only the critter's choice of player changes.

Reproduced.

## The threat search tests the critter for death, not the candidate

`0x6FB3F7FA`, `Monster.cpp`, MONSTER_CritterFindNearestThreatCallback
(0x6FB3F7D0). **Argued 2026-10-02** at Monster.cpp's close. **NOT
ESTABLISHED.**

CUNIT_IsUnitDead is handed the critter (`mov ecx, edi`), not the candidate.
The one caller, MONSTER_CritterTickAmbient's chicken and bug arm, runs the search for
a critter in MONMODE_NEUTRAL, so the test refuses only a critter flagged
UNITFLAG_ISDEAD, and a dead player or monster counts as a threat: the critter
flees from a corpse.

**What would settle it**: whether a dead unit lies in the rooms adjacent to a
chicken's or a bug's. Stock levels.txt names the chicken only in Act 1 - Town
and the bug only in Act 2 - Town (cmon1..cmon4, a scratch read); the other
chickens are QUEST_SpawnZooChickens', made by objects.txt 567. Where corpses
lie near a town's critters was not measured.

Reproduced.

## The local player handed on untested at four sites

`Monster.cpp`. **Argued 2026-10-02** at Monster.cpp's close.
**LATENT.**

    MONSTER_LinkOwnedUnits   the bone wall's arm: `call CUNIT_GetPlayer` /
                             `mov ecx, eax` / `mov eax, [ecx+20h]`
                             (0x6FB41341..0x6FB41348), where act2guard2's arm
                             tests it (`test eax, eax`, 0x6FB41307)
    MONSTER_InitHireling     into D2Common's MONSTERS_HirelingInit (#11082),
                             which reads the player's level
                             (0x6FB4470E..0x6FB4471E)
    MONSTER_CritterTickBird         into UNITS_GetDistanceToOtherUnit (#10397) as its
    MONSTER_CritterPickRandomCorpseCallback   second unit (0x6FB4033A for the
                             callback), which D2Common reads at once, `mov
                             ebp, [esp+10h]` / `mov eax, [ebp]` (0x6FDC2E42,
                             0x6FDC2E48) -- a fault, not D2MOO's assert;
                             the bird's position copies then read it too

**Why LATENT**: the two critter sites run only from MONSTER_CritterTickAmbient, from
CUNIT_UpdateAllUnits, inside the gbInGame tick, the chain OBJECT_ProcessClientSmoke's entry traces: the
one write that sets gbInGame halts unless the player's room is found. The bone
wall's arm runs at a monster's construction, from CUNIT_CreateMonsterUnit --
a monster packet, which belongs to a loaded game as packets 0x53's and 0x7E's
entry argues, or a client-only unit made in one; D2MOO's D2Game sends a
monster to a client from the rooms around that client's player, reading the
player first (D2Game/src/UNIT/SUnitMsg.cpp:48; GAME/Level.cpp:63, :167).
InitHireling runs for a pet-roster record of type 7, the local player's own
hireling, which a server packet makes (CUnit.cpp's CUNIT_CreateMonsterUnit,
RosterPets.cpp). Whether a monster packet can precede the client's own player
was not traced.

Reproduced.

## NULL tests that cannot fail, and the uses past them -- six places

`Monster.cpp`. **Argued 2026-10-02** at Monster.cpp's close.
**UNREACHABLE.**

    MONSTER_ChangeMonsterClass   a unit with no dynamic path leaves torn down,
                                 its new class stored (0x6FB419F6)
    MONSTER_CritterFleeFromTarget,      the inlined position accessors test the
    MONSTER_CritterWanderRandom         critter's path, and D2Common's PATH_GetType
                                 and PATH_GetMaxDistance then take it untested;
                                 MONSTER_CritterMoveToUnit takes it with no test
    MONSTER_DoClientAction       the death copy tested (0x6FB43799) and used
                                 untested (0x6FB437AC, 0x6FB437B4)
    MONSTER_DoClientAction       the shatter's monstats2 record read untested
                                 (`mov dl, [eax+0Bh]`, 0x6FB4365E)
    MONSTER_SetVultureFlightState  the unit tested (0x6FB4446D, 0x6FB445BC)
                                 and read past it (0x6FB4447F, 0x6FB445CE)
    MONSTER_CritterTickMaggot           the maggot tested for the mode alone
                                 (0x6FB3FE2D) and read untested (0x6FB3FE50)

**Why UNREACHABLE.** Every client monster gets its dynamic path in
MONSTER_InitClientUnit -- PATH_AllocDynamicPath stores it in the unit at once
(D2MOO D2Common/src/Path/Path.cpp:700..702) -- and loses it only in
MONSTER_FreeUnitData. InitClientUnit returns before the allocation only for a
class whose monstats or monstats2 lookup refuses, and its one caller,
CUNIT_CreateMonsterUnit, refuses that class before the unit exists
(CUnit.cpp's `nMonClassId` and `nMonStatsEx` bounds); every client monster,
the server's and the client-only ones, is made there
(CUNIT_CreateClientOnlyUnit's monster arm). So both path tests pass.
CUNIT_CreateMonsterUnit answers NULL only through those refusals, and the
death copy's class is pUnit's own, which DoClientAction's prologue looked up
and kept (0x6FB42E37, 0x6FB42E84); the shatter looks the same class up again.
D2Client writes a unit's class in CUNIT_InitUnit, CUNIT_CreateMonsterUnit
and MONSTER_ChangeMonsterClass alone (a grep of the sources), and the calls
between the two lookups -- the selection's, the click flags', four D2Common
calls -- are none of them; D2MOO's D2Common writes the class of items only (a
grep). Whether one of the stat-list remove callbacks D2Client hands D2Common
can fire there was not looked for. Both callers of the vulture's function pass
DoClientAction's unit, which that function reads at its fourth instruction
(`mov eax, [esi+10h]`, 0x6FB42D4E); the maggot's one caller,
MONSTER_CritterTickAmbient, has read the unit for its timer and its class switch.

Reproduced.

## MONSTER_DoClientAction's DEATH reads the parameter block untested

`0x6FB4357A`, `Monster.cpp`, MONSTER_DoClientAction (0x6FB42D40).
**Argued 2026-10-02** at Monster.cpp's close. **LATENT.**

DEATH stores `pParams[6]` as the last hit class with no test of the block,
where every other arm that reads it asserts or tests it.

**Why LATENT**: every caller that can hand DEATH passes a block. The packet
handlers reach it through UNITMODE_DispatchActionByUnitType with a stack
`int[7]` of their own (SCmd.cpp), and the critter ticks with theirs; the two
calls that pass NULL -- ITEM_RefreshActiveItems' (Item.cpp) and
Skills.cpp's dispatch -- pass CLTACTION_NEUTRAL. Every direct caller of
DoClientAction and of the dispatcher is ours, so the C's calls are the whole set.

Reproduced.

## MONSTER_AdvanceAnimForMode's case 3 is dead

`0x6FB41E07`, `Monster.cpp`, MONSTER_AdvanceAnimForMode
(0x6FB41D70). **Argued 2026-10-02** at Monster.cpp's close. **UNREACHABLE.**

The switch on the descriptor's nAnimEnd takes 0..3 (`cmp eax, 3 / ja`), and
case 3 rewinds to frame 1 and freezes. The descriptor comes from
gMonModeDescs or gMonModeDescsMoving, sixteen records each, which only
MONSTER_UpdateAnimMode references. Read off the image (a scratch read, both
extents inside .data's raw data, which ends at RVA 0xFB000), the 32 records
hold nAnimEnd 0, 1 and 2 only.

Reproduced.

## The knockback descriptor's trigger cannot fire

`Monster.cpp`, MONSTER_UpdateAnimMode (0x6FB41E90) and
MONSTER_DoClientSplEndEffect (0x6FB42750). **Argued 2026-10-02** at
Monster.cpp's close. **UNREACHABLE.**

gMonModeDescs[MONMODE_KNOCKBACK] is {1, 2, 2} (read off the image). Trigger 2
fires the special end on MONSTER_AdvanceAnimForMode's answer, and nAnimEnd 2
always makes it 0: edi is zeroed at 0x6FB41E02 and written in case 1 alone.
gMonModeDescsMoving[13], {0, 1, 2}, could fire it, but a mode takes that table
only when monstats2's moving bits name it, and D2Common fills them from seven
columns -- A1mv, A2mv, SCmv, S1mv..S4mv, bits 4, 5 and 7..11 (D2MOO
D2Common/src/DataTbls/MonsterTbls.cpp:2142..2148) -- none of them 13; the
fixed borrowings are SKILL3's and SKILL4's. UpdateAnimMode is
DoClientSplEndEffect's one caller (0x6FB42390).

So DoClientSplEndEffect's KNOCKBACK block -- the room recached, #10208's
flag cleared, the path type put back and MONMODE_GETHIT set by a tail jump
(0x6FB427F6) -- never runs in a stock client. A knocked-back monster freezes
on its last frame (case 2) until the server's next action, and
MONSTER_DoClientAction's prologue then recaches the room and puts the path
type back for a unit still in KNOCKBACK.

Reproduced.

## REFUTED: MONSTER_ChangeMonsterClass's hit points of 128

`0x6FB41AEB`, `0x6FB41AFA`, `Monster.cpp`. **Argued 2026-10-02** at
Monster.cpp's close. **REFUTED.**

ChangeMonsterClass sets MAXHP and HITPOINTS to 128 << 8 (`push 8000h`) where
MONSTER_InitClientUnit sets 100 << 8. **128 is the client's scale**: packet
0xAC sets MAXHP 0x8000 and HITPOINTS `nLifePct << 8` right after
CUNIT_CreateMonsterUnit, and 0xAB sets HITPOINTS `nLife << 8` (SCmd.cpp).
InitClientUnit's 100 is overwritten by 0xAC for every server monster but the
local player's own hireling, and both functions store HITPOINTS equal to
MAXHP, so either unit starts full.

Reproduced as written.

## MONSTER_SpawnBlood faults on a NULL unit

`0x6FB42A83`, `Monster.cpp`, MONSTER_SpawnBlood (0x6FB42A70).
**Argued 2026-10-02** at Monster.cpp's close. **LATENT.**

With bByChance the unit's seed is read first (0x6FB42A83); without, D2Common
#10520 reads `pUnit->pStatListEx` untested (D2Common 0x6FDB7E37; D2MOO
D2StatList.cpp:1362). The function's own tests of pUnit (0x6FB42AAD,
0x6FB42AFF) come after both.

**Why LATENT**: each of the seven call sites passes a unit it has tested or
read. MONSTER_DoClientAction's two pass its pUnit, read at its fourth
instruction (0x6FB42D4E); SKILLS_CltDoFunc38 and 40 (SkillsBar.cpp) and 93
(SkillsEMon.cpp) their path target after `if (!pTarget) return 0`;
STATES_SetFunc18_CorpseExp the unit PLRSKILLS_StatesRunStateSetFunc has tested
(`if (!pUnit) return`, PlrSkills.cpp); SKILLS_CltDoFunc85, which only States.txt
openwounds' cltactivefunc names (a scratch read of both tables' function
columns), the unit SKILLS_StatesRunActiveStateFuncs found the state on --
D2Common's STATES_CheckState answers FALSE for NULL (D2MOO D2States.cpp:34).

Reproduced.

## MONSTER_SpawnBlood's walk tests seven directions of eight, and a full ring of walls drops every splash left

`0x6FB42C94`, `0x6FB42CA3`, `Monster.cpp`, MONSTER_SpawnBlood
(0x6FB42A70). **Argued 2026-10-02** at Monster.cpp's close. **NOT
ESTABLISHED.**

The walk over the eight ring cells stops on coming round to the direction
BEFORE the first (0x6FB42C94), so it tests seven; when all seven are walls
the function returns (0x6FB42CA3) and the splashes still to come are not
thrown. The test of the first direction against that one (0x6FB42C07) cannot
succeed.

**What would settle it**: whether a bleeding unit can stand where seven of
the cells (0,2) (-1,1) (-2,0) (-1,-1) (0,-2) (1,-1) (2,0) (1,1) around it
carry COLLIDE_WALL. The collision maps were not scanned.

Reproduced.

## The quill rat's extra volley never fires in a stock game

`0x6FB426FE`, `Monster.cpp`, MONSTER_SpawnAttackMissile
(0x6FB42420). **Argued 2026-10-02** at Monster.cpp's close. **UNREACHABLE**
with the stock tables.

A monster whose base id is quillrat1 fires monstats `wAiParam[2][difficulty]`
more missiles, and wAiParam[2] is aip3 (D2MOO's loader). All eight classes
with that BaseId -- quillrat1..8, hcIdx 63..67 and 605..607 -- leave aip3,
aip3(N) and aip3(H) empty (MonStats.txt, a scratch read), so the count fails
its `jle`. D2MOO's server twin fires the same volley only when its first
missile was made (D2Game/src/MONSTER/MonsterMode.cpp:1346), where the client
fires it after any arm: a difference no stock data shows.

Reproduced.

## A monster's inventory is given by INTERACT on the client and by INVENTORY on the server

`Monster.cpp`, MONSTER_InitClientUnit (0x6FB40D60). **Argued
2026-10-02** at Monster.cpp's close. **NOT ESTABLISHED** -- a divergence from
the server.

The client allocates an inventory for a class flagged INTERACT and for the
iron golem, the valkyrie and the two shadows; D2MOO's MONSTER_
InitializeStatsAndSkills, for a class flagged INVENTORY
(D2Game/src/MONSTER/Monster.cpp:238). In stock MonStats.txt (a scratch read)
30 rows carry `inventory` and 47 `interact`: bloodraven and flyingscimitar
have an inventory on the server and none on the client (the other four
inventory-only rows are the client's four classes), and 23 interact rows --
Cain, Kashya, the act 2 guards, the ancient statues, Tyrael and others -- have
one on the client only.

**What would settle it**: whether D2Game ever sends an item into Blood
Raven's or a flying scimitar's inventory. Not looked for.

Reproduced.

## A monster's skill mode is set even where the server refuses one

`Monster.cpp`, MONSTER_InitClientUnit. **Argued 2026-10-02** at
Monster.cpp's close. **INERT** -- a divergence from the server.

The client hands monstats nSkillMode to SKILLS_SetSkillMode for every skill;
the server only when it is not negative (D2Game/src/MONSTER/Monster.cpp:257).
D2Common's SKILLS_SetSkillMode ignores a NULL skill and a mode outside 0..32
(D2MOO D2Common/src/D2Skills.cpp:2105), so the two sides end equal.

Reproduced.

## A vile child faces 0 where the client's table and the server's disagree

`Monster.cpp`, MONSTER_MatchVileMotherDirection (0x6FB41690).
**Argued 2026-10-02** at Monster.cpp's close. **REACHABLE** -- a divergence
from the server.

The client takes a vile child to be its mother's when it stands three steps
from her along HER facing, turned through {6, 7, 0, 1, 2, 3, 4, 5}; D2MOO's
AITHINK_Fn068_VileMother spits it three steps along the direction to her
TARGET, through {6, 4, 2, 2, 2, 0, 6, 6}, trying up to eight entries
(D2Game/src/AI/AiThink.cpp:5925, :5935). Where the two disagree the match fails
and the child faces 0.

**Why REACHABLE**: stock levels.txt places vilemother1..5 in Act 4 - Mesa 1,
2 and 3, Act 4 - Lava 1, Act 5 - Temple 2 and Act 5 - Baal Temple 1 (a
scratch read). Only the child's first facing differs.

Reproduced.

## A vulture lands without asking and marks its cell NO_PATH as well

`Monster.cpp`, MONSTER_SetVultureFlightState (0x6FB44460).
**Argued 2026-10-02** at Monster.cpp's close. **REACHABLE** -- a divergence
from the server.

Landing, the client sets COLLIDE_NO_PATH and COLLIDE_MONSTER on the cell
under the bird with D2Common's COLLISION_SetMask; D2MOO's landing,
sub_6FCD55D0 (D2Game/src/AI/AiThink.cpp:2126), asks sub_6FCBDFE0 first and
marks COLLIDE_MONSTER alone with COLLISION_SetMaskWithPattern. The client
lands on the server's action code, so the question was asked there; the
NO_PATH bit is the client's own.

**Why REACHABLE**: stock levels.txt places vulture1..5 in Act 2 - Desert 1, 2
and 3, Act 3 - Kurast 1 and 3, Act 3 - Jungle 3, Act 5 - Barricade Snow and
Act 5 - Siege 1 (a scratch read). How the extra bit changes the client's
pathing was not looked for.

Reproduced.

## A hireling's skill levels are capped where the server also clamps and skips

`Monster.cpp`, MONSTER_InitHireling (0x6FB446B0). **Argued
2026-10-02** at Monster.cpp's close. **NOT ESTABLISHED** -- a divergence from
the server.

The client gives each skill its txt level plus `(nLevel - record level) *
LvlPerLvl >> 5` (`sar eax, 5`, 0x6FB44792), capped at 32. D2MOO's
MONSTERAI_UpdateMercStatsAndSkills clamps the same formula to 0..32, ends the
walk at a mode of 16 or more, skips a skill above the hireling's level and
assigns no level 0 (D2Game/src/MONSTER/MonsterAI.cpp:274). Stock hireling.txt
has no Mode of 16 or more and no skill Level below 1 (a scratch read of its
120 rows), so with a level at or above its record's the two agree.

**What would settle it**: D2Common takes an id's FIRST record even when its
Level is above the level asked for (D2MOO DataTbls/MonsterTbls.cpp:1704..1710),
and the first records begin at Level 3; the client's level comes through
MONSTERS_HirelingInit from the local player. Whether a player below level 3
gets a hireling whose skill comes out at 0 or below on the client was not
traced.

Reproduced.

## MONSTER_GetBossHcIdx answers 0 where the server answers -1, on an arm no caller reaches

`0x6FB449C0`, `Monster.cpp`, MONSTER_GetBossHcIdx (0x6FB449B0).
**Argued 2026-10-02** at Monster.cpp's close. **UNREACHABLE** -- a divergence
from the server.

For a NULL unit, a unit that is not a monster or one with no monster data the
client answers 0 (`xor ax, ax`) -- superuniques.txt's first row as an index
-- and D2Game's MONSTERUNIQUE_GetBossHcIdx -1 (`or ax, 0FFFFh`, D2Game
0x6FC6ACE0). Both callers ask MONSTER_TestTypeFlag's superunique bit
first, which answers FALSE for exactly those units: CUNIT_CreateMonsterUnit
(CUnit.cpp) directly, UNITSND_GetMonSoundsRecordForUnit through
MONSTER_IsSuperUnique (`call` 0x6FB4FED0, `je` 0x6FB4FED7).

Reproduced.

## A boss's modifier id indexes three tables with no bound

`MonUnique.cpp`: MONUNIQUE_ApplyModifiers (`mov dl, [esi+ebp]`,
0x6FB451A4), MONUNIQUE_MonUModRunCltFuncsForUnit (`mov dl, [esi+ebx]`, 0x6FB46216,
0x6FB46245), MONUNIQUE_MonUModBuildMonsterModifierLabel (`mov al, [ecx+edx]`,
0x6FB463D2). **Argued 2026-10-02** at MonUnique.cpp's close. **LATENT.**

Each byte of a monster's nine-byte id array indexes gpMonUModHandlers (43
slots), gpMonUModCltFuncs (43 rows of five) or gMonUModLabelStrings (44
words, the 44th 0) with nothing between, so an id of 43 or more reads the
original's next data in the first two -- as a function pointer -- and one of
44 or more in the third. The ids reach the array
from two packets alone (a grep of the sources for its writers): 0xAC, eight
bits each until a 0 (SCMD_HandleSrvAC, SCmd.cpp, into CUNIT_CreateMonsterUnit's
copy), and 0x57, three bytes of its fields (SCMD_HandleSrv57). The server
sends the ids it assigned, and in D2MOO's reading every assignment goes
through D2GAME_BOSSES_AssignUMod_6FC6FF10 (MonsterUnique.cpp:2916) or
sub_6FC6FFD0 with ids from monumod.txt's picks, superuniques.txt's Mod
columns, skills.txt's sumumod and constants -- all under 43 with the stock
tables (monumod.txt has 43 rows).

**Why LATENT**: a stock server sends no such id; a modified or malformed one
can, and nothing on the client stops it. Ours reads the original's memory
past our copies -- the slot the original reads, by a defined route.

Reproduced.

**DECLARED DIFFERENCE, 2026-10-12.** A modifier id past the tables finds no handler, no hook and no
label in our C (MonUnique.cpp: `nUMod < 43 ? ... : NULL`, `if (nUMod >= 44)
continue;`), where the bytes read the original's .data and call what they
find. LATENT, as above.

## The client's modifier hooks would run on an id array whose first id is 0

`0x6FB461C0`, `MonUnique.cpp`, MONUNIQUE_MonUModRunCltFuncsForUnit.
**Argued 2026-10-02** at MonUnique.cpp's close. **UNREACHABLE** -- a
divergence from the server.

The dispatcher walks all nine ids whatever the first holds, where
MONUNIQUE_ApplyModifiers returns on a first 0 (`cmp byte ptr [ebp+0], 0`,
0x6FB4515D) and so does D2Game's dispatcher (`cmp byte ptr [ebp], 0`, D2Game
0x6FC6E7D5). **Why UNREACHABLE**: neither packet makes such an array. 0xAC's
reader stops at its first 0, into a zeroed buffer (SCmd.cpp); 0x57 carries
the server's first three ids, and the server stores an id into its array's
first empty slot (D2MOO MonsterUnique.cpp:2937..2939, 2971..2973), so its
array has no 0 before an id. Whether the server ever clears an id was not
looked for.

Reproduced.

## Poison-hit's client hook draws the lightning ring, and a stock boss carries poison-hit

`0x6FB45910`, `MonUnique.cpp`, MONUNIQUE_MonUModCltGetHitOverlay --
gpMonUModCltFuncs row 23 column 0. **Argued 2026-10-02** at MonUnique.cpp's
close. **REACHABLE** -- a divergence from the server.

The client's slot for row 23 column 0 holds the hook of row 17 column 2: in
the get-hit mode on frame 2, MONUNIQUE_MonUModSpawnAuraMissileRing of lightunique
(195), which is drawn only for a monster carrying type bit 0x100 -- the bit
the server's lightning hook sets when it fires (D2Game 0x6FC6CB70..
0x6FC6CBCA). D2Game's row 23 column 0 is
MONSTERUNIQUE_CastQueenPoisonCloudMissile (0x6FC6D8B0), a ring of
queenpoisoncloud (321). **Why REACHABLE**: monumod.txt gives poisonhit no
pick weight, but SuperUniques.txt's Baal Subject 2 carries it as Mod1, with
no Mod2 or Mod3 -- so the client runs this hook for a stock boss and, the bit
never set, draws nothing where the server casts its cloud. Whether column 0
runs while the boss is in the get-hit mode (MONSTER_DoClientAction,
Monster.cpp:2964) was not traced.

Reproduced.

## MONUNIQUE_MonUModSpawnAuraMissileRing's queenpoisoncloud arm is reached by no caller

`0x6FB45960`, `MonUnique.cpp`. **Argued 2026-10-02** at
MonUnique.cpp's close. **UNREACHABLE.**

The ring installs SKILLS_MissileStartSeededPath as the init function for every
missile but queenpoisoncloud (321). Its three callers pass lightunique (`mov
edx, 0C3h`, 0x6FB4594A and 0x6FB45B42) and buglightning (`mov edx, 0E1h`,
0x6FB45B97), and the function has no other reference. The
arm the server's poison cloud would need is dead on the client -- see the
entry above.

Reproduced.

## MONUNIQUE_MonUModCltWormsOnDeath tests no mode, and nothing gives a monster its modifier

`0x6FB46110`, `MonUnique.cpp` -- gpMonUModCltFuncs row 40 column 2.
**Argued 2026-10-02** at MonUnique.cpp's close. **UNREACHABLE** -- a
divergence from the server on a row no monster carries.

The hook draws the pregnant burst (missiles 117 and 545, sound 2458) on any
call of column 2 -- every MONSTER_UpdateAnimMode, which calls it with no
test (Monster.cpp:2345) -- with no mode or frame test; D2Game's row 40,
MONSTERUNIQUE_CreatePainWorm (column 1, 0x6FC6E700), acts in the death mode
alone, in D2MOO's reading. **Why UNREACHABLE**: nothing in a stock game gives
a monster modifier 40. monumod.txt's worms_on_death has no pick weight (cpick
and upick empty); no SuperUniques.txt Mod column holds 40; skills.txt's
sumumod holds 32, 33 and 42 only; and none of D2MOO's 48 server calls of
D2GAME_BOSSES_AssignUMod_6FC6FF10 passes it -- its MONUMOD_PAINWORMDEATH is
used nowhere (a grep of D2Game and D2Common). A modified table would reach
the hook, and then every column-2 call would burst.

Reproduced.

## NULL tests that cannot fail, and the reads before them -- MonUnique.cpp, five places

`MonUnique.cpp`. **Argued 2026-10-02** at MonUnique.cpp's close.
**UNREACHABLE.**

    MONUNIQUE_BuildMonsterNameText   a NULL pMonster reaches
                                     D2COMMON_SEED_InitLowSeed with 0x20
                                     (`lea ebx, [esi+20h]`, 0x6FB44ACA)
    MONUNIQUE_BuildMonsterNameText   the champion's monstats.txt row read
                                     untested (`mov cx, [eax+6]`, 0x6FB44B8F)
    MONUNIQUE_ModLight               CUNIT_GfxFreeUnitLightMap reads the unit's
                                     pLight (`mov ecx, [esi+64h]`,
                                     0x6FB296E3) before ModLight tests the
                                     unit (`test esi, esi`, 0x6FB44D34)
    MONUNIQUE_MonUModSpawnAuraMissileRing     a NULL unit read at 0x6FB459BA for any
                                     id but 195
    MONUNIQUE_MonUModCltSuicideMinionExplode  its monstats.txt row read untested
                                     (0x6FB45EF7)

**Why UNREACHABLE.** The first two functions are reached only through
gpMonUModHandlers, which MONUNIQUE_ApplyModifiers alone reads, and it
returns for a NULL unit, a unit that is not a monster and a monster with no
data before any handler (MONSTER_GetMonUmodArray's copy,
0x6FB45143..0x6FB4515B). The ring's three callers and the suicide minion's
hook are gpMonUModCltFuncs' hooks, reached only through
MONUNIQUE_MonUModRunCltFuncsForUnit, which returns for the same three
(0x6FB461C7..0x6FB461EE); the hooks have no other
reference. And the monstats.txt rows are the unit's own class's, which
CUNIT_CreateMonsterUnit bounds before the unit exists and
MONSTER_ChangeMonsterClass on entry (UIPANELS_DrawUnitNameLabel's entry above;
Monster.cpp's close).

Reproduced.

## The enchantment line's prefix, and a separator left at its end

`0x6FB462C0`, `MonUnique.cpp`, MONUNIQUE_MonUModBuildMonsterModifierLabel.
**Argued 2026-10-02** at MonUnique.cpp's close. **UNREACHABLE** in the
languages measured.

The prefix is appended and counted with no test against nCapacity
(0x6FB46396..0x6FB463C4), and a separator whose label then fails the length
test stays at the end of the line (0x6FB463F0..0x6FB46441). Both need the
budget of 255 characters reached. The one caller, UIPANELS_ShouldDrawNameLabel,
passes 256 (`push 100h`, 0x6FB2064E) and the prefix its own one caller hands
it: 0, 10077 or 10078 (UIPANELS_DrawUnitNameLabel, ui_panels.cpp:1667..1671).
Over the stock tbl files (per language), the longest line -- the longer prefix, the nine longest of
the fourteen labels and nine separators -- is 142 characters in English, 164
in Spanish, 162 in Italian and 175 in Japanese; and the walk's 480-pixel stop
comes first in any case. The Chinese and Korean counts (279, 450) are tbl
bytes, not the characters D2Lang decodes -- not established there; this
install's German, French, Polish, Portuguese and Russian packs lack tables.

Reproduced.

## UNITS_FindFreeSpotForMonster carries the room from try to try, and one NULL blocks every later try

`0x6FB3EDF0`, `Shared.cpp`. **Argued 2026-10-03** at the MONSTER
block's close. **NOT ESTABLISHED.**

Each try replaces the room it starts from with the room that holds its
point -- the copy of CLIENT_FindRoomContainingSubtile, its answer in ebx
(0x6FB3EECB..0x6FB3EF49, 0x6FB3EF9E..0x6FB3EFA5) -- and the next try starts
from that answer. A point in no room of the start room's near list answers
NULL (`xor ebx, ebx`, 0x6FB3EF49); every later try then starts from NULL,
the search answers NULL at once (`cmp ebx, ecx / jz`,
0x6FB3EECD..0x6FB3EED3), and D2Common's COLLISION_CheckMaskWithSize answers a
NULL room non-zero in every arm of its size switch (D2MOO
D2Collision.cpp:775, and :545, :469, :416 for the arms): nothing more is
placed. **Why NOT ESTABLISHED**: a NULL needs a try's point -- `% 5 - 2` on
each axis, within two subtiles of the unit (0x6FB3EE9F, 0x6FB3EEC5) -- in
no ACTIVE room near the one the try starts from; D2Common's near list holds
active rooms only (DRLGROOM_ReorderNearRoomList, `test esi, esi / jz`,
0x6FD77B6F), and a level's edge has none beyond it. The two stock callers
spawn Duriel's maggots at his corpse (MONUNIQUE_MonUModCltDurielDead) and an
object's rats or mini spiders at the object (OBJECT_AdvanceAnim); whether
any such unit stands within two subtiles of a level's edge or of an
inactive room was not looked for. A count of the critters placed against
the 4 asked for, live in x64dbg, would settle it. The cost is cosmetic:
fewer critters.

Reproduced.

## ENV_SpawnRoomCritters reads a critter's monstats.txt row untested

`0x6FB3EFB0`, `Shared.cpp`. **Argued 2026-10-03** at the MONSTER
block's close. **LATENT** -- the stock levels.txt prevents it.

The copy of DATATBLS_GetMonStatsTxtRecord (0x6FB3F02B..0x6FB3F064) stores
NULL for a class at or past monstats.txt's count, and the group's bounds are
read from it with no test (`mov bl, [eax+30h]`, 0x6FB3F06C). **What
prevents it**: the class is levels.txt's CMon, compiled from a monstats.txt
name through the monstats linker (D2MOO LevelsTbls.cpp:208..211,
TXTFIELD_NAMETOWORD; D2Common's record gives cmon1 the offset 0xCC,
0x6FD5FF0F), and a negative one ends the walk first (`jl`, 0x6FB3EFF3). The
stock levels.txt fills cmon1..4 in 183 cells with ten names -- bat, bug,
bunny, chicken, larva, minispider, parrot, rat, scorpion, snake -- and every
one is a monstats.txt Id (a read of both tables, 2026-10-03; control, a name
that is not one, refused). Whether a name the linker does not know compiles
to a negative id or to something else was not looked for.

Reproduced.

## levels.txt's four CAmt columns compile into one word, so the client's multiplier reaches the first critter column only

`0x6FB3EFB0`, `Shared.cpp` -- a defect of D2Common's levels.txt
compile, met through the client. **Argued 2026-10-03** at the MONSTER block's
close. **INERT** with stock data.

D2Common's field table for levels.txt gives camt1, camt2, camt3 and camt4
one offset, 0xDC: a single `mov eax, 0DCh` (0x6FD60011) stored into the
four records' offset slots (0x6FD6001B..0x6FD60030), where cmon1..4 take
0xCC..0xD2 and cpct1..4 0xD4..0xDA (0x6FD5FF0F..0x6FD6009E; the records are
20 bytes, name, type, length, offset, linker, and one `push` moves esp
between them, at 0x6FD60016). D2MOO's table has the same four 220s
(LevelsTbls.cpp:216..219). So D2LevelsTxt.wCAmt[0] holds whichever camt is
compiled into it last, and wCAmt[1..3] (+0xDE..+0xE2) are never written;
ENV_SpawnRoomCritters multiplies a column's group by its wCAmt[i] (`mov cx,
[ebp+10h]`, 0x6FB3F0BD), so a CAmt can apply to the first column alone, and
from the last camt cell. **Why INERT**: camt1..4 are empty in every row of
the stock levels.txt (134 data rows, read 2026-10-02), so the multiplier
never applies and nothing is lost. Whether an empty cell writes 0 over an
earlier one was not looked for.

D2Common's, so nothing here to reproduce: the C reads wCAmt[i] as the client
does.

## NULL reads and a NULL test that cannot fail -- the MONSTER block's head, three places

`Monster.cpp`, `Shared.cpp`. **Argued 2026-10-03** at the
MONSTER block's close. **UNREACHABLE.**

    MONSTER_ForEachUnitNearby   a room of the near list read untested
                              (`mov edx, [ecx+eax*4]` / `mov esi,
                              [edx+2Ch]`, 0x6FB3F2AB..0x6FB3F2AE)
    ENV_SpawnRoomCritters     the level's levels.txt record used untested
                              (`lea ebp, [eax+0CCh]`, 0x6FB3EFDC)
    ENV_SpawnRoomCritters     the room's NULL test for its seed (`test esi,
                              esi`, 0x6FB3EFD1): its NULL arm

**Why UNREACHABLE.** D2Common fills a room's near list with active rooms
only and counts what it stores (DRLGROOM_ReorderNearRoomList, `test esi, esi
/ jz` before the store, 0x6FD77B6F..0x6FD77B76; the slots past the count are
the ones it zeroes), and a room's removal moves the last entry into the
hole (D2MOO D2Dungeon.cpp:916..919): the first nNumRooms are never NULL. A
room's level id is its level's, and D2Common's DRLG_AllocLevel reads that
id's LevelDef record with no test (0x6FD74905..0x6FD7490A), bounded like the
levels.txt record by the same count (`cmp ecx, [eax+0C5Ch]`, 0x6FD60D99;
`cmp ecx, [edx+0C5Ch]`, 0x6FD603CE): a level whose id has no record faults
when it is made, before it has a room. And DUNGEON_GetLevelIdFromRoom
answers 0 for a NULL room (0x6FD8C7A4..0x6FD8C7A8), on which
ENV_SpawnRoomCritters has returned before its seed test (0x6FB3EFC5). An
aside: D2Common's DATATBLS_GetLevelsTxtRecord refuses id 0 as well (`test
ecx, ecx / jle`, 0x6FD603C4..0x6FD603C6) where D2MOO's source writes `>= 0`
(LevelsTbls.cpp:288); the client has returned on 0 already.

Reproduced.

## REFUTED: PLAYERLIST_RosterGetOrAddInfoEntry's head slot answered as an entry

`0x6FB4B850`, `0x6FB4B854`, `PlayerList.cpp`. **Argued 2026-10-03**
at the small units' close. **REFUTED.**

    6FB4B84C  mov  ebx, eax          ; the first lookup's answer
    6FB4B850  jz   loc_6FB4B89A      ; no list: eax NULL
    6FB4B854  jz   loc_6FB4B89A      ; the same GUID twice: eax the HEAD SLOT
    6FB4B89A  pop  edi ... retn      ; the common exit

The batch kept the database's `D2RosterInfoStrc *` return and marked the
GUIDs-equal exit, where eax still holds ROSTER_GetInfoListByUnitOrCorpse's
answer -- the list's head slot, a `D2RosterInfoStrc **` -- as an entry
answered by mistake. **Nothing reads it**: all five call sites
discard it, each one loading the next call's arguments straight after it --
ROSTER_ToggleLootAndNotify, ROSTER_ToggleIgnoreAndNotify and
ROSTER_ToggleSquelchAndNotify (0x6FAAFF63, 0x6FAAFFD3, 0x6FAB0043) and
ROSTER_PartySendHostileToggle (0x6FAB0594) call PLAYERLIST_RosterTestInfoFlag,
ROSTER_ApplyInfoFlags loads eax afresh (`mov eax, dword ptr [esp+8+nFlags]`,
0x6FAB1044). D2Game's twin, PLAYERLIST_AllocPlayerList (D2MOO's
PlayerList.cpp), is `void`. So the defect was the DECLARED RETURN TYPE, as
with `UIPANELS_BuildUnitLabel` above: eax at the exit is whatever the last step
left -- the walk's node, the new entry, NULL or the head slot -- and the
function is `void` since this close, in the `.i64` (one prototype, the
facts bracket's one difference), D2Ptrs.h, D2Client.h and the C.

## PlayerList.cpp uses FOG_Alloc's answer untested

`0x6FB4B889`, `PlayerList.cpp`. **Argued 2026-10-03** at the small
units' close. **LATENT.**

    6FB4B880  call FOG_Alloc         ; line 116, 12 bytes
    6FB4B885  mov  edx, eax
    6FB4B889  mov  [edx], ecx        ; the first store, nothing between

PLAYERLIST_RosterGetOrAddInfoEntry clears and links the entry FOG_Alloc answers with
no test; the unit's other allocation, PLAYERLIST_RosterAllocPlayerInfoList's head
(line 37), is asserted (`ptInfo`, line 39) with the exit after it. Only a
failed 12-byte allocation reaches the stores, and that is the Fog path of
"Roster.cpp uses FOG_Alloc's answer untested at seven sites", re-read at
this close: FOG_Alloc (Fog 0x6FF58F50, #10042) hands a NULL from its
allocator to ErrorManager #10028 with code 2 (0x6FF58F73) and returns it
(`mov eax, esi`, 0x6FF58F7B); what follows depends on the executable's
error callback, which was not read.

Reproduced -- the answer is used as the bytes use it.

## NULL arms that cannot run -- UNITGFX_GfxDrawUnit's objects.txt record, and Fault.cpp's message-source node twice

`UnitGfx.cpp`, `Fault.cpp`. **Argued 2026-10-03** at the
small units' close. **UNREACHABLE.**

    UNITGFX_GfxDrawUnit                 the objects.txt record read untested:
                                 `call D2COMMON_DATATBLS_GetObjectsTxtRecord`
                                 0x6FB4DC6E, `cmp byte ptr [eax+edi+131h], 2`
                                 0x6FB4DC7B
    FAULT_RegisterMessageSource  the node written untested: `call
                                 FAULT_TLinkedListAllocNode` 0x6FB6BBBD, `mov
                                 [eax+8], edi` 0x6FB6BBC2
    FAULT_TLinkedListAllocNode        a failed allocation linked: `xor esi, esi`
                                 0x6FB6BCB7, `push esi` / `call
                                 FAULT_TLinkedListLink` 0x6FB6BCC6..0x6FB6BCC7

**The record.** D2Common's DATATBLS_GetObjectsTxtRecord (#10626,
0x6FD718F0) tests the id against the record count (`cmp ecx, eax` / `jl`,
0x6FD718F9) and against 0 (`test ecx, ecx` / `jge`, 0x6FD7191B), and each
failing arm is FOG_DisplayAssert with `push 0FFFFFFFFh` / `call _exit` after
it (0x6FD71914..0x6FD71916, 0x6FD71936..0x6FD71938); then it answers the
table plus the id times 448 (`lea` / `sub` / `shl eax, 6`,
0x6FD7193D..0x6FD7194C), with no NULL arm. Object.cpp's verdict on
PLAYER_DispatchClickAction's "ptObjectData" (above) rested on D2MOO's
ObjectsTbls.cpp; these are the bytes.

**The node.** Storm's SMemAlloc (#401, 0x6FFCB130) answers what its
allocator (0x6FFCB460) answers, and every path that would answer NULL
first calls Storm 0x6FFCB280 -- SErrDisplayError, then ExitProcess(1)
(0x6FFCB28D, 0x6FFCB294): SMemAlloc while the dword at
dword_6FFEBC98+810h is 0 (0x6FFCB148), a size above 0x7FFFFFFF (0x6FFCB49F), no heap or no block
(0x6FFCB524, after `test edi, edi` at 0x6FFCB4E4), and a heap whose
reserve or commit fails (0x6FFCB2CF, 0x6FFCB2EF). Game.cpp's
`GAME_SGameDataAllocAndLinkNode` entry (above) read the same allocator to the
same verdict; this close re-read it whole. Also measured here: the flag 8
the template forces is Storm's zero-fill -- the allocator tests it and calls
SMemZero over the block (0x6FFCB529..0x6FFCB532).

Reproduced -- each arm is written as the bytes have it.

## A NULL read and a negative component -- GfxUtil.cpp's two bounds nothing crosses

`GfxUtil.cpp`. **Argued 2026-10-05** at GfxUtil.cpp's close.
**UNREACHABLE**, both.

    GFXUTIL_GetUnitDisplayInventory  a NULL unit sent to the own-inventory
                                     arm: `test edi, edi` / `jz` 0x6FB5DDE7,
                                     `mov eax, [edi+60h]` 0x6FB5DE2F
    GFXUTIL_UnitHasComponent         a SIGNED bound (`cmp edi, 10h` / `jge`
                                     0x6FB5E209) passing a negative
                                     component to the monster arm (`sar edi,
                                     3` 0x6FB5E2E9) and the object arm
                                     (0x6FB5E314), both reading before their
                                     field

**The NULL read.** The out-of-line body has one call,
GFX_DrawUnitCels's at 0x6FB5A9D8, and it sits behind that function's
`call GFXUTIL_IsDrawableUnitType` / `jz` (0x6FB5A84F, 0x6FB5A856), FALSE
for a NULL unit; a scan of the function's 830 instructions for jumps into
0x6FB5A85C..0x6FB5A9D8 from outside finds none (the same scan finds the
five into 0x6FB5ABBF..0x6FB5AC34, `jnz loc_6FB5ABBF` 0x6FB5AC6B among
them). The five inlined copies in GfxUtil.cpp (its `INLINED COPY:
GFXUTIL_GetUnitDisplayInventory` markers) are argued where they stand: the
two in GFXUTIL_ResolveUnitAnimIdentity run in arms only a unit's type
reaches -- the routed one in the player, monster and object case, the one
in place in the player case -- where a NULL unit's type is 6;
GFXUTIL_ResolveUnitTransLevel's needs UNIT_PLAYER,
which a NULL unit never has; GFXUTIL_GetComponentColor's has its unit
tested before it; GFXUTIL_IsUnitComponentVisible's follows D2Common's
UNITS_GetPrecisionY or UNITS_GetClientCoordX, each asserting "ptUnit"
with `call _exit` after (D2Common 0x6FDBD7D4..0x6FDBD7F1,
0x6FDBDB14..0x6FDBDB31); and what has the same shape at
GFX_GetUnitCelContext's head (0x6FB5C7D8) is Gfx.cpp's, not compared.

**Gfx.cpp's seven copies, argued at its close (2026-10-05).** Its batches
routed all seven into calls of the body, each proved
identical: GFX_GetUnitCofNode's (batch 2),
GFX_DrawUnitShadow's (3), GFX_DrawUnitHilight's (4), GFX_AsyncLoadBlock's
(6), and GFX_GetUnitCel's, GFX_HitTestUnitCof's and the one above --
GFX_IsUnitCofOnScreen's, GFX_GetUnitCelContext until batch 9 (9). None
can be handed a NULL unit. GFX_GetUnitCofNode and GFX_DrawUnitShadow test
GFXUTIL_IsDrawableUnitType first (Gfx.cpp:756, 1452); GFX_DrawUnitHilight
and GFX_GetUnitCel read the unit before their call (`pUnit->dwUnitType`,
`pUnit->pDynamicPath`). GFX_IsUnitCofOnScreen's seven callers each follow
D2Common's UNITS_GetPrecisionX or UNITS_GetClientCoordX, which assert the
unit and exit, or GFXUTIL_IsDrawableUnitType: GFX_DrawUnitCelShadow,
GFX_DrawUnitShadow, GFX_DrawUnitCelHilight and GFX_DrawUnitHilight
(Gfx.cpp:1354, 1486 and 1502, 1657, 1802 and 1821), GFX_DrawUnitCels (2192), GfxUtil.cpp's GFXUTIL_IsUnitCelOnScreen, whose two calls follow
the getters in UnitMode.cpp, and GFXUTIL_IsUnitComponentVisible (above).
GFX_HitTestUnitCof's three calls are CUNIT_HitTestUnit's, which reads the
unit's fields before them (CUnit.cpp:2277, 2299, 2305). GFX_AsyncLoadBlock
is reached through GFX_PreloadAnimMode and GFX_PreloadPlayerMode, from the
preload sweep's walk of a room's units and from SCmd.cpp's two spawn
handlers, each of which tests its unit (SCmd.cpp:586, 3002).

**The negative component.** UnitHasComponent's one caller is CUnit.cpp's
CUNIT_GetComponentScreenBox, called twice, both by CUNIT_HitTestUnit:
with 1 (`push 1` 0x6FB28D1F), and with a counter that starts at `xor edi,
edi` (0x6FB28A34) and steps `inc edi / cmp edi, 10h / jb`
(0x6FB28CFE..0x6FB28D02) -- components 0 to 15.

Reproduced -- the reads are written as the bytes have them.

## An item code the client does not hold leaves the armour components unwritten

`GfxUtil.cpp`. **Argued 2026-10-05** at GfxUtil.cpp's close.
**LATENT.**

    GFXUTIL_ResolveUnitAnimIdentity  an equippable torso item whose items.txt
                                     record is NULL: `jz` 0x6FB5E6CD skips
                                     both the copy of nArmorComp[6] and the
                                     clear, and the six bytes reach #10891
                                     COMPOSIT_GetArmorTypeFromComponent as
                                     the frame left them

ITEMS_CanBeEquipped tests only the item's flags (D2MOO, Items.cpp; #10839
at D2Common 0x6FD9FE20), so the record is the only guard. #10600
DATATBLS_GetItemsTxtRecord answers NULL for a class at or past the count,
compared unsigned (`cmp eax, ecx` / `jb`, D2Common 0x6FD5768A,
0x6FD5768C), -1 included. An item's class is the link index of the code
the server sends (D2MOO, Items.cpp:4889, `DATATBLS_GetItemIdFromItemCode`),
and Fog's #10213 answers an unknown code with a trace, "Failed to find
code '%s' in index", and -1 (Fog 0x6FF5B8B7, 0x6FF5B8CB). So it takes a
server whose items.txt holds a code the client's does not -- never stock
play. Whether such an item lives through D2Common's decode to be worn is
not established.

Reproduced: the buffer is declared uninitialised and only its address is
passed, so GfxUtil.cpp reads nothing indeterminate; D2Common does.

## The inventory graphic skips unique id 0

`GfxUtil.cpp`. **Argued 2026-10-05** at GfxUtil.cpp's close.
**LATENT** -- the stock data hides it.

    GFXUTIL_ResolveUnitAnimIdentity  the cursor arm takes a unique's own
                                     record for a file index above 0
                                     (`jle` 0x6FB5EAB0); the ground arm
                                     from 0 (`jl` 0x6FB5EB64)

Unique id 0 is The Gnasher, code hax. Its own `invfile` is invhaxu, and the
arm the skip falls to takes the base item's `uniqueinvfile`, which for hax
is invhaxu too (over the stock UniqueItems.txt and
Weapons.txt). The same file either
way. A table giving unique 0 an inventory graphic of its own would draw the
base's in the cursor and its own on the ground.

Reproduced -- each arm keeps its own compare.

## A particle block for a unit with no gfx record is allocated and lost

`Particle.cpp`. **Argued 2026-10-05** at Particle.cpp's close.
**LATENT.**

    PARTICLE_AllocUnitParticles  GFX_GetUnitParticles answers NULL for a
                                 unit with no gfx record, so a block is
                                 allocated and cleared; GFX_SetUnitParticles
                                 then stores it nowhere (`test eax, eax` /
                                 `jz`, 0x6FB5BA58), and its 76 bytes leak

A unit has no gfx record only if it is NULL or a tile. CUNIT_InitUnit
gives every other type one through UNITGFX_UnitInitGfx -- its switch sends the
UNIT_TILE arm alone past the call (CUnit.cpp:507-555) -- and UNITGFX_UnitInitGfx
asserts it and exits on NULL (line 22). Nothing clears it
afterwards: a linear sweep of D2Client's whole `.text` for stores into
`[reg+54h]`, the esp-based ones aside, finds eight, and UNITGFX_UnitInitGfx's at
0x6FB4DACA is the only one into a unit -- the others are three in DIALOG_*,
PARTY_BuildRowList, SKILLS_SpawnScatteredMissiles' parameter block and two
in the runtime's _raise; a per-function decode of all 3939 functions found
the same eight. GFX_FreeUnitGfxInfo (0x6FB59320) frees the record without
storing anything into the unit. The eighteen calls of ours, read at their
lines, pass a monster CUNIT_CreateMonsterUnit has just given its record
(five, one switch after UNITGFX_UnitInitGfx, CUnit.cpp:1030), missiles tested
against NULL beside the call or made by a spawn that tests them, or the
caller's own unit -- a skill's caster, a state's unit, a monster's action;
none passes a tile. Writes
to a unit's +54h from D2Common were not looked for.

Reproduced.

## The bounce's floor goes into the position unscaled

`Particle.cpp`. **Argued 2026-10-05** at Particle.cpp's close.
**LATENT** -- the one bounce in the game has a floor of 0.

    PARTICLE_UpdateUnitParticle  `mov [esi+0Ch], ebp` (0x6FB5FEAF): nZ =
                                 nTargetZ, the floor in game pixels, where
                                 the compare just above shifts nZ down
                                 (`sar eax, 0Bh` 0x6FB5FE92) and the arrival
                                 snap shifts a target up (`shl edx, 0Bh`
                                 0x6FB60031)

PARTICLE_StartUnitBounce has one call, MISSILE_CltHitFunc53's
(Missile.cpp:6126). The stock Missiles.txt has one row with pCltHitFunc 53,
moltenboulderemerge, whose CltHitSubMissile1 is moltenboulder and whose
cHitPar1..3 are 2, 50 and 10 -- two bounces at 50%, an arc of 10 frames
(read at the close, a control on moltenboulder's own pCltHitFunc 52). The
boulder's block is cleared when MISSILE_ClientSpawn makes it (13751), and
no target is set on it after: ClientSpawn sets none, CltHitFunc53 follows
the bounce with StartUnitArc and ClearUnitTimedMotion, and the boulder's
own functions, pCltDoFunc 47 and pCltHitFunc 52, are not among
PARTICLE_SetUnitParticleVector's twelve call sites. So the floor is 0, and
0 is 0 << 11. A floor other than 0 would put the particle at the floor
divided by 2048.

Reproduced.

## An attached particle's position is written in subtiles

`Particle.cpp`. **Argued 2026-10-05** at Particle.cpp's close,
NOT ESTABLISHED there; **settled the same day at Gfx.cpp's close:
UNREACHABLE** in effect -- both readers are reached and neither lets the
value out.

    PARTICLE_UpdateUnitParticle  the attached arm sets nX and nY from the
                                 screen offsets through D2Common #10108,
                                 DUNGEON_ClientToGameSubtileCoords (`sar 5`
                                 of 2y +/- x), so they hold whole subtiles,
                                 unscaled -- every other writer keeps game
                                 pixels << 11, 65536 times finer

The step does not read nX and nY back for an attached block (its offsets
come from the owner's cel instead). Two functions do, through
PARTICLE_GetUnitGfxCoordX/Y (two references each):
MISSILE_ClientOnHit, which copies a hitting missile's position into its
explosion (Missile.cpp:9567-9570), and Gfx.cpp's GFX_DrawUnitCelShadow
(0x6FB59490; GFX_DrawUnitComponentsPerspective until Gfx.cpp batch 3), which adds `nX >> 11` to the
unit's client X. Subtiles shifted down by 11 are 0, or -1 for a negative
one. The particles attached are the catapults' stones, made invisible as
they are spawned (Monster.cpp:1692, 2484, 2646, each beside
UNITFLAGEX_NODRAW), and a unit in the `attached` state (143), set by the
server on a rider joining a siege tower or beast (States.cpp's
STATES_SetFunc13_Attached). Whether an attached missile ever reaches
MISSILE_ClientOnHit, and what GFX_DrawUnitCelShadow draws for
a rider -- where `nX >> 11` is itself added as a client offset -- was not
settled at Particle.cpp's close: that function was Gfx.cpp's, not written.

**MISSILE_ClientOnHit, settled at Gfx.cpp's close.** The loaded balls do
reach it -- a catapult's attack hands its ball over (Monster.cpp:2464, 2626) -- but the copy runs only for `(__int16)wExplosionMissile >= 0`, and
the five balls' compiled ExplosionMissile is 0xFFFF: the stock
missiles.bin, 684 records of 420 bytes, the word at +22 (controls: the Id
at +0 of all 684; the 96 non-empty references equal to their targets' Ids;
all 588 empty ones 0xFFFF). nExplosionMissile is that word alone, and
the copy's guard tests both (Missile.cpp:9540).

**GFX_DrawUnitCelShadow, settled at Gfx.cpp's close.** A catapult's owner
is its loaded ball (`D2COMMON_UNITS_StoreOwner(<the catapult>, pMissile)`,
Monster.cpp:1689, 2481, 2643), and GFX_DrawUnitCels draws the owner of a
COMPOSIT_SPECIAL7 layer forced, `UNITGFX_GfxDrawUnit(pOwner, dwLight, 1)`
(Gfx.cpp:2328). Every COF of catapult1, 2 and 3 -- tokens 65 and 64, modes
A1, DD, DT and NU -- orders an S7 layer in every frame; catapult4's (UA)
none (its layout read as
GfxUtil.cpp's readers walk it; 3509 of the 3513 COFs there pass the size
check 28 + 9L + F + D*F*L). The forced draw skips the NODRAW test and sets
UNITFLAGEX_ISINLOS on the ball unless the line-of-sight test hits
(UnitGfx.cpp:151-169), and nothing else clears that bit: every `and` or
`btr` on +0C8h..+0CBh, read at its width -- D2Client 9 rows, whose one
variable mask is MONSTER_SetFlagEx's own (`and eax, edx` 0x6FB448FF, the
control), D2Common 5, none on bit 7. The
ball sits in its room's unit list -- MISSILE_InitClientUnit's
PATH_AllocDynamicPath adds a unit to the room it is given (D2MOO
Path.cpp:770-774) -- so WALL2_WallAddRoomToSquareGrid puts it on the shadow list
(Wall2.cpp:617), and DWALL_EnvDrawCellShadowsAndUnits hands it to
GFX_DrawUnitShadow, which sends a missile to GFX_DrawUnitCelShadow. There
the subtile-valued nX and nY are added `>> 11` -- and nothing is drawn.
GFX_IsUnitCofOnScreen asks for the COF of the unit's mode from its
animation list, and a missile's list is always empty: UNITGFX_UnitInitGfx gives a
missile a gfx block and no node (UnitGfx.cpp:50-52), GFX_RebuildUnitGfx
answers NULL for a type past UNIT_OBJECT, and the one store that links a
node (`mov [eax+28h], ebp` 0x6FB58A77) is GFX_GetUnitCofNode's, after that
function's GFXUTIL_IsDrawableUnitType refusal. That store was found by a
scan of every store to +28h in the functions that take a gfx block -- 55,
the control 0x6FB58A77 among them; the rest clear the list, unlink a node
or write another record. IDA's own
field cross-references answered none for pAnimList and none for
pOverlayList, which is read everywhere: the operands are untyped. So
GFX_IsUnitCofOnScreen answers FALSE and GFX_DrawUnitCelShadow returns
before its draw -- for the ball, and by the same reading for every unit
that reaches it, all of them types past UNIT_OBJECT.

A rider in the `attached` state is a monster, drawn by components, and the
component path in GFX_DrawUnitShadow reads the block's Z and screen
offsets, never PARTICLE_GetUnitGfxCoordX/Y.

Reproduced.

## The worldstone chips' targets are set 2048 times out

`Monster.cpp` (MONSTER_EnvTickWorldstoneChips), read from Particle.cpp's
side. **Argued 2026-10-05** at Particle.cpp's close. **LATENT** -- the chips
end first.

    MONSTER_EnvTickWorldstoneChips  PARTICLE_SetUnitParticleVector(pChip, 0, 0,
                             300, 1) for a rising chip and (..., -100, 1)
                             for a falling one (Monster.cpp:1115, 1121):
                             the flag shifts the target << 11, where the
                             step keeps a target unscaled -- held as 614400
                             and -204800 game pixels

The other ten calls pass 0. A chip starts at -100 rising or at 300
falling, at 6 game pixels a frame (its SetUnitGfxCoords and
SetUnitGfxVector beside the call), with no acceleration --
MISSILE_ClientSpawnSimple asks MISSILE_ClientSpawn for MISSILESPAWN_STARTPOS
only (Missile.cpp:10040), so no arc -- and is given 60 frames
(MISSILE_SetTotalFrames and MISSILE_SetCurrentFrame, Monster.cpp:1126, 1127). In 60 steps it reaches 260 or -60, 266 or -66 counting the step
made at spawn (1125): short of 300 and of -100, so even the unshifted
target would never be reached, and the shift changes nothing a chip does
in the life it is given.

Reproduced, in Monster.cpp as the bytes have it.

## GFX_DrawOverlayAt's second pass draws nothing

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**UNREACHABLE.**

    GFX_DrawOverlayAt  the list cursor is not reset between the two passes:
                       pass 0 leaves esi NULL (`jnz loc_6FB57D55` 0x6FB57E5E
                       falls through) and pass 1 opens with `test esi, esi`
                       / `jz` (0x6FB57D4D), so no PreDraw entry is drawn

The function has one caller, UIPANELS_DrawChargeUpIcons (0x6FB21B45), whose
whole body sits behind `mov eax, gbChargeUpIconsEnabled` / `test` / `je`
to its exit (0x6FB21A32..0x6FB21A39). That gate has one reference in the
module, this read, and lies past `.data`'s raw size, where the loader
zero-fills (not file-backed). UIPANELS_DrawChargeUpIcons' one caller is
UIPANELS_DrawInGamePanels. No pass of this function runs in a stock client.

Reproduced.

## A player's preload of a given mode answers the drawn mode's node

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**NOT ESTABLISHED** -- harmless wherever it was followed.

    GFX_RebuildUnitGfx  with a mode given, the PLAYER and OBJECT arms load
                        that mode and then look up the node of the unit's
                        CURRENT drawn mode -- the player arm's copy of
                        GFX_GetUnitAnimNode with a constant -1
                        (0x6FB58658..0x6FB586B8), the object arm's `push
                        0FFFFFFFFh` (0x6FB58705) -- where the MONSTER arm
                        answers the given mode's

A mode reaches this function only from GFX_AsyncLoadBlock: directly
(`push ebx` 0x6FB5B6A6), and through GFXUTIL_ResolveUnitAnimIdentity,
whose mode is FillGfxData's -- -1 at thirteen call sites, the loaded mode
at AsyncLoadBlock's one (Gfx.cpp:2745, after the node is built). AsyncLoadBlock serves the preload
sweeps, which preload monsters and players only, so the OBJECT arm with a
mode is never taken. A player's preload of a mode other than its drawn one
answers the drawn mode's node, and both consumers test the answer for NULL
only: a node lets the load go on; a NULL -- the drawn mode with no node of
that weapon class code -- traces "sAsyncLoadBlock failed on async:%d ...",
and GFX_PreloadPlayerMode clears the mode's queued and loaded flags in
gaPlayerPreloadTable (Gfx.cpp:3925-3928), so a later sweep asks again.
Whether a player can be preloaded before its drawn mode has a node was
not looked for.

Reproduced.

## A COF that will not load costs the node made for it

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**NOT ESTABLISHED.**

    GFX_GetUnitCofNode  the 20-byte animation node, allocated and zeroed,
                        is returned past with no FOG_Free when
                        GFX_LoadCofFile fails (0x6FB58A2E..0x6FB58A4F)
    GFX_LoadCofFile     the 28-byte cache node, allocated and not yet
                        linked, is returned past when ARCHIVE_LoadFile
                        fails (`jz loc_6FB58DA6` 0x6FB58CF1)

Both need a COF the archives do not hold: ARCHIVE_LoadFile traces "Error
opening file" and answers NULL (Archive.cpp:158). GFX_LoadCofFile's other
caller, GFX_AddMonsterCofFiles, halts on that NULL ("Could not add cof
file", Gfx.cpp:3640 and the FOG_DisplayHalt after it), so only
GFX_GetUnitCofNode's path goes on: both nodes are lost, GFX_GetUnitCofNode
traces "Could not add cof file: %s" and answers NULL, and its caller does
without the mode.
Every path is built from the unit's token, mode and weapon class --
D2Common's #10885, and for a monster BuildMonsterCofName's
"%s\%s\COF\%s%s%s.COF" (D2MOO D2Composit.cpp:172). What would settle it:
the stock archives' COF list against every path those two can build for a
class, a mode and a weapon class a unit can be in. Not done.

Reproduced.

## GFX_LoadCofFile's "already cached" test looks up the title of the title

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**LATENT** -- the callers search first.

    GFX_LoadCofFile  the title is taken from the path (0x6FB58B35..
                     0x6FB58B72), then the cache is searched by the title
                     of THAT (0x6FB58B9C..0x6FB58BE1): a stem with no '.',
                     so the empty string its buffer was cleared to

Both callers search the cache by the true title themselves and call only
on a miss (Gfx.cpp:835-843 in GFX_GetUnitCofNode, 3631-3640 in
GFX_AddMonsterCofFiles). The test for "" matches only a node whose
eight-byte name is all zero -- GFX_CofNameEquals compares two dwords --
and the nodes these two callers make are named by COF titles, every one
of them seven characters (the entry below); the names LoadActCofData gives
an act block's nodes were not read. GFX_HashCofName asserts only that a
title is shorter than 8, so "" hashes and the search runs. Nothing is
found, and the callers' own lookups do the work the test was meant to.

Reproduced.

## GFX_LoadCofFile copies the title into the node before asserting its length

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**UNREACHABLE** in effect.

    GFX_LoadCofFile  the title copied into the node's eight-byte szName
                     (0x6FB58CC4..0x6FB58CCC) before GFX_HashCofName
                     asserts its length below 8 (0x6FB58D29): a longer one
                     runs over pData and unk0x18 first

A title of 8 or more halts at that assert right after, so the overrun
could only precede a halt; and no title the code builds is that long. A
title is the unit's token, a mode and a weapon class: monstats.txt's Code
is two characters in all 704 rows, objects.txt's Token two in 572 and
three in one -- row 0, Dummy, `NU0`, whose eight Mode flags are all 0, and
GFX_LoadObjectModeGfx halts on an unused mode before any path is built
(Gfx.cpp:1073) -- PlrMode.txt's, MonMode.txt's and ObjMode.txt's codes two,
monstats2.txt's BaseW and WeaponClass.txt's codes three. The install's own COFs agree: 3512 of
3513 are named with seven characters; the eighth, CHARS\AM\COF\
amblxbow.cof, 72 bytes whose header does not match their count, is a name
no two-two-three path can form (a census of the install's COF names).

Reproduced: the copy is made as the bytes make it.

## GFX_LoadCofFile uses FOG_Alloc's answer untested

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**LATENT.**

    GFX_LoadCofFile  FOG_Alloc(28, Gfx.cpp, 219, 0), then the node
                     cleared, flagged and named with no test of the answer

Only a failed 28-byte allocation reaches the stores, and that is the Fog
path of "PlayerList.cpp uses FOG_Alloc's answer untested" (above): the
NULL handed to ErrorManager #10028 and returned, what follows depending on
the executable's error callback, which was not read.

Reproduced -- the answer is used as the bytes use it.

## Two exits of the shadow and of the layer draw leave the player's unit perspective off

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**NOT ESTABLISHED.**

    GFX_DrawUnitShadow  a failed rebuild (`jz loc_6FB59A73` 0x6FB59A1A) and
                        the missing COF's trace (0x6FB59A6B) return through
                        0x6FB59A73, which restores nothing
    GFX_DrawUnitCels    a failed rebuild (`jnz` not taken, 0x6FB5AA02) and
                        the missing COF's trace (0x6FB5AA60)

The unit perspective is forced off only for the local player drawn as a
player (Gfx.cpp:1458-1479, 2242-2247), so only the player's own rebuild or
COF failing leaves it off -- and then until something sets it again.
That needs a COF missing for the player's class, a mode and a weapon
class: the question "A COF that will not load costs the node made for it"
(above) leaves open.

Reproduced -- the exits return as the bytes return.

## The objects.txt row is read untested by the shadow and the layer draw

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**UNREACHABLE.**

    GFX_DrawUnitShadow  an object's offsets read from #10626's answer with
                        no NULL test (0x6FB5986C)
    GFX_DrawUnitCels    `cmp byte ptr [eax+ecx+131h], 1` (0x6FB5AB32) on
                        the same answer

D2Common's DATATBLS_GetObjectsTxtRecord asserts both bounds, exits, and has
no NULL arm -- "NULL arms that cannot run -- UNITGFX_GfxDrawUnit's objects.txt
record, and Fault.cpp's message-source node twice" (above), measured at
D2Common 0x6FD718F0.

Reproduced.

## GFX_GetUnitHilightPaletteIndex reads a monstats.txt row it did not find

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**LATENT.**

    GFX_GetUnitHilightPaletteIndex  a class outside the table makes the
                                    copied lookup's NULL (`xor eax, eax`
                                    0x6FB59EB9) and `mov al, [eax+0Dh]`
                                    (0x6FB59EC1) reads it

The class is the unit's after #11013, for a unit drawn as a monster: a
monster's own class, or the one states.txt's gfxclass gives a transform --
430 wolf, 431 bear, 135 vampire5, 212 bonefetish1 for the four GfxType 1
rows (wolf, bear, monsterset, delerium; read at the close) -- all in the
table. A monster's own class is bounded where the unit is made and where
it changes: CUNIT_CreateMonsterUnit refuses one out of range before it
allocates, and MONSTER_ChangeMonsterClass bounds a new one on entry
(CUnit.cpp's verdict on UIPANELS_DrawUnitNameLabel).

Reproduced.

## The life percentage of the hit flash can overflow

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**LATENT.**

    GFX_GetUnitHilightPaletteIndex  `lea` / `lea` / `shl 2`
                                    (0x6FB59EEA..0x6FB59EF0): STAT_HITPOINTS
                                    times 100 in 32 bits, the stat 8.8
                                    fixed point -- more than 83886 hit
                                    points wraps

Every write of STAT_HITPOINTS in D2Client's image was listed. The client keeps a monster's life on a
scale of 128: its STAT_MAXHP is 0x8000, and its life is the constants
25600 and 0x8000, packets 0xAB's and 0xAC's 16-bit wLifeScaled, or a
MONSTER_DoClientAction parameter shifted left by 8 -- in the GETHIT and
HIT arms a packet byte masked to 7 bits and stepped by one, 128 at most
(Monster.cpp:3134, 3168). 100 * (65535 << 8) is below 2^31. The KNOCKBACK
arm's pParams[5] (Monster.cpp:3192) and the write at 0x6FB43D49 take their
value unmasked, and their packet fields were not traced; a wrap needs
83886 or more there. The 32-bit stat packets (0x20, 0x9E..0xA2) can carry
a hireling's real life, which would need 83886 hit points or more. The
function answers only for a unit drawn as a monster.

Reproduced: written unsigned, which wraps the same way and is defined.

## The green-blood palette is fetched for a blood overlay and thrown away

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**LATENT** -- the stock Color.txt keeps green blood off.

    GFX_DrawUnitOverlays  for an overlay whose overlay.txt LocalBlood is set
                          (`mov al, [ebp+81h]` 0x6FB5A6A2), with green blood
                          on, PALSHIFT_GetGreenBloodRemap is called
                          (0x6FB5A6B5) and its answer overwritten by the next
                          load (0x6FB5A6BA); TEXTURE_CelDraw gets `push 0`
                          (0x6FB5A6CE)

Green blood is on only when Data\Local\Color.txt's first byte is '1'
(Game.cpp, the read in GAME_InitGame, through the archive). The
install holds one Color.txt, `0\r\n` (data\local\Color.txt), so the remap
is never asked for. Where a build or a file holds '1', the four LocalBlood
overlays of the stock overlay.txt -- durieldead_fwd, durieldead_rear,
tentacleheadbloodripple, suicide_death; the other 289 rows hold 0 -- are
drawn with no palette, red, where a LocalBlood monster's
COMPOSIT_SPECIAL8 layer does take the remap (GFX_DrawUnitCels,
Gfx.cpp:2314-2322).

Reproduced.

## GFX_DrawUnitCels reads a missiles.txt row it did not find

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**LATENT.**

    GFX_DrawUnitCels  a class outside the table makes the copied lookup's
                      NULL (`xor ecx, ecx` 0x6FB5B00F) and `mov al,
                      [ecx+18Dh]` (0x6FB5B011) reads it

A client missile's class is refused before the unit is made:
MISSILE_InitClientUnit bounds it (0x6FB39771..0x6FB39777), as "Chilling
Armor's retaliation reads a NULL missiles.txt row for a class outside the
table -- the client and the server alike" (above) measured; only a
malformed server could hand one past.

Reproduced.

## The highlight's brightened light is kept from layer to layer

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**REACHABLE.**

    GFX_DrawUnitCels  a highlighted layer's light: the low byte doubled and
                      held to 64..255, and the result written back
                      (`mov [esp+124h+dwGamma], ecx` 0x6FB5ADCC)

So the next highlighted layer doubles what this one doubled, and the
pass-0 overlays after the layers and an owner drawn through
COMPOSIT_SPECIAL7 get the result. The arm is taken for every layer of a
unit CUNIT_ShouldHighlightUnit names -- the selected unit, a door
excepted (CUnit.cpp) -- that neither overrides its transparency in the COF
nor has a level of its own, when the unit is not ghostly
(Gfx.cpp:2366-2377). For a unit whose COF orders two or more such layers
-- the catapults' COFs order seven or eight -- in light
whose low byte is below 128 the first layer is drawn at twice the light
and each later one brighter, up to 255. How the difference shows on
screen is D2Gfx's renderer's to say; not read.

Reproduced -- dwLight is written back.

## GFX_ResetUnitGfx clears sixteen of the twenty mode flags

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**INERT.**

    GFX_ResetUnitGfx  the mode loop stops at 16 (`cmp esi, 10h`
                      0x6FB5B143) where the block holds wModeLoaded[20]:
                      modes 16..19 keep their flag while the list that held
                      their nodes is freed

The flags are read only by GFXUTIL_IsUnitModeLoaded (GfxUtil.cpp:1125),
whose two callers ask for a monster's modes, 0..15
(GFX_LoadMonsterModeGfx), and an object's, 0..7 (GFX_LoadObjectModeGfx). Modes 16..19 are a player's
(PLRMODE_SPECIAL4, _DEAD, _SEQUENCE, _KNOCKBACK), and no player's flag is
ever read. Reads inlined into code not yet written were not looked for.

Reproduced -- the loop stops at 16.

## The preload sweep draws the loading screen for 31 rooms in 32, and keeps a cleared bAsync

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**REACHABLE**, both; whether the first is a defect, NOT ESTABLISHED.

    GFX_PreloadVisibleUnits  a synchronous or must-preload sweep counts the
                             rooms and draws the loading screen for each
                             whose count is NOT a multiple of 32 (`and dl,
                             1Fh` / `test dl, dl` / `jz`,
                             0x6FB5B297..0x6FB5B2A1)
                             a player met while gbPreloadRequired is set
                             clears bAsync (`mov [esp+14h], ebp`
                             0x6FB5B456) for itself and every unit after it
                             in the sweep, monsters included

The first runs on every such sweep; that the test reads inverted -- a
screen drawn 31 times in 32 where one in 32 looks meant -- the bytes
cannot settle. The second needs gbPreloadRequired, which is D2Win's
#10185 (0x6F8A5DF0): 1 when d2char.mpq is found neither in the install
nor in the current directory (GetFileAttributesA) -- a minimum install,
which leaves that archive on the CD. There a sweep that meets a player
loads every later unit synchronously.

Reproduced.

## GFX_AsyncLoadBlock would resolve a -1 mode for the node and not for the COF

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**UNREACHABLE.**

    GFX_AsyncLoadBlock  nAnimMode -1 resolved to the drawn mode for the
                        node (`cmp ebx, 0FFFFFFFFh` 0x6FB5B62F) and compared
                        as -1 by the COF search (`cmp [eax], ebx`
                        0x6FB5B70D), which would trace "Unable to get cof
                        file"

Its six call sites pass a mode read from the preload
tables: 0x6FB5B4D9, 0x6FB5B9AB, 0x6FB5D7A8, 0x6FB5D8AA, 0x6FB5D9BE,
0x6FB5DCDF.

Reproduced.

## A shadow's COF box is moved left by half its height, and its right edge not at all

`Gfx.cpp`. **Argued 2026-10-05** at Gfx.cpp's close.
**NOT ESTABLISHED.**

    GFX_IsUnitCofOnScreen  with bShadow set, the box's left edge is moved
                           left by (nYMax - nYMin) / 2, divided signed
                           (0x6FB5C983..0x6FB5C988); the right edge's test
                           uses nXMax unmoved

Whether half the HEIGHT on the X axis is the shear a shadow is drawn with
or a slip: D2Gfx's #10075 TEXTURE_CelDrawShadow (0x6FA74360) adds 2 to X
and hands the draw to the renderer's interface slot +90h (0x6FA7436E,
0x6FA74375), so the shear is D2DDraw's, D2Direct3D's or D2Glide's, and
none of the three was open at the close. The two callers that set bShadow
are the shadow draws (Gfx.cpp:1376, 1538), GFX_DrawUnitCelShadow's never
drawing (above, "An attached particle's position is written in
subtiles").

Reproduced.

## The sound channels read an instance's sounds.txt record untested

`SoundChan.cpp`. **Argued 2026-10-05** at SoundChan.cpp's close.
**LATENT.**

    SOUNDCHAN_ReapFinished         `cmp [eax+3Eh], bl` (0x6FB52D2D) on
                                   SOUNDHDR_GetRecord's answer
    SOUNDCHAN_StartInstance        the asked record's `mov edx, [eax+64h]`
                                   (0x6FB5255D) and the chosen one's
                                   `mov al, [ebp+4Fh]` (0x6FB5256E)
    SOUNDCHAN_ApplyVolumeAndPosition  `mov al, [ebp+53h]` (0x6FB52A04)

SOUNDHDR_GetRecord answers NULL for an id outside sounds.txt, and each of
these reads the answer with nothing between, so a bad id would fault at
3Eh, 64h, 4Fh or 53h. The ids are the instances' own. In the 123 functions
of the sound area two places write an instance's nSoundId (ida_all.txt's
pseudocode): SOUNDINST_PlaySound stores the id it was called with, after
reading that id's own record untested (`!pSoundHdr->nVolume`) -- so an id
outside the table faults there first and never reaches a channel -- and
StartInstance stores SOUNDHDR_PickRandomInGroup's pick, the id plus
SOUNDSONG_RollRandom(Group Size): a value in [0, n) (`and` with n - 1 or an
unsigned `div`, 0x6FB57150..0x6FB571A6). In the stock Sounds.txt the
group never runs past the table: 4699 rows, Index equal to the row in every
one, 692 rows with a Group Size above 1, none reaching past row 4698.

Reproduced.

## The sound units read a sounds.txt row off ids they bound below only

`SoundHdr.cpp`, `SoundInst.cpp`, `SoundSong.cpp`. **Argued
2026-10-06** at the close of SoundEnv.cpp, SoundHdr.cpp, SoundInst.cpp and
SoundSong.cpp. **LATENT.**

SOUNDHDR_GetRecord answers NULL for an id outside [0, gnSoundHeaderCount),
and twenty-five MARKED heads read its answer, or the NULL arm of an inlined
copy of it, with nothing between:

    SoundHdr.cpp   GetGroupBaseIndex, GetGroupSize, IsLooping (the entry
                   "The three SOUND accessors fault on their own bounds-check
                   arm"); AdjustRefCount and ClearRefCount, a group member
                   past the table (0x6FB54A3E, 0x6FB54AF2); FreeSoundData
                   (0x6FB54CEB); LoadWaveData (0x6FB54DE9, and each eviction
                   candidate's row, 0x6FB55013); GetRangeParam (0x6FB55254);
                   GetFalloffDistance (0x6FB552C4); PickRandomInGroup
                   (0x6FB5533D); BuildSoundPath (the `repne scasb`,
                   0x6FB5545F); TouchCacheEntry (0x6FB55502)
    SoundInst.cpp  DetachFromUnit, MarkStarted, PlaySound (0x6FB55868,
                   0x6FB5589F), UpdateForUnit, the six releases of a set,
                   ServiceInstances' four reads
    SoundSong.cpp  UpdateMusic (0x6FB56FE9), ScheduleSting (0x6FB5706A)

Every id reaches them from SOUNDINST_PlaySound -- an instance's, a group's
pick from it, a song from the song table -- or from a caller that hands the
same kinds of id. PlaySound bounds its id below only (`jg`, 0x6FB55844), so
the answer is the ids its callers pass, measured over all 270 call sites in
D2Client:

* 168 are constants, the largest 0x1220 (4640); the stock sounds.txt has
  4699 rows, Index equal to the row in every one;
* the other 102 are excel columns and the values made from them. Every
  sound-naming column of the stock tables names a sounds.txt row -- items'
  (armor, weapons, misc, uniqueitems) dropsound and usesound, missiles.txt's
  three, monsounds.txt's sound columns, skills.txt's eight, states.txt's two:
  0 values of any of them outside sounds.txt's names; soundenviron.txt's numeric Song, Day/Night Ambience and Event are
  at most 4684;
* one is a packet's field, 0x5D's for quest 33 (QuestLog.cpp, Quest.cpp until 2026-10-08): the stock server
  sends the reward's items.txt DropSound there, and only when it is positive
  (D2MOO's A5Q3.cpp, 719..725);
* a group's pick is the id plus SOUNDSONG_RollRandom(Group Size), in [0,
  n); no group of the stock table runs past row 4698.

With no table GetRecord answers NULL for every id. Before the first game
the flag PlaySound tests first, gbSoundPlaybackEnabled, is 0 -- .data past
its raw size (`ida_bytes.is_loaded` False) -- and SOUNDHDR_Init loads the
tables before it raises it (SOUNDINST_ResetPlayback); the pump runs only
while the channels are up. But SOUNDHDR_Shutdown frees the tables and
zeroes their pointers after SOUNDINST_FreeAllInstances has set the flag to
1: a PlaySound between a game's Shutdown and the next game's Init would
read off NULL. Whether D2Client plays a sound in that window was not looked
for -- NOT ESTABLISHED for that window alone. A sounds.txt shorter than the
ids, or a server sending another id, is what the missing bound would have
caught.

Reproduced at every site.

## The sound environment reads its levels.txt and soundenviron.txt rows untested

`SoundHdr.cpp` (SOUNDHDR_GetCurrentSoundEnvironId), `SoundEnv.cpp`
(SOUNDENV_UpdateEnvironment). **Argued 2026-10-06** at the four sound
units' close. **LATENT.**

GetCurrentSoundEnvironId reads levels.txt's SoundEnv off D2Common #10631's
answer untested (`mov cl, [eax+21Ch]`, 0x6FB53D18), NULL for a level id
outside the table. The id is the level of the player's room, and D2Common
builds every level through DRLG_AllocLevel, which reads the id's level
definition untested first (D2MOO's DrlgDrlg.cpp:412) -- a bad id faults in
D2Common before a room of it exists.

UpdateEnvironment tests SOUNDHDR_GetEnvRecord's answer once and reads it
twice more (`[edi+4]` 0x6FB5393B, `[edi+1Ch]` 0x6FB53B77). The answer is
NULL with no table or an id outside [0, 50): the function runs only from
SOUNDHDR_PumpPreload, which returns while the channels are down, and
SOUNDHDR_Init raises them after LoadSoundsTxt and Shutdown takes them down
before it frees the tables; the id is levels.txt's SoundEnv, 0 to 49 in
every row of the stock table, against soundenviron.txt's 50 rows.

Reproduced.

## The footstep clear steps by four over the monster groups of three

`SoundEnv.cpp` (SOUNDENV_UpdateEnvironment). **Argued 2026-10-06**
at the four sound units' close. **REACHABLE**; what it costs settled at
UnitSnd.cpp's close, 2026-10-07 (the last paragraph).

On a change of level the reference counts are cleared from
SOUND_LIGHT_WALK_DIRT_1 (2720) to SOUND_WALK_HOOF_4 in steps of 4, forty
calls of SOUNDHDR_ClearRefCount. Past SOUND_WALK_FLESH_1 (2864) the groups
have 4, 3, 3 and 4 rows (ginormous_low 2868, ginormous_high 2871, hoof
2874), so 2872 and 2876 are members, whose group size is 1 and head the
group's: ClearRefCount(2872) clears 2871 alone and ClearRefCount(2876) 2874
alone. ginormous_high_2, _3 and hoof_2.._4 keep their counts. The step is
taken on every change of level, and the counts are not the environment's:
SOUNDENV takes only the material groups below 2864; the monster footsteps'
counts are UNITSND_AdjustUnitSoundRefCounts' (seven
calls, beside UpdateEnvironment's four), and stock monsters walk on these groups -- andariel, baal and
baalcrab on walk_hoof_1, bloodlord, pinhead, siegebeast and thornhulk on
walk_ginormous_high_1 (monsounds.txt FootstepLayer).

What it costs, settled at UnitSnd.cpp's close (2026-10-07).
UNITSND_AdjustUnitSoundRefCounts counts a unit in once and out once -- its
bHasActiveSound: CUNIT_AddUnitToTable's +1, CUNIT_DestroyUnit's and
UNITSND_PlayHitOrDeathSound's -1 -- and SOUNDHDR_AdjustRefCount takes or
releases a group through its head, every row of it. A monster of one of the
two groups that is in the client's table across a change of level keeps its
+1 on the members the clear left; at its -1 the head is 0, and AdjustRefCount
releases nothing for a head at 0 (`ptHeader->dwRefCount > 0 || nDelta !=
-1`, SoundHdr.cpp). So ginormous_high_2, _3 and hoof_2.._4 stay referenced
until the table is freed at SOUNDHDR_Shutdown: SOUNDHDR_ServiceAsyncLoads
loads a referenced row with no data, and the cache's first eviction pass
skips a referenced row (SoundHdr.cpp). A member never falls below its head,
so the release assert (the original's SoundHdr.cpp:476) cannot fire. What is heard was not
seen to change: an instance that starts a row with no data loads it
(SOUNDHDR_LoadWaveData's forced load, SoundHdr.cpp).

Reproduced.

## The song table is allocated at every SOUNDHDR_Init and never freed

`SoundHdr.cpp` (SOUNDHDR_LoadSoundsTxt). **Argued 2026-10-06** at
the four sound units' close. **REACHABLE.**

LoadSoundsTxt allocates one dword per song id, GetEnvironLookupMin to Max
(0x6FB5483B), and nothing frees it: SOUNDHDR_Shutdown frees the two tables
and zeroes the bounds but not this block, and SoundSong.cpp, the only
other code that touches it (through SOUNDHDR_GetEnvironLookup), zeroes and
writes slots. GAME_AppModeGame calls SOUNDHDR_Init (Game.cpp) and
SOUNDHDR_Shutdown once a game, so every game after the first in one run of
the client allocates over the previous block: 112 bytes a game with the
stock sounds.txt.

Reproduced.

## The async pass stops at row 2933, the stock table's last unstreamed row

`SoundHdr.cpp` (SOUNDHDR_ServiceAsyncLoads). **Argued 2026-10-06**
at the four sound units' close. **LATENT.**

The pass walks the rows 1..2933 whatever the table's size. In the stock
sounds.txt every row from 2934 on is Stream 1 and Async Only 0 (1765 rows)
and every row below is Stream 0: the rows never serviced are the streamed
ones, which load no wave data, and the bound is the table's own shape --
not a defect while that holds. A sounds.txt of fewer than 2934 rows would
be walked past its end.

Reproduced.

## FreeAllInstances would spin on a NULL list value, which nothing pushes

`SoundInst.cpp` (SOUNDINST_FreeAllInstances). **Argued 2026-10-06**
at the four sound units' close. **UNREACHABLE.**

The walk re-reads the list's head each pass and, on a node whose value is
NULL, returns to its own test with the head unchanged (0x6FB56AD5 ->
0x6FB56B25): it would spin forever. ServiceInstances' sweep and
IsSpeechPlayingForUnit test for a NULL value too. Nothing makes one: the
one push onto gpSoundInstances is PlaySound's, of a record of the pool --
an element's address, or the function returns before the push when the
pool is full (SoundInst.cpp) -- and the sort only swaps values already
there. The reap list takes instances (four pushes, ServiceInstances). So
the spin never runs, and the C, which a spinning loop would make undefined
in C++, stays as written.

Reproduced.

## The speech scan replaces the group scan's conflict

`SoundInst.cpp` (SOUNDINST_ServiceInstances). **Argued 2026-10-06**
at the four sound units' close. **INERT.**

Before a start, a row with Defer Inst or Stop Inst finds the oldest playing
instance of its group (on the instance's unit, when it has exactly one), and
then a speech line on exactly one unit sets the conflict to NULL and looks
for any playing speech line on that unit (0x6FB566B2) -- the first answer is
replaced, not combined. It cannot be dropped in the stock data: no group
crosses the speech block's edges (2934, 4656), so a group conflict of a
speech line on its unit is itself a playing speech line on that unit, and
the second scan finds one. Which one it finds can differ only if two speech
lines play on one unit, which the same scan prevents at every start (all
1723 speech rows carry Defer Inst or Stop Inst).

Reproduced.

## A resumed loop whose start fails is left at volume 0

`SoundInst.cpp` (SOUNDINST_ServiceInstances). **Argued 2026-10-06**
at the four sound units' close. **NOT ESTABLISHED.**

A start with a saved stream position resumes over a 3-tick fade-in and
zeroes the instance's volume first (`mov [esi+1Ch], ebp`, 0x6FB56747). If
SOUNDCHAN_StartInstance then answers NULL -- every channel of the kind held
by instances that outrank it -- a row that does not loop is reaped the same
pass, but a looping one stays in state 0 at volume 0, and the pass starts
only instances of non-zero volume: it is silent until something sets its
volume. What would settle it: a measurement in play of a loop stopped with
a saved position (SOUNDCHAN_StopAndDetach saves one) and restarted while
its kind's channels are all taken.

Reproduced.

## The player-seed rolls read the player untested

`SoundSong.cpp` (SOUNDSONG_RollRandom, RollRandomInRange,
RandomOffset; UpdateMusic). **Argued 2026-10-06** at the four sound units'
close. **NOT ESTABLISHED** for the rolls; LATENT for UpdateMusic.

Each roll takes CUNIT_GetPlayer()'s seed at +20h untested, so with no
player a roll of a positive range reads address 20h. SoundEnv.cpp's three
rolls run past SOUNDHDR_GetPlayerLevelId, which answers 0 without a
player; UpdateMusic's player, handed to CUNIT_IsUnitDead untested
(0x6FB56EC8), is the same, the function having returned on a level of 0.
The others -- SOUNDHDR_PickRandomInGroup's two (a grouped sound started),
OBJECT_PickNpcSpeechSound's two (an NPC's speech; MonUnique.cpp's until
2026-10-07, Object.cpp's since) and UnitSnd.cpp's six -- run whenever a sound
starts. UnitSnd.cpp's, listed at its close (2026-10-07): PlayFootstep,
UpdateUnitModeSound's two, PlayAttackSound, PlayUnitSoundEvent and
PlayMonsterInitSound, reached from the per-frame unit pass, the action
dispatch, unit creation, the belt and the server's packets. What would settle it: whether any sound is started, or a
monster speaks, before the client's player unit exists.

Reproduced.

## The ring distance's negations

`SoundSong.cpp` (SOUNDSONG_GetCircularDistance). **Argued
2026-10-06** at the four sound units' close. **LATENT.**

The bytes make each difference positive with `neg` (`jns` / `neg`), which
wraps at -2^31; the C's `-` is undefined there. A difference of -2^31 needs
operands near 2^30. The three calls are UNITSND_PlayFootstep's, with an
animation's position and length (the modulus an unsigned quotient, `div
ebx`, 0x6FB50182).

Reproduced.

## A Falloff 4 sound past 2000 on an axis divides by a zero span

`SoundChan.cpp` (SOUNDCHAN_ApplyVolumeAndPosition), with
`SoundInst.cpp` (SOUNDINST_ServiceInstances) and `SoundHdr.cpp` (the two
falloff distances). **Argued 2026-10-06** at the four sound units' close,
from the lead SoundHdr.cpp batch 4 left. **NOT ESTABLISHED**
whether the game makes one; the value the division gives is the fade's own
0.

ApplyVolumeAndPosition attenuates a sound past its range as `(falloff -
distance) * volume / (falloff - range)` (`fsub` / `fimul` / `fsub` /
`fdivp`, 0x6FB52BE6..0x6FB52BF8). sounds.txt's Falloff picks the two
distances (SOUNDHDR_GetRangeParam, GetFalloffDistance), and only Falloff 4
gives one number for both, 2000: the span is 0. In the stock table that is
1723 rows, the speech block 2934..4656 whole, 1720 of them Tracking.

A sound reaches the function only from SOUNDINST_ServiceInstances -- the
start, through SOUNDCHAN_StartInstance (0x6FB5674C, then 0x6FB52737), and
state 1 (0x6FB5682D), the call graph's edges -- and both are gated by the
instance's distance squared against the falloff's (`fcomp`, 0x6FB56569 and
0x6FB56820). That square is taken from the position held to +/-2000 on
each axis (SOUNDINST_SetPosition, PlaySound, the tracking pass), and
ApplyVolumeAndPosition's distance from the position itself. The position
is a difference of client coordinates, x whole and y twice a whole
(SOUNDINST_GetUnitSoundPosition), so off the axes a held coordinate puts
the held square past 2000 squared by at least 1, and the sound is not
mixed; on an axis -- y 0 and x at least 2000 away, or x 0 and y -- it holds
at exactly 2000 squared, passes, and is mixed past 2000.

There the x87, its exceptions masked (the process default; an unmasking
was not looked for), makes the quotient an infinity, or a NaN when the
volume is 0, and __ftol (0x6FB60A20, `fistp qword ptr`) stores the integer
indefinite, 0x8000000000000000: the attenuated volume, its low dword, is 0
-- the fade's value at its far end. The sound is silent and kept: state 1
stops a channel only past the held falloff, so the instance holds its
channel until its Duration or its stream ends or its held square moves
past 2000 squared. Whether the client holds a unit speaking 2000 away on
the player's axis was not looked for.

Our C takes the 0 without dividing (SoundChan.cpp): C++ leaves the division
by zero and the conversion of an infinity or a NaN undefined.

Reproduced, the 0 by a test the bytes do not make.

## The hit-class sound delay divides by the speed sign-extended, unsigned

`UnitSnd.cpp` (UNITSND_GetHitClassSoundDelay). **Argued
2026-10-07** at UnitSnd.cpp's close. **UNREACHABLE.**

The delay is `(nHitFrame * 256 + 128) / wAnimSpeed`, the speed sign-extended
(`movsx`, 0x6FB4F55C) and divided unsigned (`div ecx`, 0x6FB4F57A), 1 for a
speed of 0: a negative speed would divide as 2^32 less its magnitude and
give 0. Its three calls are SKILLS_RunCltStFunc's (Skills.cpp), on the
skill's unit, and only a player or a monster gets there: from
PLAYER_DoClientAction, MONSTER_DoClientAction and UNITMODE_InteractWithUnit
through SKILLS_CltStartSkillAtCoords and ..OnUnit,
and from the server's replayed item casts (SKILLS_ClientCastAtTarget) only
for an ItemCltCheckStart skill -- in the stock Skills.txt Corpse Explosion
and Poison Explosion, neither of them StSoundDelay or WeaponSnd, the two
columns that call the delay.

A player's or a monster's speed is never negative. Every instruction of
D2Client and D2Common that writes memory off a register other than esp
whose bytes overlap +4Ch..+4Dh -- 19 and 30 (iced-x86, any width or
mnemonic), each placed by the type Hex-Rays gives its base -- writes another
record's member or wAnimSpeed. D2Client's three write 0
(MONSTER_AdvanceAnimForMode, 0x6FB41E38 and 0x6FB41E6B; PLAYER_AdvanceAnimMode,
0x6FB48D1E, whose bp is zeroed on every path to it). D2Common's
UNITS_SetAnimStartFrame (#10349) writes unclamped values in its object,
missile and item arms only and hands a player or a monster to
UNITS_UpdateAnimRateAndVelocity (#10376), whose eight stores are each held
to [0, 0x7FFF]; UNITS_SetAnimationSpeed (#10377) holds its speed there too,
and MISSILE_SetAnimSpeed (#11139) writes a missile only.

Reproduced.

## A unit with no room steps on the dirt take

`UnitSnd.cpp` (UNITSND_GetSurfaceMaterialUnderUnit,
GetFootstepSoundId). **Argued 2026-10-07** at UnitSnd.cpp's close. **NOT
ESTABLISHED.**

GetSurfaceMaterialUnderUnit answers a NULL room from D2COMMON_UNITS_GetRoom
as material 0 (`test esi, esi` / `jnz`, 0x6FB4FB2B, then the epilogue with
eax 0, 0x6FB4FB2F..0x6FB4FB34), not the level's default, and
GetFootstepSoundId takes 0 as it takes 1, the dirt take -- where a unit in a
room whose tile is not found takes the level's soundenviron.txt Material 1.
So a unit with no room steps on dirt on any floor. The local player cannot
get there: PLAYER_AdvanceAnimMode hands the same room to DUNGEON_UpdatePops
just before its footstep, which D2MOO asserts non-NULL (D2Dungeon.cpp:1070;
not read in D2Common's bytes). A unit left in a freed room carries dwFlags
bit 23 and UNITMODE_UpdateUnit skips it. But CUNIT_CreateMonsterUnit makes a
monster whose CUNIT_FindRoomForCoords answered NULL, and
PATH_AllocDynamicPath keeps the NULL room (D2MOO, Path.cpp:710). What would
settle it: whether the server assigns a monster where the client holds no
room, and whether the rest of MONSTER_UpdateAnimMode's frame takes it as far
as its footstep.

Reproduced.

## The wing beats test the first bird of each family by class, not by base

`UnitSnd.cpp` (UNITSND_PlayFootstep). **Argued 2026-10-07** at
UnitSnd.cpp's close. **REACHABLE.**

A monster steps only walking or running, but the foul crow at
MONMODE_NEUTRAL too and the vulture only at MONMODE_SKILL1 -- and the test
is on the CLASS, 15 and 110 (`cmp edx, 0Fh`, 0x6FB500B8; `cmp edx, 6Eh`,
0x6FB500C4). Fourteen classes of the stock MonStats.txt name those two
monsounds.txt rows as MonSound and UMonSound: foulcrow1..4 (15..18),
vulture1..4 (110..113), the eagle (429), foulcrow5..8 (590..593) and
vulture5 (608), every BaseId but the eagle's foulcrow1 or vulture1 -- the
base Monster.cpp's own vulture tests compare. Levels.txt's mon, nmon and
umon columns place eight of the other twelve -- foulcrow2..5 and vulture2..5
-- and each has NU and WL in MonStats2.txt, the vultures S1 as well.
MONSTER_UpdateAnimMode calls PlayFootstep every frame (Monster.cpp), so a
foulcrow2..5 hovering in NU is silent where foulcrow1 beats its wings, and
vulture2..5 beat theirs walking and are silent in S1, the reverse of
vulture1. The step is the rows' FootstepLayer, hawk_wing_1 and
vulturedemon_wing_1, their Footstep being 0.

Reproduced.

## PlayHitOrDeathSound reads a NULL unit's hit class, which its caller never passes

`UnitSnd.cpp` (UNITSND_PlayHitOrDeathSound). **Argued 2026-10-07**
at UnitSnd.cpp's close. **UNREACHABLE.**

The inlined type copy tests the unit -- 6 for NULL -- and dwLastHitClass is
then read through it with no test (`mov ebp, [esi+0B0h]`, 0x6FB511A3), a
fault at 0xB0. The one caller, UNITSND_PlayModeSound, calls it only in its
UNIT_PLAYER and UNIT_MONSTER arms, on the type UNITSND_GetSoundUnitType
gives the same unit: 6 for NULL.

Reproduced.

## The area-entry walk is bounded four slots past its table

`UnitSnd.cpp` (UNITSND_PlayAreaEntryQuestLine). **Argued
2026-10-07** at UnitSnd.cpp's close. **UNREACHABLE.**

The walk over gpAreaEntryQuestLines (0x6FB964E8, sixteen pointers) stops at
a NULL pointer or at 0x6FB96538 (`cmp edi, 6FB96538h` / `jl`, 0x6FB52103) --
twenty pointers past the head, the last four the first sixteen bytes of an
assert string. Slots 14 and 15 are NULL in the image and nothing writes the
table: its one reference is the walk's own `mov edi, offset
gpAreaEntryQuestLines` (0x6FB520E2), none inside it and none indexed. Every walk ends at slot 14 at the latest, and the
bound never stops one.

Reproduced.

## The game-data table's bucket array is used unchecked when Storm allocates nothing

`Game.cpp` (gGameData'S CONSTRUCTION; the original's
CRT_LISTARRAY_SetCount and CRT_GAMEDATA_ConstructTable). **Argued
2026-10-08** at the globals plan's unit 14, part B. **LATENT.**

CRT_LISTARRAY_SetCount (0x6FAAD090) takes the bucket array from
STORM_SMemReAlloc(NULL, 48, the tag, -2, 0x10) (0x6FAAD13B) and, if that
answers NULL, from STORM_SMemAlloc(48, the tag, -2, 0) (0x6FAAD150) --
the first answer tested only to take the fallback (`test eax, eax` / `jnz`,
0x6FAAD140), the fallback's not at all: CRT_LIST_ConstructChecked's null
test (0x6FAAD210) passes over the list at address 0, and the next, at 0xC,
faults on its first store, to 0x10 (0x6FAAD217). Storm 1.10's SMemReAlloc
hands a NULL pointer straight to SMemAlloc (`test ebx, ebx` / `je`, Storm
0x6FFCCE71; the call at 0x6FFCCEFF), which answers NULL at least when the
dword at Storm 0x6FFEC4A8 is zero (Storm 0x6FFCB130..0x6FFCB14F, after
reporting 0x8510007D) -- a state of Storm's whose meaning was not
established. This runs once, in D2Client's static initialisation; whether
that dword can be zero then, or Storm's allocator fail otherwise, was not
looked for -- a failure at load, if it is one, not in play.

Declared, not reproduced: what a store through NULL does is undefined in
C++. Ours, built by Game.cpp's sgGameDataLifetime, tests the two answers as
the original does -- the first to take the fallback, the fallback's not at
all.

## `RECORD_Play` takes a negative record count from the file as a byte count

`0x6FAAEA18` and `0x6FAAEA60`, `Record.cpp`. **Argued 2026-10-09** at the
globals plan's batch 2, unit 17 (Record.h). **LATENT.**

    6FAAEA0E  mov  eax, gRecordFileHeader.nRecords   ; 0x6FBA9A28, the file's own
    6FAAEA13  cmp  eax, 1F4h
    6FAAEA18  jg   short 6FAAEA4A                    ; SIGNED: a negative count passes
    6FAAEA60  shl  eax, 4
    6FAAEA66  call ebp                               ; ReadFile(hFile, gpRecordBlock, eax, ...)

The clamp to 500 records is a signed compare, so a negative nRecords is
neither clamped nor refused: the `<= 500` arm stores it as the first
holder's count (0x6FAAEA25), and when the file has one holder -- with more,
the assert at 0x6FAAEA2C stops the game on the first pass -- ReadFile is
asked for `(DWORD)nRecords << 4` bytes into gRecordHolderFirst, which holds
8012. The shift wraps: a negative count asks for any multiple of 16 from 0
to 0xFFFFFFF0, more than 8012 for most of them.

**How it is reached.** Only by a Record.dr1 the game did not write.
RECORD_SaveToFile writes nRecords as the last holder's count plus 500 for
each holder before it (0x6FAAE86D..0x6FAAE87C), a holder's count is 0..500,
so a file it wrote never holds a negative one; `-p` reads whatever file is
there. What ReadFile does with a request longer than the buffer -- whether
it writes past the holder, into the four flags at 0x6FBAB984..0x6FBAB993
and beyond, as far as the file's remaining bytes go -- was not looked for.

Reproduced as far as the code goes: ours computes the same `(DWORD)nEntries
<< 4` and hands it to the same ReadFile (Record.cpp, RECORD_Play). What an
overrun would hit is not: the original's runs into the flags after
gRecordHolderFirst, ours into whatever the linker put after Record.h's
object -- declared, not reproduced.

## Three unbounded string copies into the label buffers, each with a margin

`ui_panels.cpp` (UIPANELS_BuildMonsterImmunityLabel, UIPANELS_SetTransientText,
UIPANELS_SetUnitLabelText). **Argued 2026-10-13**. **LATENT.**

    6FB21654  mov  ecx, 6FBBA4A0h      ; gwszUnitLabelLine2: 0x200 bytes to gnUnitLabelLine2Color
    6FB21659  call ds:[6FB6DAA4h]      ; D2Lang's Unicode::strcpy
    6FB205A2  mov  ecx, 6FBBA1F0h      ; gwszTransientText: 0xA0 bytes to gnTransientTextX
    6FB205A7  call ds:[6FB6DAA4h]
    6FB205E2  mov  ecx, 6FBB9FE0h      ; gwszDrawTextScratch: 0x200 bytes to gnUnitLabelColor
    6FB205E7  call ds:[6FB6DAA4h]

No length is tested before any of the three, so what bounds each copy is the strings the game hands it, all from
the .tbl files. Measured over every table the stock install holds (each string looked up by its ENG KEY -- the non-English string.tbl files on disk lay their
indices out otherwise, 3994 is `strchratr` in DEU and FRA, not `colorcode` -- and a value's byte length taken as the
bound of its Unicode length):

* **the immunity line** (256 Unicodes): six names, each behind `colorcode` and a digit, joined by `KeyComma` and
  `space` -- at most 176 bytes (POL, whose disk holds patchstring.tbl alone, so its code and separators counted 0;
  204 with ENG's six codes and five separators), FRA 159, DEU 147, ENG 130. The 256-Unicode local it is built in has the same bound.
* **the hireling's name** (80): the monstats NameStr of the three hireling classes, a name of hireling.txt's
  NameFirst..NameLast (150 indices), or 11021 -- at most 24 (JPN). The id is the server's: D2MOO's D2Game picks it in
  that range (SUnitNpc.cpp:69-76) and puts a saved one back into it (PlrSave2.cpp:1225-1228); the client takes it
  unbounded (CUnit.cpp:1161, RosterPets.cpp:250, to Monster.cpp:3838-3842), so only a server other than the stock
  one could hand a longer string.
* **the object label** (256): `DemonID` or `UndeadDescriptX`, `space` and a monstats DescStr (24 keys), or the
  DescStr alone -- two arms of ours, ui_panels.cpp:1688 and 1692, one call in the bytes (0x6FB21097) -- at most
  52 (ESP).

Not measured: RUS, POR and SIN have no string tables in this install (data/local/lng holds none of theirs; each
Language_<x>.mpq is D2SE's 2911-byte switch, its largest file 986 bytes), POL has no string.tbl, only ENG has an
expansionstring.tbl, and ESP, JPN and KOR lack one or two of the keys. The smallest margin measured is 51 Unicodes.

Reproduced: ours copies with the same unbounded strcpy into objects of the same sizes (ui.h). An overrun would run,
in the original, into the global after the buffer, in ours past our object -- declared, not reproduced; no table of
the stock install reaches either.

## The vendor page never reaches 4 where a tab is selected

`inv.cpp` (INV_GetVendorPageInventory). **Argued 2026-10-13**. **UNREACHABLE.**

    6FAE8F99  cmp  bl, 5 / jb             ; the page in cl kept below 5
    6FAE908D  inc  dl                      ; the wrap from an empty page...
    6FAE9090  cmp  dl, cl                  ; ...against gnVendorMaxPage (0x6FBB5908)
    6FAE9092  mov  gnVendorPage, dl
    6FAE9098  ja   6FAE909F                ; above it: 0 (0x6FAE90A1)
    6FAE90D2  mov  dword ptr [edx*2+6FB7956Ah], 1   ; edx = page * 9: gVendorTabs[page].bSelected

The table is four 18-byte records (0x6FB79560); a page of 4 would write past it -- into gnNpcShopTabCount's upper
half in the original, past our object in ours. The page selected is the caller's when its bucket is not empty, else
one the wrap reaches, which is at most gnVendorMaxPage. Of the three callers, two pass
gnVendorPage (loaded at 0x6FADEB3B and 0x6FAE93D3); the third, INV_NpcShopOnLButtonDown's call at 0x6FAEA593, the
tab clicked, a constant 0, 1, 2 or 3 in cl (0x6FAEA4D6, 0x6FAEA513, 0x6FAEA545, 0x6FAEA587).
Every write of the two globals:

* **gnVendorMaxPage**: 1 for a gambling vendor (0x6FAE91BD, `bl` from 0x6FAE916F), 3 at 0x6FAE931A and 0x6FAE934E,
  all in INV_NpcShopBuildTabs; read by INV_GetVendorPageInventory and INV_NpcShopOnLButtonUp. Its initial value is
  0: 0x6FBB5908 is past .data's raw data (0x6FB9B000), zero-filled at load.
* **gnVendorPage** (0x6FBB5900, zero-filled the same way): the constants 0, 1, 2, 3; INV_ResetPanelState's 0
  (`xor ebx, ebx`, 0x6FAE9101); INV_GetVendorPageInventory's own page (0x6FAE9052, 0x6FAE9092, 0x6FAE90A1), which
  holds max + 1 only between 0x6FAE9092 and the reset at 0x6FAE90A1, where nothing else runs; and **two 4s**, each
  between a save and a restore: 0x6FADF379..0x6FADF392 in INV_UpdateAndDrawPanels around CUNIT_GetPlayer and
  INV_DrawInventoryGridItems, and 0x6FAE8619..0x6FAE867A in INV_OnMouseMove around
  D2COMMON_INVENTORY_GetCursorItem, GAME_IsExpansionGame and INV_PixelToGridCell -- both arms after the 4 `jmp` to
  the restore. Neither window reaches a caller: the call paths from each of INV_DrawInventoryGridItems,
  CUNIT_GetPlayer and INV_PixelToGridCell, depth 12, code edges, names none of INV_GetVendorPageInventory and its
  three callers (its only indirect calls are the runtime's).

So the page passed in is 0..3 at every call, the wrap ends at most at 3, and the tab
written is [0]..[3]. The bucket of inventory page 4 is counted (`cmp al, 5`, 0x6FAE901B) and never selected.
gnVendorTabCount, the bound of the tab loops, is 4 in the image (0x6FB795A8) and written by nothing. Reproduced; a
note beside the write.

## The unit tables' row is taken unchecked

`CUnit.cpp` (CUNIT_FindUnitByGuidAndType, CUNIT_FindUnitByClientIdAndType, CUNIT_RemoveUnitById,
CUNIT_RemoveUnitByClientId, CUNIT_RegisterUnit and the two it alone calls). **Argued 2026-10-13**.
**NOT ESTABLISHED** for the stock server, and a **DECLARED DIFFERENCE**.

    6FB269FE  shl  ecx, 7                 ; row = nUnitType * 128, no bound
    6FB26A03  mov  esi, gpUnitTableById[eax*4]

gpUnitTableById and gpUnitTableByClientId are six rows of 128 (0x6FBBAA00, 0x6FBBB600); a type of 6 or more reads
the .data after them -- row 6 of the first is the second's row 0. Every call of the five was classified
(240 calls, the 95 whose type is not a constant traced one by one):

* **a constant**: 138 calls of UNIT_PLAYER..UNIT_TILE; 7 of UNIT_TYPES_COUNT (6) with id -1, every one in a dead
  NULL arm -- Missile.cpp's two (this file, "the NULL missiles"), Item.cpp's four and Player.cpp's one after a
  pointer that D2Common #10255 never empties (`mov eax, [ebp] / test eax, eax / jne`, D2Common 0x6FD8FB4B..52) or
  that the code dereferenced first.
* **a server packet field**, uint8, bounded by no handler: 0x0A, 0x0B, 0x11, 0x15, 0x16, 0x21, 0x26, 0x28, 0x2C,
  0x42, 0x48, 0x73, 0x76, 0x7C, 0x7D, 0x8A, 0x92, 0x99, 0x9A, 0x9D (ten item messages), 0xA3, 0xA5, 0xA7..0xAB, and
  the action arrays rows 0x0C..0x10 and 0x67..0x6D fill. One handler bounds it (`nUnitType < 6`, 0x62,
  npcmenu.cpp:4274); 0x28 refuses 6 alone.
* **a unit's own type, or its owner's**, copied into lights, paths, skill parameters and the queued command: set at
  creation from a constant, from a stock DS1 preset -- types 1 and 2 only, over all 2276 files of the install
  (parsed as D2MOO's DRLGPRESET_ParseDS1File) -- or from packet 0x09 or 0x51's type, which
  CUNIT_CreateUnit and CUNIT_CreateObjectUnit hand to D2Common's UNITS_AllocUnit unchecked. STATES_SetFunc4's
  source-unit type is bounded (`>= UNIT_TYPES_COUNT`, States.cpp:209).
* **the router's byte past a one-byte packet** (the entry "The router reads a unit header off five packets that are
  one byte long", and its 2026-10-13 paragraph): the one path that needs no faulty server.

**Declared, since 2026-10-13**: a type of 6 or more finds nothing, removes nothing, and a unit
of one is refused by CUNIT_RegisterUnit as a second failed add is (destroyed, NULL). The original walks whatever its
.data holds past the six rows; ours has no seventh row to walk. GAME_UpdateFrame's packet buffer is zeroed for the
same path (Game.cpp).

## The act COF table is indexed by the server's act byte

`Gfx.cpp` (GFX_LoadActCofData). **Argued 2026-10-13**. **LATENT.**

    6FB5C9E6  and  ecx, 0FFh                  ; the act, widened, no bound
    6FB5C9EC  mov  eax, gActCofDataFiles[ecx*4] ; five names, acts I..V

Two callers, both setting cl alone. SCMD_HandleSrv7E's act is D2Common's DRLG_GetActNoFromLevelId for the player's
room's level: levels.txt's `Act` column, which holds 0..4 in every one of the 133 stock rows with an Id (0: 40,
1: 35, 2: 28, 3: 6, 4: 24). GAME_PreloadActData's act is packet 0x03's byte,
which D2MOO's D2Game sends from D2GAME_PACKETS_SendPacket0x03 with the act it also uses to index its own five acts
(`DUNGEON_GetTownLevelIdFromAct(pGame->pAct[nAct])`, Level.cpp:219). So only a server other than the stock one could
hand 5 or more -- after D2Common's DUNGEON_AllocAct has taken the same act first. Reproduced unbounded; past the five
in ours is undefined, the original reading its .data on -- declared, not reproduced.

## How to add an entry

1. **Measure it.** The instruction and its address, or the tool run, goes in the
   entry. A verdict with no measurement does not go in.
2. **Argue the verdict separately from the defect.** "The code is wrong" and
   "the game can hit it" are two claims and the second is usually the harder one.
   If it will not settle, say NOT ESTABLISHED and say what would settle it.
3. **Check whether it belongs in REFUTED instead.** Two of the entries here
   started life in another section.
4. **Reproduce it in the `.cpp` files** and write the reading beside the code.
   This file is the index; the source carries the argument.

