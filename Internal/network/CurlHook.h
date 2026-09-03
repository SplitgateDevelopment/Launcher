#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <MinHook.h>

#include <format>
#include <string>
#include <vector>

#include "../utils/Logger.h"
#include "../utils/Util.h"
#include "HttpLogger.h"
#include "Redirect.h"

/**
 * @file
 * @brief libcurl hook — Splitgate's UE HTTP goes through libcurl (`FCurlHttpRequest`), which
 * is **statically linked**, so its symbols aren't exported and can't be resolved with
 * GetProcAddress. Instead `curl_easy_setopt` is located by an AOB signature (below) and hooked;
 * on `CURLOPT_URL` the URL is logged and rewritten. Fill @ref signature once from a RE pass
 * (the exact bytes are build-specific); until then this no-ops and WinHTTP is the fallback.
 *
 * curl copies the string during setopt, so passing a rewritten temporary is safe. On x64 the
 * single vararg of `curl_easy_setopt(CURL*, CURLoption, ...)` lands in one register, so the
 * three-parameter prototype below is ABI-compatible for the CURLOPT_URL (char*) case.
 */
namespace Network::Curl
{
	constexpr int CURLOPT_URL = 10002; ///< from curl.h (CURLOPTTYPE_STRINGPOINT + 2)

	using SetOpt_t = int(__cdecl*)(void* handle, int option, void* param);
	inline SetOpt_t Original = nullptr;
	inline bool installed = false;

	/**
	 * AOB signature for `curl_easy_setopt`'s prologue in the game module. **Empty by default**
	 * — fill it after a one-time RE pass (IDA/Ghidra/x64dbg on the game exe). `0x00` bytes are
	 * wildcards for Util's FindSignature. While empty, the libcurl hook stays inert.
	 */
	inline const std::vector<BYTE> signature = {};

	inline int __cdecl HookedSetOpt(void* handle, int option, void* param)
	{
		if (option != CURLOPT_URL || !param) return Original(handle, option, param);

		const std::string original(static_cast<const char*>(param));
		Http::Log("", original);

		const std::string rewritten = Redirect::RewriteUrl(original);
		if (rewritten == original) return Original(handle, option, param);

		Logger::Log("INFO", std::format("[Network] curl {} -> {}", original, rewritten));
		static thread_local std::string buffer; // outlives the setopt copy
		buffer = rewritten;
		return Original(handle, option, buffer.data());
	}

	/// Locates curl_easy_setopt via @ref signature and hooks it. No-op if the signature is
	/// unset or not found (WinHTTP remains the fallback).
	inline void Install()
	{
		if (installed || signature.empty()) return;

		MODULEINFO info{};
		if (!K32GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(nullptr), &info, sizeof(info))) return;

		auto* base = static_cast<BYTE*>(info.lpBaseOfDll);
		BYTE* target = FindSignature(base, base + info.SizeOfImage, const_cast<BYTE*>(signature.data()), signature.size());
		if (!target)
		{
			Logger::Log("ERROR", "[Network] curl_easy_setopt signature not found");
			return;
		}

		if (MH_CreateHook(target, &HookedSetOpt, reinterpret_cast<void**>(&Original)) != MH_OK) return;
		MH_EnableHook(target);

		installed = true;
		Logger::Log("SUCCESS", "[Network] libcurl hook installed");
	}
} // namespace Network::Curl
