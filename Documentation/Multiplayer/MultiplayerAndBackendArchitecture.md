# Multiplayer Networking & FastAPI Authoritative Backend Architecture

## 1. Unreal Engine Authoritative Server Networking
- **Server Authority (Rules 7 & 10)**: Clients only send input intents (`Input_Move`, `ServerEnterVehicle`, `ServerFireShot`). The authoritative server validates distances, cooldowns, ammo counts, and line-of-sight before mutating `AIUCharacter::Attributes` or `AIUPlayerState`.
- **Two-Player Listen-Server Prototype (Alpha 0.1)**:
  - Host launches `L_Hyderabad_Prototype?listen` via `UIUSessionSubsystem::CreateSession(..., MaxPlayers=2, bDedicated=false)`.
  - Client 2 discovers and joins the session via `UIUSessionSubsystem::JoinSession` and spawns at `PlayerStart_Charminar_East_P2`.
- **Disconnect & Reconnect Persistence**:
  - When a player disconnects (`AIUGameMode::HandlePlayerDisconnect`), any possessed vehicle is safely released, and their exact world transform, INR balance, XP, and Wanted level are cached in `DisconnectedCache` and flushed to the FastAPI backend. Reconnecting restores their exact position in Hyderabad.

## 2. FastAPI Backend Service (`Backend/`)
- **Real-Time vs. Persistent Separation**: FastAPI handles persistent player accounts, inventory, garage ownership, mission rewards, party assembly, matchmaking queues, and leaderboards. Real-time character/vehicle movement stays on the Unreal Engine UDP `IpNetDriver` (`60 Hz` tick rate).
- **Cryptographic Anti-Cheat (`X-IU-Server-Signature`)**:
  - Endpoints that grant currency, XP, items, kills, or Battle Royale victories (`POST /api/v1/missions/complete` and `POST /api/v1/combat/record-kill`) verify an `HMAC-SHA256` signature computed over the canonical payload using `IU_SERVER_HMAC_SECRET`.
  - Any direct client request lacking a valid server HMAC signature is immediately rejected with `HTTP 403 Forbidden` (verified by `test_02_client_cannot_self_award_currency_or_xp`).
