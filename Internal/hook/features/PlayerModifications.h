#pragma once

#include "../Feature.h"
#include "../../utils/Globals.h"

class PlayerModifications : public Feature
{
private:
	bool bSentWelcomeMessage = false;
	std::string OriginalPlayerName = "";
	bool bActorCollision = false;

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
		UpdateEnabled();

		if (!Initialized) return false;
		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->PlayerState) return false;

		return Enabled;
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

	void Destroy()
	{
	};

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

		if (bActorCollision && Player->GetActorEnableCollision()) Player->SetActorEnableCollision(false);
		else if (!bActorCollision && !Player->GetActorEnableCollision()) Player->SetActorEnableCollision(true);

		if (!bSentWelcomeMessage)
		{
			Globals::PlayerController->SendChatMessage(FString(std::format("Welcome, {}", OriginalPlayerName)));
			Globals::PlayerController->SendChatMessage(FString(Settings.MENU.Watermark));

			bSentWelcomeMessage = true;
		};
	};
};