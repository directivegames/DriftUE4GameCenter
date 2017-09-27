
#include "DriftGameCenterPCH.h"

#include "DriftGameCenterAuthProvider.h"
//#include "IErrorReporter.h"

#include "OnlineSubsystemTypes.h"
#include "OnlineSubsystemUtils.h"


DEFINE_LOG_CATEGORY_STATIC(LogDriftGameCenter, Log, All);


FDriftGameCenterAuthProvider::FDriftGameCenterAuthProvider()
{
}


void FDriftGameCenterAuthProvider::InitCredentials(InitCredentialsCallback callback)
{
    const auto localUserNum = 0;

    auto identityInterface = Online::GetIdentityInterface(nullptr, IOS_SUBSYSTEM);
    if (identityInterface.IsValid())
    {
        if (identityInterface->GetLoginStatus(localUserNum) == ELoginStatus::LoggedIn)
        {
            auto id = identityInterface->GetUniquePlayerId(localUserNum);
            if (id.IsValid())
            {
                OnLoginComplete(localUserNum, true, *id.Get(), TEXT(""), callback);
            }
            else
            {
                UE_LOG(LogDriftGameCenter, Error, TEXT("Failed to get a valid user ID from Game Center"));

                callback(false);
            }
        }
        else
        {
            auto externalUIInterface = Online::GetExternalUIInterface(nullptr, IOS_SUBSYSTEM);
            if (externalUIInterface.IsValid())
            {
                if (!externalUIInterface->ShowLoginUI(localUserNum, true, false,
                    FOnLoginUIClosedDelegate::CreateSP(this, &FDriftGameCenterAuthProvider::OnLoginUIClosed, callback)))
                {
                    UE_LOG(LogDriftGameCenter, Error, TEXT("Failed to show external login UI"));

                    callback(false);
                }
            }
            else
            {
                UE_LOG(LogDriftGameCenter, Error, TEXT("Failed to get online external UI interface"));

                callback(false);
            }
        }
    }
    else
    {
        UE_LOG(LogDriftGameCenter, Error, TEXT("Failed to to get identity interface for Game Center"));

        callback(false);
    }
}


void FDriftGameCenterAuthProvider::GetFriends(GetFriendsCallback callback)
{
    auto friendsInterface = Online::GetFriendsInterface(nullptr, IOS_SUBSYSTEM);
    if (friendsInterface.IsValid())
    {
        friendsInterface->ReadFriendsList(0, TEXT("Default"), FOnReadFriendsListComplete::CreateLambda([this, callback](int32, bool success, const FString& listName, const FString& error)
        {
            if (success)
            {
                TArray<TSharedRef<FOnlineFriend>> friends;
                auto friendsInterface = Online::GetFriendsInterface(nullptr, IOS_SUBSYSTEM);
                if (friendsInterface.IsValid())
                {
                    if (friendsInterface->GetFriendsList(0, TEXT("Default"), friends))
                    {
                        callback(true, friends);
                        return;
                    }
                    else
                    {
                        UE_LOG(LogDriftGameCenter, Warning, TEXT("Failed to get friends list"));
                    }
                }
                else
                {
                    UE_LOG(LogDriftGameCenter, Warning, TEXT("Failed to get online friends interface"));
                }
            }
            else
            {
                UE_LOG(LogDriftGameCenter, Warning, TEXT("Failed to read friends list: \"%s\""), *error);
            }
            callback(false, TArray<TSharedRef<FOnlineFriend>>());
        }));
        return;
    }
    else
    {
        UE_LOG(LogDriftGameCenter, Warning, TEXT("Failed to get online friends interface"));
    }
    callback(false, TArray<TSharedRef<FOnlineFriend>>{});
}


void FDriftGameCenterAuthProvider::FillProviderDetails(DetailsAppender appender) const
{
    appender(TEXT("public_key_url"), data.public_key_url);
    appender(TEXT("signature"), data.signature);
    appender(TEXT("salt"), data.salt);
    appender(TEXT("timestamp"), FString::Printf(TEXT("%lld"), data.timestamp));
    appender(TEXT("player_id"), data.player_id);
    appender(TEXT("player_name"), data.player_name);
    appender(TEXT("app_bundle_id"), data.app_bundle_id);
}


