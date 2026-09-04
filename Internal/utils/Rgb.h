#pragma once

/// @file
/// @brief The time-based rainbow color behind the "RGB" menu toggle
/// (Settings.MENU.Rgb), shared by the watermark, menu accent, and radar self-icon.

#include "../settings/Settings.h"

#include <chrono>
#include <cmath>

/// @brief The cycling rainbow color driving the optional "RGB" menu accent.
namespace Rgb
{
	/// Full sweep of the hue wheel, in seconds.
	inline constexpr double Period = 5.0;

	/**
	 * The current point on the rainbow, driven off the wall clock so it advances on its own
	 * regardless of frame rate. Full saturation and value; alpha is always 1.
	 *
	 * @return an opaque RGBA @ref Color to feed ImGui / an FLinearColor at the call site.
	 */
	inline Color Current()
	{
		using namespace std::chrono;
		const double now = duration<double>(steady_clock::now().time_since_epoch()).count();
		const float hue = static_cast<float>(std::fmod(now / Period, 1.0)); // [0, 1)

		// HSV(hue, 1, 1) -> RGB.
		const float h = hue * 6.f;
		const float x = 1.f - std::fabs(std::fmod(h, 2.f) - 1.f);
		float r = 0.f, g = 0.f, b = 0.f;
		switch (static_cast<int>(h) % 6)
		{
		case 0: r = 1.f, g = x; break;
		case 1: r = x, g = 1.f; break;
		case 2: g = 1.f, b = x; break;
		case 3: g = x, b = 1.f; break;
		case 4: r = x, b = 1.f; break;
		default: r = 1.f, b = x; break; // case 5
		}
		return Color{r, g, b, 1.f};
	}
} // namespace Rgb
