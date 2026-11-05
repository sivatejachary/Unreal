// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUAudioSoundtrackSubsystem.h"
#include <fstream>

UIUAudioSoundtrackSubsystem::UIUAudioSoundtrackSubsystem()
{
    InitializeSoundtrackCatalog();
}

void UIUAudioSoundtrackSubsystem::InitializeSoundtrackCatalog()
{
    SoundtrackCatalog.clear();
    SoundtrackCatalog.push_back({EIUMusicStemState::MainTheme,             "OST_01", "Content/Audio/Soundtrack/OST_01_MainTheme_DeccanPulse.wav",     108.0f, "Raga Bhairav Heroic Pulse"});
    SoundtrackCatalog.push_back({EIUMusicStemState::Exploration,           "OST_02", "Content/Audio/Soundtrack/OST_02_Exploration_OldCityBreeze.wav",  84.0f, "Raga Yaman Sitar & Bansuri"});
    SoundtrackCatalog.push_back({EIUMusicStemState::Cinematic,             "OST_03", "Content/Audio/Soundtrack/OST_03_Cinematic_CharminarDawn.wav",    72.0f, "Raga Bhairav Dawn Overture"});
    SoundtrackCatalog.push_back({EIUMusicStemState::CombatAction,          "OST_04", "Content/Audio/Soundtrack/OST_04_Action_BattleRoyaleDrop.wav",   138.0f, "Raga Darbari Dholak & Sub-Bass"});
    SoundtrackCatalog.push_back({EIUMusicStemState::VehicleDriving,        "OST_05", "Content/Audio/Soundtrack/OST_05_Driving_NecklaceRoadNitro.wav", 128.0f, "Raga Yaman Electro-Tabla Drive"});
    SoundtrackCatalog.push_back({EIUMusicStemState::EmotionalStory,        "OST_06", "Content/Audio/Soundtrack/OST_06_Emotional_MusiSunset.wav",       66.0f, "Raga Darbari Melodic Strings"});
    SoundtrackCatalog.push_back({EIUMusicStemState::NightAtmosphere,       "OST_07", "Content/Audio/Soundtrack/OST_07_Night_CyberTowersSynth.wav",     92.0f, "Raga Bhairav Cyber Pad"});
    SoundtrackCatalog.push_back({EIUMusicStemState::EnvironmentalAmbience, "AMB_08", "Content/Audio/Soundtrack/AMB_08_HyderabadBazaarAndMonsoon.wav",  60.0f, "Charminar Bazaar & Monsoon Rain"});
}

const FIUSoundtrackTrackDescriptor* UIUAudioSoundtrackSubsystem::TransitionToMusicState(EIUMusicStemState NewState)
{
    ActiveStem = NewState;
    for (const auto& Track : SoundtrackCatalog)
    {
        if (Track.StemState == NewState)
        {
            return &Track;
        }
    }
    return nullptr;
}

bool UIUAudioSoundtrackSubsystem::VerifyAllWavAssetsOnDisk(const FString& ProjectRootDir) const
{
    if (SoundtrackCatalog.size() != 8)
    {
        return false;
    }
    for (const auto& Track : SoundtrackCatalog)
    {
        const FString FullPath = ProjectRootDir + "/" + Track.RelativeWavPath;
        std::ifstream In(FullPath, std::ios::binary);
        if (!In.is_open())
        {
            return false;
        }
        char RiffHeader[4] = {0};
        In.read(RiffHeader, 4);
        if (RiffHeader[0] != 'R' || RiffHeader[1] != 'I' || RiffHeader[2] != 'F' || RiffHeader[3] != 'F')
        {
            return false;
        }
    }
    return true;
}
