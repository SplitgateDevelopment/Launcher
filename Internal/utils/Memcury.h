#pragma once

/// @file
/// @brief A small memcury-style AOB engine: IDA-string pattern scanning with `??` wildcards,
/// RIP-relative resolution (follow a lea/call disp32 to its target), and string-reference discovery.
/// A clearer alternative to Util's `0x00`-as-wildcard `FindSignature` (where a real `0x00` byte
/// silently becomes a wildcard) that can also follow relative instructions. The core (Parse /
/// FindPattern / Scanner / FindStringRef) is pure and unit-tested against raw buffers
/// (Tests/MemcuryTests.cpp); the Scan()/module helpers wrap the current module for runtime use.
///
/// Inspired by projectnovafn/Sinum's memcury.h (MIT). See docs/roadmap.md and
/// docs/ue4-cheatsheet.md, and Tools/find_signature.py for the offline counterpart.

#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include <Windows.h>
#include <Psapi.h>

namespace Memcury
{
	/// One byte of a parsed pattern: its value, or a wildcard that matches anything.
	struct PatternByte
	{
		uint8_t value = 0;
		bool wildcard = false;
	};

	/// Parse an IDA-style pattern ("48 8B ?? E8 ?? ?? ?? ??"); `??` / `?` are wildcards.
	inline std::vector<PatternByte> Parse(std::string_view ida)
	{
		std::vector<PatternByte> out;
		size_t i = 0;
		while (i < ida.size())
		{
			if (ida[i] == ' ')
			{
				++i;
				continue;
			}
			if (ida[i] == '?')
			{
				out.push_back({0, true});
				while (i < ida.size() && ida[i] == '?') ++i; // consume "??" or "?"
			}
			else
			{
				out.push_back({static_cast<uint8_t>(std::stoul(std::string(ida.substr(i, 2)), nullptr, 16)), false});
				i += 2;
			}
		}
		return out;
	}

	/// Find the first occurrence of @p pattern in `[start, start+size)`. Returns nullptr if not found.
	inline uint8_t* FindPattern(const std::vector<PatternByte>& pattern, uint8_t* start, size_t size)
	{
		if (pattern.empty() || !start) return nullptr;
		const size_t m = pattern.size();
		if (size < m) return nullptr;

		for (size_t off = 0; off + m <= size; ++off)
		{
			bool match = true;
			for (size_t k = 0; k < m; ++k)
				if (!pattern[k].wildcard && start[off + k] != pattern[k].value)
				{
					match = false;
					break;
				}
			if (match) return start + off;
		}
		return nullptr;
	}

	/// Convenience: parse then scan.
	inline uint8_t* FindPattern(std::string_view ida, uint8_t* start, size_t size)
	{
		return FindPattern(Parse(ida), start, size);
	}

	/// Fluent address wrapper: follow relative instructions and read the result out.
	class Scanner
	{
		uint8_t* address = nullptr;

	  public:
		Scanner() = default;
		explicit Scanner(uint8_t* a) : address(a) {}

		explicit operator bool() const { return address != nullptr; }
		uint8_t* Get() const { return address; }
		template <typename T>
		T GetAs() const { return reinterpret_cast<T>(address); }

		/// Add a fixed byte offset.
		Scanner& Add(int offset)
		{
			if (address) address += offset;
			return *this;
		}

		/// Resolve a RIP-relative disp32 located @p dispOffset bytes into the current instruction (e.g.
		/// 1 for a near `call`/`jmp` E8/E9, 3 for a REX `lea reg,[rip+..]`). The displacement is added to
		/// the address of the *next* instruction (disp32 end), giving the absolute target.
		Scanner& RelativeOffset(int dispOffset)
		{
			if (!address) return *this;
			const int32_t disp = *reinterpret_cast<int32_t*>(address + dispOffset);
			address = address + dispOffset + 4 + disp;
			return *this;
		}
	};

	// --- module helpers (runtime use against the current process' main module) ---

	inline uint8_t* ModuleBase()
	{
		return reinterpret_cast<uint8_t*>(GetModuleHandleA(nullptr));
	}

	inline size_t ModuleSize()
	{
		MODULEINFO info{};
		if (!K32GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(nullptr), &info, sizeof(info))) return 0;
		return info.SizeOfImage;
	}

	/// Scan the current module for an IDA pattern.
	inline Scanner Scan(std::string_view ida)
	{
		return Scanner(FindPattern(ida, ModuleBase(), ModuleSize()));
	}

	/// Find a null-terminated string in `[start, start+size)`; returns its address or nullptr.
	inline uint8_t* FindString(std::string_view str, uint8_t* start, size_t size)
	{
		if (str.empty() || !start || size < str.size()) return nullptr;
		for (size_t off = 0; off + str.size() <= size; ++off)
			if (std::memcmp(start + off, str.data(), str.size()) == 0)
				return start + off;
		return nullptr;
	}

	/// Find a string, then the first `lea reg,[rip+disp]` (REX.W 8D, ModRM rm=101) that references it.
	/// Returns the address of that lea (a point inside the function that uses the string), or nullptr.
	inline uint8_t* FindStringRef(std::string_view str, uint8_t* start, size_t size)
	{
		uint8_t* strAddr = FindString(str, start, size);
		if (!strAddr) return nullptr;

		for (size_t off = 0; off + 7 <= size; ++off)
		{
			uint8_t* p = start + off;
			if ((p[0] == 0x48 || p[0] == 0x4C) && p[1] == 0x8D && (p[2] & 0xC7) == 0x05)
			{
				const int32_t disp = *reinterpret_cast<int32_t*>(p + 3);
				if (p + 7 + disp == strAddr) return p;
			}
		}
		return nullptr;
	}
} // namespace Memcury
