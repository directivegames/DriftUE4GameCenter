
#include "DriftGameCenterAuthProviderFactory.h"

#include "DriftGameCenterAuthProvider.h"


FName FDriftGameCenterAuthProviderFactory::GetAuthProviderName() const
{
	return FName(TEXT("GameCenter"));
}


TUniquePtr<IDriftAuthProvider> FDriftGameCenterAuthProviderFactory::GetAuthProvider()
{
	return MakeUnique<FDriftGameCenterAuthProvider>();
}
