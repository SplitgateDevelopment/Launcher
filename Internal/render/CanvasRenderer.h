#pragma once

/// @file
/// @brief Renderer backend that draws through the UE canvas (K2_DrawLine / K2_DrawText). Each call
/// is a ProcessEvent — the default, but the reason ImGuiRenderer exists.

#include <cmath>

#include "Renderer.h"
#include "../utils/Globals.h"

class CanvasRenderer : public Renderer
{
  public:
	void Line(const FVector2D& a, const FVector2D& b, float thickness, const FLinearColor& color) override
	{
		if (Globals::Canvas) Globals::Canvas->K2_DrawLine(a, b, thickness, color);
	}

	void Text(const FVector2D& pos, const std::string& text, float scale, const FLinearColor& color) override
	{
		if (Globals::Canvas)
			Globals::Canvas->K2_DrawText(0, FString(text), pos, {scale, scale}, color, 1.f, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f}, true, false, true, {0.f, 0.f, 0.f, 1.f});
	}

	void RectFilled(const FVector2D& min, const FVector2D& max, const FLinearColor& color) override
	{
		// The UE canvas has no cheap color-fill (only textured/outline draws), so approximate with
		// horizontal lines. Cap the count so a large rect can't spam ProcessEvent; thickness = step
		// keeps the fill solid.
		if (!Globals::Canvas) return;
		const float height = max.Y - min.Y;
		if (height <= 0.f || max.X <= min.X) return;

		constexpr int maxLines = 200;
		float step = height / maxLines;
		if (step < 1.f) step = 1.f;
		for (float y = min.Y; y < max.Y; y += step)
			Globals::Canvas->K2_DrawLine({min.X, y}, {max.X, y}, step + 1.f, color);
	}

	void CircleFilled(const FVector2D& center, float radius, const FLinearColor& color) override
	{
		// Scanline fill: one horizontal chord per row, chord half-width = sqrt(r^2 - dy^2). Bounded.
		if (!Globals::Canvas || radius <= 0.f) return;

		constexpr int maxLines = 200;
		float step = (radius * 2.f) / maxLines;
		if (step < 1.f) step = 1.f;
		for (float dy = -radius; dy <= radius; dy += step)
		{
			const float halfWidth = std::sqrt((std::max)(0.f, radius * radius - dy * dy));
			Globals::Canvas->K2_DrawLine({center.X - halfWidth, center.Y + dy}, {center.X + halfWidth, center.Y + dy}, step + 1.f, color);
		}
	}

	void RectGradient(const FVector2D& min, const FVector2D& max, const FLinearColor& top, const FLinearColor& bottom) override
	{
		if (!Globals::Canvas) return;
		const float height = max.Y - min.Y;
		if (height <= 0.f || max.X <= min.X) return;

		constexpr int maxLines = 200;
		float step = height / maxLines;
		if (step < 1.f) step = 1.f;
		for (float y = min.Y; y < max.Y; y += step)
		{
			const float t = (y - min.Y) / height;
			const FLinearColor c{
				top.R + (bottom.R - top.R) * t, top.G + (bottom.G - top.G) * t,
				top.B + (bottom.B - top.B) * t, top.A + (bottom.A - top.A) * t};
			Globals::Canvas->K2_DrawLine({min.X, y}, {max.X, y}, step + 1.f, c);
		}
	}
};
