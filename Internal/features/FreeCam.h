#pragma once

#include "Feature.h"
#include "../utils/Globals.h"

// Detaches the camera into a free-flying mode while enabled, via
// APlayerController::ClientSetCameraMode. Like ThirdPerson it is a OneTime
// feature so the networked call fires once on enable and reverts once on disable.
//
// NOTE: the camera-mode name ("FreeCam") is a game-specific guess — confirm
// in-game (LogProcessEvent watching ClientSetCameraMode) and adjust if needed.
class FreeCam : public Feature
{
private:
	FName freeCamMode{};
	FName firstPersonMode{};

	FName MakeName(const char* text)
	{
		return reinterpret_cast<UKismetStringLibrary*>(Globals::PlayerController)
			->Conv_StringToName(FString(text));
	}

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
		if (!Globals::PlayerController)
		{
			Initialized = false;
			return;
		}

		freeCamMode = MakeName("FreeCam");
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
		Globals::PlayerController->ClientSetCameraMode(freeCamMode);
	};
};
