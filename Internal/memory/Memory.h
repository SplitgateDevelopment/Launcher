#pragma once

/**
 * @file
 * @brief Our own byte-pattern (AOB signature) scanning + pointer-resolution utility.
 *
 * Replaces the old @c utils/Util.cpp helpers with a small, self-contained, unit-tested namespace
 * (see Tests/MemoryTests.cpp). Two ways to describe a pattern:
 *   - @ref Memory::Parse for IDA-style strings ("48 8B 05 ? ? ? ? 90"), the clear form for new code;
 *   - @ref Memory::FromBytes for the project's existing raw byte arrays, where a @c 0x00 byte means
 *     "wildcard" (so the hand-written engine signatures keep working unchanged).
 *
 * The core (Parse / FromBytes / Matches / Find / Relative) is pure and testable against plain
 * buffers; Scan / ModuleBase / ModuleSize / FindPointer add the running-module convenience on top.
 */

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <vector>

#include <Windows.h>
#include <Psapi.h>

namespace Memory
{
	/// A byte matcher: at each index, @ref bytes is compared only where @ref mask is true (false = a
	/// wildcard that matches any byte). The two vectors are always the same length.
	struct Signature
	{
		std::vector<uint8_t> bytes;
		std::vector<bool> mask;

		size_t size() const { return bytes.size(); }
		bool empty() const { return bytes.empty(); }
	};

	/// Hex nibble value of an ASCII char, or -1 if it isn't a hex digit.
	inline int HexDigit(char c)
	{
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		return -1;
	}

	/// Build a Signature from an IDA-style pattern: space-separated hex byte pairs with "?" or "??"
	/// for wildcards, e.g. "48 8B 05 ? ? ? ? 90". Malformed tokens are skipped.
	inline Signature Parse(std::string_view ida)
	{
		Signature sig;
		for (size_t i = 0; i < ida.size();)
		{
			const char c = ida[i];
			if (c == ' ' || c == '\t')
			{
				i++;
				continue;
			}
			if (c == '?')
			{
				sig.bytes.push_back(0);
				sig.mask.push_back(false);
				i++;
				if (i < ida.size() && ida[i] == '?') i++; // accept "?" or "??"
				continue;
			}
			const int hi = HexDigit(c);
			const int lo = (i + 1 < ida.size()) ? HexDigit(ida[i + 1]) : -1;
			if (hi < 0 || lo < 0)
			{
				i++; // skip a stray character rather than aborting the whole parse
				continue;
			}
			sig.bytes.push_back(static_cast<uint8_t>((hi << 4) | lo));
			sig.mask.push_back(true);
			i += 2;
		}
		return sig;
	}

	/// Build a Signature from a raw byte array where a @c 0x00 byte is treated as a wildcard — the
	/// convention the project's hand-written signatures use (the wildcard bytes are the RIP-relative
	/// displacements). A literal @c 0x00 that must match can't be expressed this way; use @ref Parse
	/// with an explicit "00" for those.
	inline Signature FromBytes(const uint8_t* bytes, size_t size)
	{
		Signature sig;
		sig.bytes.assign(bytes, bytes + size);
		sig.mask.resize(size);
		for (size_t i = 0; i < size; i++) sig.mask[i] = (bytes[i] != 0x00);
		return sig;
	}

	/// Whether the bytes at @p at match @p sig (honouring its wildcard mask). @p at must have at
	/// least @c sig.size() readable bytes.
	inline bool Matches(const uint8_t* at, const Signature& sig)
	{
		for (size_t i = 0; i < sig.bytes.size(); i++)
			if (sig.mask[i] && at[i] != sig.bytes[i]) return false;
		return true;
	}

	/// First occurrence of @p sig within [begin, end), or nullptr if absent (or the inputs are empty
	/// / too small).
	inline uint8_t* Find(uint8_t* begin, uint8_t* end, const Signature& sig)
	{
		const size_t n = sig.size();
		if (n == 0 || !begin || end <= begin) return nullptr;
		if (static_cast<size_t>(end - begin) < n) return nullptr;

		for (uint8_t* at = begin; at <= end - n; at++)
			if (Matches(at, sig)) return at;
		return nullptr;
	}

	/// Base address of @p module (nullptr = the running process's main module).
	inline uint8_t* ModuleBase(HMODULE module = nullptr)
	{
		return reinterpret_cast<uint8_t*>(module ? module : GetModuleHandleA(nullptr));
	}

