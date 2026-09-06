#pragma once

/// @file
/// @brief Renderer backend that draws through the UE canvas (K2_DrawLine / K2_DrawText / K2_DrawBox
/// / K2_TextSize / K2_StrLen). Each call is a ProcessEvent — the default, but the reason
/// ImGuiRenderer exists.

#include <cmath>

#include "Renderer.h"
#include "adapters/Ue.h"
#include "../ue/Engine.h"

class CanvasRenderer : public Renderer
{
  public:
	void Line(const Render::Vec2& a, const Render::Vec2& b, float thickness, const Render::Color& color) override
	{
		if (Engine::Canvas) Engine::Canvas->K2_DrawLine(a.To<FVector2D>(), b.To<FVector2D>(), thickness, color.To<FLinearColor>());
	}

	void Text(const Render::Vec2& pos, const std::string& text, float scale, const Render::Color& color, bool centered) override
	{
		if (Engine::Canvas)
			Engine::Canvas->K2_DrawText(0, FString(text), pos.To<FVector2D>(), {scale, scale}, color.To<FLinearColor>(), 1.f, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f}, centered, false, true, {0.f, 0.f, 0.f, 1.f});
	}

	Render::Vec2 TextSize(const std::string& text, float scale) override
	{
		if (!Engine::Canvas) return {0.f, 0.f};
		return Engine::Canvas->K2_TextSize(nullptr, FString(text), {scale, scale}); // null font -> Roboto fallback
	}

	Render::Vec2 StrLen(const std::string& text) override
	{
		if (!Engine::Canvas) return {0.f, 0.f};
		return Engine::Canvas->K2_StrLen(nullptr, FString(text));
	}

	void RectFilled(const Render::Vec2& min, const Render::Vec2& max, const Render::Color& color) override
	{
		// The UE canvas has no cheap color-fill (only textured/outline draws), so approximate with
		// horizontal lines. Cap the count so a large rect can't spam ProcessEvent; thickness = step
		// keeps the fill solid.
		if (!Engine::Canvas) return;
		const FVector2D mn = min.To<FVector2D>(), mx = max.To<FVector2D>();
		const FLinearColor col = color.To<FLinearColor>();
		const float height = mx.Y - mn.Y;
		if (height <= 0.f || mx.X <= mn.X) return;

		constexpr int maxLines = 200;
		float step = height / maxLines;
		if (step < 1.f) step = 1.f;
		for (float y = mn.Y; y < mx.Y; y += step)
			Engine::Canvas->K2_DrawLine({mn.X, y}, {mx.X, y}, step + 1.f, col);
	}

	void Rect(const Render::Vec2& min, const Render::Vec2& max, float thickness, const Render::Color& color) override
	{
		if (Engine::Canvas)
			Engine::Canvas->K2_DrawBox(min.To<FVector2D>(), {max.x - min.x, max.y - min.y}, thickness, color.To<FLinearColor>());
	}

	void CircleFilled(const Render::Vec2& center, float radius, const Render::Color& color) override
	{
		// Scanline fill: one horizontal chord per row, chord half-width = sqrt(r^2 - dy^2). Bounded.
		if (!Engine::Canvas || radius <= 0.f) return;
		const FVector2D c = center.To<FVector2D>();
		const FLinearColor col = color.To<FLinearColor>();

		constexpr int maxLines = 200;
		float step = (radius * 2.f) / maxLines;
		if (step < 1.f) step = 1.f;
		for (float dy = -radius; dy <= radius; dy += step)
		{
			const float halfWidth = std::sqrt((std::max)(0.f, radius * radius - dy * dy));
			Engine::Canvas->K2_DrawLine({c.X - halfWidth, c.Y + dy}, {c.X + halfWidth, c.Y + dy}, step + 1.f, col);
		}
	}

	void RectGradient(const Render::Vec2& min, const Render::Vec2& max, const Render::Color& top, const Render::Color& bottom) override
	{
		if (!Engine::Canvas) return;
		const FVector2D mn = min.To<FVector2D>(), mx = max.To<FVector2D>();
		const FLinearColor t = top.To<FLinearColor>(), bt = bottom.To<FLinearColor>();
		const float height = mx.Y - mn.Y;
		if (height <= 0.f || mx.X <= mn.X) return;

		constexpr int maxLines = 200;
		float step = height / maxLines;
		if (step < 1.f) step = 1.f;
		for (float y = mn.Y; y < mx.Y; y += step)
		{
			const float f = (y - mn.Y) / height;
			const FLinearColor c{
				t.R + (bt.R - t.R) * f, t.G + (bt.G - t.G) * f,
				t.B + (bt.B - t.B) * f, t.A + (bt.A - t.A) * f};
			Engine::Canvas->K2_DrawLine({mn.X, y}, {mx.X, y}, step + 1.f, c);
		}
	}
};
