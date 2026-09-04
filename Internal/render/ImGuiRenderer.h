#pragma once

/// @file
/// @brief Renderer backend that draws via ImGui (near-free, no ProcessEvent). The features run in
/// PostRender but ImGui draws only inside the Present-hook frame, so Line/Text *record* commands
/// into a per-frame buffer that Flush() replays into the background draw list (called from
/// GUI::Overlay after ImGui::NewFrame). See docs/planned-features.md feature 6.

#include <string>
#include <vector>

#include <imgui.h>

#include "Renderer.h"

class ImGuiRenderer : public Renderer
{
  private:
	struct LineCmd
	{
		ImVec2 a, b;
		float thickness;
		ImU32 color;
	};
	struct TextCmd
	{
		ImVec2 pos;
		std::string text;
		float scale;
		ImU32 color;
	};
	struct RectCmd
	{
		ImVec2 min, max;
		ImU32 color;
	};
	struct CircleCmd
	{
		ImVec2 center;
		float radius;
		ImU32 color;
	};
	struct GradientCmd
	{
		ImVec2 min, max;
		ImU32 top, bottom;
	};

	std::vector<LineCmd> lines;
	std::vector<TextCmd> texts;
	std::vector<RectCmd> rects;
	std::vector<CircleCmd> circles;
	std::vector<GradientCmd> gradients;

	static ImU32 ToU32(const FLinearColor& c) { return ImGui::ColorConvertFloat4ToU32(ImVec4(c.R, c.G, c.B, c.A)); }

  public:
	void Line(const FVector2D& a, const FVector2D& b, float thickness, const FLinearColor& color) override
	{
		lines.push_back({ImVec2(a.X, a.Y), ImVec2(b.X, b.Y), thickness, ToU32(color)});
	}

	void Text(const FVector2D& pos, const std::string& text, float scale, const FLinearColor& color) override
	{
		texts.push_back({ImVec2(pos.X, pos.Y), text, scale, ToU32(color)});
	}

	void RectFilled(const FVector2D& min, const FVector2D& max, const FLinearColor& color) override
	{
		rects.push_back({ImVec2(min.X, min.Y), ImVec2(max.X, max.Y), ToU32(color)});
	}

	void CircleFilled(const FVector2D& center, float radius, const FLinearColor& color) override
	{
		circles.push_back({ImVec2(center.X, center.Y), radius, ToU32(color)});
	}

	void RectGradient(const FVector2D& min, const FVector2D& max, const FLinearColor& top, const FLinearColor& bottom) override
	{
		gradients.push_back({ImVec2(min.X, min.Y), ImVec2(max.X, max.Y), ToU32(top), ToU32(bottom)});
	}

	/// Replay this frame's recorded commands into the background draw list, then clear. Call once
	/// per frame from the Present hook, after ImGui::NewFrame(). No-op when nothing was recorded.
	void Flush()
	{
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		ImFont* font = ImGui::GetFont();
		const float baseSize = ImGui::GetFontSize();

		// Fills first so lines/text draw on top.
		for (const auto& g : gradients)
			drawList->AddRectFilledMultiColor(g.min, g.max, g.top, g.top, g.bottom, g.bottom);
		for (const auto& r : rects)
			drawList->AddRectFilled(r.min, r.max, r.color);
		for (const auto& c : circles)
			drawList->AddCircleFilled(c.center, c.radius, c.color);

		for (const auto& l : lines)
			drawList->AddLine(l.a, l.b, l.color, l.thickness);

		for (const auto& t : texts)
		{
			const float size = baseSize * t.scale;
			const ImVec2 dim = font->CalcTextSizeA(size, FLT_MAX, 0.f, t.text.c_str());
			drawList->AddText(font, size, ImVec2(t.pos.x - dim.x * 0.5f, t.pos.y), t.color, t.text.c_str());
		}

		lines.clear();
		texts.clear();
		rects.clear();
		circles.clear();
		gradients.clear();
	}
};
