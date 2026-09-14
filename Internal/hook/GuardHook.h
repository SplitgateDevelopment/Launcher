#pragma once

/// @file
/// @brief GuardHook — a code-patch-free hooking primitive (our own implementation), an alternative to
/// the MinHook trampoline used elsewhere. MinHook rewrites the target's prologue bytes, which a
/// code-integrity check can spot; a guard-page hook modifies **no** target bytes. It marks the page
/// containing the target `PAGE_GUARD` and installs a vectored exception handler: when execution reaches
/// the target the guard fault fires, the handler redirects RIP to the detour and single-steps once to
/// re-arm the guard (the CPU auto-clears PAGE_GUARD on the fault).
///
/// Trade-offs (see docs/hooking.md): one exception per call — fine for a rarely-called function, far
/// too slow for a per-frame path like PostRender. It hooks by resolved address (no trampoline back to
/// the original — the detour is responsible for continuing execution), and there is an inherent race:
/// while the guard is cleared between the fault and the single-step re-arm, a concurrent call on
/// another thread runs the real target un-redirected. Ship it opt-in (e.g. behind a Debug toggle) for
/// a low-frequency target, not as a drop-in for the MinHook path.

#include <Windows.h>

#include <cstdint>
#include <vector>

namespace Hook
{
	/// Guard-page (PAGE_GUARD + VEH) hooking. Install(target, detour) / Remove(target).
	namespace GuardHook
	{
		/// x86 EFlags trap flag: set it to single-step the next instruction (raises EXCEPTION_SINGLE_STEP).
		inline constexpr DWORD TrapFlag = 0x100;
		/// STATUS_GUARD_PAGE_VIOLATION — raised when code touches a PAGE_GUARD page (winnt has no macro).
		inline constexpr DWORD GuardPageViolation = 0x80000001;

		/// One installed hook: redirect @ref target to @ref detour.
		struct Entry
		{
			void* target;
			void* detour;
		};

		inline std::vector<Entry> entries;	 ///< installed hooks; small, mutated only by Install/Remove
		inline PVOID vehHandle = nullptr;	 ///< the one registered vectored handler (installed lazily)
		/// The target page to re-arm on this thread's next single-step (thread-local so concurrent
		/// faults don't clobber each other's pending re-arm).
		inline thread_local void* pendingRearm = nullptr;

		/// Whether @p a and @p b sit in the same 4 KB page.
		inline bool SamePage(const void* a, const void* b)
		{
			return (reinterpret_cast<uintptr_t>(a) & ~static_cast<uintptr_t>(0xFFF)) ==
				   (reinterpret_cast<uintptr_t>(b) & ~static_cast<uintptr_t>(0xFFF));
		}

		/// (Re)mark the page containing @p target as PAGE_GUARD, preserving its base protection.
		inline bool Arm(void* target)
		{
			MEMORY_BASIC_INFORMATION mbi{};
			if (!VirtualQuery(target, &mbi, sizeof(mbi))) return false;
			DWORD old = 0;
			return VirtualProtect(target, 1, mbi.Protect | PAGE_GUARD, &old) != 0;
		}

		/// The vectored handler: redirect target faults to the detour and re-arm the guard on the step.
		inline LONG CALLBACK Handler(PEXCEPTION_POINTERS ex)
		{
			CONTEXT* ctx = ex->ContextRecord;
			const DWORD code = ex->ExceptionRecord->ExceptionCode;

			if (code == GuardPageViolation)
			{
				// Execution fault → RIP is the faulting address. The OS has already cleared PAGE_GUARD
				// on this page, so we must re-arm it (deferred to the single-step below).
				const void* rip = reinterpret_cast<void*>(ctx->Rip);
				for (const Entry& e : entries)
				{
					if (rip == e.target) // our target: redirect to the detour
					{
						ctx->Rip = reinterpret_cast<DWORD64>(e.detour);
						ctx->EFlags |= TrapFlag;
						pendingRearm = e.target;
						return EXCEPTION_CONTINUE_EXECUTION;
					}
					if (SamePage(rip, e.target)) // same page, different code: run it, just re-arm
					{
						ctx->EFlags |= TrapFlag;
						pendingRearm = e.target;
						return EXCEPTION_CONTINUE_EXECUTION;
					}
				}
				return EXCEPTION_CONTINUE_SEARCH; // not one of ours (e.g. a stack guard page)
			}

			if (code == EXCEPTION_SINGLE_STEP && pendingRearm)
			{
				Arm(pendingRearm);
				pendingRearm = nullptr;
				ctx->EFlags &= ~TrapFlag; // one step only (the CPU also clears TF after the trap)
				return EXCEPTION_CONTINUE_EXECUTION;
			}

			return EXCEPTION_CONTINUE_SEARCH;
		}

		/// Redirect calls to @p target into @p detour without patching any bytes.
		/// @return false on a null argument or if the guard/VEH couldn't be installed.
		/// @note The detour receives control in place of the target; there is no trampoline back to the
		///       original, so the detour owns continuing/emulating the target's work.
		inline bool Install(void* target, void* detour)
		{
			if (!target || !detour) return false;

			if (!vehHandle)
			{
				// First in the chain so we intercept our own guard/step faults before the crash handler's
				// last-chance SetUnhandledExceptionFilter sees them.
				vehHandle = AddVectoredExceptionHandler(1, Handler);
				if (!vehHandle) return false;
			}

			entries.push_back({target, detour});
			if (Arm(target)) return true;

			entries.pop_back();
			return false;
		}

		/// Remove a hook installed by Install: clear the page guard and drop the entry. Removes the
		/// vectored handler once the last hook is gone.
		inline void Remove(void* target)
		{
			MEMORY_BASIC_INFORMATION mbi{};
			DWORD old = 0;
			if (VirtualQuery(target, &mbi, sizeof(mbi)))
				VirtualProtect(target, 1, mbi.Protect & ~static_cast<DWORD>(PAGE_GUARD), &old);

			std::erase_if(entries, [&](const Entry& e) { return e.target == target; });

			if (entries.empty() && vehHandle)
			{
				RemoveVectoredExceptionHandler(vehHandle);
				vehHandle = nullptr;
			}
		}

		inline bool demoDetourRan = false;			  ///< set by SelfTest's detour to prove it ran
		inline void SelfTestDetour() { demoDetourRan = true; } ///< the SelfTest detour (a normal, un-guarded function)

		/// In-process proof that the primitive works, safe to run in the game (touches no game code):
		/// guard-hook a scratch page holding a lone `ret`, call it, and confirm the detour ran instead.
		/// The target lives on its own VirtualAlloc'd page so guarding it can never fault our own handler
		/// or this function. @return true if the detour ran (hook worked).
		inline bool SelfTest()
		{
			demoDetourRan = false;

			auto* page = static_cast<BYTE*>(VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
			if (!page) return false;
			page[0] = 0xC3; // x64 `ret` — the "original" we expect to be redirected away from

			bool ok = false;
			if (Install(page, reinterpret_cast<void*>(&SelfTestDetour)))
			{
				// Call through a volatile pointer so the compiler can't inline past the guarded page.
				void(*volatile call)() = reinterpret_cast<void (*)()>(page);
				call();
				Remove(page);
				ok = demoDetourRan;
			}

			VirtualFree(page, 0, MEM_RELEASE);
			return ok;
		}
	} // namespace GuardHook
} // namespace Hook
