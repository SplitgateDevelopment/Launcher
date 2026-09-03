#pragma once

/// @file
/// The PlayerModifications feature: an always-enabled per-frame pass that
/// applies assorted local-player tweaks (display name, FOV, movement speed,
/// out-of-bounds timers, a NoClip collision toggle), updates Discord Rich
/// Presence, and sends the one-time welcome chat messages.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../discord/rpc.h"

/// Bundle of local-player modifications that always runs (Enabled is forced
/// true); individual effects are gated by their own settings inside Run().
class PlayerModifications : public Feature
{
  private:
	bool bSentWelcomeMessage = false;	 ///< true once the join messages were sent this match
	std::string OriginalPlayerName = ""; ///< real player name captured at Init, used in the welcome
	bool bActorCollision = false;		 ///< current NoClip toggle state (collision disabled when true)

  public:
	PlayerModifications()
	{
		Name = "PlayerModifications";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = true;
	};

	bool Check()
	{
		if (!Initialized) return false;
		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->PlayerState) return false;

		return true;
	};

	void Init()
	{
		auto PlayerState = Globals::PlayerController->PlayerState;
		if (!PlayerState)
		{
			Initialized = false;
			return;
		}

		OriginalPlayerName = PlayerState->PlayerNamePrivate.ToString();

		Initialized = true;
		Log("Initialized");
	};

	void Destroy() {
	};

	/// Apply name/FOV every tick; while in a match also apply speed, disable the
	/// out-of-bounds timer, honour the NoClip hotkey, set Discord to "In Game",
	/// and send the welcome messages once. Resets that flag and shows "In Menu"
	/// when not in a match.
	void Run()
	{
		Globals::PlayerController->SetName(FString((Settings.MISC.PlayerName)));
		Globals::PlayerController->FOV(Settings.EXPLOITS.FOV);

		if (!Globals::PlayerController->IsInGame())
		{
			bSentWelcomeMessage = false;
			DiscordRPC::UpdateState("In Menu");

			return;
		};

		DiscordRPC::UpdateState("In Game");

		auto Player = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);

		Player->CustomTimeDilation = Settings.EXPLOITS.PlayerSpeed;
		Player->curTimeOutOfBounds = 0.f;
		Player->maxTimeOutOfBounds = 999.0f;

		bActorCollision = false;
		if (GetAsyncKeyState(Settings.EXPLOITS.NoClip)) bActorCollision = !bActorCollision;

		if (bActorCollision && Player->GetActorEnableCollision())
			Player->SetActorEnableCollision(false);
		else if (!bActorCollision && !Player->GetActorEnableCollision())
			Player->SetActorEnableCollision(true);

		if (!bSentWelcomeMessage)
		{
			Globals::PlayerController->SendChatMessage(FString(std::format("Welcome, {}", OriginalPlayerName)));
			Globals::PlayerController->SendChatMessage(FString(Settings.MENU.Watermark));

			bSentWelcomeMessage = true;
		};
	};
};