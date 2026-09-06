#pragma once

/// @file
/// @brief Render::Palette — the app's named color palette, built on the neutral Render::Color. These
/// are the theme's semantic colors (lifted from the menu's cream/red style); the menu theme and the
/// watermark reference them instead of repeating literals. Platform-neutral: it includes only the
/// core Color.h, and the consumer brings whichever adapter its `.To<T>()` needs.

#include "Color.h"

namespace Render
{
	/// The menu's cream/red theme palette. Reuse `.Alpha(a)` for a color at a different opacity.
	namespace Palette
	{
		inline constexpr Color Primary{1.00f, 0.00f, 0.00f, 1.00f}; ///< the accent red (titles, active controls)
		inline constexpr Color Text{0.40f, 0.39f, 0.38f, 1.00f};	///< body text on the cream ground
		inline constexpr Color Cream{0.92f, 0.91f, 0.88f, 1.00f};	///< window background
		inline constexpr Color Frame{1.00f, 0.98f, 0.95f, 1.00f};	///< lighter frame/child/title background
		inline constexpr Color Border{0.84f, 0.83f, 0.80f, 1.00f};	///< border tint
		inline constexpr Color White{1.00f, 1.00f, 1.00f, 1.00f};
		inline constexpr Color Black{0.00f, 0.00f, 0.00f, 1.00f};
		inline constexpr Color Blue{0.10f, 0.40f, 0.75f, 1.00f};  ///< separator/interaction accent
		inline constexpr Color Slate{0.43f, 0.43f, 0.50f, 1.00f}; ///< neutral separator gray
	} // namespace Palette
} // namespace Render
