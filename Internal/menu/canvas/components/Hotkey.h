#pragma once

/// @file
/// @brief Hotkey widget: a key-capture button that, once armed, binds the next pressed key. Includes
/// VirtualKeyCodeToString, which renders a virtual-key code as its display name.

#include "Core.h"

namespace UCanvasGUI
{
	inline int activeHotkey = -1;	 ///< elementCount of the hotkey currently capturing a key (-1 = none)
	inline bool alreadyPressed = false; ///< guards against binding the mouse click that armed capture

	/// Human-readable name for a virtual-key code (mouse buttons and the extended-key set handled).
	inline std::string VirtualKeyCodeToString(UCHAR virtualKey)
	{
		switch (virtualKey)
		{
		case VK_LBUTTON: return "MOUSE0";
		case VK_RBUTTON: return "MOUSE1";
		case VK_MBUTTON: return "MBUTTON";
		case VK_XBUTTON1: return "XBUTTON1";
		case VK_XBUTTON2: return "XBUTTON2";
		}

		UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
		switch (virtualKey)
		{
		case VK_LEFT:
		case VK_UP:
		case VK_RIGHT:
		case VK_DOWN:
		case VK_RCONTROL:
		case VK_RMENU:
		case VK_LWIN:
		case VK_RWIN:
		case VK_APPS:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_END:
		case VK_HOME:
		case VK_INSERT:
		case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
			scanCode |= KF_EXTENDED;
		}

		char name[128];
		GetKeyNameTextA(scanCode << 16, name, sizeof(name));
		return name;
	}

	inline bool Hotkey(const char* name, FVector2D size, int* key)
	{
		elementCount++;
		bool changed = false;

		const FVector2D pos = NextPos({5, 10}, size.Y + 10.0f, lastElementSize.Y / 2 - size.Y / 2);
		const bool isHovered = MouseInZone(pos, size);

		DrawRect(pos, size.X, size.Y, Colors::Accent);
		if (isHovered)
			elementHovered = true;

		const FVector2D textPos{pos.X + size.X / 2, pos.Y + size.Y / 2};
		if (activeHotkey == elementCount)
		{
			TextCenter("[Press Key]", textPos, Colors::Text, false);

			if (!Input::IsAnyMouseDown())
				alreadyPressed = false;

			// Bind the highest key currently held (once the arming click has been released).
			if (!alreadyPressed)
				for (int code = 0; code < 255; code++)
					if (GetAsyncKeyState(code) & 0x8000)
					{
						*key = code;
						changed = true;
						activeHotkey = -1;
					}
		}
		else
		{
			TextCenter(VirtualKeyCodeToString(*key).c_str(), textPos, Colors::Text, false);

			if (Input::IsMouseClicked(0, elementCount, false))
			{
				if (isHovered)
				{
					alreadyPressed = true;
					activeHotkey = elementCount;

					// Drain the keys down right now so the arming click/keys aren't captured as the bind.
					for (int code = 0; code < 255; code++)
						GetAsyncKeyState(code);
				}
				else
				{
					activeHotkey = -1;
				}
			}
		}

		EndElement(pos, size);
		return changed;
	}
} // namespace UCanvasGUI
