#pragma once

/// @file
/// @brief Combo rich widgets: BeginCombo / EndCombo — a combo whose item list the caller draws inline.

#include "Core.h"

namespace UCanvasGUI
{
	inline bool comboOpen[256];  ///< per-id open state for BeginCombo
	/// Combo box that opens an inline item list (drawn by the caller via Selectable); returns open.
	inline bool BeginCombo(const char* name, const char* preview, float width)
	{
		elementCount++;
		const int id = elementCount;

		const FVector2D size{width, 22.0f};
		const FVector2D pos = NextPos(FVector2D{10, 10}, size.Y + 10.0f);
		const bool isHovered = MouseInZone(pos, size);

		DrawRect(pos, size.X, size.Y, Colors::Frame);
		if (isHovered) elementHovered = true;

		if (preview && preview[0]) TextLeft(preview, FVector2D{pos.X + 4.0f, pos.Y + size.Y / 2}, Colors::Text, false);
		if (name && name[0] && name[0] != '#')
			TextLeft(name, FVector2D{pos.X + size.X + 5.0f, pos.Y + size.Y / 2}, Colors::Text, false);

		if (isHovered && Input::IsMouseClicked(0, elementCount, false))
			comboOpen[id] = !comboOpen[id];

		EndElement(pos, size);
		return comboOpen[id];
	}
	inline void EndCombo() {} ///< items drew inline; nothing to close
} // namespace UCanvasGUI
