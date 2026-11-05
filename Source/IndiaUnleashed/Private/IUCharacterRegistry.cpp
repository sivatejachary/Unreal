// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUCharacterRegistry.h"

UIUCharacterRegistry* UIUCharacterRegistry::Instance = nullptr;

UIUCharacterRegistry* UIUCharacterRegistry::Get()
{
    if (Instance == nullptr)
    {
        Instance = new UIUCharacterRegistry();
        Instance->InitializeRegistry();
    }
    return Instance;
}

UIUCharacterRegistry::UIUCharacterRegistry()
{
}

void UIUCharacterRegistry::InitializeRegistry()
{
    RegisteredCharacters.clear();

    // CHARACTER 001: AAA_Female_Rigged (Default & ONLY playable character for current phase)
    FIUCharacterDefinition Char001;
    Char001.CharacterId = TEXT("CH_001_AAA_FEMALE");
    Char001.DisplayName = TEXT("AAA Female Operative");
    Char001.Description = TEXT("High-fidelity rigged AAA female operative with full PBR baked materials, dynamic IK, and combat traversal rig.");
    Char001.FBXAssetPath = TEXT("/Game/IndiaUnleashed/Characters/Heroes/AAA_Female_Rigged/AAA_Female_Rigged.fbx");
    Char001.SkeletalMeshAsset = TEXT("/Game/IndiaUnleashed/Characters/Heroes/AAA_Female_Rigged/SK_AAA_Female_Rigged");
    Char001.SkeletonAsset = TEXT("/Game/IndiaUnleashed/Characters/Heroes/AAA_Female_Rigged/SKEL_AAA_Female");
    Char001.AnimBlueprintClass = TEXT("/Game/IndiaUnleashed/Characters/Heroes/AAA_Female_Rigged/ABP_AAA_Female");
    Char001.PhysicsAsset = TEXT("/Game/IndiaUnleashed/Characters/Heroes/AAA_Female_Rigged/PHYS_AAA_Female");
    Char001.Tier = EIUCharacterTier::SpecialForces;
    Char001.BaseHealth = 100.0f;
    Char001.BaseStamina = 100.0f;
    Char001.WalkSpeed = 250.0f;
    Char001.JogSpeed = 500.0f;
    Char001.SprintSpeed = 850.0f;
    Char001.CrouchSpeed = 260.0f;
    Char001.ProneSpeed = 120.0f;
    Char001.JumpZVelocity = 480.0f;
    Char001.bIsPlayable = true;
    Char001.bIsUnlockedByDefault = true;

    RegisteredCharacters[Char001.CharacterId] = Char001;
    ActiveCharacterId = Char001.CharacterId;
}

bool UIUCharacterRegistry::RegisterCharacter(const FIUCharacterDefinition& NewCharacter)
{
    if (NewCharacter.CharacterId.empty())
    {
        return false;
    }
    RegisteredCharacters[NewCharacter.CharacterId] = NewCharacter;
    return true;
}

const FIUCharacterDefinition* UIUCharacterRegistry::GetCharacter(const FString& CharacterId) const
{
    auto It = RegisteredCharacters.find(CharacterId);
    if (It != RegisteredCharacters.end())
    {
        return &It->second;
    }
    return nullptr;
}

const FIUCharacterDefinition* UIUCharacterRegistry::GetDefaultCharacter() const
{
    return GetCharacter(TEXT("CH_001_AAA_FEMALE"));
}

TArray<FIUCharacterDefinition> UIUCharacterRegistry::GetAvailableCharacters() const
{
    TArray<FIUCharacterDefinition> Result;
    for (const auto& Pair : RegisteredCharacters)
    {
        if (Pair.second.bIsPlayable)
        {
            Result.push_back(Pair.second);
        }
    }
    return Result;
}

void UIUCharacterRegistry::SetActiveCharacterId(const FString& CharacterId)
{
    // During current phase: enforce only Character 001
    if (RegisteredCharacters.find(CharacterId) != RegisteredCharacters.end())
    {
        ActiveCharacterId = CharacterId;
    }
    else
    {
        ActiveCharacterId = TEXT("CH_001_AAA_FEMALE");
    }
}
