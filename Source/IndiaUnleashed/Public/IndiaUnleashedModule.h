// Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.

#pragma once

#include "IUUnrealCoreCompat.h"

class INDIAUNLEASHED_API FIndiaUnleashedModule
{
public:
    void StartupModule();
    void ShutdownModule();
    static const char* GetModuleVersion() { return "0.1.0-alpha-hyderabad"; }
};
