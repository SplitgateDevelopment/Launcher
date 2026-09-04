#pragma once

/// @file
/// @brief Renderer backend that draws nothing — a global "hide overlays" and a zero-cost baseline
/// for measuring the canvas/ImGui drawing overhead.

#include "Renderer.h"

class NullRenderer : public Renderer
{
  public:
	void Line(const FVector2D&, const FVector2D&, float, const FLinearColor&) override {}
	void Text(const FVector2D&, const std::string&, float, const FLinearColor&) override {}
};
