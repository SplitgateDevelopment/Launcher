#pragma once

/// @file
/// @brief Lazily-built, shared list of every UFont in GObjects (short name + pointer), so the UE-canvas
/// renderer can draw text in a chosen game font instead of the default Roboto. Mirrors ClassCache: fonts
/// load once and rarely change, so the walk is done on demand and reused; Rebuild() (a Refresh button)
/// picks up newly-loaded fonts.

#include <string>
#include <vector>

#include "../ue/Engine.h"

namespace FontCache
{
	/// One cached font: its short name (for display / the setting) and the object pointer.
	struct Entry
	{
		std::string name;
		UFont* font;
	};

	inline std::vector<Entry> fonts;
	inline bool built = false;

	/// Walk GObjects once and collect every UFont instance (skipping the class default object).
	inline void Rebuild()
	{
		fonts.clear();
		built = true;
		if (!Engine::GObjects) return;

		UObject* fontClass = Engine::GObjects->FindObject("Class Engine.Font");
		if (!fontClass) return;

		const auto count = Engine::GObjects->NumElements;
		for (auto i = 0u; i < count; i++)
		{
			auto* obj = Engine::GObjects->GetObjectPtr(i);
			if (!obj || obj->IsDefaultObject() || !obj->IsA(fontClass)) continue;
			fonts.push_back({obj->GetName(), reinterpret_cast<UFont*>(obj)});
		}
	}

	/// The cached fonts, building them on first use.
	inline const std::vector<Entry>& Get()
	{
		if (!built) Rebuild();
		return fonts;
	}

	/// Resolve a font by its short name; nullptr for an empty/unknown name (→ the renderer's default).
	inline UFont* Find(const std::string& name)
	{
		if (name.empty()) return nullptr;
		for (const Entry& e : Get())
			if (e.name == name) return e.font;
		return nullptr;
	}
} // namespace FontCache
