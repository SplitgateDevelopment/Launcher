#pragma once

/// @file
/// The FreeCam feature: toggles a free-flying camera mode while enabled. See
/// the class comment for the OneTime enable/disable semantics and the
/// unverified camera-mode-name caveat.

#include "Feature.h"
#include "../utils/Globals.h"

// Toggles a free-flying camera while enabled by running the UE console command
// `ToggleDebugCamera` (the game forces first person, so the ClientSetCameraMode approach didn't
// engage — but a console command does). OneTime, so the toggle fires once on enable and once on
// disable. ToggleDebugCamera is the stock UE command; if a different command works in this build,
// change the string here (or run it from Debug > Console command).
class FreeCam : public Feature
{
  private:
	static constexpr const char* Command = "ToggleDebugCamera";

  public:
	FreeCam()
	{
		Name = "FreeCam";
		OneTime = true;
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.FreeCam;
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
		Initialized = (Globals::PlayerController != nullptr);
		Log("Initialized");
	};

	void Destroy()
	{
		Globals::PlayerController->SendToConsole(FString(Command)); // toggle back off
	};

	void Run()
	{
		Globals::PlayerController->SendToConsole(FString(Command));
	};
};
