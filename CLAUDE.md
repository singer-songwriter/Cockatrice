# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Cockatrice is an open-source, multiplatform application for playing tabletop card games (primarily Magic: The Gathering) over a network. It consists of three main applications:
- **Cockatrice** - Qt-based game client
- **Servatrice** - Game server with MySQL backend
- **Oracle** - Card database management tool

A **webclient** (React/TypeScript) is also in development.

## Build Commands

### Desktop Applications (C++/Qt)

```bash
# Basic build
mkdir build && cd build
cmake ..
make

# Build with server
cmake .. -DWITH_SERVER=1

# Build with tests
cmake .. -DTEST=1
make
make test

# Debug build (enables -Werror)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Install to release folder
make install
```

### Webclient

```bash
cd webclient
npm install
npm start          # Development server at localhost:3000
npm run build      # Production build
npm test           # Run tests
npm run lint       # ESLint check
npm run lint:fix   # ESLint with auto-fix
```

### Code Formatting

```bash
# Format all C++ files in a PR
./format.sh

# Format a single file
clang-format -i <filename>
```

## Architecture

### Shared Libraries (libcockatrice_*)

The codebase is organized into reusable libraries:
- **libcockatrice_protocol** - Protocol Buffers definitions (159 .proto files) for client-server communication
- **libcockatrice_card** - Card data structures and database
- **libcockatrice_deck_list** - Deck parsing and management
- **libcockatrice_filters** - Card filtering language
- **libcockatrice_network** - Network communication layer
- **libcockatrice_rng** - Random number generator (SFMT implementation)
- **libcockatrice_settings** - Settings/preferences management
- **libcockatrice_utility** - Utility functions, password hashing
- **libcockatrice_models** - Data models
- **libcockatrice_interfaces** - Abstract interfaces

### Client Structure (cockatrice/src/)

- `client/` - Network communication, settings
- `game/` - Game logic and mechanics
- `game_graphics/` - Visual rendering
- `interface/` - UI components, dialogs, widgets, themes
- `database/` - Local database operations
- `filters/` - Card filtering logic

### Protocol Buffers

Client-server communication uses Protocol Buffers. Proto files are in `libcockatrice_protocol/libcockatrice/protocol/pb/`. Changes to proto files require careful testing for backwards compatibility.

## Code Style

- **C++20** standard with Qt data structures preferred (`QString` over `std::string`, `QList` over `std::vector`)
- Use `clang-format` with the included `.clang-format` config
- 120 character line limit, 4-space indentation
- Braces on own line for functions/classes, same line for control statements
- `UpperCamelCase` for classes, `lowerCamelCase` for functions/variables
- Constructor arguments with same name as members: prefix with underscore (`_myData`)
- Pointer/reference with variable: `Foo *foo` not `Foo* foo`
- Use `nullptr`, not `NULL`
- Prefer stack allocation and references over heap allocation and pointers
- Include order: project includes first (alphabetical), then library includes (alphabetical)

## Database Migrations

When modifying the Servatrice database schema (`servatrice/servatrice.sql`):
1. Increment `cockatrice_schema_version` in servatrice.sql
2. Increment `DATABASE_SCHEMA_VERSION` in `servatrice_database_interface.h`
3. Create migration file in `servatrice/migrations/`
4. Run `servatrice/check_schema_version.sh` to verify

## Active Features in Development

### Trigger Reminder Panel (Branch: trigger-reminder-stable)

**Purpose**: Displays game phase triggers from cards on battlefield and eminence abilities from sideboard.

**Architecture**:
- `TriggerParser` - Parses card oracle text using regex patterns to identify triggers
- `TriggerManager` - Scans player zones, caches triggers, emits `triggersChanged()` signal
- `TriggerReminderWidget` - Displays triggers organized by phase (Upkeep, Draw, Combat, etc.)
- `TriggerReminderDockWidget` - QDockWidget wrapper for integration into TabGame

**Key Files**:
- `cockatrice/src/game/triggers/` - Core trigger system
- `cockatrice/src/client/ui/widgets/game/trigger_reminder_*.{h,cpp}` - UI widgets
- `cockatrice/src/client/tabs/tab_game.cpp` - Integration point (see createTriggerReminderDock)

