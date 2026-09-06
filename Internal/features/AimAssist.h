#pragma once

/// @file
/// The AimAssist feature: amplifies the current weapon's built-in aim-assist/magnetism (a soft
/// "legit" aim that never moves the view). Writes inflated values into the equipped gun's
/// AutoAimConfig each frame, from a pristine baseline captured per weapon, and restores it on
/// disable.
///
/// NOTE: this rides on the game's own aim-assist path, which the game may only apply on controller
/// input — verify it does anything on mouse & keyboard in-game.

#include "Feature.h"
#include "../ue/Globals.h"

class AimAssist : public Feature
{
  private:
	AGun* trackedGun = nullptr;	 ///< the gun whose config we last inflated
	FAutoAimData original{};	 ///< its pristine config, to restore
	bool haveOriginal = false;

	static APortalWarsCharacter* LocalCharacter()
	{
		if (!Globals::PlayerController) return nullptr;
		return reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character);
	}

	/// Put the tracked gun's config back to its captured baseline.
	void Restore()
	{
		if (haveOriginal && trackedGun) trackedGun->AutoAimConfig = original;
		trackedGun = nullptr;
		haveOriginal = false;
	}

  public:
	AimAssist()
	{
		Name = "AimAssist";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.AIM.AimAssist;
	};

	bool Check()
	{
		return Initialized && Globals::PlayerController && Globals::PlayerController->IsInGame();
	};

	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	void Destroy()
	{
		Restore();
	};

	void Run()
	{
		auto* character = LocalCharacter();
		if (!character) return;
		auto* gun = character->CurrentWeapon;
		if (!gun) return;

		// Re-baseline when the weapon changes (restore the old one first), so scaling never compounds.
		if (gun != trackedGun)
		{
			Restore();
			trackedGun = gun;
			original = gun->AutoAimConfig;
			haveOriginal = true;
		}

		const float s = Settings.AIM.AimAssistStrength;
		auto& cfg = gun->AutoAimConfig;
		cfg.AutoAimRadius = original.AutoAimRadius * s;
		cfg.AutoAimRadiusZoomed = original.AutoAimRadiusZoomed * s;
		cfg.AutoAimRange = original.AutoAimRange * s;
		cfg.MagnetismRange = original.MagnetismRange * s;
		cfg.MagnetismAngle = original.MagnetismAngle * s;
		cfg.ShouldUseMagnetism = true;
	};
};
