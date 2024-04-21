#pragma once
#include "ue/Engine.h"
#include "../settings/Settings.h"
#include "../utils/Logger.h"
#include "../discord/rpc.h"
#include "../scripting/Scripts.h"
#include "../utils/Globals.h"

class Features {
public:
	DiscordRPC* rpc = new DiscordRPC();
	bool sentWelcomeMessage = false;

	void handle(APlayerController* PlayerController) {
		do {
			ExecuteUserScripts();

			Globals::PlayerController = (APortalWarsPlayerController*)PlayerController;
			APortalWarsCharacter* Player = (APortalWarsCharacter*)PlayerController->Character;

			PlayerController->SetName(FString((Settings.MISC.PlayerName))); 
			PlayerController->FOV(Settings.EXPLOITS.FOV);

			if (Settings.MISC.LoadIntoMap) {
				//TODO Enable Play Button
				//UPortalWarsPlayButtonWidget:UPortalWarsButtonWidget:UPortalWarsGenericButton:UPortalWarsUserWidget:UUserWidget:UWidget:UVisual:UObject
				//bool IsEnabled(); // Function PortalWars.PortalWarsButtonWidget.IsEnabled // (Native|Public|Const) // @ game+0x1664af0

				Logger::Log("INFO", "Loading into map");
				PlayerController->SwitchLevel(L"Simulation_Alpha");
				Settings.MISC.LoadIntoMap = false;
			};

			if (!PlayerController->IsInGame()) {
				sentWelcomeMessage = false;
				SetState("In Menu");
				break;
			};
			SetState("In game");

			collision = false;
			if (GetAsyncKeyState(Settings.EXPLOITS.NoClip)) collision = !collision;

			if (collision && Player->GetActorEnableCollision()) Player->SetActorEnableCollision(false);
			else if (!collision && !Player->GetActorEnableCollision()) Player->SetActorEnableCollision(true);
			
			if (Settings.EXPLOITS.NoClip) {
				Player->curTimeOutOfBounds = 0.f;
				Player->maxTimeOutOfBounds = 999.0f;
			}

			if (Player->Health != 0) {
				UpdatePlayerHealth(Player);
			};

			USkeletalMeshComponent* PlayerMesh = Player->Mesh1P;
			AGun* PlayerGun = Player->CurrentWeapon;

			if (Settings.EXPLOITS.SpinBot && PlayerMesh && PlayerGun) {
				if (spin_yaw > 360.f) spin_yaw = 0.f;

				FHitResult resultPlayer;
				FHitResult resultWeapon;

				USkeletalMeshComponent* WeaponMesh = PlayerGun->Mesh1P;
				PlayerMesh->K2_SetRelativeRotation(FRotator(0.f, spin_yaw, 0.f), true, resultPlayer, false);
				WeaponMesh->K2_SetRelativeRotation(FRotator(0.f, spin_yaw, 0.f), true, resultWeapon, false);

				spin_yaw += 10.f;
			};

			if (Settings.EXPLOITS.NoRecoil && PlayerGun)
			{
				PlayerGun->recoilConfig.horizontalRecoilAmount = 0.f;
				PlayerGun->recoilConfig.verticalRecoilAmount = 0.f;
				PlayerGun->recoilConfig.recoilKick = 0.f;
				PlayerGun->recoilConfig.visualRecoil = 0.f;
				PlayerGun->recoilConfig.recoilKick = 0.f;
				PlayerGun->recoilConfig.recoilRiseTime = 0.f;
				PlayerGun->recoilConfig.recoilTotalTime = 0.f;
			};

			if (Settings.EXPLOITS.GodMelee && PlayerGun)
			{
				Player->MeleeRange = 999999.f;
				Player->TimeBetweenMelee = 0.1f;
				PlayerGun->WeaponConfig.MeleeDamage = 999.f;
			};

			Player->CustomTimeDilation = Settings.EXPLOITS.PlayerSpeed;

			if (Settings.EXPLOITS.InfinteJetpack)
			{
				Player->thrusterRechargeDelay = 0.1f;
				Player->thrusterCurrentTime = 0.0f;
				Player->ThrusterLoudnessForBots = 0.0f;
			};

			if (!sentWelcomeMessage)
			{
				Globals::PlayerController->SendChatMessage(FString("Welcome"));
				Globals::PlayerController->SendChatMessage(FString(Settings.MENU.Watermark));

				sentWelcomeMessage = true;
			};

			DrawActors();
		} while (false);

		return;
	};

private:
	bool collision = false;
	float spin_yaw = 0.f;

	void SetState(const char* state) {
		if (rpc->GetState() == state) return;
		return rpc->UpdateState(state);
	}

	void UpdatePlayerHealth(APortalWarsCharacter* Player) {
		if (Settings.EXPLOITS.GodMode) {
			float health = 9999;

			if (Player->MaxHealth != health) Player->MaxHealth = health;
			if (Player->Health != health) Player->Health = health;

			//Instigator->healthRechargeDelay = 0.1f;

			return;
		}

		if (Player->Health > 100) {
			Player->MaxHealth = 100;
			Player->Health = 100;

			return;
		}

		return;
	}

	void ExecuteUserScripts() {
		if (!Settings.MISC.UserScriptsEnabled) return;

		for (int i = 0; i < Scripts::scriptList.size(); i++)
		{
			Scripts::Execute(i);
		}
	}

	void DrawLevelActors(ULevel* Level)
	{
		static UObject* CharacterClass = ObjObjects->FindObject("Class PortalWars.PortalWarsCharacter");

		if (!Settings.DEBUG.DrawActors) return;
		if (!Level) return;

		auto Actors = Level->Actors;
		for (auto a = 0; a < Actors.Num(); a++) {
			if (!Actors.IsValidIndex(a)) continue;

			auto Actor = Actors[a];

			if (!Actor) continue;
			if (!Actor->RootComponent) continue;
			if (!Actor->IsA(CharacterClass)) continue;

			auto Character = reinterpret_cast<APortalWarsCharacter*>(Actor);
			auto Mesh = Character->Mesh;

			FVector2D rootPos2D = Mesh->GetBone(BoneFNames::Root, Globals::PlayerController);
			if (!rootPos2D.X && !rootPos2D.Y) continue;

			if (!Globals::Canvas) continue;
			Globals::Canvas->K2_DrawText(0, Actor->GetName(), rootPos2D, { 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, 1.f, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, false, false, true, { 0.0f, 0.0f, 0.0f, 1.f });
		};
	};

	void DrawActors()
	{
		if (!Settings.DEBUG.DrawActors) return;

		UWorld* World = UWorld::GetWorld();

		for (auto l = 0; l < World->Levels.Num(); l++) {
			if (!World->Levels.IsValidIndex(l)) continue;

			ULevel* Level = World->Levels[l];
			if (!Level) continue;
			DrawLevelActors(Level);
		}
	};
};