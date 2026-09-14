#pragma once

/// @file
/// @brief Tab button: a selectable tab-strip entry (ButtonTab), highlighted while active/hovered.

#include "Core.h"

namespace UCanvasGUI
{
	inline bool ButtonTab(const char* name, FVector2D size, bool active)
	{
		elementCount++;

		const FVector2D pos = NextPos({5, 10}, size.Y + 10.0f);
		const bool isHovered = MouseInZone(pos, size);

		DrawRect(pos, size.X, size.Y, Colors::Accent);
		if (isHovered && !active)
			elementHovered = true;

		TextCenter(name, FVector2D{pos.X + size.X / 2, pos.Y + size.Y / 2}, Colors::Text, false);

		EndElement(pos, size);
		return isHovered && Input::IsMouseClicked(0, elementCount, false);
	}
} // namespace UCanvasGUI
