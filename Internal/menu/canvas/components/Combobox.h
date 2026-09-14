#pragma once

/// @file
/// @brief Combobox widget: a dropdown over a string array. The open item list is queued through
/// PostRenderer so it replays on top of the widgets drawn below it.

#include "Core.h"

namespace UCanvasGUI
{
	inline bool comboboxOpen[256]; ///< per-id open state for the array Combobox

	/// Dropdown combo over @p count option strings; writes the picked index to @p value.
	/// @return true on the frame the selection changes.
	inline bool Combobox(const char* name, FVector2D size, int* value, const char* const* items, int count)
	{
		elementCount++;
		const int id = elementCount;
		bool changed = false;

		const FVector2D pos = NextPos({5, 10}, size.Y + 10.0f);
		const bool isHovered = MouseInZone(pos, size);

		DrawRect(pos, size.X, size.Y, Colors::Frame);
		if (isHovered || comboboxOpen[id])
			elementHovered = true;

		TextLeft(name, FVector2D{pos.X + size.X + 5.0f, pos.Y + size.Y / 2}, Colors::Text, false);

		// The selected label sits on the closed box; the full list draws (on top) only when open.
		bool overItem = false;
		FVector2D itemPos = pos;
		for (int num = 0; num < count; num++)
		{
			const char* item = items[num];
			if (num == *value)
				TextCenter(item, FVector2D{pos.X + size.X / 2, pos.Y + size.Y / 2}, Colors::Text, false);

			if (!comboboxOpen[id])
				continue;

			itemPos.Y += 25.0f;
			const bool overThis = MouseInZone(itemPos, FVector2D{size.X, 25.0f});
			overItem = overItem || overThis;

			PostRenderer::DrawRect(itemPos, size.X, 25.0f, Colors::Frame);
			if (overThis)
			{
				elementHovered = true;
				if (Input::IsMouseClicked(0, elementCount, false))
				{
					*value = num;
					changed = true;
					comboboxOpen[id] = false;
				}
			}
			PostRenderer::TextLeft(item, FVector2D{itemPos.X + 5.0f, itemPos.Y + 15.0f}, Colors::Text, false);
		}

		EndElement(pos, size);

		// Toggle open on the box; close when clicking away from both the box and the list.
		if (Input::IsMouseClicked(0, elementCount, false))
		{
			if (isHovered)
				comboboxOpen[id] = !comboboxOpen[id];
			else if (!overItem)
				comboboxOpen[id] = false;
		}

		return changed;
	}
} // namespace UCanvasGUI
