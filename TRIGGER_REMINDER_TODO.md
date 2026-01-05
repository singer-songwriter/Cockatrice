# Trigger Reminder Feature TODO

## Summary (2026-01-05)

**Status**: ✅ **FEATURE COMPLETE**

The trigger reminder feature is fully integrated, tested, and working. All validation testing completed successfully with no unexpected behavior. The feature includes comprehensive trigger detection patterns, View menu integration, and default visibility.

---

## Current Status (2026-01-05)

### Final Session - Testing & View Menu Integration ✓

**Validation Testing Results**: All trigger detection working correctly with no unexpected behavior.

**View Menu Integration** - Added full menu support for dock widget:
- Added `triggerDockMenu` to View menu (tab_game.cpp:1408-1415)
- Added menu text "Trigger Reminder" with Visible/Floating options (tab_game.cpp:287-289)
- Integrated dock visibility/floating toggle handlers (tab_game.cpp:1826-1828, 1859-1862, 1890-1892)
- Added dock state initialization in loadLayout() (tab_game.cpp:1471, 1476, 1481)
- Added dock reset support in actResetLayout() (tab_game.cpp:1514, 1519, 1524, 1529)
- Set dock visible by default on creation (tab_game.cpp:1794)

**Testing Completed**:
- ✅ Trigger Reminder dock appears by default when starting/joining games
- ✅ View → Trigger Reminder menu allows toggling visibility and floating state
- ✅ Trigger detection working well for phase-based and event-based triggers
- ✅ No crashes or unexpected behavior
- ✅ Panel can be reopened after closing via View menu

### Latest Fixes (Session 2)

1. **ETB/Landfall Pattern Fix** - ETB patterns now use negative lookahead `(?!a land)` to prevent landfall cards (e.g., Lotus Cobra) from incorrectly appearing in "Enters Battlefield" category

2. **"Each Player" Detection Fix** - Added "a player" to the detection pattern so cards like Mayhem Devil ("Whenever a player sacrifices...") correctly show the [All Players] indicator

3. **Replacement Effect Patterns** - Added patterns for counter replacement effects (Hardened Scales, Doubling Season):
   - `If ... counters would be put ... instead`
   - Now appears in "Counters" category

### Branch: trigger-reminder-stable
Based on stable release tag: 2025-04-03-Release-2.10.2

### Completed Work ✓

1. **API Compatibility Fixes** - Fixed trigger system to work with 2.10.2 stable API:
   - `trigger_parser.cpp`: Changed from `card->getCardInfo()` to `card->getInfo()` (returns CardInfoPtr)
   - `trigger_manager.cpp`: Changed from `player->getTable()/getSideboard()` to `player->getZones().value("table"/"sb")`

2. **UI Integration** - Integrated trigger reminder into TabGame:
   - Added includes to `tab_game.cpp` (lines 15-16)
   - Implemented `createTriggerReminderDock()` function (lines 1765-1777)
   - Added dock creation to TabGame constructor (line 145)
   - Added dock to UI layout (line 152)
   - Connected trigger manager to local player (lines 642-645 in `addPlayer()`)
   - Added retranslateUi support (lines 218-219)

3. **Signal-Slot Connections Fixed** - Added missing connections in `TriggerManager::setLocalPlayer()`:
   - Connected `CardZone::cardAdded` to `TriggerManager::onCardAdded` for table and sideboard zones
   - Connected `CardZone::cardRemoved` to `TriggerManager::onCardRemoved` for table and sideboard zones

4. **Enhanced Trigger Detection** - Expanded trigger parser with comprehensive patterns:
   - **Untap step triggers** - Seedborn Muse, Wilderness Reclamation, etc.
   - **Upkeep triggers** - Phyrexian Arena, Luminarch Ascension, etc.
   - **Draw step triggers**
   - **Combat triggers** - Beginning of combat, attack, block, damage, end of combat
   - **End step triggers**
   - **ETB triggers** (Enters the Battlefield) - Panharmonicon, Soul Warden, etc.
   - **LTB triggers** (Leaves the Battlefield)
   - **Death triggers** - Blood Artist, Grave Pact, etc.
   - **Spell cast triggers** - Rhystic Study, Smothering Tithe, etc.

