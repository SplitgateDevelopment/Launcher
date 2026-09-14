#pragma once

/// @file
/// @brief Small cross-project helpers shared by the launcher and the DLL: putting text on and
/// reading text off the clipboard, and opening a folder in the system file browser.

#include <Windows.h>
#include <shellapi.h>

#include <cstring>
#include <filesystem>
#include <string>

#pragma comment(lib, "shell32.lib")

/// @brief Reusable clipboard / shell helpers used by both the launcher and the DLL.
namespace Shared::Utilities
{
	/**
	 * Places @p text on the clipboard as CF_TEXT. Best-effort — returns false if the clipboard
	 * couldn't be opened or the allocation failed. On success the system takes ownership of the
	 * allocated buffer (it must not be freed by the caller).
	 * @return true if the text was placed on the clipboard.
	 */
	inline bool CopyToClipboard(const std::string& text)
	{
		if (!OpenClipboard(nullptr)) return false;

		bool ok = false;
		if (EmptyClipboard())
		{
			const size_t bytes = text.size() + 1; // include the terminating NUL
			if (HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes))
			{
				if (void* dst = GlobalLock(mem))
				{
					memcpy(dst, text.c_str(), bytes);
					GlobalUnlock(mem);
					// SetClipboardData takes ownership of mem on success; free it ourselves on failure.
					ok = SetClipboardData(CF_TEXT, mem) != nullptr;
				}
				if (!ok) GlobalFree(mem);
			}
		}

		CloseClipboard();
		return ok;
	}

	/**
	 * Reads CF_TEXT off the clipboard. Best-effort — returns an empty string if the clipboard
	 * couldn't be opened or holds no text.
	 * @return the clipboard text (empty on failure).
	 */
	inline std::string PasteFromClipboard()
	{
		std::string text;
		if (!OpenClipboard(nullptr)) return text;

		if (HANDLE data = GetClipboardData(CF_TEXT))
			if (const char* src = static_cast<const char*>(GlobalLock(data)))
			{
				text = src;
				GlobalUnlock(data);
			}

		CloseClipboard();
		return text;
	}

	/// Opens @p path in the system file browser (Explorer), creating the folder first so the call
	/// succeeds even on a fresh install. No-op on an empty path.
	inline void OpenFolder(const std::filesystem::path& path)
	{
		if (path.empty()) return;
		std::error_code ec;
		std::filesystem::create_directories(path, ec);
		ShellExecuteW(nullptr, L"open", path.wstring().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	}
} // namespace Shared::Utilities
