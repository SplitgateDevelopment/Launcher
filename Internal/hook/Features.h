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

	void handle(APlayerController* PlayerController) {
		do {
			ExecuteUserScripts();

			APortalWarsCharacter* Player = (APortalWarsCharacter*)PlayerController->Character;

			std::string name = std::string(Settings.MISC.PlayerName);
			LPCWSTR playerName = std::wstring(name.begin(), name.end()).c_str();

			PlayerController->SetName(playerName);
			PlayerController->FOV(Settings.EXPLOITS.FOV);

			if (Settings.MISC.LoadIntoMap) {
				//TODO Enable Play Button
				//UPortalWarsPlayButtonWidget:UPortalWarsButtonWidget:UPortalWarsGenericButton:UPortalWarsUserWidget:UUserWidget:UWidget:UVisual:UObject
				//bool IsEnabled(); // Function PortalWars.PortalWarsButtonWidget.IsEnabled // (Native|Public|Const) // @ game+0x1664af0

				Logger::Log("INFO", "Loading into map");
				PlayerController->SwitchLevel(L"Simulation_Alpha");
				Settings.MISC.LoadIntoMap = false;
			};

			if (!isIngame(PlayerController)) {
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

			USkeletalMeshComponent* PlayerMesh = Player->Mesh;
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

		} while (false);

		return;
	};

private:
	bool collision = false;
	float spin_yaw = 0.f;

	bool isIngame(APlayerController* PlayerController) {
		return (PlayerController->AcknowledgedPawn);
	}

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
};