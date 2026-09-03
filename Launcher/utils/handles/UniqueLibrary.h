#pragma once

#include <Windows.h>
#include <utility>

namespace Launcher
{
	// Owns an HMODULE, released with FreeLibrary on destruction. Freeing the launcher's own
	// mapping does not unload the copy the hook injects into the target process.
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

		HMODULE get() const { return handle; }
		explicit operator bool() const { return handle != nullptr; }

		// Give up ownership without freeing.
		HMODULE release() { return std::exchange(handle, nullptr); }

		void reset(HMODULE newHandle = nullptr)
		{
			if (handle) FreeLibrary(handle);
			handle = newHandle;
		}

	private:
		HMODULE handle;
	};
};
