#pragma once

/// @file
/// @brief The active drawing backend. Features call Render::Line/Text; Render::Select swaps the
/// backend (canvas / imgui) live from the setting, and Render::Flush (from the Present hook)
/// replays the ImGui backend's recorded commands.

#include "CanvasRenderer.h"
#include "ImGuiRenderer.h"
#include "NullRenderer.h"
#include "Adapters.h" // UE/ImGui/settings conversions, so every call site has them in scope
#include "../settings/Settings.h"

namespace Render
{
	inline CanvasRenderer canvas;
	inline ImGuiRenderer imgui;
	inline NullRenderer null;

	/// The backends, indexed by RendererMode (order must match the enum). Adding a renderer is a new
	/// enum value + a new entry here — no branching at the call site.
	inline Renderer* const Backends[] = {
		&canvas, // RendererMode::Canvas
		&imgui,	 // RendererMode::ImGui
		&null,	 // RendererMode::Null
		&imgui,	 // RendererMode::External — same recorder; replayed into the external overlay window
	};

	inline Renderer* active = Backends[0];

	/// Point the active backend at the one the setting selects (called each frame before drawing).
	inline void Select(RendererMode mode)
	{
		const size_t index = static_cast<size_t>(mode);
		active = (index < (sizeof(Backends) / sizeof(*Backends))) ? Backends[index] : &canvas;
	}

	inline void Line(const Vec2& a, const Vec2& b, float thickness, const Color& color)
	{
		active->Line(a, b, thickness, color);
	}

	inline void Text(const Vec2& pos, const std::string& text, float scale, const Color& color, bool centered = true)
	{
		active->Text(pos, text, scale, color, centered);
	}

	inline float Measure(const std::string& text, float scale)
	{
		return active->Measure(text, scale);
	}

	/// Screen size (width, height) of @p text at @p scale — parity with UCanvas::K2_TextSize.
	inline Vec2 TextSize(const std::string& text, float scale)
	{
		return active->TextSize(text, scale);
	}

	/// Screen size of @p text at scale 1 — parity with UCanvas::K2_StrLen.
	inline Vec2 StrLen(const std::string& text)
	{
		return active->StrLen(text);
	}

	inline void RectFilled(const Vec2& min, const Vec2& max, const Color& color)
	{
		active->RectFilled(min, max, color);
	}

	/// Draw the outline of the rectangle @p min..@p max — parity with UCanvas::K2_DrawBox.
	inline void Rect(const Vec2& min, const Vec2& max, float thickness, const Color& color)
	{
		active->Rect(min, max, thickness, color);
	}

	inline void CircleFilled(const Vec2& center, float radius, const Color& color)
	{
		active->CircleFilled(center, radius, color);
	}

	inline void RectGradient(const Vec2& min, const Vec2& max, const Color& top, const Color& bottom)
	{
		active->RectGradient(min, max, top, bottom);
	}

	/// Replay the ImGui backend's per-frame command buffer into the current context's background draw
	/// list (no-op in canvas mode — the buffer is empty). Call once per frame from the Present hook
	/// after ImGui::NewFrame().
	inline void Flush()
	{
		imgui.Flush();
	}

	/// Replay the ImGui backend's per-frame command buffer into an explicit draw list — used by the
	/// streamproof external overlay window, which has its own ImGui context. @see ImGuiRenderer::Flush.
	inline void Flush(ImDrawList* drawList, ImFont* font, float baseSize)
	{
		imgui.Flush(drawList, font, baseSize);
	}
} // namespace Render
