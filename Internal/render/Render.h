#pragma once

/// @file
/// @brief The active drawing backend. Features call Render::Line/Text; Render::Select swaps the
/// backend (canvas / imgui) live from the setting, and Render::Flush (from the Present hook)
/// replays the ImGui backend's recorded commands.

#include "CanvasRenderer.h"
#include "ImGuiRenderer.h"
#include "../settings/Settings.h"

namespace Render
{
	inline CanvasRenderer canvas;
	inline ImGuiRenderer imgui;
	inline Renderer* active = &canvas;

	/// Point the active backend at the one the setting selects (called each frame before drawing).
	inline void Select(RendererMode mode)
	{
		active = (mode == RendererMode::ImGui) ? static_cast<Renderer*>(&imgui) : static_cast<Renderer*>(&canvas);
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
