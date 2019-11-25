
#include "DriftGameCenterModule.h"

#include "Interfaces/IPluginManager.h"
#include "Features/IModularFeatures.h"


IMPLEMENT_MODULE(FDriftGameCenterModule, DriftGameCenter)


FDriftGameCenterModule::FDriftGameCenterModule()
{
}


void FDriftGameCenterModule::StartupModule()
{
    IModularFeatures::Get().RegisterModularFeature(TEXT("DriftAuthProviderFactory"), &providerFactory);
}


void FDriftGameCenterModule::ShutdownModule()
{
    IModularFeatures::Get().UnregisterModularFeature(TEXT("DriftAuthProviderFactory"), &providerFactory);
}
