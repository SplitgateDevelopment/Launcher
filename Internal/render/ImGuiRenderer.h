#pragma once

/// @file
/// @brief Renderer backend that draws via ImGui (near-free, no ProcessEvent). The features run in
/// PostRender but ImGui draws only inside the Present-hook frame, so Line/Text *record* commands
/// into a per-frame buffer that Flush() replays into the background draw list (called from
/// GUI::Overlay after ImGui::NewFrame). See docs/planned-features.md feature 6.

#include <mutex>
#include <string>
#include <vector>

#include "Renderer.h"
#include "adapters/ImGui.h" // ImVec2/ImVec4/ImU32 conversions + Render::ToU32

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
		bool centered;
	};
	struct RectCmd
	{
		ImVec2 min, max;
		ImU32 color;
	};
	struct BoxCmd
	{
		ImVec2 min, max;
		float thickness;
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
	std::vector<BoxCmd> boxes;
	std::vector<CircleCmd> circles;
	std::vector<GradientCmd> gradients;

	// Features record (game thread, from PostRender) while Flush replays (render thread, from Present)
	// ~a frame later — the same vectors touched from two threads. Guard every access: recording locks
	// briefly per command, and Flush swaps the buffers out under the lock, then replays the snapshot
	// without holding it. Without this, a concurrent push_back reallocating a vector mid-replay is an
	// access violation.
	std::mutex mtx;

  public:
	void Line(const Render::Vec2& a, const Render::Vec2& b, float thickness, const Render::Color& color) override
	{
		std::lock_guard<std::mutex> guard(mtx);
		lines.push_back({a.To<ImVec2>(), b.To<ImVec2>(), thickness, Render::ToU32(color)});
	}

	void Text(const Render::Vec2& pos, const std::string& text, float scale, const Render::Color& color, bool centered) override
	{
		std::lock_guard<std::mutex> guard(mtx);
		texts.push_back({pos.To<ImVec2>(), text, scale, Render::ToU32(color), centered});
	}

	Render::Vec2 TextSize(const std::string& text, float scale) override
	{
		ImFont* font = ImGui::GetFont();
		if (!font) return {text.length() * scale * 7.f, scale * 14.f};
		const ImVec2 size = font->CalcTextSizeA(font->FontSize * scale, FLT_MAX, 0.f, text.c_str());
		return {size.x, size.y};
	}

	Render::Vec2 StrLen(const std::string& text) override { return TextSize(text, 1.f); }

	void RectFilled(const Render::Vec2& min, const Render::Vec2& max, const Render::Color& color) override
	{
		std::lock_guard<std::mutex> guard(mtx);
		rects.push_back({min.To<ImVec2>(), max.To<ImVec2>(), Render::ToU32(color)});
	}

	void Rect(const Render::Vec2& min, const Render::Vec2& max, float thickness, const Render::Color& color) override
	{
		std::lock_guard<std::mutex> guard(mtx);
		boxes.push_back({min.To<ImVec2>(), max.To<ImVec2>(), thickness, Render::ToU32(color)});
	}

	void CircleFilled(const Render::Vec2& center, float radius, const Render::Color& color) override
	{
		std::lock_guard<std::mutex> guard(mtx);
		circles.push_back({center.To<ImVec2>(), radius, Render::ToU32(color)});
	}

	void RectGradient(const Render::Vec2& min, const Render::Vec2& max, const Render::Color& top, const Render::Color& bottom) override
	{
		std::lock_guard<std::mutex> guard(mtx);
		gradients.push_back({min.To<ImVec2>(), max.To<ImVec2>(), Render::ToU32(top), Render::ToU32(bottom)});
	}

	/// Replay this frame's recorded commands into the current ImGui context's background draw list.
	/// Call once per frame from the Present hook, after ImGui::NewFrame(). Delegates to the explicit
	/// overload with this context's draw list/font. No-op when nothing was recorded.
	void Flush()
	{
		Flush(ImGui::GetBackgroundDrawList(), ImGui::GetFont(), ImGui::GetFontSize());
	}

	/// Replay this frame's recorded commands into an explicitly supplied draw list. Lets a second
	/// ImGui context (the streamproof external overlay window) replay the same recorded commands into
	/// its own draw list; the no-arg overload targets the current context. Swaps the recorded commands
	/// out under the lock into a local snapshot, then replays that — so it never iterates a buffer the
	/// game thread is appending to. No-op when nothing was recorded or the draw list/font isn't ready.
	/// @param drawList target draw list (e.g. the target context's background draw list); may be null.
	/// @param font     font to lay text out with; may be null (whole call becomes a no-op).
	/// @param baseSize the target context's base font size, scaled per Text command.
	void Flush(ImDrawList* drawList, ImFont* font, float baseSize)
	{
		// Snapshot this frame's commands and reset the recording buffers, holding the lock only for the
		// (O(1)) swaps.
		std::vector<LineCmd> l;
		std::vector<TextCmd> t;
		std::vector<RectCmd> r;
		std::vector<BoxCmd> b;
		std::vector<CircleCmd> c;
		std::vector<GradientCmd> g;
		{
			std::lock_guard<std::mutex> guard(mtx);
			l.swap(lines);
			t.swap(texts);
			r.swap(rects);
			b.swap(boxes);
			c.swap(circles);
			g.swap(gradients);
		}

		if (!drawList || !font) return; // atlas/draw list not ready (e.g. mid device reset)

		// Fills first so strokes/text draw on top.
		for (const auto& gr : g)
			drawList->AddRectFilledMultiColor(gr.min, gr.max, gr.top, gr.top, gr.bottom, gr.bottom);
		for (const auto& re : r)
			drawList->AddRectFilled(re.min, re.max, re.color);
		for (const auto& ci : c)
			drawList->AddCircleFilled(ci.center, ci.radius, ci.color);

		// Strokes: rect outlines then lines.
		for (const auto& bo : b)
			drawList->AddRect(bo.min, bo.max, bo.color, 0.f, 0, bo.thickness);
		for (const auto& li : l)
			drawList->AddLine(li.a, li.b, li.color, li.thickness);

		for (const auto& te : t)
		{
			const float size = baseSize * te.scale;
			// Centered text is offset left by half its width; left-aligned text sits at pos.x as-is.
			const float x = te.centered ? te.pos.x - font->CalcTextSizeA(size, FLT_MAX, 0.f, te.text.c_str()).x * 0.5f : te.pos.x;
			drawList->AddText(font, size, ImVec2(x, te.pos.y), te.color, te.text.c_str());
		}
	}
};
