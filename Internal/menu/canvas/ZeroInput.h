#pragma once

/// @file
/// @brief Win32 mouse/keyboard polling for the Canvas (ZeroGUI) menu backend. Sampled once per
/// frame by Handle(); the widgets read the per-button state and edge-detect clicks per widget id.
/// Everything is `inline` because this header is pulled into multiple translation units.

#include <Windows.h>

namespace ZeroGUI
{
	namespace Input
	{
		inline bool mouseDown[5];
		inline bool mouseDownAlready[256];

		inline bool keysDown[256];
		inline bool keysDownAlready[256];

		inline bool IsAnyMouseDown()
		{
			if (mouseDown[0]) return true;
			if (mouseDown[1]) return true;
			if (mouseDown[2]) return true;
			if (mouseDown[3]) return true;
			if (mouseDown[4]) return true;

			return false;
		}

		/// Rising-edge (or, with @p repeat, level) detection of button @p button for widget @p element_id.
		inline bool IsMouseClicked(int button, int element_id, bool repeat)
		{
			if (mouseDown[button])
			{
				if (!mouseDownAlready[element_id])
				{
					mouseDownAlready[element_id] = true;
					return true;
				}
				if (repeat)
					return true;
			}
			else
			{
				mouseDownAlready[element_id] = false;
			}
			return false;
		}

		inline bool IsKeyPressed(int key, bool repeat)
		{
			if (keysDown[key])
			{
				if (!keysDownAlready[key])
				{
					keysDownAlready[key] = true;
					return true;
				}
				if (repeat)
					return true;
			}
			else
			{
				keysDownAlready[key] = false;
			}
			return false;
		}

		/// Sample every mouse button and key once per frame (high bit = currently down).
		inline void Handle()
		{
			mouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
			mouseDown[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
			mouseDown[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
			mouseDown[3] = (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0;
			mouseDown[4] = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;

			for (int i = 0; i < 256; i++)
				keysDown[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
		}
	} // namespace Input
} // namespace ZeroGUI
