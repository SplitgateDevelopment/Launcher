#pragma once

/// @file
/// The Camera feature: a single first/third/free camera selector (Settings.EXPLOITS.Camera). Replaces
/// the old separate Third Person / Free Cam toggles — only one camera is ever active. Third person
/// uses the game's built-in camera mode (ClientSetCameraMode); free cam uses its debug fly-camera
/// (ToggleDebugCamera). First person is the default (no override).
///
/// NOTE: the ClientSetCameraMode mode names are game-specific and unverified — confirm in-game if
/// third person doesn't engage.

#include "Feature.h"
#include "../utils/Globals.h"

class Camera : public Feature
{
  private:
	CameraMode applied = CameraMode::FirstPerson; ///< the mode currently in effect
	FName thirdPersonMode{};
	FName firstPersonMode{};

	// Conv_StringToName is a static Blueprint helper; the call object is ignored.
	FName MakeName(const char* text)
	{
		return reinterpret_cast<UKismetStringLibrary*>(Globals::PlayerController)->Conv_StringToName(FString(text));
	}

	void Apply(CameraMode mode)
	{
		auto* pc = Globals::PlayerController;
		switch (mode)
		{
		case CameraMode::ThirdPerson:
			pc->ClientSetCameraMode(thirdPersonMode);
			break;
		case CameraMode::FreeCam:
			pc->SendToConsole(FString(std::string("ToggleDebugCamera")));
			break;
		default:
			break;
		}
	}

	void Revert(CameraMode mode)
	{
		auto* pc = Globals::PlayerController;
		switch (mode)
		{
		case CameraMode::ThirdPerson:
			pc->ClientSetCameraMode(firstPersonMode);
			break;
		case CameraMode::FreeCam:
			pc->SendToConsole(FString(std::string("ToggleDebugCamera"))); // stateful toggle → off
			break;
		default:
			break;
		}
	}

  public:
	Camera()
	{
		Name = "Camera";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.Camera != CameraMode::FirstPerson;
	};

	bool Check()
	{
		return Initialized && Globals::PlayerController && Globals::PlayerController->IsInGame();
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

	/// Restore first person on the enabled -> disabled edge (mode set back to First).
	void Destroy()
	{
		if (applied != CameraMode::FirstPerson)
		{
			Revert(applied);
			applied = CameraMode::FirstPerson;
		}
	};

	/// Switch camera when the selected mode changes (revert the old, apply the new).
	void Run()
	{
		const CameraMode want = Settings.EXPLOITS.Camera;
		if (want == applied) return;

		Revert(applied);
		Apply(want);
		applied = want;
	};
};
