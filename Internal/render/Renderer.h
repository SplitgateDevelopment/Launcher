#pragma once

/// @file
/// @brief Abstract drawing backend. The visual features draw through this instead of the canvas
/// directly, so a UE-canvas backend and an ImGui backend can be swapped at runtime (strategy
/// pattern). Coordinates are screen-space FVector2D; colors are FLinearColor.

#include <string>

#include "../ue/Engine.h"

/// Drawing primitives the visual features need. Implemented by CanvasRenderer and ImGuiRenderer.
class Renderer
{
  public:
	virtual ~Renderer() = default;

	/// Draw a line between two screen points.
	virtual void Line(const FVector2D& a, const FVector2D& b, float thickness, const FLinearColor& color) = 0;

	/// Draw text centred horizontally at @p pos. @p scale is relative to the base font size.
	virtual void Text(const FVector2D& pos, const std::string& text, float scale, const FLinearColor& color) = 0;

	/// Fill the axis-aligned rectangle from @p min (top-left) to @p max (bottom-right). ImGui fills
	/// natively; the canvas backend approximates it with (bounded) horizontal lines.
	virtual void RectFilled(const FVector2D& min, const FVector2D& max, const FLinearColor& color) = 0;

	/// Fill a circle at @p center with @p radius. ImGui fills natively; the canvas scanline-fills it.
	virtual void CircleFilled(const FVector2D& center, float radius, const FLinearColor& color) = 0;

	/// Fill @p min..@p max with a vertical gradient (@p top at the top edge to @p bottom at the
	/// bottom). ImGui uses a multi-color rect; the canvas interpolates over (bounded) horizontal lines.
	virtual void RectGradient(const FVector2D& min, const FVector2D& max, const FLinearColor& top, const FLinearColor& bottom) = 0;
};
