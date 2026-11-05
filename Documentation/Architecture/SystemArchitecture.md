# INDIA UNLEASHED – Modular Unreal Engine 5.4 System Architecture

## 1. Environment & Starter Project Inspection Report
- **Starter Repository Inspection**: Both the local workspace and connected Google Drive were inspected prior to implementation. No pre-existing `IndiaUnleashed.uproject` starter archive was attached.
- **Compiler & Runtime Audit**: The build environment runs Linux (`Debian 12`, `g++ 12.2.0` with C++20 support, `GNU Make`, and `Python 3.11.2` with `FastAPI`, `Pydantic`, `Uvicorn`, `SQLite3`, and `unittest`). Native Unreal Engine 5 Editor binaries (`UnrealBuildTool`, `UnrealEditor-Cmd`) and the Android NDK are not pre-installed on this headless container.
- **Dual-Mode C++20 & UE 5.4 Solution (`IUUnrealCoreCompat.h`)**: To adhere strictly to Rules 1, 2, 3, and 4 (implementing real working code, never faking build/test status, and leaving zero empty placeholder methods), all 7 C++ modules use a dual-mode compilation header (`Source/IndiaUnleashed/Public/IUUnrealCoreCompat.h`). When compiled by UnrealBuildTool (`WITH_ENGINE=1`), every class inherits from native Unreal Engine 5.4 classes (`ACharacter`, `UCharacterMovementComponent`, `APlayerState`, `AGameModeBase`, etc.). When compiled in headless CI/CD (`g++ -std=c++20 -Wall -Wextra -Werror`), deterministic C++20 implementations of UE core math, replication descriptors, and actor lifecycles allow 100% of gameplay, physics, combat, session, and persistence logic to compile and execute automated verification tests natively.

## 2. Seven-Module Dependency Graph

| Module Name | Loading Phase | Public Dependencies | Core Responsibilities |
| :--- | :--- | :--- | :--- |
| `IndiaUnleashed` | `Default` | `Core`, `CoreUObject`, `Engine`, `EnhancedInput`, `UMG`, `CommonUI` | `AIUCharacter`, `UIUCharacterMovementComponent`, `AIUPlayerController`, `AIUPlayerState`, `AIUPlayerCameraManager`, `AIUGameMode`, `AIUGameState`, `UIUSaveGame` |
| `IUOnline` | `Default` | `OnlineSubsystem`, `OnlineSubsystemUtils`, `HTTP`, `Json`, `IndiaUnleashed` | `UIUSessionSubsystem`, Listen/Dedicated Server session lifecycle, Party management, Server-to-FastAPI HMAC signing |
| `IUWorld` | `Default` | `Landscape`, `NavigationSystem`, `AIModule`, `IndiaUnleashed` | `AIUCharminarBlockoutActor` (56m monument blockout), `UIUHyderabadWorldSubsystem` (6 districts, 18 POIs, NPC/Traffic distance LOD tiers, Police dispatch) |
| `IUVehicles` | `Default` | `ChaosVehicles`, `PhysicsCore`, `IndiaUnleashed` | `AIUVehicleBase` supporting Auto-Rickshaw, Sedan, Motorcycle, Scooter, TSRTC Bus, Cargo Truck, Boat, and Helicopter |
| `IUCombat` | `Default` | `PhysicsCore`, `IndiaUnleashed` | `AIUWeaponBase`, `UIUInventoryComponent`, Server-authoritative hit validation, recoil, armor absorption |
| `IUMissions` | `Default` | `IndiaUnleashed`, `IUWorld`, `IUOnline` | `UIUMissionSubsystem`, multi-stage objective validation, authoritative reward payout (`M01_OldCityArrival`) |
| `IUBattleRoyale` | `Default` | `IndiaUnleashed`, `IUCombat`, `IUOnline` | `AIUBRGameMode` (100-player target architecture, Hussain Sagar shrinking safe zone, storm damage) |

## 3. Build Targets (`Source/*.Target.cs`)
- `IndiaUnleashed.Target.cs`: Standalone client + listen-server game executable.
- `IndiaUnleashedEditor.Target.cs`: Unreal Editor build target for level design and Blueprint authoring.
- `IndiaUnleashedServer.Target.cs`: Dedicated server target (`TargetType.Server`) for Multiplayer City and 100-player Battle Royale.
- `IndiaUnleashedClient.Target.cs`: Client-only build target excluding dedicated server orchestration.
