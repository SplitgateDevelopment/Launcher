#pragma once

/// @file
/// The AimFov feature: draws the aimbot's FOV as a circle centred on the crosshair, so the
/// lock-on radius (Settings.AIM.AimFov, in pixels) is visible while tuning.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../utils/Rgb.h"
#include "../render/Render.h"

#include <cmath>

/// Draws a circle of radius AimFov (px) at screen centre through the active render backend.
class AimFov : public Feature
{
  private:
	static FLinearColor ToColor(const Color& c) { return FLinearColor{c.R, c.G, c.B, c.A}; }

  public:
	AimFov()
	{
		Name = "AimFov";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.AIM.DrawAimFov;
	};

	bool Check()
	{
		if (!Initialized) return false;
		if (!Globals::PlayerController) return false;
		if (!Globals::PlayerController->IsInGame()) return false;
		if (!Globals::Canvas) return false;

		return true;
	};

	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	void Destroy() {
	};

	void Run()
	{
		const float cx = Globals::Canvas->ClipX * 0.5f;
		const float cy = Globals::Canvas->ClipY * 0.5f;
		const float radius = Settings.AIM.AimFov;
		// RGB overrides the configured color with the cycling rainbow, like the ESP box/lines.
		const FLinearColor color = ToColor(Settings.MENU.Rgb ? Rgb::Current() : Settings.AIM.AimFovColor);

		// Approximate the circle with a fixed number of segments (drawn through Render so it
		// follows whichever renderer is active).
		constexpr int segments = 48;
		constexpr float twoPi = 6.28318530718f;
		FVector2D prev{cx + radius, cy};
		for (int i = 1; i <= segments; i++)
		{
			const float a = twoPi * i / segments;
			FVector2D cur{cx + radius * cosf(a), cy + radius * sinf(a)};
			Render::Line(prev, cur, 1.f, color);
			prev = cur;
		}
	};
};
