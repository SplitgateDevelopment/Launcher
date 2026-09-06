#pragma once

/// @file
/// @brief Button widget: a clickable labelled button; returns true on the click frame.

#include "Core.h"

namespace UCanvasGUI
{
	inline bool Button(const char* name, FVector2D size)
	{
		elementCount++;

		const FVector2D pos = NextPos({5, 10}, size.Y + 10.0f);
		const bool isHovered = MouseInZone(pos, size);

		DrawRect(pos, size.X, size.Y, Colors::Accent);
		if (isHovered)
			elementHovered = true;

		TextCenter(name, FVector2D{pos.X + size.X / 2, pos.Y + size.Y / 2}, Colors::Text, false);

		EndElement(pos, size);
		return isHovered && Input::IsMouseClicked(0, elementCount, false);
	}
} // namespace UCanvasGUI
