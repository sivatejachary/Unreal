// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUSaveGame.h"
#include <fstream>

uint64 UIUSaveGame::ComputeIntegrityHash() const
{
    // Deterministic FNV-1a 64-bit hash over core progression fields + secret salt
    uint64 Hash = 14695981039346656037ULL;
    auto MixString = [&Hash](const FString& S) {
        for (char c : S)
        {
            Hash ^= static_cast<uint8>(c);
            Hash *= 1099511628211ULL;
        }
    };
    MixString(AccountId);
    MixString(std::to_string(SavedCurrencyINR));
    MixString(std::to_string(SavedExperienceXP));
    MixString(CurrentDistrict);
    for (const auto& V : OwnedVehicleIds) MixString(V);
    for (const auto& M : CompletedMissionIds) MixString(M);
    MixString("IU_HYDERABAD_SALT_2026");
    return Hash;
}

bool UIUSaveGame::SaveToDisk(const FString& FilePath) const
{
    std::ofstream Out(FilePath, std::ios::out | std::ios::trunc);
    if (!Out.is_open())
    {
        return false;
    }

    Out << "IU_SAVE_V1\n";
    Out << AccountId << "\n";
    Out << SavedCurrencyINR << "\n";
    Out << SavedExperienceXP << "\n";
    Out << LastPlayerLocation.X << " " << LastPlayerLocation.Y << " " << LastPlayerLocation.Z << "\n";
    Out << CurrentDistrict << "\n";

    Out << OwnedVehicleIds.size() << "\n";
    for (const auto& V : OwnedVehicleIds) Out << V << "\n";

    Out << CompletedMissionIds.size() << "\n";
    for (const auto& M : CompletedMissionIds) Out << M << "\n";

    Out << ComputeIntegrityHash() << "\n";
    return Out.good();
}

bool UIUSaveGame::LoadFromDisk(const FString& FilePath)
{
    std::ifstream In(FilePath, std::ios::in);
    if (!In.is_open())
    {
        return false;
    }

    FString Header;
    std::getline(In, Header);
    if (Header != "IU_SAVE_V1")
    {
        return false;
    }

    std::getline(In, AccountId);
    In >> SavedCurrencyINR >> SavedExperienceXP;
    In >> LastPlayerLocation.X >> LastPlayerLocation.Y >> LastPlayerLocation.Z;
    In.ignore(1024, '\n');
    std::getline(In, CurrentDistrict);

    size_t VehicleCount = 0;
    In >> VehicleCount;
    In.ignore(1024, '\n');
    OwnedVehicleIds.clear();
    for (size_t i = 0; i < VehicleCount; ++i)
    {
        FString V;
        std::getline(In, V);
        OwnedVehicleIds.push_back(V);
    }

    size_t MissionCount = 0;
    In >> MissionCount;
    In.ignore(1024, '\n');
    CompletedMissionIds.clear();
    for (size_t i = 0; i < MissionCount; ++i)
    {
        FString M;
        std::getline(In, M);
        CompletedMissionIds.push_back(M);
    }

    uint64 StoredHash = 0;
    In >> StoredHash;
    if (StoredHash != ComputeIntegrityHash())
    {
        return false; // Tampered save rejected!
    }
    return true;
}
