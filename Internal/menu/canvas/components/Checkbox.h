#pragma once

/// @file
/// @brief Checkbox widget: a labelled boolean toggle with a filled mark when enabled.

#include "Core.h"

namespace UCanvasGUI
{
	inline bool Checkbox(const char* name, bool* value)
	{
		elementCount++;

		constexpr float box = 18.0f;
		const FVector2D pos = NextPos({10, 10}, box + 10.0f);
		const bool isHovered = MouseInZone(pos, FVector2D{box, box});

		DrawRect(pos, box, box, Colors::Accent);
		if (isHovered)
			elementHovered = true;
		if (*value)
			DrawRect(FVector2D{pos.X + 3, pos.Y + 3}, box - 6, box - 6, Colors::Enabled);

		TextLeft(name, FVector2D{pos.X + box + 5.0f, pos.Y + box / 2}, Colors::Text, false);

		EndElement(pos);

		if (isHovered && Input::IsMouseClicked(0, elementCount, false))
		{
			*value = !*value;
			return true;
		}
		return false;
	}
} // namespace UCanvasGUI
