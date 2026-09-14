/// @file
/// @brief Unit tests for the rainbow color helper (Internal/utils/Rgb.h).
///
/// Rgb::Current() is clock-driven and not deterministic, so the pure HSV->RGB mapping was extracted
/// into Rgb::FromHue(); these tests pin its anchor colors and its full-saturation / full-value
/// invariants across a hue sweep.
#define NOMINMAX // Settings.h (pulled in by Rgb.h) includes <Windows.h>

#include <algorithm>
#include <cmath>

#include <gtest/gtest.h>

// Tests' IncludePath doesn't cover Internal/utils, so reach the header by relative path.
#include "../Internal/utils/Rgb.h"

namespace
{
	constexpr float kEps = 1e-4f;

	void ExpectColor(const Color& c, float r, float g, float b)
	{
		EXPECT_NEAR(c.R, r, kEps);
		EXPECT_NEAR(c.G, g, kEps);
		EXPECT_NEAR(c.B, b, kEps);
		EXPECT_FLOAT_EQ(c.A, 1.f);
	}
} // namespace

// Anchor at hues that are exactly representable in float (0 and 0.5), so the sextant branch can't
// straddle a boundary: hue 0 -> red, hue 0.5 (h == 3.0) -> cyan.
TEST(RgbTest, AnchorHuesAreExact)
{
	ExpectColor(Rgb::FromHue(0.f), 1.f, 0.f, 0.f); // red
	ExpectColor(Rgb::FromHue(0.5f), 0.f, 1.f, 1.f); // cyan
}

// Hues outside [0,1) wrap, so a whole-turn offset yields the same color.
TEST(RgbTest, HueWrapsIntoUnitRange)
{
	ExpectColor(Rgb::FromHue(1.f), 1.f, 0.f, 0.f);	// == FromHue(0)
	ExpectColor(Rgb::FromHue(2.5f), 0.f, 1.f, 1.f); // == FromHue(0.5)
	ExpectColor(Rgb::FromHue(-1.f), 1.f, 0.f, 0.f); // negative wraps too
}

// HSV(hue, 1, 1) is always fully saturated (min channel 0) and full value (max channel 1), opaque.
TEST(RgbTest, SweepIsFullySaturatedFullValueAndOpaque)
{
	for (int i = 0; i < 600; ++i)
	{
		const float hue = i / 600.f;
		const Color c = Rgb::FromHue(hue);

		EXPECT_GE(c.R, 0.f);
		EXPECT_LE(c.R, 1.f);
		EXPECT_GE(c.G, 0.f);
		EXPECT_LE(c.G, 1.f);
		EXPECT_GE(c.B, 0.f);
		EXPECT_LE(c.B, 1.f);

		const float hi = (std::max)({c.R, c.G, c.B});
		const float lo = (std::min)({c.R, c.G, c.B});
		EXPECT_NEAR(hi, 1.f, kEps) << "value should be 1 at hue " << hue;
		EXPECT_NEAR(lo, 0.f, kEps) << "saturation should be 1 at hue " << hue;
		EXPECT_FLOAT_EQ(c.A, 1.f);
	}
}
