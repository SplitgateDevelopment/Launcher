#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UObject.h"

// Class PortalWars.PortalWarsNotificationManager
// Size: 0x4e0 (Inherited: 0x28)
struct UPortalWarsNotificationManager : UObject
{
	char pad_28[0x8]; // 0x28(0x08)
	/*struct UPortalWarsMessageDialogWidget* MessageDialogWidgetClass; // 0x30(0x08)
	struct FPortalWarsUISceneInitData MessageSceneData; // 0x38(0x50)
	struct UPortalWarsDialogWidget* RewardReceivedWidgetClass; // 0x88(0x08)
	struct FPortalWarsUISceneInitData RewardReceivedSceneData; // 0x90(0x50)
	struct UPortalWarsDialogWidget* StreakIncreaseWidgetClass; // 0xe0(0x08)
	struct FPortalWarsUISceneInitData StreakIncreaseSceneData; // 0xe8(0x50)
	struct UPortalWarsUnlockDialogWidget* UnlockDialogWidgetClass; // 0x138(0x08)
	struct FPortalWarsUISceneInitData UnlockDialogSceneData; // 0x140(0x50)
	struct UPortalWarsUIScene* ProgressionUpdateSceneClass; // 0x190(0x08)
	struct FPortalWarsUISceneInitData ProgressionSceneData; // 0x198(0x50)
	struct UPortalWarsBadgeRewardWidget* BadgeRewardWidgetClass; // 0x1e8(0x08)
	struct FPrimaryAssetId LevelBadge; // 0x1f0(0x10)
	struct FPortalWarsUISceneInitData UserLegacyProgressionNotificationDialogSceneData; // 0x200(0x50)
	struct FPortalWarsUISceneInitData UserBadgeRewardDialogSceneData; // 0x250(0x50)
	struct UPortalWarsBPPurchaseDialogWidget* BattlePassPurchaseDialogWidgetClass; // 0x2a0(0x08)
	struct FPortalWarsUISceneInitData BattlePassPurchaseSceneData; // 0x2a8(0x50)
	struct UPortalWarsDialogWidget* PlaylistActiveWidgetClass; // 0x2f8(0x08)
	struct FPortalWarsUISceneInitData PlaylistActiveSceneData; // 0x300(0x50)
	struct UPortalWarsDialogWidget* GameSessionReconnectWidgetClass; // 0x350(0x08)
	struct FPortalWarsUISceneInitData GameSessionReconnectSceneData; // 0x358(0x50)
	struct UPortalWarsDialogWidget* LimitedTimeOfferWidgetClass; // 0x3a8(0x08)
	struct FPortalWarsUISceneInitData LimitedTimeOfferInitData; // 0x3b0(0x50)
	struct FPortalWarsUISceneInstanceInitData PostMatchSceneData; // 0x400(0x58)
	struct UPortalWarsDialogWidget* RankUpdateWidgetClass; // 0x458(0x08)
	struct FPortalWarsUISceneInitData RankUpdateSceneInitData; // 0x460(0x50)
	char pad_4B0[0x8]; // 0x4b0(0x08)
	struct TArray<struct UObject*> ReferencedObjects; // 0x4b8(0x10)
	struct FText BadgeUnlockTitle; // 0x4c8(0x18)*/

