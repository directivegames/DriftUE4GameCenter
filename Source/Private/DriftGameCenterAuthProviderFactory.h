
#pragma once


#include "IDriftAuthProviderFactory.h"
#include "Templates/UniquePtr.h"


class FDriftGameCenterAuthProviderFactory : public IDriftAuthProviderFactory
{
    FName GetAuthProviderName() const override;
    TUniquePtr<IDriftAuthProvider> GetAuthProvider() override;
};
