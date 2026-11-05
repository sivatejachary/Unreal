CXX ?= g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -Werror

CPP_SOURCES := \
	Source/IndiaUnleashed/Private/IndiaUnleashedModule.cpp \
	Source/IndiaUnleashed/Private/IUCharacterMovementComponent.cpp \
	Source/IndiaUnleashed/Private/IUPlayerState.cpp \
	Source/IndiaUnleashed/Private/IUPlayerCameraManager.cpp \
	Source/IndiaUnleashed/Private/IUPlayerController.cpp \
	Source/IndiaUnleashed/Private/IUCharacter.cpp \
	Source/IndiaUnleashed/Private/IUCharacterRegistry.cpp \
	Source/IndiaUnleashed/Private/IUGameState.cpp \
	Source/IndiaUnleashed/Private/IUGameMode.cpp \
	Source/IndiaUnleashed/Private/IUSaveGame.cpp \
	Source/IndiaUnleashed/Private/IUAnimationSystem.cpp \
	Source/IndiaUnleashed/Private/IUAudioSoundtrackSubsystem.cpp \
	Source/IndiaUnleashed/Private/IUAndroidProfiler.cpp \
	Source/IndiaUnleashed/Private/IUMobileTouchInputSystem.cpp \
	Source/IndiaUnleashed/Private/IUWeaponAndTraversalAnimation.cpp \
	Source/IndiaUnleashed/Private/IULobbyInventoryAndHudSystem.cpp \
	Source/IndiaUnleashed/Private/IUGameplayHUD.cpp \
	Source/IUOnline/Private/IUSessionSubsystem.cpp \
	Source/IUOnline/Private/IUMultiplayerCitySystems.cpp \
	Source/IUWorld/Private/IUCharminarBlockoutActor.cpp \
	Source/IUWorld/Private/IURoadSplineActor.cpp \
	Source/IUWorld/Private/IUProceduralBuildingActor.cpp \
	Source/IUWorld/Private/IUGPSNavigationSystem.cpp \
	Source/IUWorld/Private/IUHyderabadWorldSubsystem.cpp \
	Source/IUWorld/Private/IUAISystem.cpp \
	Source/IUWorld/Private/IUCommerceAndGarage.cpp \
	Source/IUWorld/Private/IUCinematicDirector.cpp \
	Source/IUWorld/Private/IUGeospatialAssetPipeline.cpp \
	Source/IUVehicles/Private/IUVehicleBase.cpp \
	Source/IUCombat/Private/IUWeaponBase.cpp \
	Source/IUMissions/Private/IUMissionSubsystem.cpp \
	Source/IUBattleRoyale/Private/IUBRGameMode.cpp \
	Source/IndiaUnleashed/Private/Tests/IUAutomationTests.cpp

.PHONY: all test-cpp test-backend test clean

all: test

test-cpp:
	$(CXX) $(CXXFLAGS) $(CPP_SOURCES) -o /tmp/iu_cpp_test_runner
	/tmp/iu_cpp_test_runner

test-backend:
	PYTHONPATH=Backend python3 -m unittest discover -s Backend/tests -p "test_*.py" -v

test: test-cpp test-backend

clean:
	rm -f /tmp/iu_cpp_test_runner
