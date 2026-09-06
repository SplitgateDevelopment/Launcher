#pragma once

/// @file
/// @brief The draggable menu window: chrome (background, tab column, header bar, title) and the
/// pointer-driven drag. Opens the frame — resets the layout cursor and widget counter — and draws the
/// chrome; returns false (drawing nothing) while closed or while the game window isn't focused.

#include "Core.h"

namespace UCanvasGUI
{
	inline FVector2D dragPos; ///< cursor-to-window offset captured at drag start (zero while not dragging)

	/// @param pos     in/out window top-left; updated while dragged.
	/// @param isOpen  whether the menu is shown.
	/// @return true if the window is drawn (callers should draw their widgets only then).
	inline bool Window(const char* name, FVector2D* pos, FVector2D size, bool isOpen)
	{
		elementCount = 0;
		static HWND window = FindWindow(L"UnrealWindow", L"PortalWars  ");
		if (!isOpen || GetActiveWindow() != window)
			return false;

		const bool isHovered = MouseInZone(*pos, size);
		const bool lmb = GetAsyncKeyState(VK_LBUTTON);

		// Release the dragged slider once the button is up.
		if (activeElement != -1 && !lmb)
			activeElement = -1;

		if (elementHovered && lmb)
		{
			// A widget is capturing this click — don't start a window drag.
		}
		else if ((isHovered || dragPos.X != 0) && !elementHovered)
		{
			if (Input::IsMouseClicked(0, elementCount, true))
			{
				FVector2D cursor = CursorPos();
				cursor.X -= size.X;
				cursor.Y -= size.Y;
				if (dragPos.X == 0)
					dragPos = FVector2D{cursor.X - pos->X, cursor.Y - pos->Y};
				pos->X = cursor.X - dragPos.X;
				pos->Y = cursor.Y - dragPos.Y;
			}
			else
			{
				dragPos = FVector2D{0, 0};
			}
		}
		else
		{
			elementHovered = false;
		}

		offsetX = 0.0f;
		offsetY = 0.0f;
		menuPos = *pos;

		// Chrome: window background, left tab column, header bar.
		DrawRect(*pos, size.X, size.Y, Colors::Background);
		DrawRect(*pos, 122, size.Y, Colors::Frame);
		DrawRect(*pos, size.X, 25.0f, Colors::Accent);
		offsetY += 25.0f;

		TextCenter(name, FVector2D{pos->X + size.X / 2, pos->Y + 25 / 2}, Colors::Text, false);
		return true;
	}
} // namespace UCanvasGUI
