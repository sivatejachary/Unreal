# INDIA UNLEASHED – Alpha 0.1 QA Report & Automated Test Log

## 1. Build & Compilation Status
- **C++20 Compiler**: `g++ (Debian 12.2.0-14+deb12u1) 12.2.0` with `-std=c++20 -O2 -Wall -Wextra -Werror`
- **Translation Units Compiled**: 17 `.cpp` files across all 7 modules (`IndiaUnleashed`, `IUOnline`, `IUWorld`, `IUVehicles`, `IUCombat`, `IUMissions`, `IUBattleRoyale`)
- **Compilation Warnings / Errors**: `0 warnings, 0 errors`
- **FastAPI Backend Test Runner**: `Python 3.11.2` (`unittest` + `fastapi.testclient.TestClient`)

## 2. Executed Test Matrix (19 / 19 Total Automated Tests Passed)

| Test ID | Suite | Target Requirement (Section 16) | Status | Verified Metrics & Outcome |
| :--- | :--- | :--- | :--- | :--- |
| `CPP-01` | `Test01_CharacterMovement` | Character movement, swimming & camera | **PASS** | Jog (`500 cm/s`), Sprint (`760 cm/s`), Stamina drain (`14/s`), Crouch (`260 cm/s`), Jump (`500 cm/s Z`), Swim transition at `115 cm` depth, Sprint FOV interpolation (`85° -> 96°`), Speed-hack velocity rejection |
| `CPP-02` | `Test02_CharacterReplication` | Character replication & authority | **PASS** | 6 replicated properties registered; non-authority `ROLE_SimulatedProxy` damage calls blocked |
| `CPP-03` | `Test03_PlayerStateReplication` | PlayerState replication & anti-tamper | **PASS** | 11 replicated fields; client currency/XP calls rejected; server INR, XP, Level, and 2-Star Wanted heat verified |
| `CPP-04` | `Test04_SessionCreation` | Session creation (Listen & Dedicated) | **PASS** | 2-player Listen Server (`1/2` initial) and 32-player Dedicated Server (`0/32` initial) created |
| `CPP-05` | `Test05_SessionJoining` | Session joining & capacity limits | **PASS** | Session search found open match; Player 2 joined (`2/2`); Player 3 rejected (`false`) when full |
| `CPP-06` | `Test06_PlayerDisconnectAndReconnect` | Player disconnect & state restore | **PASS** | Player 2 disconnect released pawn and cached state; reconnect restored exact Laad Bazaar coordinates (`-8500, 200, 110`) and `9,000 INR` |
| `CPP-07` | `Test07_VehicleSpawning` | Vehicle spawning & presets | **PASS** | Verified Auto-Rickshaw (`68 km/h`), Sport Motorcycle (`210 km/h`), Sedan (`185 km/h`), Bus (`2,800 HP`), Boat (`95 km/h`) |
| `CPP-08` | `Test08_VehiclePossession` | Vehicle possession, physics & exit | **PASS** | Entry > `3.8m` rejected; entry at `2.0m` possessed vehicle, switched to `IMC_Vehicle_Driving`, simulated throttle/fuel, and exited cleanly |
| `CPP-09` | `Test09_DamageValidation` | Server combat & damage validation | **PASS** | Valid headshot applied `2x` damage with `30%` armor absorption; rapid-fire (`10ms`) and out-of-range (`300m > 180m`) exploits rejected |
| `CPP-10` | `Test10_InventoryValidation` | Weapon inventory & weight validation | **PASS** | 3-slot cap enforced; duplicate weapon ID rejected; active weapon slot switching verified |
| `CPP-11` | `Test11_MissionProgression` | Mission progression (`M01_OldCityArrival`) | **PASS** | Completed all 3 stages (Charminar -> Laad Bazaar -> Tank Bund); server paid `15,000 INR` and `1,500 XP` |
| `CPP-12` | `Test12_SaveAndLoad` | Save/Load & integrity checksum | **PASS** | Saved & restored progression from disk; hex-edited save file (`42500 -> 99999`) rejected by FNV-1a 64-bit checksum |
| `CPP-13` | `Test13_AndroidAndHyderabadWorldConfig` | Android config & Hyderabad blockout | **PASS** | Verified Charminar `56m` blockout (18 primitives), 18 POIs across 6 districts, NPC distance LOD tiers, BR SafeZone shrink, and `AndroidEngine.ini` |
| `API-01` | `test_01_health_and_auth_login` | Backend Auth & Profile initialization | **PASS** | `/health` 200 OK; `/api/v1/auth/login` issued session token and initialized `5,000 INR` profile |
| `API-02` | `test_02_client_cannot_self_award` | Backend Anti-Cheat (`Rule 10`) | **PASS** | Forged client request to `/api/v1/missions/complete` without HMAC signature rejected with `HTTP 403 Forbidden` |
| `API-03` | `test_03_authoritative_mission_reward` | Server-signed mission reward payout | **PASS** | HMAC-signed server request granted `25,000 INR`, `1,500 XP` (Level 2), and `WPN_AR_VAJRA` |
| `API-04` | `test_04_vehicle_purchase_and_garage` | Garage vehicle purchase transaction | **PASS** | Deducted `18,000 INR` atomically and added `VH_DECCAN_SEDAN_GT` to `owned_vehicles` |
| `API-05` | `test_05_authoritative_combat_kill` | Server combat kill & leaderboard | **PASS** | Recorded kill, death, BR victory bonus, and returned sorted global leaderboard |
| `API-06` | `test_06_parties_and_matchmaking` | Party creation & server allocation | **PASS** | Created 4-player party and allocated `ap-south-hyderabad` server instance on `L_Hyderabad_Prototype` |

## 3. Known Limitations & External Dependencies
1. **Unreal Editor Binary Cooking**: Cooking binary `.uasset` and `.umap` packages and compiling `Win64` / `Android_ASTC` binaries requires a machine with **Unreal Engine 5.4 Editor**, **Visual Studio 2022 (MSVC)**, and **Android Studio NDK r25b** installed. Run `Scripts/BuildHyderabadPrototypeInEditor.py` inside Unreal Editor to materialize `L_Hyderabad_Prototype.umap` from the C++ classes.
2. **3D Art & Audio Assets**: Alpha 0.1 uses procedural architectural box/arch geometry (`AIUCharminarBlockoutActor`) for Charminar and data-driven physics presets for vehicles. Production high-poly photogrammetry meshes, skeletal meshes, and engine/ambient audio banks are scheduled for Phase 2 and Phase 3.
3. **Physical Android Device Profiling**: `Android_Low`, `Android_Mid`, and `Android_High` device profiles are configured in `Config/DefaultDeviceProfiles.ini`, but GPU frame-time profiling via Unreal Insights requires deployment to physical Adreno/Mali Android hardware in Phase 6.
