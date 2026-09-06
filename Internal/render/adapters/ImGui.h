#pragma once

/// @file
/// @brief ImGui adapter: bridges the neutral Render::Vec2 / Render::Color to ImVec2 / ImVec4 by
/// specializing the render customization points, plus a `ToU32` helper for the packed draw-list
/// color. The only render header that names ImGui types — include it wherever `.To<ImVec2>()` /
/// `.To<ImVec4>()` / `ToU32` (or an implicit conversion from ImVec2/ImVec4) is needed.

#include <imgui.h>

#include "../Vec2.h"
#include "../Color.h"

namespace Render
{
	template <>
	struct Vec2Traits<ImVec2>
	{
		static Vec2 From(const ImVec2& v) { return {v.x, v.y}; }
		static ImVec2 To(const Vec2& v) { return ImVec2(v.x, v.y); }
	};

	template <>
	struct ColorTraits<ImVec4>
	{
		static Color From(const ImVec4& c) { return {c.x, c.y, c.z, c.w}; }
		static ImVec4 To(const Color& c) { return ImVec4(c.r, c.g, c.b, c.a); }
	};

	/// Pack a color into ImGui's draw-list ImU32 (respects ImGui's channel packing). A free helper
	/// rather than a `ColorTraits<ImU32>` specialization, since ImU32 is a scalar alias (unsigned int).
	inline ImU32 ToU32(const Color& c)
	{
		return ImGui::ColorConvertFloat4ToU32(c.To<ImVec4>());
	}
} // namespace Render
