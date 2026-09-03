#pragma once

/// @file
/// The ThirdPerson feature: switches the camera to third person while enabled.
/// See the class comment for the OneTime enable/disable semantics and the
/// unverified camera-mode-name caveat.

#include "Feature.h"
#include "../utils/Globals.h"

// Switches the camera to third person while enabled (Splitgate is first person by
// default). Implemented as a OneTime feature so the networked ClientSetCameraMode
// call fires once on enable and the revert fires once on disable, rather than
// every frame.
//
// NOTE: the camera-mode names below are game-specific and unverified. Confirm
// them in-game (enable LogProcessEvent and watch for ClientSetCameraMode, or
// try the built-in modes) and adjust if third person does not engage.
class ThirdPerson : public Feature
{
  private:
	FName thirdPersonMode{}; ///< cached FName of the third-person mode, applied by Run()
	FName firstPersonMode{}; ///< cached FName of the default mode, restored by Destroy()

	// Conv_StringToName is a static Blueprint function, so the object it is
	// invoked on is ignored — any valid UObject works as the call context.
	FName MakeName(const char* text)
	{
		return reinterpret_cast<UKismetStringLibrary*>(Globals::PlayerController)
			->Conv_StringToName(FString(text));
	}

  public:
	ThirdPerson()
	{
		Name = "ThirdPerson";
		OneTime = true;
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.ThirdPerson;
	};

	bool Check()
	{
		if (!Initialized) return false;

		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;

		return true;
	};

	void Init()
	{
		if (!Globals::PlayerController)
		{
			Initialized = false;
			return;
		}

		thirdPersonMode = MakeName("ThirdPerson");
		firstPersonMode = MakeName("FirstPerson");

		Initialized = true;
		Log("Initialized");
	};

	void Destroy()
	{
		Globals::PlayerController->ClientSetCameraMode(firstPersonMode);
	};

	void Run()
	{
		Globals::PlayerController->ClientSetCameraMode(thirdPersonMode);
	};
};
