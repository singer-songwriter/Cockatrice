# Cockatrice Dev Notes

## Architecture: Client / Server / Protocol

This is a fork of Cockatrice. UI and widget changes in `cockatrice/src/` are local-only and invisible to other players and the server.

### The protocol layer

All game messages are defined as Protobuf messages in `libcockatrice_protocol/libcockatrice/protocol/pb/` (159 `.proto` files). This is the wire format shared between all clients and the server. As long as these files are unchanged, this fork connects to any standard Servatrice instance and interoperates with official clients.

### The server is a relay, not a rules engine

Servatrice (`servatrice/`) does not enforce MTG rules. It validates basic ownership/zone constraints, then relays commands and broadcasts events to all players. There is no central authority enforcing game logic.

### Adding new game behavior

Whether all players need to run this fork depends on what the new behavior does:

| Change type | Who needs the fork |
|---|---|
| UI / rendering only | Just you |
| Server enforces outcome using existing messages (`MoveCard`, `CreateToken`, `IncCounter`, etc.) | Just the server |
| New interactive behavior requiring a new message type | All players |

**Key question:** can the server express the new behavior's outcome entirely through messages that already exist? If yes, official clients handle it transparently.

### Running a private server

`docker-compose.yml` is present at the repo root. Pointing clients at a private Servatrice instance is a config change only. Adding new `.proto` messages uses Protobuf field numbering, so existing messages are not broken.

### Version handshake

The server checks client version on connect. Many servers are lenient, but a significantly diverged build may warn or be rejected. This is independent of the wire protocol compatibility.
