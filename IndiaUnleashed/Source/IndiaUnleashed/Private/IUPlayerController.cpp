// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#include "../Public/IUPlayerController.h"

AIUPlayerController::AIUPlayerController()
{
    PlayerCameraManager = new AIUPlayerCameraManager();
}

void AIUPlayerController::BeginPlay()
{
    APlayerController::BeginPlay();
    ActiveInputMappingContext = "IMC_Default_OnFoot";
}

void AIUPlayerController::SwitchInputDevice(EIUInputDeviceType NewDevice)
{
    ActiveInputDevice = NewDevice;
}

void AIUPlayerController::SetVehicleInputContext(bool bInVehicle)
{
    ActiveInputMappingContext = bInVehicle ? "IMC_Vehicle_Driving" : "IMC_Default_OnFoot";
    if (auto* CamMgr = static_cast<AIUPlayerCameraManager*>(PlayerCameraManager))
    {
        CamMgr->SetCameraMode(bInVehicle ? EIUCameraMode::VehicleFollow : EIUCameraMode::ThirdPersonExploration);
    }
}
