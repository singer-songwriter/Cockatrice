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

## Translations

Translatable strings use Qt's `tr()` function. Translation files are managed via Transifex and updated automatically by CI. Don't manually update .ts files.
