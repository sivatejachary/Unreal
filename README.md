# INDIA UNLEASHED (Unreal Engine 5.4 – Alpha 0.1 Prototype)

**INDIA UNLEASHED** is a AAA-style Indian Open-World, Multiplayer City, and Battle Royale action game set in a fictionalized, geographically recognizable **Hyderabad**.

## Quick Verification (Linux / macOS / WSL)
```bash
make test
```
- Compiles all 7 C++ game modules (`17` translation units) using `g++ -std=c++20 -O2 -Wall -Wextra -Werror` and executes all **13 C++ Automated Verification Test Suites** (`13/13 PASS`).
- Executes the **FastAPI Authoritative Backend Test Suite** (`6/6 PASS`), including cryptographic HMAC anti-cheat verification.

## Directory Structure
- `IndiaUnleashed.uproject` — Unreal Engine 5.4 project descriptor with all 7 modules and required plugins enabled.
- `Config/` — `DefaultEngine.ini`, `DefaultGame.ini`, `DefaultInput.ini`, `DefaultScalability.ini`, `DefaultDeviceProfiles.ini`, and `Android/AndroidEngine.ini`.
- `Source/` — 7 modular C++ runtime modules (`IndiaUnleashed`, `IUOnline`, `IUWorld`, `IUVehicles`, `IUCombat`, `IUMissions`, `IUBattleRoyale`) + 4 build targets (`Game`, `Editor`, `Server`, `Client`) + `Tests/IUAutomationTests.cpp`.
- `Content/` — Modular content hierarchy with `L_Hyderabad_Prototype.umap.json`, `DT_HyderabadPOIs.json`, and `IMC_Default_OnFoot.json`.
- `Scripts/BuildHyderabadPrototypeInEditor.py` — Unreal Editor Python script to automatically construct `/Game/Maps/Hyderabad/L_Hyderabad_Prototype.umap`.
- `Backend/` — FastAPI + Pydantic + SQLite/PostgreSQL + Docker authoritative backend service and automated test suite.
- `Documentation/` — Complete Architecture, Blueprint, World Design, Multiplayer, Asset Manifest, QA Report, Build Instructions, and Roadmap manuals.
