#pragma once

/// @file
/// @brief Abstract drawing backend. The visual features draw through this instead of the canvas
/// directly, so a UE-canvas backend and an ImGui backend can be swapped at runtime (strategy
/// pattern). Its surface is backend-neutral — positions are Render::Vec2, colors are Render::Color;
/// each backend converts to its own types (FVector2D/FLinearColor, ImVec2/ImU32) internally.

#include <string>

#include "Vec2.h"
#include "Color.h"

/// Drawing primitives the visual features need. Implemented by CanvasRenderer and ImGuiRenderer.
class Renderer
{
  public:
	virtual ~Renderer() = default;

	/// Draw a line between two screen points.
	virtual void Line(const Render::Vec2& a, const Render::Vec2& b, float thickness, const Render::Color& color) = 0;

	/// Draw text at @p pos. Centred horizontally on @p pos when @p centered (the default), else
	/// left-aligned with its left edge at @p pos.x. @p scale is relative to the base font size.
	virtual void Text(const Render::Vec2& pos, const std::string& text, float scale, const Render::Color& color, bool centered) = 0;

	/// Screen size (width, height) of @p text at @p scale — parity with UCanvas::K2_TextSize. The
	/// base is a rough approximation; the ImGui/canvas backends override it with real font metrics.
	/// The metric is approximate across backends (UE atlas vs ImGui atlas differ).
	virtual Render::Vec2 TextSize(const std::string& text, float scale) { return {text.length() * scale * 7.f, scale * 14.f}; }

	/// Screen size of @p text at scale 1 — parity with UCanvas::K2_StrLen.
	virtual Render::Vec2 StrLen(const std::string& text) { return TextSize(text, 1.f); }

	/// Pixel width of @p text at @p scale, for laying out adjacent labels. Delegates to TextSize so
	/// width has a single source of truth.
	virtual float Measure(const std::string& text, float scale) { return TextSize(text, scale).x; }

	/// Fill the axis-aligned rectangle from @p min (top-left) to @p max (bottom-right). ImGui fills
	/// natively; the canvas backend approximates it with (bounded) horizontal lines.
	virtual void RectFilled(const Render::Vec2& min, const Render::Vec2& max, const Render::Color& color) = 0;

	/// Draw the outline (stroke only) of the axis-aligned rectangle @p min..@p max — parity with
	/// UCanvas::K2_DrawBox. Default is a no-op so a minimal backend need not implement it.
	virtual void Rect(const Render::Vec2& min, const Render::Vec2& max, float thickness, const Render::Color& color) {}

	/// Fill a circle at @p center with @p radius. ImGui fills natively; the canvas scanline-fills it.
	virtual void CircleFilled(const Render::Vec2& center, float radius, const Render::Color& color) = 0;

	/// Fill @p min..@p max with a vertical gradient (@p top at the top edge to @p bottom at the
	/// bottom). ImGui uses a multi-color rect; the canvas interpolates over (bounded) horizontal lines.
	virtual void RectGradient(const Render::Vec2& min, const Render::Vec2& max, const Render::Color& top, const Render::Color& bottom) = 0;
};
