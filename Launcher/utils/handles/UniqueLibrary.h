#pragma once

/// @file
/// @brief Move-only RAII wrapper owning an HMODULE, released with FreeLibrary.

#include <Windows.h>
#include <utility>

namespace Launcher
{
	// Owns an HMODULE, released with FreeLibrary on destruction. Freeing the launcher's own
	// mapping does not unload the copy the hook injects into the target process.
	/// Move-only owner of an HMODULE; frees it with FreeLibrary on reset/destruction.
	class UniqueLibrary
	{
	  public:
		explicit UniqueLibrary(HMODULE handle = nullptr) : handle(handle) {}
		~UniqueLibrary() { reset(); }

		UniqueLibrary(const UniqueLibrary&) = delete;
		UniqueLibrary& operator=(const UniqueLibrary&) = delete;

		UniqueLibrary(UniqueLibrary&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}
		UniqueLibrary& operator=(UniqueLibrary&& other) noexcept
		{
			if (this == &other) return *this;
			reset();
			handle = std::exchange(other.handle, nullptr);
			return *this;
		}

		/// Returns the owned module handle without transferring ownership.
		HMODULE get() const { return handle; }
		/// True when a non-null module is owned.
		explicit operator bool() const { return handle != nullptr; }

		/// Give up ownership without freeing.
		HMODULE release() { return std::exchange(handle, nullptr); }

		/// Frees any currently owned module and takes ownership of `newHandle` (default: empty).
		void reset(HMODULE newHandle = nullptr)
		{
			if (handle) FreeLibrary(handle);
			handle = newHandle;
		}

	  private:
		HMODULE handle; ///< Owned module handle, or nullptr when empty.
	};
}; // namespace Launcher
