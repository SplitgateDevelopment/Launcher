#pragma once

/// @file
/// @brief Key polling that only reports presses while the game window is focused, so hotkeys (aim,
/// trigger, super jump, teleport, ...) don't fire while you're typing in another app. Features poll
/// Input::Down() instead of GetAsyncKeyState() directly.

#include <Windows.h>

namespace Input
{
	/// True when the foreground window belongs to this (the game) process. Covers any of the game's
	/// windows without needing a specific HWND, and excludes other applications.
	inline bool GameFocused()
	{
		HWND foreground = GetForegroundWindow();
		if (!foreground) return false;
		DWORD pid = 0;
		GetWindowThreadProcessId(foreground, &pid);
		return pid == GetCurrentProcessId();
	}

	/// Whether @p vk is currently held — but only while the game is focused. Works for mouse buttons
	/// (VK_LBUTTON, ...) too. Features that need an edge track their own previous state around this.
	inline bool Down(int vk)
	{
		return GameFocused() && (GetAsyncKeyState(vk) & 0x8000) != 0;
	}
} // namespace Input