FString FDriftGameCenterAuthProvider::GetNickname()
{
    auto identityInterface = Online::GetIdentityInterface(nullptr, IOS_SUBSYSTEM);
    if (identityInterface.IsValid() && identityInterface->GetLoginStatus(0) == ELoginStatus::LoggedIn)
    {
        return identityInterface->GetPlayerNickname(0);
    }
    return TEXT("");
}


FString FDriftGameCenterAuthProvider::ToString() const
{
    return FString::Printf(TEXT("Game Center ID: %s"), *data.player_id);
}


void FDriftGameCenterAuthProvider::GetAvatarUrl(GetAvatarUrlCallback callback)
{
    callback(TEXT(""));
}


void FDriftGameCenterAuthProvider::OnLoginComplete(int32 localPlayerNum, bool success, const FUniqueNetId& userID, const FString& error, InitCredentialsCallback callback)
{
    if (success)
    {
        auto identityInterface = Online::GetIdentityInterface(nullptr, IOS_SUBSYSTEM);
        if (identityInterface.IsValid())
        {
            identityInterface->ClearOnLoginCompleteDelegate_Handle(localPlayerNum, loginCompleteDelegateHandle);

            if (identityInterface->GetLoginStatus(localPlayerNum) == ELoginStatus::LoggedIn)
            {
                auto id = identityInterface->GetUniquePlayerId(localPlayerNum);
                if (id.IsValid())
                {
                    gameCenterID = id->ToString();
                    GetIdentityValidationData(callback);
                    return;
                }
                else
                {
                    UE_LOG(LogDriftGameCenter, Error, TEXT("Failed to get unique player ID"));
                }
            }
            else
            {
                UE_LOG(LogDriftGameCenter, Error, TEXT("Login status is not LoggedIn"));
            }
        }
        else
        {
            UE_LOG(LogDriftGameCenter, Error, TEXT("Failed to get online user identity interface"));
        }
    }
    else
    {
        UE_LOG(LogDriftGameCenter, Error, TEXT("Failed to login to Game Center: %s"), *error);
    }

    callback(false);
}


void FDriftGameCenterAuthProvider::OnLoginUIClosed(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerNum, InitCredentialsCallback callback)
{
    bool success = UniqueId.IsValid();
    if (success)
    {
        OnLoginComplete(LocalPlayerNum, success, *UniqueId, FString{}, callback);
    }
    else
    {
        OnLoginComplete(LocalPlayerNum, success, FUniqueNetIdString{FString{}}, FString{}, callback);
    }
}


void FDriftGameCenterAuthProvider::GetIdentityValidationData(InitCredentialsCallback callback)
{
    auto player = [GKLocalPlayer localPlayer];
    if (player.isAuthenticated)
    {
        [player generateIdentityVerificationSignatureWithCompletionHandler : ^ (NSURL* publicKeyUrl, NSData* signature, NSData* salt, uint64_t timestamp, NSError* error)
        {
            // The iOS main thread is not the same as the UE4 main thread
            [FIOSAsyncTask CreateTaskWithBlock : ^ bool(void)
            {
                // TODO: Handle if the connection has been reset since the task begun
                if (error != nil)
                {
/*
                    auto extra = MakeShared<FJsonObject>();
                    extra->SetStringField(L"error_code", int32(error.code));
                    extra->SetStringField(L"error_domain", FString(error.domain));
                    extra->SetStringField(L"error_localized", FString(error.localizedDescription));
                    IErrorReporter::Get()->AddError("LogDriftGameCenter" TEXT("Failed to generate verification signature"), extra);
 */
                    UE_LOG(LogDriftGameCenter, Error, TEXT("Failed to generate verification signature: %d, %s"), int32(error.code), *FString(error.domain));
                    callback(false);
                }
                else
                {
                    data = FGameCenterVerficationData{ publicKeyUrl, signature, salt, timestamp, player };
                    callback(true);
                }
                return true;
            }];
        }];
    }
    else
    {
        callback(false);
    }
}
