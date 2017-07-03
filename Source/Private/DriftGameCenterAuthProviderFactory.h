
#pragma once


#include "IDriftAuthProviderFactory.h"


class FDriftGameCenterAuthProviderFactory : public IDriftAuthProviderFactory
{
    FName GetAuthProviderName() const override;
    TUniquePtr<IDriftAuthProvider> GetAuthProvider() override;
};
