# Build, Dedicated Server & Android Packaging Instructions

## 1. Headless C++20 & FastAPI Backend Verification (Linux / WSL / CI)
Run the complete C++20 and FastAPI automated test suites with a single command from the `IndiaUnleashed/` root directory:
```bash
make test
```
Or run them individually:
```bash
make test-cpp      # Compiles all 7 C++ modules with g++ -std=c++20 -Wall -Wextra -Werror and runs 13 test suites
make test-backend  # Runs 6 FastAPI + SQLite + HMAC anti-cheat integration tests
```

## 2. Building in Unreal Engine 5.4 on Windows PC
1. Install **Unreal Engine 5.4** via Epic Games Launcher (or source build for Dedicated Server targets) and **Visual Studio 2022** with the *Game development with C++* workload.
2. Right-click `IndiaUnleashed.uproject` -> **Generate Visual Studio project files**.
3. Compile the Editor target from command line or Visual Studio:
   ```powershell
   "C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" IndiaUnleashedEditor Win64 Development "%CD%\IndiaUnleashed.uproject" -WaitMutex
   ```
4. Generate the binary `/Game/Maps/Hyderabad/L_Hyderabad_Prototype.umap` automatically via the included Editor Python script:
   ```powershell
   "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "%CD%\IndiaUnleashed.uproject" -ExecutePythonScript="%CD%\Scripts\BuildHyderabadPrototypeInEditor.py"
   ```
5. **Two-Player Listen Server Test on Windows PC**:
   - Launch Host (Player 1):
     ```powershell
     "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe" "%CD%\IndiaUnleashed.uproject" /Game/Maps/Hyderabad/L_Hyderabad_Prototype?listen -game -ResX=1280 -ResY=720 -WinX=50 -WinY=50
     ```
   - Launch Client (Player 2):
     ```powershell
     "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe" "%CD%\IndiaUnleashed.uproject" 127.0.0.1 -game -ResX=1280 -ResY=720 -WinX=1350 -WinY=50
     ```

## 3. Building the Dedicated Server (`IndiaUnleashedServer`)
Requires an Unreal Engine 5.4 source build:
```powershell
Build.bat IndiaUnleashedServer Win64 Development "%CD%\IndiaUnleashed.uproject" -WaitMutex
```

## 4. Packaging for Android (`ASTC` / `Vulkan` + `OpenGL ES 3.2`)
1. Install **Android Studio + SDK API 34 + NDK r25b** via `Engine/Extras/Android/SetupAndroid.bat`.
2. Verify `Config/Android/AndroidEngine.ini` (`bBuildForArm64=True`, `bSupportsVulkan=True`, `MinSDKVersion=29`, `TargetSDKVersion=34`) and `Config/DefaultDeviceProfiles.ini` (`Android_Low`, `Android_Mid`, `Android_High` disables Lumen/Nanite/VSM and caps shadow cascades).
3. Run UAT Cook & Package:
   ```powershell
   RunUAT.bat BuildCookRun -project="%CD%\IndiaUnleashed.uproject" -noP4 -platform=Android -cookflavor=ASTC -clientconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory="%CD%\Saved\StagedBuilds\Android_ASTC"
   ```
