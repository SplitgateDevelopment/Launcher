#pragma once

/// @file
/// The Watermark feature: draws the product name and live FPS in the top-left corner through the
/// Render backend, so it follows the active renderer automatically — drawn on the game window in the
/// normal modes, and on the capture-excluded external window in streamproof mode — instead of drawing
/// to an ImGui draw list directly. Shown in menus too, not only in-game.

#include "Feature.h"
#include "../render/Render.h"
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
		// RGB on: the cycling rainbow. Off: the theme's title red (Styles' ImGuiCol_TitleBgActive).
		FLinearColor color{1.f, 0.f, 0.f, 1.f};
		if (Settings.MENU.Rgb)
		{
			const Color c = Rgb::Current();
			color = FLinearColor{c.R, c.G, c.B, c.A};
		}

		TextLine(0.f, "Splitgate Internal", color);
		TextLine(15.f, std::format("FPS: {:.0f}", Fps()), color);
	};

  private:
	/// Draw one left-aligned line at the top-left. Render::Text centers on pos.x, so offset by half the
	/// measured width to put the left edge at x = 0.
	void TextLine(float y, const std::string& text, const FLinearColor& color)
	{
		const float scale = 1.f;
		const float width = Render::Measure(text, scale);
		Render::Text(FVector2D{width * 0.5f, y}, text, scale, color);
	};

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
