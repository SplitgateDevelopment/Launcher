#pragma once

/// @file
/// @brief Lazily-built, shared list of every UClass / BlueprintGeneratedClass object (full name +
/// pointer). Classes load once and rarely change, so this is safe to cache and reuse — the SDK tab's
/// class search and the Misc spawn picker both read it instead of each walking GObjects. Call
/// Rebuild() (a Refresh button) to pick up newly-loaded classes.

#include <cstdint>
#include <string>
#include <vector>

#include "../ue/Engine.h"

namespace ClassCache
{
	/// One cached class: its full name (for FindObject / display) and the object pointer.
	struct Entry
	{
		std::string name;
		UObject* object;
	};

	inline std::vector<Entry> classes;
	inline bool built = false;

	/// Walk GObjects once and collect every class object. Identifies classes by their class pointer
	/// (the Class / BlueprintGeneratedClass meta-class) so GetFullName() — which allocates and walks
	/// the outer chain — is only paid on class objects, not on all ~100k GObjects.
	inline void Rebuild()
	{
		classes.clear();
		if (!Engine::ObjObjects) return;

		UObject* classMeta = Engine::ObjObjects->FindObject("Class CoreUObject.Class");
		UObject* bgcMeta = Engine::ObjObjects->FindObject("Class Engine.BlueprintGeneratedClass");
		const bool fast = (classMeta || bgcMeta);

		const auto count = Engine::ObjObjects->NumElements;
		for (auto i = 0u; i < count; i++)
		{
			auto* obj = Engine::ObjObjects->GetObjectPtr(i);
			if (!obj) continue;
			auto* cls = reinterpret_cast<UObject*>(obj->ClassPrivate);
			if (fast && cls != classMeta && cls != bgcMeta) continue;

			std::string full = obj->GetFullName();
			if (!fast && full.rfind("Class ", 0) != 0 && full.rfind("BlueprintGeneratedClass ", 0) != 0) continue;

			classes.push_back({std::move(full), obj});
		}
		built = true;
	}

	/// The cached classes, building them on first use.
	inline const std::vector<Entry>& Get()
	{
		if (!built) Rebuild();
		return classes;
	}
} // namespace ClassCache