	/// Image size in bytes of @p module (nullptr = main module), or 0 if it can't be queried.
	inline size_t ModuleSize(HMODULE module = nullptr)
	{
		MODULEINFO info{};
		HMODULE target = module ? module : GetModuleHandleA(nullptr);
		if (!K32GetModuleInformation(GetCurrentProcess(), target, &info, sizeof(info))) return 0;
		return info.SizeOfImage;
	}

	/// Scan an entire module for @p sig; nullptr if not found or the module can't be measured.
	inline uint8_t* Scan(const Signature& sig, HMODULE module = nullptr)
	{
		uint8_t* base = ModuleBase(module);
		const size_t size = ModuleSize(module);
		if (!base || size == 0) return nullptr;
		return Find(base, base + size, sig);
	}

	/// Scan a module for an IDA-style pattern (convenience over @ref Parse + @ref Scan).
	inline uint8_t* Scan(std::string_view ida, HMODULE module = nullptr)
	{
		return Scan(Parse(ida), module);
	}

	/// Resolve a 32-bit RIP-relative reference: read the int32 displacement at @p at + @p dispOffset
	/// and return the absolute target it points at (the displacement is relative to the end of the
	/// 4-byte operand, i.e. @p dispOffset + 4). nullptr-safe.
	inline uint8_t* Relative(uint8_t* at, int dispOffset)
	{
		if (!at) return nullptr;
		const int32_t disp = *reinterpret_cast<const int32_t*>(at + dispOffset);
		return at + dispOffset + 4 + disp;
	}

	/// Scan @p module for a @c 0x00-wildcard byte signature, then follow the RIP-relative operand that
	/// starts at the first wildcard byte and add @p addition. This is the "mov/lea reg, [rip+disp]"
	/// global-locator the SDK uses for GObjects / GNames / GWorld. Returns nullptr if not found.
	inline void* FindPointer(HMODULE module, const uint8_t* bytes, size_t size, int addition)
	{
		const Signature sig = FromBytes(bytes, size);
		uint8_t* at = Scan(sig, module);
		if (!at) return nullptr;

		int dispOffset = 0; // the operand begins at the first wildcard (first 0x00 in the raw sig)
		while (dispOffset < static_cast<int>(size) && sig.mask[dispOffset]) dispOffset++;

		return Relative(at, dispOffset) + addition;
	}

	/// First occurrence of the null-terminated ASCII string @p str within [begin, end), or nullptr.
	/// Matches the bytes AND the trailing '\0' so it doesn't hit a substring of a longer string.
	inline uint8_t* FindString(uint8_t* begin, uint8_t* end, std::string_view str)
	{
		const size_t n = str.size();
		if (n == 0 || !begin || end <= begin) return nullptr;
		if (static_cast<size_t>(end - begin) < n + 1) return nullptr;

		for (uint8_t* at = begin; at <= end - (n + 1); at++)
			if (std::memcmp(at, str.data(), n) == 0 && at[n] == '\0') return at;
		return nullptr;
	}

	/// First `lea reg, [rip+disp]` (REX.W 8D, mod=00 rm=101) in [begin, end) whose resolved target is
	/// @p target, or nullptr. Used to find the instruction that references a located string.
	inline uint8_t* FindLeaTo(uint8_t* begin, uint8_t* end, uint8_t* target)
	{
		if (!begin || !target || end <= begin) return nullptr;

		for (uint8_t* at = begin; at + 7 <= end; at++)
		{
			if ((at[0] & 0xF8) != 0x48) continue; // REX prefix (0x48-0x4F)
			if (at[1] != 0x8D) continue;		  // lea
			if ((at[2] & 0xC7) != 0x05) continue; // mod=00, rm=101 → rip-relative disp32
			if (Relative(at, 3) == target) return at;
		}
		return nullptr;
	}

	/// Locate a string in @p module, then the `lea` that references it (the string-ref discovery
	/// pattern — far more update-stable than a raw byte prologue). Returns the lea, or nullptr.
	inline uint8_t* FindStringRef(std::string_view str, HMODULE module = nullptr)
	{
		uint8_t* base = ModuleBase(module);
		const size_t size = ModuleSize(module);
		if (!base || size == 0) return nullptr;

		uint8_t* stringAddr = FindString(base, base + size, str);
		if (!stringAddr) return nullptr;

		return FindLeaTo(base, base + size, stringAddr);
	}
} // namespace Memory
