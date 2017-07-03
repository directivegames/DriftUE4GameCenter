// Copyright 2016 Directive Games Limited - All Rights Reserved

#pragma once

#include "DriftGameCenterAuthProviderFactory.h"

#include "ModuleManager.h"


class FDriftGameCenterModule : public IModuleInterface
{
public:
	FDriftGameCenterModule();
    
    bool IsGameModule() const override
    {
        return true;
    }

    void StartupModule() override;
    void ShutdownModule() override;

private:
	FDriftGameCenterAuthProviderFactory providerFactory;
};
