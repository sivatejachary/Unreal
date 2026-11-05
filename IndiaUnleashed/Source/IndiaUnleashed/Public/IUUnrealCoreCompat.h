// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Dual-Mode Header: Compiles natively inside Unreal Engine 5.4 (when WITH_ENGINE is defined
// by UnrealBuildTool) AND compiles standalone under standard C++20 (g++ / clang++ / MSVC)
// for deterministic headless verification and CI/CD automated unit testing.

#pragma once

#if defined(WITH_ENGINE) && WITH_ENGINE
    #include "CoreMinimal.h"
    #include "GameFramework/Actor.h"
    #include "GameFramework/Pawn.h"
    #include "GameFramework/Character.h"
    #include "GameFramework/CharacterMovementComponent.h"
    #include "GameFramework/PlayerController.h"
    #include "GameFramework/PlayerState.h"
    #include "Camera/PlayerCameraManager.h"
    #include "GameFramework/GameModeBase.h"
    #include "GameFramework/GameStateBase.h"
    #include "GameFramework/SaveGame.h"
    #include "Subsystems/GameInstanceSubsystem.h"
    #include "Subsystems/WorldSubsystem.h"
    #include "Net/UnrealNetwork.h"
    #include "InputActionValue.h"
#else
    #include <cmath>
    #include <cstdint>
    #include <cstring>
    #include <string>
    #include <vector>
    #include <map>
    #include <unordered_map>
    #include <memory>
    #include <algorithm>
    #include <functional>
    #include <sstream>
    #include <iomanip>
    #include <iostream>

    // Unreal Engine reflection & API macros for standalone C++20 verification
    #define INDIAUNLEASHED_API
    #define IUONLINE_API
    #define IUWORLD_API
    #define IUVEHICLES_API
    #define IUCOMBAT_API
    #define IUMISSIONS_API
    #define IUBATTLEROYALE_API

    #define UCLASS(...)
    #define USTRUCT(...)
    #define UENUM(...)
    #define UPROPERTY(...)
    #define UFUNCTION(...)
    #define GENERATED_BODY()
    #define TEXT(x) x

    using int32 = std::int32_t;
    using uint32 = std::uint32_t;
    using int64 = std::int64_t;
    using uint64 = std::uint64_t;
    using uint8 = std::uint8_t;
    using FString = std::string;
    using FName = std::string;

    template <typename T>
    using TArray = std::vector<T>;

    template <typename K, typename V>
    using TMap = std::map<K, V>;

    struct FVector
    {
        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;

        constexpr FVector() = default;
        constexpr FVector(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}

        FVector operator+(const FVector& Other) const { return FVector(X + Other.X, Y + Other.Y, Z + Other.Z); }
        FVector operator-(const FVector& Other) const { return FVector(X - Other.X, Y - Other.Y, Z - Other.Z); }
        FVector operator*(float Scalar) const { return FVector(X * Scalar, Y * Scalar, Z * Scalar); }
        FVector& operator+=(const FVector& Other) { X += Other.X; Y += Other.Y; Z += Other.Z; return *this; }

        float Size() const { return std::sqrt(X * X + Y * Y + Z * Z); }
        float Size2D() const { return std::sqrt(X * X + Y * Y); }
        static float Dist(const FVector& A, const FVector& B) { return (A - B).Size(); }
        static float Dist2D(const FVector& A, const FVector& B) { return (A - B).Size2D(); }
        FVector GetSafeNormal() const
        {
            float Len = Size();
            return (Len > 1e-5f) ? FVector(X / Len, Y / Len, Z / Len) : FVector(0.0f, 0.0f, 0.0f);
        }
        static FVector ZeroVector() { return FVector(0.0f, 0.0f, 0.0f); }
    };

    struct FVector2D
    {
        float X = 0.0f;
        float Y = 0.0f;
        constexpr FVector2D() = default;
        constexpr FVector2D(float InX, float InY) : X(InX), Y(InY) {}
        float Size() const { return std::sqrt(X * X + Y * Y); }
    };

    struct FRotator
    {
        float Pitch = 0.0f;
        float Yaw = 0.0f;
        float Roll = 0.0f;

        constexpr FRotator() = default;
        constexpr FRotator(float InPitch, float InYaw, float InRoll) : Pitch(InPitch), Yaw(InYaw), Roll(InRoll) {}

        FVector Vector() const
        {
            const float PitchRad = Pitch * (3.1415926535f / 180.0f);
            const float YawRad = Yaw * (3.1415926535f / 180.0f);
            const float CP = std::cos(PitchRad);
            return FVector(CP * std::cos(YawRad), CP * std::sin(YawRad), std::sin(PitchRad));
        }
        static FRotator ZeroRotator() { return FRotator(0.0f, 0.0f, 0.0f); }
    };

    struct FTransform
    {
        FVector Translation{0.0f, 0.0f, 0.0f};
        FRotator Rotation{0.0f, 0.0f, 0.0f};
        FVector Scale3D{1.0f, 1.0f, 1.0f};

        constexpr FTransform() = default;
        constexpr FTransform(const FRotator& InRot, const FVector& InTrans, const FVector& InScale = FVector(1.0f, 1.0f, 1.0f))
            : Translation(InTrans), Rotation(InRot), Scale3D(InScale) {}
    };

    struct FMath
    {
        template <typename T>
        static constexpr T Clamp(T Val, T MinVal, T MaxVal)
        {
            return (Val < MinVal) ? MinVal : ((Val > MaxVal) ? MaxVal : Val);
        }
        static float FInterpTo(float Current, float Target, float DeltaTime, float InterpSpeed)
        {
            if (InterpSpeed <= 0.0f) return Target;
            const float Dist = Target - Current;
            if ( std::abs(Dist) < 1e-4f ) return Target;
            const float DeltaMove = Dist * Clamp(DeltaTime * InterpSpeed, 0.0f, 1.0f);
            return Current + DeltaMove;
        }
        static float Abs(float V) { return std::abs(V); }
        static float Max(float A, float B) { return (A > B) ? A : B; }
        static float Min(float A, float B) { return (A < B) ? A : B; }
    };

    enum ENetRole
    {
        ROLE_None = 0,
        ROLE_SimulatedProxy = 1,
        ROLE_AutonomousProxy = 2,
        ROLE_Authority = 3
    };

    enum EMovementMode
    {
        MOVE_None = 0,
        MOVE_Walking = 1,
        MOVE_NavWalking = 2,
        MOVE_Falling = 3,
        MOVE_Swimming = 4,
        MOVE_Flying = 5,
        MOVE_Custom = 6
    };

    struct FInputActionValue
    {
        FVector Value{0.0f, 0.0f, 0.0f};
        FInputActionValue() = default;
        explicit FInputActionValue(bool bIn) : Value(bIn ? 1.0f : 0.0f, 0.0f, 0.0f) {}
        explicit FInputActionValue(float InX) : Value(InX, 0.0f, 0.0f) {}
        explicit FInputActionValue(FVector2D In2D) : Value(In2D.X, In2D.Y, 0.0f) {}
        explicit FInputActionValue(FVector In3D) : Value(In3D) {}

        bool GetBool() const { return std::abs(Value.X) > 0.5f; }
        float GetMagnitude() const { return Value.X; }
        FVector2D GetAxis2D() const { return FVector2D(Value.X, Value.Y); }
        FVector GetAxis3D() const { return Value; }
    };

    struct FLifetimeProperty
    {
        FString ClassName;
        FString PropertyName;
    };

    #define DOREPLIFETIME(ClassName, VarName)         OutLifetimeProps.push_back(FLifetimeProperty{#ClassName, #VarName})

    class UObject
    {
    public:
        virtual ~UObject() = default;
        FString ObjectName = "UObject";
        virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
        {
            (void)OutLifetimeProps;
        }
    };

    class UActorComponent : public UObject
    {
    public:
        class AActor* Owner = nullptr;
        bool bIsReplicated = false;
        void SetIsReplicatedByDefault(bool bNewReplicated) { bIsReplicated = bNewReplicated; }
        virtual void TickComponent(float DeltaTime) { (void)DeltaTime; }
    };

    class AActor : public UObject
    {
    public:
        FVector ActorLocation{0.0f, 0.0f, 0.0f};
        FRotator ActorRotation{0.0f, 0.0f, 0.0f};
        ENetRole Role = ROLE_Authority;
        bool bReplicates = true;
        bool bAlwaysRelevant = false;

        bool HasAuthority() const { return Role == ROLE_Authority; }
        void SetRole(ENetRole NewRole) { Role = NewRole; }
        ENetRole GetLocalRole() const { return Role; }

        FVector GetActorLocation() const { return ActorLocation; }
        void SetActorLocation(const FVector& NewLoc) { ActorLocation = NewLoc; }
        FRotator GetActorRotation() const { return ActorRotation; }
        void SetActorRotation(const FRotator& NewRot) { ActorRotation = NewRot; }
        FVector GetActorForwardVector() const { return ActorRotation.Vector(); }

        virtual void BeginPlay() {}
        virtual void Tick(float DeltaSeconds) { (void)DeltaSeconds; }
    };

    class APlayerState : public AActor
    {
    public:
        int32 PlayerId = 0;
        FString PlayerNamePrivate = "Player";
        float Score = 0.0f;

        void SetPlayerName(const FString& S) { PlayerNamePrivate = S; }
        FString GetPlayerName() const { return PlayerNamePrivate; }
        int32 GetPlayerId() const { return PlayerId; }
        void SetPlayerId(int32 InId) { PlayerId = InId; }
    };

    class AController : public AActor
    {
    public:
        class APawn* PossessedPawn = nullptr;
        APlayerState* PlayerState = nullptr;
        FRotator ControlRotation{0.0f, 0.0f, 0.0f};

        virtual void Possess(class APawn* InPawn);
        virtual void UnPossess();
        class APawn* GetPawn() const { return PossessedPawn; }
        FRotator GetControlRotation() const { return ControlRotation; }
        void SetControlRotation(const FRotator& InRot) { ControlRotation = InRot; }
    };

    class APawn : public AActor
    {
    public:
        AController* Controller = nullptr;
        APlayerState* PlayerState = nullptr;

        AController* GetController() const { return Controller; }
        APlayerState* GetPlayerState() const { return PlayerState; }
        void SetPlayerState(APlayerState* InPS) { PlayerState = InPS; }
    };

    inline void AController::Possess(APawn* InPawn)
    {
        if (PossessedPawn)
        {
            PossessedPawn->Controller = nullptr;
        }
        PossessedPawn = InPawn;
        if (InPawn)
        {
            InPawn->Controller = this;
            if (PlayerState)
            {
                InPawn->SetPlayerState(PlayerState);
            }
        }
    }

    inline void AController::UnPossess()
    {
        if (PossessedPawn)
        {
            PossessedPawn->Controller = nullptr;
            PossessedPawn = nullptr;
        }
    }

    class UCharacterMovementComponent : public UActorComponent
    {
    public:
        float MaxWalkSpeed = 500.0f;
        float MaxWalkSpeedCrouched = 260.0f;
        float MaxSwimSpeed = 320.0f;
        float JumpZVelocity = 480.0f;
        float AirControl = 0.35f;
        float GravityScale = 1.0f;
        float BrakingDecelerationWalking = 2048.0f;
        float BrakingDecelerationSwimming = 600.0f;
        float Buoyancy = 1.05f;
        EMovementMode MovementMode = MOVE_Walking;
        FVector Velocity{0.0f, 0.0f, 0.0f};
        bool bOrientRotationToMovement = true;

        void SetMovementMode(EMovementMode NewMovementMode) { MovementMode = NewMovementMode; }
        bool IsSwimming() const { return MovementMode == MOVE_Swimming; }
        bool IsFalling() const { return MovementMode == MOVE_Falling; }
        bool IsMovingOnGround() const { return MovementMode == MOVE_Walking || MovementMode == MOVE_NavWalking; }
    };

    class ACharacter : public APawn
    {
    public:
        UCharacterMovementComponent* CharacterMovement = nullptr;
        bool bIsCrouched = false;

        virtual void Jump()
        {
            if (CharacterMovement && CharacterMovement->IsMovingOnGround())
            {
                CharacterMovement->Velocity.Z = CharacterMovement->JumpZVelocity;
                CharacterMovement->SetMovementMode(MOVE_Falling);
            }
        }
        virtual void StopJumping() {}
        virtual void Crouch() { bIsCrouched = true; }
        virtual void UnCrouch() { bIsCrouched = false; }
    };

    class APlayerCameraManager : public AActor
    {
    public:
        float DefaultFOV = 85.0f;
        float CurrentFOV = 85.0f;
        float ViewPitchMin = -75.0f;
        float ViewPitchMax = 70.0f;
        virtual void UpdateCamera(float DeltaTime) { (void)DeltaTime; }
    };

    class APlayerController : public AController
    {
    public:
        APlayerCameraManager* PlayerCameraManager = nullptr;
    };

    class AGameStateBase : public AActor
    {
    public:
        TArray<APlayerState*> PlayerArray;
    };

    class AGameModeBase : public AActor
    {
    public:
        AGameStateBase* GameState = nullptr;
    };

    class USaveGame : public UObject {};
    class UGameInstanceSubsystem : public UObject {};
    class UWorldSubsystem : public UObject {};
#endif
