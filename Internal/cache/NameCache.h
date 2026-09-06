#pragma once

/// @file
/// @brief Lazily-built, shared snapshot of the whole FName pool (every string the engine has
/// interned — object names, map/package names, function names, tags). Unlike GObjects, the name
/// pool holds names for content that isn't loaded yet, so this is where a not-yet-loaded map name
/// (e.g. a level to travel to) can be found. Built on demand; call Rebuild() (a Refresh button) to
/// pick up names interned since the last scan.

#include <cstdint>
#include <string>
#include <vector>

#include "../ue/Engine.h"

namespace NameCache
{
	/// Bytes per FName block. The pool allocates fixed-size blocks; entry handles address them in
	/// stride-2 units (see FNamePool::GetEntry), so a block spans 65536 units = 0x20000 bytes.
	inline constexpr uint32_t BlockSizeBytes = 0x20000;

	/// Every interned ANSI name, in pool order. Wide names are skipped (FNameEntry::String yields
	/// only ANSI); gameplay/map/package names are ANSI, so this covers the searchable surface.
	inline std::vector<std::string> names;
	inline bool built = false;

	/// Walk the FName pool block by block and collect every entry's string. Each block is packed as
	/// [uint16 header (len/flags)][string bytes], stride-2 aligned; a block fully used before a new one
	/// starts, so a zero-length header marks the unused tail — stop there. Only the current block is
	/// bounded by CurrentByteCursor.
	inline void Rebuild()
	{
		names.clear();
		if (!Engine::GNames) return;

		const uint32_t currentBlock = Engine::GNames->CurrentBlock;
		const uint32_t currentCursor = Engine::GNames->CurrentByteCursor;

		for (uint32_t block = 0; block <= currentBlock; block++)
		{
			BYTE* blockPtr = Engine::GNames->Blocks[block];
			if (!blockPtr) continue;

			const uint32_t limit = (block == currentBlock) ? currentCursor : BlockSizeBytes;
			for (uint32_t offset = 0; offset + sizeof(uint16_t) <= limit;)
			{
				FNameEntry* entry = Engine::GNames->GetEntry(FNameEntryHandle(block, offset / 2));
				const uint16_t len = entry->Len;
				if (len == 0) break; // reached the block's zeroed tail

				std::string s = entry->String();
				if (!s.empty()) names.push_back(std::move(s));

				const uint32_t bytes = sizeof(uint16_t) + (entry->bIsWide ? len * 2u : len);
				offset += (bytes + 1) & ~1u; // advance to the next stride-2 boundary
			}
		}
		built = true;
	}

	/// The cached names, building them on first use.
	inline const std::vector<std::string>& Get()
	{
		if (!built) Rebuild();
		return names;
	}
} // namespace NameCache
