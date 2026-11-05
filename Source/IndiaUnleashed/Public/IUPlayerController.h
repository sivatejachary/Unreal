// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
// Enhanced Input Player Controller supporting Keyboard/Mouse, Gamepad,
// and Android Touch Virtual Joystick + Action Buttons.

#pragma once

#include "IUUnrealCoreCompat.h"
#include "IUPlayerCameraManager.h"

UENUM(BlueprintType)
enum class EIUInputDeviceType : uint8
{
    KeyboardMouse = 0,
    Gamepad = 1,
    AndroidTouch = 2
};

UCLASS()
class INDIAUNLEASHED_API AIUPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AIUPlayerController();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Input")
    EIUInputDeviceType ActiveInputDevice = EIUInputDeviceType::KeyboardMouse;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="IU|Input")
    FString ActiveInputMappingContext = "IMC_Default_OnFoot";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="IU|Input")
    float LookSensitivityMouse = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="IU|Input")
    float LookSensitivityTouch = 0.65f;

    virtual void BeginPlay() override;

    void SwitchInputDevice(EIUInputDeviceType NewDevice);
    void SetVehicleInputContext(bool bInVehicle);
};