	void ShowBattlePassPurchaseDialog();																									   // Function PortalWars.PortalWarsNotificationManager.ShowBattlePassPurchaseDialog // (Final|Native|Protected) // @ game+0x16b1ed0
	void QueueNotice(struct FErrorInfo& ErrorInfo);																							   // Function PortalWars.PortalWarsNotificationManager.QueueNotice // (Final|Native|Public|HasOutParms) // @ game+0x16b1c90
	void QueueError(struct FErrorInfo& ErrorInfo);																							   // Function PortalWars.PortalWarsNotificationManager.QueueError // (Final|Native|Public|HasOutParms) // @ game+0x16b1b00
	void OpenNoticeDialog(struct FErrorInfo& ErrorInfo);																					   // Function PortalWars.PortalWarsNotificationManager.OpenNoticeDialog // (Final|Native|Public|HasOutParms) // @ game+0x16b1970
	void OpenErrorDialog(struct FErrorInfo& ErrorInfo);																						   // Function PortalWars.PortalWarsNotificationManager.OpenErrorDialog // (Final|Native|Public|HasOutParms) // @ game+0x16b17e0
	void OnSessionQueryError(struct FErrorInfo& ErrorInfo);																					   // Function PortalWars.PortalWarsNotificationManager.OnSessionQueryError // (Final|Native|Protected|HasOutParms) // @ game+0x16b1560
	void OnRewardsReceived(struct FString Type, struct TArray<struct FRewardData> Rewards);													   // Function PortalWars.PortalWarsNotificationManager.OnRewardsReceived // (Final|Native|Protected) // @ game+0x16b1360
	void OnRankUpdate(struct FUserRankUpdate Data);																							   // Function PortalWars.PortalWarsNotificationManager.OnRankUpdate // (Final|Native|Protected) // @ game+0x16b0f10
	void OnProgressionUpdate(struct FUserProgressionUpdate Data);																			   // Function PortalWars.PortalWarsNotificationManager.OnProgressionUpdate // (Final|Native|Protected) // @ game+0x16b0d20
	void OnProgressionMatchUpdate(struct FUserProgressionUpdate Data);																		   // Function PortalWars.PortalWarsNotificationManager.OnProgressionMatchUpdate // (Final|Native|Protected) // @ game+0x16b0b30
	void OnPostGameStats(struct TArray<struct FUserPostGameStat> Data);																		   // Function PortalWars.PortalWarsNotificationManager.OnPostGameStats // (Final|Native|Protected) // @ game+0x16b0940
	void OnPlaylistActiveUpdate(struct FString PlaylistType);																				   // Function PortalWars.PortalWarsNotificationManager.OnPlaylistActiveUpdate // (Final|Native|Protected) // @ game+0x16b08a0
	void OnPartyError(struct FErrorInfo& ErrorInfo);																						   // Function PortalWars.PortalWarsNotificationManager.OnPartyError // (Final|Native|Protected|HasOutParms) // @ game+0x16b0710
	void OnLimitedTimeOfferReceived(struct FStoreCategoryInfo& LimitedTimeOffer);															   // Function PortalWars.PortalWarsNotificationManager.OnLimitedTimeOfferReceived // (Final|Native|Protected|HasOutParms) // @ game+0x16b0560
	void OnLegacyProgressionNotification(struct FAccelByteModelsUserLegacyProgressionNotificationInfo& UserLegacyProgressionNotificationInfo); // Function PortalWars.PortalWarsNotificationManager.OnLegacyProgressionNotification // (Final|Native|Public|HasOutParms) // @ game+0x16b04c0
	void OnGameSessionReconnectAvailable(struct FString GameSessionId, struct FDateTime DropDateTime);										   // Function PortalWars.PortalWarsNotificationManager.OnGameSessionReconnectAvailable // (Final|Native|Protected|HasDefaults) // @ game+0x16b0150
	void OnDailyPlayStreakIncreased(struct FUserDailyStreakInfo Data);																		   // Function PortalWars.PortalWarsNotificationManager.OnDailyPlayStreakIncreased // (Final|Native|Protected) // @ game+0x16aff20
	void OnChallengesUpdated(struct TArray<struct FUserChallengeUpdate> Challenges);														   // Function PortalWars.PortalWarsNotificationManager.OnChallengesUpdated // (Final|Native|Protected) // @ game+0x16afe30
	void OnBadgeRewards(struct TArray<struct FName>& BadgeNames);																			   // Function PortalWars.PortalWarsNotificationManager.OnBadgeRewards // (Final|Native|Public|HasOutParms) // @ game+0x16afd80
	struct UPortalWarsRewardReceivedViewModel* GetRewardReceivedViewModelForType(struct FString TypeString);								   // Function PortalWars.PortalWarsNotificationManager.GetRewardReceivedViewModelForType // (Final|Native|Protected) // @ game+0x16afbb0
	struct UPortalWarsPostMatchViewModel* GetPostMatchViewModel();																			   // Function PortalWars.PortalWarsNotificationManager.GetPostMatchViewModel // (Final|Native|Protected) // @ game+0x16afb80

	static constexpr const char* ClassName = "Class PortalWars.PortalWarsNotificationManager";
};
