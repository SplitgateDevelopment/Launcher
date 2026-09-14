#pragma once

/// @file
/// @brief Move-only RAII wrapper owning an HHOOK, removed with UnhookWindowsHookEx.

#include <Windows.h>
#include <utility>

namespace Launcher
{
	// Owns an HHOOK, removed with UnhookWindowsHookEx on destruction. After the DLL captures
	// the handle (via Hook::injectionHook), call release() so the launcher does NOT unhook on exit —
	// the DLL owns teardown, and unhooking here could unload the injected module.
	/// Move-only owner of an HHOOK; unhooks with UnhookWindowsHookEx on reset/destruction.
	class UniqueHook
	{
	  public:
		explicit UniqueHook(HHOOK handle = nullptr) : handle(handle) {}
		~UniqueHook() { reset(); }

		UniqueHook(const UniqueHook&) = delete;
		UniqueHook& operator=(const UniqueHook&) = delete;

		UniqueHook(UniqueHook&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}
		UniqueHook& operator=(UniqueHook&& other) noexcept
		{
			if (this == &other) return *this;
			reset();
			handle = std::exchange(other.handle, nullptr);
			return *this;
		}

		/// Returns the owned hook handle without transferring ownership.
		HHOOK get() const { return handle; }
		/// True when a non-null hook is owned.
		explicit operator bool() const { return handle != nullptr; }

		/// Give up ownership without unhooking (hand the hook off to the DLL).
		HHOOK release() { return std::exchange(handle, nullptr); }

		/// Unhooks any currently owned hook and takes ownership of `newHandle` (default: empty).
		void reset(HHOOK newHandle = nullptr)
		{
			if (handle) UnhookWindowsHookEx(handle);
			handle = newHandle;
		}

	  private:
		HHOOK handle; ///< Owned hook handle, or nullptr when empty.
	};
}; // namespace Launcher
