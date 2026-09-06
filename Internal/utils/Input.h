#pragma once

/// @file
/// @brief Key polling that only reports presses while the game window is focused, so hotkeys (aim,
/// trigger, super jump, teleport, ...) don't fire while you're typing in another app. Features poll
/// Input::Down() instead of GetAsyncKeyState() directly.

#include <Windows.h>

#include "../scripting/Events.h"

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

	/// Whether @p vk went from up to down since the last poll of that key — a single up->down edge,
	/// reported only while the game is focused. Uses the GetAsyncKeyState high bit + its own previous-
	/// state table (indexed by vk), so it is immune to the `& 1` "pressed since last call" bit that any
	/// other poller (e.g. @ref DispatchHotKeys, which sweeps every key each frame) clears. Use for
	/// press-once actions that poll a known key directly instead of subscribing to HotKeyPressed — e.g.
	/// the menu toggle. The state table is shared across callers, so poll any given key from one site.
	inline bool Pressed(int vk)
	{
		if (vk < 0 || vk > 255) return false;
		static bool prev[256] = {};
		const bool down = Down(vk); // high bit AND game-focused; false (no edge) while unfocused
		const bool pressed = down && !prev[vk];
		prev[vk] = down;
		return pressed;
	}

	/// Poll every key/mouse button once per frame and dispatch Events::HotKeyPressed on each
	/// up->down edge (only while the game is focused, and only when something subscribes). payload.value
	/// carries the vk code. Call once per frame before features run. Press-once actions (super jump,
	/// teleport) subscribe to this instead of polling every frame themselves.
	inline void DispatchHotKeys()
	{
		static bool prev[256] = {};

		if (!GameFocused())
		{
			for (bool& p : prev) p = false; // don't fire a stale edge when focus returns
			return;
		}
		if (!Events::HasHandlers(Events::Type::HotKeyPressed)) return;

		for (int vk = 1; vk < 256; vk++)
		{
			const bool down = (GetAsyncKeyState(vk) & 0x8000) != 0;
			if (down && !prev[vk])
				Events::Dispatch(Events::Type::HotKeyPressed, Events::Payload{.value = static_cast<float>(vk)});
			prev[vk] = down;
		}
	}
} // namespace Input