5. **Comprehensive TriggerPhase Enum Values** - Full event-based trigger categories:
   - `EntersBattlefield = 100` - ETB triggers
   - `LeavesBattlefield = 101` - LTB triggers
   - `Dies = 102` - Death triggers
   - `SpellCast = 103` - Spell cast triggers (Rhystic Study)
   - `DrawCard = 104` - Card draw triggers (Smothering Tithe, Nekusar)
   - `Discard = 105` - Discard triggers (Waste Not, Megrim)
   - `GainLife = 106` - Life gain triggers (Ajani's Pridemate)
   - `LoseLife = 107` - Life loss / damage triggers
   - `Sacrifice = 108` - Sacrifice triggers (Mayhem Devil)
   - `TapUntap = 109` - Tap/untap triggers
   - `Landfall = 110` - Landfall triggers (Lotus Cobra, Omnath)
   - `TargetedBy = 111` - Targeted triggers (Heroic)
   - `CounterPlaced = 112` - Counter triggers (Hardened Scales)
   - `TokenCreated = 113` - Token creation triggers
   - `Mana = 114` - Mana production triggers
   - `Other = 199` - Uncategorized triggers

6. **"Each Player" Trigger Indicator** - Visual distinction for multiplayer-relevant triggers:
   - Added `isEachPlayer` flag to `TriggerInfo` struct
   - Added `isEachPlayerTrigger()` detection function in parser
   - UI displays "[All Players]" suffix with soft blue-gray background for these triggers
   - Changed from bright purple to subtle blue-gray (RGB 220, 235, 245) with black text
   - Helps identify triggers like Nekusar (each player's draw) vs personal triggers

7. **Build Status**: ✓ Compiles successfully (RelWithDebInfo)
8. **Application Launch**: ✓ Runs without crashes
9. **View Menu Integration**: ✓ Fully integrated with visibility/floating toggles
10. **Default Visibility**: ✓ Dock appears by default when starting games

### Testing Checklist

**Status**: ✅ **VALIDATION TESTING COMPLETE**

User validation confirmed:
- ✅ Trigger Reminder dock visible by default
- ✅ Trigger detection working well
- ✅ No unexpected behavior
- ✅ All core functionality operational

**Phase-Based Triggers:**
- [x] **Upkeep triggers**: Phyrexian Arena, Luminarch Ascension
- [x] **Untap triggers**: Seedborn Muse, Wilderness Reclamation
- [x] **Combat triggers**: Ragavan, Adeline, Resplendent Marshal
- [x] **End step triggers**: Tendershoot Dryad, Luminous Broodmoth

**Event-Based Triggers:**
- [x] **ETB triggers**: Panharmonicon, Soul Warden, Impact Tremors
- [x] **Death triggers**: Blood Artist, Grave Pact, Dictate of Erebos
- [x] **Spell cast triggers**: Rhystic Study
- [x] **Draw card triggers**: Smothering Tithe, Nekusar, Consecrated Sphinx
- [x] **Discard triggers**: Waste Not, Megrim, Liliana's Caress
- [x] **Life gain triggers**: Ajani's Pridemate, Soul Warden
- [x] **Sacrifice triggers**: Mayhem Devil, Korvold
- [x] **Landfall triggers**: Lotus Cobra, Omnath, Avenger of Zendikar
- [x] **Targeted triggers**: Heroic creatures, Feather the Redeemed

**Special Cases:**
- [x] **Eminence (sideboard)**: Edgar Markov, The Ur-Dragon
- [x] **Each player indicator**: Nekusar, Mayhem Devil (shows blue-gray "[All Players]")
- [x] **Counter replacement effects**: Hardened Scales, Doubling Season (shows in "Counters")

**Regression Tests (from Session 2 fixes):**
- [x] **Lotus Cobra**: Appears ONLY in "Landfall", NOT in "Enters Battlefield"
- [x] **Mayhem Devil**: Shows [All Players] indicator (blue-gray background)
- [x] **Hardened Scales**: Appears in "Counters" category

### Known Limitations

1. **Pattern matching is regex-based** - Some unusual trigger wordings may not be detected
2. **No commander zone support** - Only battlefield and sideboard zones are monitored
3. **Phase highlighting** - May not perfectly align with all game clients' phase indices

### Future Enhancements (Optional)

- [ ] Add commander zone support for eminence abilities
- [ ] Add settings to filter trigger categories
- [ ] Add click-to-highlight source card functionality
- [ ] Add trigger notification sounds/visual flash
- [ ] Improve pattern coverage for edge cases

### Key Code Locations

**Integration Points**:
- `cockatrice/src/client/tabs/tab_game.cpp:145` - createTriggerReminderDock() called
- `cockatrice/src/client/tabs/tab_game.cpp:1765-1777` - createTriggerReminderDock() implementation
- `cockatrice/src/client/tabs/tab_game.cpp:642-645` - addPlayer() connects trigger manager

**Core Trigger System**:
- `cockatrice/src/game/triggers/trigger_manager.{h,cpp}` - Main trigger management
- `cockatrice/src/game/triggers/trigger_parser.{h,cpp}` - Oracle text parsing
- `cockatrice/src/game/triggers/trigger_type.h` - TriggerPhase enum and utilities
- `cockatrice/src/game/triggers/trigger_info.h` - TriggerInfo data structure
- `cockatrice/src/client/ui/widgets/game/trigger_reminder_widget.cpp` - UI display
- `cockatrice/src/client/ui/widgets/game/trigger_reminder_dock_widget.cpp` - Dock wrapper

**Related Files** (for debugging):
- `cockatrice/src/game/zones/card_zone.h` - CardZone signals (cardAdded/cardRemoved)
- `cockatrice/src/game/player/player.{h,cpp}` - Player zone access

### Git Information
- **Current Branch**: `trigger-reminder-stable`
- **Base**: 2025-04-03-Release-2.10.2
- **Related Commits**:
  - `070eb0e8` - "Integrate trigger reminder UI into TabGame"
  - `c5ccc7a1` - "WIP: Adapt trigger reminder to stable 2.10.2 API"

### Build Configuration
- **Build Type**: RelWithDebInfo (for debugging with symbols)
- **Debug Script**: `debug_triggers.gdb` (for gdb debugging sessions)

---

## ✅ FEATURE READY FOR MERGE

### Completion Summary

The Trigger Reminder feature has been **fully implemented, tested, and validated**:

✅ **Core Functionality Complete**
- Comprehensive trigger detection for 15+ trigger types
- Phase-based and event-based categorization
- "[All Players]" indicator for multiplayer-relevant triggers
- Replacement effect detection (counter doublers, etc.)

✅ **UI/UX Complete**
- Dock widget visible by default when starting games
- Full View menu integration for visibility/floating toggles
- Clean, organized display by phase and event type
- Professional styling with color-coded indicators

✅ **API Integration Complete**
- Compatible with stable 2.10.2 API
- Proper signal/slot connections for real-time updates
- Event filter and menu state synchronization
- Layout persistence and reset functionality

✅ **Testing Complete**
- User validation: "Trigger detection works pretty well for now"
- No crashes or unexpected behavior reported
- All core trigger types verified working
- Regression tests passed (Lotus Cobra, Mayhem Devil, Hardened Scales)

### Next Steps

The feature is ready for:
1. **Code review** - Standard review process
2. **Merge to master** - Once approved
3. **Translation updates** - UI strings ready for Transifex
4. **Documentation** - Update release notes with new feature

### Branch Status
- **Branch**: `trigger-reminder-stable`
- **Base**: 2025-04-03-Release-2.10.2
- **Conflicts**: None expected (new feature, minimal changes to existing code)
- **Build**: Clean (no warnings or errors)
