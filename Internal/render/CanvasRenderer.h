#pragma once

/// @file
/// @brief Renderer backend that draws through the UE canvas (K2_DrawLine / K2_DrawText). Each call
/// is a ProcessEvent — the default, but the reason ImGuiRenderer exists.

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
};
