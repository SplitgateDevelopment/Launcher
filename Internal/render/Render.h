#pragma once

/// @file
/// @brief The active drawing backend. Features call Render::Line/Text; Render::Select swaps the
/// backend (canvas / imgui) live from the setting, and Render::Flush (from the Present hook)
/// replays the ImGui backend's recorded commands.

#include "CanvasRenderer.h"
#include "ImGuiRenderer.h"
#include "NullRenderer.h"
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
	};

	inline Renderer* active = Backends[0];

	/// Point the active backend at the one the setting selects (called each frame before drawing).
	inline void Select(RendererMode mode)
	{
		const size_t index = static_cast<size_t>(mode);
		active = (index < (sizeof(Backends) / sizeof(*Backends))) ? Backends[index] : &canvas;
	}

	inline void Line(const FVector2D& a, const FVector2D& b, float thickness, const FLinearColor& color)
	{
		active->Line(a, b, thickness, color);
	}

	inline void Text(const FVector2D& pos, const std::string& text, float scale, const FLinearColor& color)
	{
		active->Text(pos, text, scale, color);
	}

	/// Replay the ImGui backend's per-frame command buffer (no-op in canvas mode — the buffer is
	/// empty). Call once per frame from the Present hook after ImGui::NewFrame().
	inline void Flush()
	{
		imgui.Flush();
	}
} // namespace Render
