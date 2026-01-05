# Trigger Reminder Feature TODO

## Current Status (2026-01-05)

### Branch: trigger-reminder-stable
Based on stable release tag: 2025-04-03-Release-2.10.2

### Completed Work ✓
1. **API Compatibility Fixes** - Fixed trigger system to work with 2.10.2 stable API:
   - `trigger_parser.cpp`: Changed from `card->getCardInfo()` to `card->getInfo()` (returns CardInfoPtr)
   - `trigger_manager.cpp`: Changed from `player->getTable()/getSideboard()` to `player->getZones().value("table"/"sb")`

2. **UI Integration** - Integrated trigger reminder into TabGame:
   - Added includes to `tab_game.cpp` (lines 15-16)
   - Implemented `createTriggerReminderDock()` function (lines 1756-1768)
   - Added dock creation to TabGame constructor (line 145)
   - Added dock to UI layout (line 152)
   - Connected trigger manager to local player (lines 642-645 in `addPlayer()`)
   - Added retranslateUi support (lines 218-219)

3. **Build Status**: ✓ Compiles successfully
4. **Application Launch**: ✓ Runs without crashes

### Current Issue 🐛

**Problem**: Trigger reminder dock panel appears in UI but remains empty - triggers are not populating

**Symptoms**:
- Trigger Reminders dock widget is visible in the right dock area
- Panel is empty (no triggers displayed)
- No errors during build or runtime

**Possible Causes to Investigate**:
1. **Signal connections missing**: CardZone signals (cardAdded/cardRemoved) may not be connected to TriggerManager slots
2. **Player zone initialization**: Zones might not be set up when trigger manager scans them
3. **Trigger parsing**: Parser patterns might not match card text, or cards might not have oracle text loaded
4. **UI update**: TriggerReminderWidget might not be responding to `triggersChanged()` signal
5. **Timing issue**: Trigger manager might scan zones before cards are loaded

### Next Steps for Debugging

1. **Check Signal Connections**:
   - Verify CardZone emits `cardAdded()` and `cardRemoved()` signals
   - Verify TriggerManager connects to these signals in `setLocalPlayer()`
   - File: `cockatrice/src/game/triggers/trigger_manager.cpp`

2. **Add Debug Logging**:
   ```cpp
   // In TriggerManager::scanZone()
   qDebug() << "Scanning zone:" << zone->getName() << "with" << cards.size() << "cards";

   // In TriggerParser::parseCardTriggers()
   qDebug() << "Parsing card:" << cardName << "oracle text:" << oracleText;
   ```

3. **Verify Zone Access**:
   - Check that `player->getZones()` returns valid zones when trigger manager calls `setLocalPlayer()`
   - Zones might not be initialized yet

4. **Check TriggerReminderWidget**:
   - File: `cockatrice/src/client/ui/widgets/game/trigger_reminder_widget.cpp`
   - Verify it connects to `TriggerManager::triggersChanged()` signal
   - Verify it calls `triggerManager->getTriggersByPhase()` when signal fires

5. **Test Card Loading**:
   - Add a card with a simple trigger pattern (e.g., "At the beginning of your upkeep")
   - Verify card has oracle text loaded via `card->getInfo()->getText()`

### Files Modified (Uncommitted)
- `cockatrice/src/client/tabs/tab_game.cpp` - UI integration
- `cockatrice/src/game/triggers/trigger_manager.cpp` - API fixes
- `cockatrice/src/game/triggers/trigger_parser.cpp` - API fixes

### Key Files for Debugging
- `cockatrice/src/game/triggers/trigger_manager.h` - TriggerManager class
- `cockatrice/src/game/triggers/trigger_manager.cpp` - Scanning and caching logic
- `cockatrice/src/game/triggers/trigger_parser.cpp` - Oracle text parsing
- `cockatrice/src/client/ui/widgets/game/trigger_reminder_widget.cpp` - UI display
- `cockatrice/src/client/ui/widgets/game/trigger_reminder_dock_widget.cpp` - Dock wrapper
- `cockatrice/src/game/zones/card_zone.h` - Check for cardAdded/cardRemoved signals
- `cockatrice/src/game/player/player.cpp` - Check zone initialization

### Related Commit
- `c5ccc7a1` - "WIP: Adapt trigger reminder to stable 2.10.2 API" (previous work on this feature)
