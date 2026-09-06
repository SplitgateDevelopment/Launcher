#pragma once

/// @file
/// @brief Text widget: a laid-out (left- or center-aligned) text row that advances the layout cursor.

#include "Core.h"

namespace UCanvasGUI
{
	inline void Text(const char* text, bool center = false, bool outline = false)
	{
		elementCount++;

		constexpr float height = 25.0f;
		const FVector2D pos = NextPos({10, 10}, height + 10.0f);
		const FVector2D textPos{pos.X + 5.0f, pos.Y + height / 2};

		if (center)
			TextCenter(text, textPos, Colors::Text, outline);
		else
			TextLeft(text, textPos, Colors::Text, outline);

		EndElement(pos);
	}
} // namespace UCanvasGUI
