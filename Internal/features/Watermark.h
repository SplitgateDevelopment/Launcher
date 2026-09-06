#pragma once

/// @file
/// The Watermark feature: draws the product name and live FPS in the top-left corner through the
/// Render backend, so it follows the active renderer automatically — drawn on the game window in the
/// normal modes, and on the capture-excluded external window in streamproof mode — instead of drawing
/// to an ImGui draw list directly. Shown in menus too, not only in-game.

#include "Feature.h"
#include "../render/Render.h"
#include "../render/Colors.h"
#include "../utils/Rgb.h"

#include <chrono>
#include <format>
#include <string>

/// Draws the "Splitgate Internal" watermark + FPS each frame while enabled.
class Watermark : public Feature
{
  public:
	Watermark()
	{
		Name = "Watermark";
		UpdateEnabled();

		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.MENU.ShowWatermark;
	};

	/// Always valid once initialized — the watermark shows out of game (menus) too, so it does not
	/// gate on Engine::IsInGame like the other visual features.
	bool Check()
	{
		return Initialized;
	};

	/// No game object to resolve; ready immediately.
	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	void Destroy() {
	};

	void Run()
	{
		// RGB on: the cycling rainbow. Off: the palette's Primary red (the theme accent).
		Render::Color color = Render::Palette::Primary;
		if (Settings.MENU.Rgb)
			color = Rgb::Current(); // settings ::Color -> Render::Color (settings adapter)

		// Left-aligned (centered = false) so the corner text starts at x = 0.
		const float scale = 1.f;
		Render::Text(Render::Vec2{0.f, 0.f}, "Splitgate Internal", scale, color, false);
		Render::Text(Render::Vec2{0.f, 15.f}, std::format("FPS: {:.0f}", Fps()), scale, color, false);
	};

  private:
	/// Smoothed frames-per-second from this feature's own per-frame tick, so it doesn't read ImGui's
	/// IO from the game thread.
	float Fps()
	{
		using clock = std::chrono::steady_clock;
		static clock::time_point last = clock::now();
		static float fps = 0.f;

		const clock::time_point now = clock::now();
		const float dt = std::chrono::duration<float>(now - last).count();
		last = now;

		if (dt > 0.f) fps = (fps == 0.f) ? (1.f / dt) : (fps * 0.9f + (1.f / dt) * 0.1f);
		return fps;
	};
};
