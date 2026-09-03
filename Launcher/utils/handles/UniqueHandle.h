#pragma once

#include <Windows.h>
#include <utility>

namespace Launcher
{
	// Owns a generic HANDLE (e.g. an event), closed with CloseHandle on destruction.
	// Treats nullptr as empty (matches CreateEventW, which returns nullptr on failure);
	// APIs that fail with INVALID_HANDLE_VALUE would need a different empty sentinel.
	class UniqueHandle
	{
	  public:
		explicit UniqueHandle(HANDLE handle = nullptr) : handle(handle) {}
		~UniqueHandle() { reset(); }

		UniqueHandle(const UniqueHandle&) = delete;
		UniqueHandle& operator=(const UniqueHandle&) = delete;

		UniqueHandle(UniqueHandle&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}
		UniqueHandle& operator=(UniqueHandle&& other) noexcept
		{
			if (this == &other) return *this;
			reset();
			handle = std::exchange(other.handle, nullptr);
			return *this;
		}

		HANDLE get() const { return handle; }
		explicit operator bool() const { return handle != nullptr; }

		// Give up ownership without closing (e.g. after transferring the handle elsewhere).
		HANDLE release() { return std::exchange(handle, nullptr); }

		void reset(HANDLE newHandle = nullptr)
		{
			if (handle) CloseHandle(handle);
			handle = newHandle;
		}

	  private:
		HANDLE handle;
	};
}; // namespace Launcher
