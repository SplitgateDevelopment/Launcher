#pragma once

#include <Windows.h>

// Lightweight inter-process signaling between the launcher and the injected DLL, built on
// named Win32 events. This is distinct from the in-game event bus in
// Internal/scripting/Events.h — that one dispatches inside the game; this one crosses the
// launcher/DLL process boundary. Both processes run in the same user session, so the
// "Local\" event namespace is used.
//
// Usage:
//   Launcher: HANDLE e = Ipc::Create(Ipc::Event::Initialized);   // then Ipc::Wait(e, ms)
//   DLL:      Ipc::Signal(Ipc::Event::Initialized);              // once init succeeds
namespace Ipc
{
	enum class Event
	{
		Initialized, // the DLL finished Hook::Init(); the launcher may stop waiting
	};

	inline const wchar_t* Name(Event event)
	{
		switch (event)
		{
		case Event::Initialized:
			return L"Local\\SplitgateInitialized";
		}

		return L"Local\\SplitgateUnknown";
	}

	// DLL side: signal that the event occurred. Returns false if nobody is listening (no
	// launcher created the event) or the signal failed.
	inline bool Signal(Event event)
	{
		HANDLE handle = OpenEventW(EVENT_MODIFY_STATE, FALSE, Name(event));
		if (!handle) return false;

		const bool ok = SetEvent(handle);
		CloseHandle(handle);
		return ok;
	}

	// Launcher side: create the manual-reset event to wait on. Returns a raw HANDLE the
	// caller owns — wrap it in Launcher::UniqueHandle. Created before the DLL is triggered so
	// the DLL can open it.
	inline HANDLE Create(Event event)
	{
		return CreateEventW(nullptr, TRUE, FALSE, Name(event));
	}

	// Launcher side: wait up to timeoutMs for the event to be signaled.
	inline bool Wait(HANDLE handle, DWORD timeoutMs)
	{
		return WaitForSingleObject(handle, timeoutMs) == WAIT_OBJECT_0;
	}
};
