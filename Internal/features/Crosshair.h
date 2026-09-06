#pragma once

/// @file
/// The Crosshair feature: draws a configurable cross at the screen centre through the active
/// render backend (four arms with a centre gap). RGB-aware, like the ESP/aim-FOV overlays.

#include "Feature.h"
#include "../ue/Engine.h"
#include "../utils/Rgb.h"
#include "../render/Render.h"

class Crosshair : public Feature
{
  private:
	static FLinearColor ToColor(const Color& c) { return FLinearColor{c.R, c.G, c.B, c.A}; }

  public:
	Crosshair()
	{
		Name = "Crosshair";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.VISUALS.Crosshair;
	};

	bool Check()
	{
		if (!Initialized) return false;
		if (!Engine::PlayerController) return false;
		if (!Engine::PlayerController->IsInGame()) return false;
		if (!Engine::Canvas) return false;

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
		const auto& v = Settings.VISUALS;
		const float cx = Engine::Canvas->ClipX * 0.5f;
		const float cy = Engine::Canvas->ClipY * 0.5f;
		const float gap = v.CrosshairGap;
		const float len = v.CrosshairSize;
		const float t = v.CrosshairThickness;
		const FLinearColor color = ToColor(Settings.MENU.Rgb ? Rgb::Current() : v.CrosshairColor);

		Render::Line({cx - gap - len, cy}, {cx - gap, cy}, t, color); // left
		Render::Line({cx + gap, cy}, {cx + gap + len, cy}, t, color); // right
		Render::Line({cx, cy - gap - len}, {cx, cy - gap}, t, color); // top
		Render::Line({cx, cy + gap}, {cx, cy + gap + len}, t, color); // bottom
	};
};
