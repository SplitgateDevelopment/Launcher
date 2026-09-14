#pragma once

/// @file
/// @brief Renderer backend that draws nothing — a global "hide overlays" and a zero-cost baseline
/// for measuring the canvas/ImGui drawing overhead.

#include "Renderer.h"

class NullRenderer : public Renderer
{
  public:
	void Line(const Render::Vec2&, const Render::Vec2&, float, const Render::Color&) override {}
	void Text(const Render::Vec2&, const std::string&, float, const Render::Color&, bool) override {}
	Render::Vec2 TextSize(const std::string&, float) override { return {0.f, 0.f}; }
	Render::Vec2 StrLen(const std::string&) override { return {0.f, 0.f}; }
	void RectFilled(const Render::Vec2&, const Render::Vec2&, const Render::Color&) override {}
	void Rect(const Render::Vec2&, const Render::Vec2&, float, const Render::Color&) override {}
	void CircleFilled(const Render::Vec2&, float, const Render::Color&) override {}
	void RectGradient(const Render::Vec2&, const Render::Vec2&, const Render::Color&, const Render::Color&) override {}
};
