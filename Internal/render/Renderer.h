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
};