**Important API Differences (2.10.2 vs master)**:
- ✓ Use `card->getInfo()` (returns `CardInfoPtr`) NOT `card->getCardInfo()`
- ✓ Use `player->getZones().value("table")` NOT `player->getTable()`
- ✓ Use `player->getZones().value("sb")` NOT `player->getSideboard()`
- Zone names: "table" (battlefield), "sb" (sideboard), "hand", "deck", "grave", "rfg", "stack"

**Signal Flow**:
1. CardZone emits `cardAdded(CardItem*)` / `cardRemoved(CardItem*)` when cards enter/leave
2. TriggerManager slots update cache and emit `triggersChanged()`
3. TriggerReminderWidget refreshes display from `triggerManager->getTriggersByPhase()`

**CardZone API** (important for zone change handling):
- `addCard()` - Adds card to zone, emits `cardAdded`
- `takeCard()` - Removes card for transfer to another zone, emits `cardRemoved`, returns the card
- `removeCard()` - Removes and deletes card permanently, emits `cardRemoved`

Both `takeCard()` and `removeCard()` emit `cardRemoved` so listeners are notified regardless of whether the card is being moved or deleted.

## Translations

Translatable strings use Qt's `tr()` function. Translation files are managed via Transifex and updated automatically by CI. Don't manually update .ts files.

## Troubleshooting Tips

### Pattern Matching / Regex-Based Features

When developing features that rely on pattern matching (regex, string parsing, text classification):

**Common failure modes:**
1. **Incomplete coverage** - Patterns written from memory miss real-world variations. Example: assuming "spell cast" covers all opponent triggers, missing "draws a card" triggers.
2. **Overly greedy patterns** - Broad patterns match unintended cases. Example: `Whenever [^,.]+ enters` matching both creature ETB and landfall text.
3. **Semantic edge cases** - Subtle language differences missed. Example: "a player" (any player) vs "each player" vs "an opponent" all have different semantics.
4. **Syntax assumptions** - Assuming all relevant cases use expected keywords. Example: replacement effects ("If X would... instead") vs triggers ("Whenever X...").

**Prevention strategies:**
1. **Gather real examples first** - Before writing patterns, collect 10+ real examples of the text you're trying to match. For MTG, use actual oracle text from cards.
2. **Test-driven pattern development** - Write test cases with real data before implementing patterns. Verify each pattern against known matches AND known non-matches.
3. **Add debug/trace logging** - During development, log what patterns match and what they capture. Remove or gate behind debug flag before committing.
4. **Consider exclusions upfront** - When writing a broad pattern, ask "what might this accidentally match?" and add negative lookahead/exclusions.
5. **Create structured test data** - Build a test dataset covering each category (e.g., a deck with one card per trigger type).
6. **Review domain-specific templating** - Many domains have standardized text templates (MTG oracle text, legal documents, API responses). Study the template rules.

**Debugging approach:**
1. Find a specific failing case (e.g., "Smothering Tithe not detected")
2. Get the exact input text being parsed
3. Test each pattern against that text in isolation
4. Identify whether it's a missing pattern, wrong pattern, or pattern conflict
5. Fix and add the case to your test suite to prevent regression

### Signal-Slot / Event-Driven Debugging (Qt)

When signals aren't triggering expected behavior:

1. **Verify connection exists** - Use `QObject::connect()` return value or add debug output in slots
2. **Check connection timing** - Ensure objects exist when `connect()` is called
3. **Verify signal emission** - Add debug output at emit site to confirm signal fires
4. **Check thread context** - Use `Qt::QueuedConnection` for cross-thread signals
5. **Trace the chain** - Map out: Source → Signal → Slot → Action, verify each link

### API Version Compatibility

When code works on one branch but not another:

1. **Document API differences** - Keep notes on method name changes between versions
2. **Search both codebases** - Use `git show branch:file` to compare implementations
3. **Check return types** - Same method name may return different types (raw pointer vs smart pointer)
4. **Verify zone/collection names** - String-based lookups may use different keys
