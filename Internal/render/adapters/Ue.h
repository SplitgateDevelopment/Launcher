#pragma once

/// @file
/// @brief UE adapter: bridges the neutral Render::Vec2 / Render::Color to the UE SDK's FVector2D /
/// FLinearColor by specializing the render customization points. This is the only render header that
/// names UE types — include it wherever `.To<FVector2D>()` / `.To<FLinearColor>()` (or an implicit
/// conversion from those) is needed.

#include "../Vec2.h"
#include "../Color.h"
#include "../../ue/sdk/FVector2D.h"
#include "../../ue/sdk/FLinearColor.h"

namespace Render
{
	template <>
	struct Vec2Traits<FVector2D>
	{
		static Vec2 From(const FVector2D& v) { return {v.X, v.Y}; }
		static FVector2D To(const Vec2& v) { return {v.x, v.y}; }
	};

	template <>
	struct ColorTraits<FLinearColor>
	{
		static Color From(const FLinearColor& c) { return {c.R, c.G, c.B, c.A}; }
		static FLinearColor To(const Color& c) { return {c.r, c.g, c.b, c.a}; }
	};
} // namespace Render
