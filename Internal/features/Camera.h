#pragma once

/// @file
/// The Camera feature: a single first/third/free camera selector (Settings.EXPLOITS.Camera). Only one
/// camera is ever active.
///  - First person: default, no override.
///  - Third person: a custom over-the-shoulder camera driven every frame via ClientSetSpectatorCamera
///    (the game's own arbitrary-pose camera). ClientSetCameraMode("ThirdPerson") does NOT work here —
///    the game forces first person in its camera update, so a style change is ignored; driving the
///    spectator-camera pose overrides it.
///  - Free cam: the game's debug fly-camera (ToggleDebugCamera console command).

#include "Feature.h"
#include "../ue/Globals.h"

#include <cmath>
#include <string>

class Camera : public Feature
{
  private:
	CameraMode applied = CameraMode::FirstPerson; ///< the mode currently in effect

	static void ToggleDebugCamera()
	{
		Globals::PlayerController->SendToConsole(FString(std::string("ToggleDebugCamera")));
	}

	/// Place the camera behind the pawn, looking along the aim — called every frame while in third
	/// person, so the game's per-frame camera update can't reassert first person.
	void DriveThirdPerson()
	{
		auto* pc = Globals::PlayerController;
		auto* pawn = reinterpret_cast<AActor*>(pc->AcknowledgedPawn);
		if (!pawn) return;

		const FRotator rot = pc->ControlRotation;
		constexpr float toRad = 3.14159265f / 180.f;
		const float p = rot.Pitch * toRad, y = rot.Yaw * toRad;
		const FVector forward{cosf(p) * cosf(y), cosf(p) * sinf(y), sinf(p)};

		FVector eye = pawn->K2_GetActorLocation();
		eye.Z += 60.f;

		const float d = Settings.EXPLOITS.ThirdPersonDistance;
		const float h = Settings.EXPLOITS.ThirdPersonHeight;
		pc->ClientSetSpectatorCamera(FVector{eye.X - forward.X * d, eye.Y - forward.Y * d, eye.Z - forward.Z * d + h}, rot);
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
		Initialized = true;
		Log("Initialized");
	};

	/// Leave free cam (toggle its debug camera off) on the disable edge; third person just stops
	/// being driven (the game resumes its own view).
	void Destroy()
	{
		if (applied == CameraMode::FreeCam) ToggleDebugCamera();
		applied = CameraMode::FirstPerson;
	};

	void Run()
	{
		const CameraMode want = Settings.EXPLOITS.Camera;

		if (want != applied)
		{
			if (applied == CameraMode::FreeCam) ToggleDebugCamera(); // leaving free: toggle off
			applied = want;
			if (want == CameraMode::FreeCam) ToggleDebugCamera(); // entering free: toggle on
		}

		if (want == CameraMode::ThirdPerson) DriveThirdPerson(); // custom camera, every frame
	};
};
