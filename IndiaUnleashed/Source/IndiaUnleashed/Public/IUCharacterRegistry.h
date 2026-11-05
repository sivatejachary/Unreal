// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Modular Character Registry & Architecture for INDIA UNLEASHED.
// Character 001 is AAA_Female_Rigged (Default and ONLY playable character in current phase).
// Future characters can be registered dynamically via FIUCharacterDefinition data assets.

#pragma once

#include "IUUnrealCoreCompat.h"

UENUM(BlueprintType)
enum class EIUCharacterTier : uint8
{
    Standard,
    SpecialForces,
    Legendary
};

USTRUCT(BlueprintType)
struct FIUCharacterDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Character")
    FString CharacterId = TEXT("CH_001_AAA_FEMALE");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Character")
    FString DisplayName = TEXT("AAA Female Operative");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Character")
    FString Description = TEXT("Elite Hyderabad Special Operations operative with advanced ballistic, tactical and traversal training.");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Character")
    FString FBXAssetPath = TEXT("/Game/IndiaUnleashed/Characters/Heroes/AAA_Female_Rigged/AAA_Female_Rigged.fbx");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Character")
    FString SkeletalMeshAsset = TEXT("/Game/IndiaUnleashed/Characters/Heroes/AAA_Female_Rigged/SK_AAA_Female_Rigged");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Character")
    FString SkeletonAsset = TEXT("/Game/IndiaUnleashed/Characters/Heroes/AAA_Female_Rigged/SKEL_AAA_Female");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Character")
    FString AnimBlueprintClass = TEXT("/Game/IndiaUnleashed/Characters/Heroes/AAA_Female_Rigged/ABP_AAA_Female");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Character")
    FString PhysicsAsset = TEXT("/Game/IndiaUnleashed/Characters/Heroes/AAA_Female_Rigged/PHYS_AAA_Female");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Character")
    EIUCharacterTier Tier = EIUCharacterTier::SpecialForces;

    // Tactical Gameplay Attributes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float BaseHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float BaseStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float WalkSpeed = 250.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float JogSpeed = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float SprintSpeed = 850.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float CrouchSpeed = 260.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float ProneSpeed = 120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Attributes")
    float JumpZVelocity = 480.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Status")
    bool bIsPlayable = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IU|Status")
    bool bIsUnlockedByDefault = true;
};

UCLASS(BlueprintType)
class INDIAUNLEASHED_API UIUCharacterRegistry : public UObject
{
    GENERATED_BODY()

public:
    static UIUCharacterRegistry* Get();

    UIUCharacterRegistry();

    // Registry Management
    void InitializeRegistry();
    bool RegisterCharacter(const FIUCharacterDefinition& NewCharacter);
    const FIUCharacterDefinition* GetCharacter(const FString& CharacterId) const;
    const FIUCharacterDefinition* GetDefaultCharacter() const;
    TArray<FIUCharacterDefinition> GetAvailableCharacters() const;

    // Active Selection State (Enforced: Character 001 for current development phase)
    FString GetActiveCharacterId() const { return ActiveCharacterId; }
    void SetActiveCharacterId(const FString& CharacterId);

private:
    static UIUCharacterRegistry* Instance;
    TMap<FString, FIUCharacterDefinition> RegisteredCharacters;
    FString ActiveCharacterId = TEXT("CH_001_AAA_FEMALE");
};
