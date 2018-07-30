#pragma once

#include "IDriftAuthProvider.h"

#include "Base64.h"
#include "OnlineSubsystemTypes.h"

#include <GameKit/GKLocalPlayer.h>


struct FOnlineError;

class FDriftGameCenterAuthProvider : public IDriftAuthProvider
{
public:
    FDriftGameCenterAuthProvider();

    FString GetProviderName() const override { return TEXT("gamecenter"); }
    void InitCredentials(InitCredentialsCallback callback) override;
    void GetFriends(GetFriendsCallback callback) override;
    void GetAvatarUrl(GetAvatarUrlCallback callback) override;
    void FillProviderDetails(DetailsAppender appender) const override;
    FString GetNickname() override;

    FString ToString() const override;
    
private:
    void OnLoginComplete(int32 localPlayerNum, bool success, const FUniqueNetId& userID, const FString& error, InitCredentialsCallback callback);
    void OnLoginUIClosed(TSharedPtr<const FUniqueNetId> UniqueId, int LocalPlayerNum, const FOnlineError& Error, InitCredentialsCallback callback);
    void GetIdentityValidationData(InitCredentialsCallback callback);

private:
    FDelegateHandle loginCompleteDelegateHandle;

    FString gameCenterID;

    struct FGameCenterVerficationData
    {
        FGameCenterVerficationData() = default;

        FGameCenterVerficationData(NSURL* publicKeyUrl, NSData* signature, NSData* salt, uint64_t timestamp, GKLocalPlayer* localPlayer)
            : timestamp{ timestamp }
            , public_key_url{ publicKeyUrl.absoluteString }
            , player_id{ localPlayer.playerID }
            , player_name{ localPlayer.alias }
            , app_bundle_id{ [[NSBundle mainBundle] bundleIdentifier] }
        {
            TArray<uint8> signatureBuffer;
            signatureBuffer.AddZeroed(signature.length);
            FMemory::Memcpy(signatureBuffer.GetData(), signature.bytes, signature.length);
            this->signature = FBase64::Encode(signatureBuffer);

            TArray<uint8> saltBuffer;
            saltBuffer.AddZeroed(salt.length);
            FMemory::Memcpy(saltBuffer.GetData(), salt.bytes, salt.length);
            this->salt = FBase64::Encode(saltBuffer);
        }

        uint64_t timestamp{ 0 };
        FString public_key_url;
        FString signature;
        FString salt;
        FString player_id;
        FString player_name;
        FString app_bundle_id;
    };

    FGameCenterVerficationData data;
};
