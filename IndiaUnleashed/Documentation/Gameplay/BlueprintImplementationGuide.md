# Blueprint & Enhanced Input Implementation Guide (Alpha 0.1)

All core state, physics, movement validation, and server authority reside in C++. Designers configure visual assets, skeletal meshes, animation graphs, and UI widgets via Blueprint subclasses under `Content/`.

## 1. Character & Animation Blueprints (`Content/Characters/`)
1. **`BP_IUCharacter`** (Parent Class: `AIUCharacter`):
   - Assign the third-person Skeletal Mesh (`/Game/Characters/Meshes/SKM_Hero_Hyderabad`) to the Mesh component.
   - Attach a `USpringArmComponent` (`TargetArmLength = 285.0`, `SocketOffset = (0, 55, 65)`, `bUsePawnControlRotation = true`) and a `UCameraComponent`.
   - Add `UIUInventoryComponent` as a default subobject.
2. **`ABP_IUCharacter`** (Parent Class: `UAnimInstance`):
   - Read `GetIUMovementComponent()->GetCurrentGait()`, `Velocity.Size2D()`, `bIsCrouched`, `bIsSprinting`, `bIsAiming`, and `IsSwimming()` in `BlueprintThreadSafeUpdateAnimation`.
   - Drive the **Locomotion State Machine**:
     - `Idle_To_Walk_Jog_Sprint` (1D/2D Blend Space or Motion Matching Pose Search database).
     - `Crouch_Idle_Walk` state triggered by `bIsCrouched`.
     - `Swim_Tread_Stroke` state triggered by `IsSwimming()` (used in Hussain Sagar Lake).
   - Apply **Control Rig** (`CR_IUCharacter_FootIK`) on post-locomotion evaluation to plant feet accurately on uneven Old City granite cobblestones and Golconda steps.

## 2. Enhanced Input Configuration (`Content/Core/`)
Create the following Input Actions (`UInputAction`) and bind them inside `IMC_Default_OnFoot` and `IMC_Vehicle_Driving`:
- `IA_Move` (`Axis2D`): Bound to `AIUCharacter::Input_Move` (`WASD` / Left Stick / Android Virtual Joystick).
- `IA_Look` (`Axis2D`): Bound to `AIUCharacter::Input_Look` (`Mouse XY` / Right Stick / Android Right Touch Swipe).
- `IA_Sprint` (`Digital Bool`): `Started` -> `Input_StartSprint`, `Completed` -> `Input_StopSprint` (`Left Shift`).
- `IA_Crouch` (`Digital Bool`): `Started` -> `Input_ToggleCrouch` (`C` key).
- `IA_Jump` (`Digital Bool`): `Started` -> `Input_Jump` (`SpaceBar`).
- `IA_Interact` (`Digital Bool`): `Started` -> `Input_Interact` (`E` key — enters nearby `AIUVehicleBase` within 3.8m or interacts with shopkeepers/mission triggers).
- `IA_Fire` / `IA_Aim` / `IA_Reload`: Bound to `AIUWeaponBase::ServerFireShot`, `Input_StartAim`, and `ServerReload`.

## 3. UI & CommonUI HUD Hierarchy (`Content/UI/`)
- **`WBP_MainMenu`** (`UCommonActivatableWidget`):
  - Buttons for **Play Open World (Hyderabad)**, **Multiplayer City (Join/Host Listen Server)**, **Battle Royale (Prototype Queue)**, **Character Customization**, **Weapon Inventory**, **Garage**, **Friends/Party**, **Settings**, and **Exit**.
- **`WBP_InGameHUD`**:
  - Top-Right: Wanted Level Stars (`0–5` bound to `AIUPlayerState::WantedLevel`), Cash in `₹ INR` (`CurrencyINR`), Active Weapon & Ammo (`CurrentAmmoInMag / ReserveAmmo`).
  - Bottom-Left: Circular Hyderabad Minimap centered on player coordinates with Charminar and active mission waypoint pins, bordered by Health (`CurrentHealth`), Armor (`CurrentArmor`), and Stamina (`CurrentStamina`) bars.
  - Bottom-Right (when driving): Vehicle Speedometer (`CurrentSpeedKmH`), Fuel Gauge (`CurrentFuelLiters`), and Chassis Integrity bar.
- **`WBP_AndroidTouchOverlay`** (`Content/Mobile/`):
  - Activated automatically when `AIUPlayerController::ActiveInputDevice == EIUInputDeviceType::AndroidTouch`. Renders left thumb virtual joystick, right thumb camera look zone, and context-sensitive Fire, Aim, Jump, Sprint, Crouch, and Enter/Exit Vehicle buttons.
