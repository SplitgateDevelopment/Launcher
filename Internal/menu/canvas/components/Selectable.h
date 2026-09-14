#pragma once

/// @file
/// @brief Selectable rich widget: a clickable text row highlighted when hovered/selected.

#include "Core.h"

namespace UCanvasGUI
{
	/// Clickable text row (no background unless hovered/selected); returns true on click.
	inline bool Selectable(const char* name, bool selected, float width)
	{
		elementCount++;

		const FVector2D size{width, 20.0f};
		const FVector2D pos = NextPos(FVector2D{10, 2}, size.Y + 2.0f);
		const bool isHovered = MouseInZone(pos, size);

		if (selected || isHovered)
		{
			DrawRect(pos, size.X, size.Y, isHovered ? Colors::Frame : Colors::Selection);
			if (isHovered) elementHovered = true;
		}
		TextLeft(name, FVector2D{pos.X + 4.0f, pos.Y + size.Y / 2}, Colors::Text, false);

		EndElement(pos, size);
		return isHovered && Input::IsMouseClicked(0, elementCount, false);
	}
} // namespace UCanvasGUI
