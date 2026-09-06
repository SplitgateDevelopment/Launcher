#pragma once

/// @file
/// @brief TextField rich widget: an editable single-line field (plus CharFromVK) with hint and trailing label.

#include "Core.h"

namespace UCanvasGUI
{
	inline int activeField = -1; ///< elementCount id of the focused text field
	/// Translate a virtual-key code to a printable character (basic US layout), honoring @p shift.
	inline char CharFromVK(int vk, bool shift)
	{
		if (vk >= 'A' && vk <= 'Z') return shift ? (char)vk : (char)(vk - 'A' + 'a');
		if (vk >= '0' && vk <= '9')
		{
			if (!shift) return (char)vk;
			static const char* sym = ")!@#$%^&*(";
			return sym[vk - '0'];
		}
		switch (vk)
		{
		case VK_SPACE:
			return ' ';
		case VK_OEM_MINUS:
			return shift ? '_' : '-';
		case VK_OEM_PLUS:
			return shift ? '+' : '=';
		case VK_OEM_PERIOD:
			return shift ? '>' : '.';
		case VK_OEM_COMMA:
			return shift ? '<' : ',';
		case VK_OEM_1:
			return shift ? ':' : ';';
		case VK_OEM_2:
			return shift ? '?' : '/';
		case VK_OEM_5:
			return shift ? '|' : '\\';
		}
		return 0;
	}

	/// Editable single-line text field with a trailing label; @p hint shows when empty and focused.
	inline bool TextField(const char* name, char* buf, size_t size, float width, float height, const char* hint)
	{
		elementCount++;
		bool changed = false;

		const FVector2D fieldSize{width, height};
		const FVector2D pos = NextPos(FVector2D{10, 10}, height + 10.0f);
		const bool isHovered = MouseInZone(pos, fieldSize);

		DrawRect(pos, fieldSize.X, fieldSize.Y, Colors::Frame);
		if (isHovered) elementHovered = true;

		if (Input::IsMouseClicked(0, elementCount, false))
			activeField = isHovered ? elementCount : (activeField == elementCount ? -1 : activeField);

		if (activeField == elementCount)
		{
			const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			size_t len = strlen(buf);
			if (Input::IsKeyPressed(VK_BACK, false) && len > 0)
			{
				buf[len - 1] = '\0';
				changed = true;
			}
			for (int vk = 0x20; vk < 256; vk++)
			{
				if (!Input::IsKeyPressed(vk, false)) continue;
				char ch = CharFromVK(vk, shift);
				if (ch && len < size - 1)
				{
					buf[len++] = ch;
					buf[len] = '\0';
					changed = true;
				}
			}
		}

		const bool empty = (buf[0] == '\0');
		const char* shown = empty ? (hint ? hint : "") : buf;
		if (shown[0]) TextLeft(shown, FVector2D{pos.X + 4.0f, pos.Y + fieldSize.Y / 2}, Colors::Text, false);
		if (name && name[0] && name[0] != '#')
			TextLeft(name, FVector2D{pos.X + fieldSize.X + 5.0f, pos.Y + fieldSize.Y / 2}, Colors::Text, false);

		EndElement(pos, fieldSize);
		return changed;
	}
} // namespace UCanvasGUI
