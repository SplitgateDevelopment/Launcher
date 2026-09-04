#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <MinHook.h>

#include <cstdarg>
#include <cstdint>
#include <format>
#include <string>
#include <vector>

#include "../utils/Logger.h"
#include "../utils/Util.h"
#include "../settings/Settings.h"
#include "HttpLogger.h"
#include "Redirect.h"

/**
 * @file
 * @brief libcurl hook — Splitgate's UE HTTP goes through libcurl (`FCurlHttpRequest`), which
 * is **statically linked**, so its symbols aren't exported and can't be resolved with
 * GetProcAddress. Instead `curl_easy_setopt` is located by an AOB signature (below) and hooked;
 * on `CURLOPT_URL` the URL is logged and rewritten. @ref signature holds the prologue bytes
 * (build-specific — for the current libcurl 7.55.1 build); if empty or not found this no-ops and
 * WinHTTP is the fallback.
 *
 * curl copies the string during setopt, so passing a rewritten temporary is safe. On x64 the
 * single vararg of `curl_easy_setopt(CURL*, CURLoption, ...)` lands in one register, so the
 * three-parameter prototype below is ABI-compatible for the CURLOPT_URL (char*) case.
 */
namespace Network::Curl
{
	constexpr int CURLOPT_URL = 10002;			   ///< from curl.h (CURLOPTTYPE_STRINGPOINT + 2)
	constexpr int CURLOPT_SSL_VERIFYPEER = 64;	   ///< CURLOPTTYPE_LONG + 64 (verify the peer's cert)
	constexpr int CURLOPT_SSL_VERIFYHOST = 81;	   ///< CURLOPTTYPE_LONG + 81 (verify the cert's hostname)

	using SetOpt_t = int(__cdecl*)(void* handle, int option, void* param);
	inline SetOpt_t Original = nullptr;
	inline bool installed = false;

	/**
	 * AOB signature for `curl_easy_setopt`'s prologue in the game module. `0x00` bytes are
	 * wildcards for Util's FindSignature; while empty, the libcurl hook stays inert.
	 *
	 * Derived for the shipping build's statically-linked **libcurl 7.55.1** (Sep 2026). The
	 * function was located by its call sites — the only target reached by dozens of
	 * `mov edx, <CURLOPT>; call` sites (URL/WRITEDATA/HTTPHEADER/WRITEFUNCTION/...) — and its
	 * prologue is the canonical wrapper: home edx/r8/r9, `test rcx,rcx` (if(!data)), return 43
	 * via `lea eax,[rcx+0x2b]`, else `call Curl_vsetopt`. These 36 bytes are unique in .text (no
	 * wildcards needed); re-derive if the game updates curl or is recompiled.
	 *
	 *   mov  [rsp+0x10], edx      89 54 24 10
	 *   mov  [rsp+0x18], r8       4C 89 44 24 18
	 *   mov  [rsp+0x20], r9       4C 89 4C 24 20
	 *   sub  rsp, 0x28            48 83 EC 28
	 *   test rcx, rcx            48 85 C9
	 *   jne  +8                   75 08
	 *   lea  eax, [rcx+0x2b]      8D 41 2B
	 *   add  rsp, 0x28 / ret      48 83 C4 28 C3
	 *   lea  r8, [rsp+0x40]       4C 8D 44 24 40
	 */
	inline const std::vector<BYTE> signature = {
		0x89, 0x54, 0x24, 0x10, 0x4C, 0x89, 0x44, 0x24, 0x18, 0x4C, 0x89, 0x4C, 0x24, 0x20, 0x48, 0x83,
		0xEC, 0x28, 0x48, 0x85, 0xC9, 0x75, 0x08, 0x8D, 0x41, 0x2B, 0x48, 0x83, 0xC4, 0x28, 0xC3, 0x4C,
		0x8D, 0x44, 0x24, 0x40};

	inline int __cdecl HookedSetOpt(void* handle, int option, void* param)
	{
		// TLS bypass: force cert/host verification off so a redirected host can serve a self-signed
		// cert without curl rejecting it. These options take a long, passed in the same slot as
		// `param`, so overriding it with 0 sets the value to 0. (Disables verification for ALL curl
		// traffic while on — hence the opt-in setting.)
		if (Settings.NETWORK.BypassSslVerify && (option == CURLOPT_SSL_VERIFYPEER || option == CURLOPT_SSL_VERIFYHOST))
			return Original(handle, option, reinterpret_cast<void*>(0));

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

	// The internal Curl_vsetopt(CURL*, CURLoption, va_list) that curl_easy_setopt forwards to. We
	// resolve it (it's the call target inside curl_easy_setopt) and hook it too, so setopt calls made
	// through curl's *internal* path (not the public wrapper) are also covered. The public wrapper's
	// hook still runs first; both rewrites are idempotent (a re-checked URL/already-0 verify is a
	// no-op), so the two coexist without double-applying.
	using VSetOpt_t = int(__cdecl*)(void* data, int option, va_list args);
	inline VSetOpt_t OriginalV = nullptr;

	inline int __cdecl HookedVSetOpt(void* data, int option, va_list args)
	{
		// On x64 Windows a va_list is a pointer to the 8-byte argument slots; the first (only) vararg
		// of a setopt call sits at *(void**)args. We patch that slot in place, then forward the same
		// va_list — never advancing it with va_arg, so `args` still points at slot 0 for the original.
		if (Settings.NETWORK.BypassSslVerify && (option == CURLOPT_SSL_VERIFYPEER || option == CURLOPT_SSL_VERIFYHOST))
		{
			*reinterpret_cast<std::uintptr_t*>(args) = 0; // force the long argument to 0
			return OriginalV(data, option, args);
		}

		if (option == CURLOPT_URL)
		{
			auto* slot = reinterpret_cast<char**>(args);
			if (*slot)
			{
				const std::string original(*slot);
				const std::string rewritten = Redirect::RewriteUrl(original);
				if (rewritten != original)
				{
					static thread_local std::string buffer; // outlives the setopt copy
					buffer = rewritten;
					*slot = buffer.data();
				}
			}
		}

		return OriginalV(data, option, args);
	}

	/// Follow the single near-call inside curl_easy_setopt to resolve Curl_vsetopt.
	inline BYTE* ResolveVSetOpt(BYTE* easySetOpt)
	{
		for (int i = 0; i < 0x40; i++)
		{
			if (easySetOpt[i] != 0xE8) continue; // near call rel32
			const int rel = *reinterpret_cast<int*>(easySetOpt + i + 1);
			return easySetOpt + i + 5 + rel;
		}
		return nullptr;
	}

	/// Locates curl_easy_setopt via @ref signature and hooks it (plus the internal Curl_vsetopt it
	/// forwards to). No-op if the signature is unset or not found (WinHTTP remains the fallback).
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

		// Also hook the internal Curl_vsetopt it calls (address followed from the call), so setopt
		// calls that bypass the public wrapper are covered too. Best-effort: failure here doesn't
		// disable the working public hook.
		if (BYTE* vsetopt = ResolveVSetOpt(target))
		{
			if (MH_CreateHook(vsetopt, &HookedVSetOpt, reinterpret_cast<void**>(&OriginalV)) == MH_OK &&
				MH_EnableHook(vsetopt) == MH_OK)
				Logger::Log("SUCCESS", "[Network] Curl_vsetopt hook installed");
			else
				Logger::Log("ERROR", "[Network] Curl_vsetopt hook failed");
		}

		installed = true;
		Logger::Log("SUCCESS", "[Network] libcurl hook installed");
	}
} // namespace Network::Curl
