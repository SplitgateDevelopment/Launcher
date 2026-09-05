#pragma once

/// @file
/// @brief Renderer backend that draws via ImGui (near-free, no ProcessEvent). The features run in
/// PostRender but ImGui draws only inside the Present-hook frame, so Line/Text *record* commands
/// into a per-frame buffer that Flush() replays into the background draw list (called from
/// GUI::Overlay after ImGui::NewFrame). See docs/planned-features.md feature 6.

#include <mutex>
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

	// Features record (game thread, from PostRender) while Flush replays (render thread, from Present)
	// ~a frame later — the same vectors touched from two threads. Guard every access: recording locks
	// briefly per command, and Flush swaps the buffers out under the lock, then replays the snapshot
	// without holding it. Without this, a concurrent push_back reallocating a vector mid-replay is an
	// access violation.
	std::mutex mtx;

	static ImU32 ToU32(const FLinearColor& c) { return ImGui::ColorConvertFloat4ToU32(ImVec4(c.R, c.G, c.B, c.A)); }

  public:
	void Line(const FVector2D& a, const FVector2D& b, float thickness, const FLinearColor& color) override
	{
		std::lock_guard<std::mutex> guard(mtx);
		lines.push_back({ImVec2(a.X, a.Y), ImVec2(b.X, b.Y), thickness, ToU32(color)});
	}

	void Text(const FVector2D& pos, const std::string& text, float scale, const FLinearColor& color) override
	{
		std::lock_guard<std::mutex> guard(mtx);
		texts.push_back({ImVec2(pos.X, pos.Y), text, scale, ToU32(color)});
	}

	float Measure(const std::string& text, float scale) override
	{
		ImFont* font = ImGui::GetFont();
		if (!font) return text.length() * scale * 7.f;
		return font->CalcTextSizeA(font->FontSize * scale, FLT_MAX, 0.f, text.c_str()).x;
	}

	void RectFilled(const FVector2D& min, const FVector2D& max, const FLinearColor& color) override
	{
		std::lock_guard<std::mutex> guard(mtx);
		rects.push_back({ImVec2(min.X, min.Y), ImVec2(max.X, max.Y), ToU32(color)});
	}

	void CircleFilled(const FVector2D& center, float radius, const FLinearColor& color) override
	{
		std::lock_guard<std::mutex> guard(mtx);
		circles.push_back({ImVec2(center.X, center.Y), radius, ToU32(color)});
	}

	void RectGradient(const FVector2D& min, const FVector2D& max, const FLinearColor& top, const FLinearColor& bottom) override
	{
		std::lock_guard<std::mutex> guard(mtx);
		gradients.push_back({ImVec2(min.X, min.Y), ImVec2(max.X, max.Y), ToU32(top), ToU32(bottom)});
	}

	/// Replay this frame's recorded commands into the background draw list. Call once per frame from
	/// the Present hook, after ImGui::NewFrame(). Swaps the recorded commands out under the lock into a
	/// local snapshot, then replays that — so it never iterates a buffer the game thread is appending
	/// to. No-op when nothing was recorded.
	void Flush()
	{
		// Snapshot this frame's commands and reset the recording buffers, holding the lock only for the
		// (O(1)) swaps.
		std::vector<LineCmd> l;
		std::vector<TextCmd> t;
		std::vector<RectCmd> r;
		std::vector<CircleCmd> c;
		std::vector<GradientCmd> g;
		{
			std::lock_guard<std::mutex> guard(mtx);
			l.swap(lines);
			t.swap(texts);
			r.swap(rects);
			c.swap(circles);
			g.swap(gradients);
		}

		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		ImFont* font = ImGui::GetFont();
		if (!drawList || !font) return; // atlas/draw list not ready (e.g. mid device reset)
		const float baseSize = ImGui::GetFontSize();

		// Fills first so lines/text draw on top.
		for (const auto& gr : g)
			drawList->AddRectFilledMultiColor(gr.min, gr.max, gr.top, gr.top, gr.bottom, gr.bottom);
		for (const auto& re : r)
			drawList->AddRectFilled(re.min, re.max, re.color);
		for (const auto& ci : c)
			drawList->AddCircleFilled(ci.center, ci.radius, ci.color);

		for (const auto& li : l)
			drawList->AddLine(li.a, li.b, li.color, li.thickness);

		for (const auto& te : t)
		{
			const float size = baseSize * te.scale;
			const ImVec2 dim = font->CalcTextSizeA(size, FLT_MAX, 0.f, te.text.c_str());
			drawList->AddText(font, size, ImVec2(te.pos.x - dim.x * 0.5f, te.pos.y), te.color, te.text.c_str());
		}
	}
};
