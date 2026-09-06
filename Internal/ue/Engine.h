#pragma once

/// @file
/// @brief The Splitgate UE SDK entry point the hooks and features include.
///
/// Pulls together the generated SDK (sdk.h -> ue/sdk/*), the hand-written free
/// helpers (custom.h), and namespace Engine (the scanned globals, resolved game
/// objects, Init/ResolveObjects, and the StaticClass<T>/GetDefaultObj<T> helpers).
#include <atomic>

#include "sdk.h"

// Hand-written free helpers layered on the generated SDK (bodies in custom.cpp).
#include "custom.h"

/// Engine runtime state: the signature-scanned internals, the resolved game objects,
/// and the two bootstrap steps that fill them in.
namespace Engine
{
	// --- Signature-scanned internals (raw addresses, resolved once by Init) ---
	inline FNamePool* GNames = nullptr;		  ///< the FName pool
	inline TUObjectArray* GObjects = nullptr; ///< the global UObject array
	inline UWorld* GWorld = nullptr;		  ///< address of the game's UWorld* slot
	inline uintptr_t GetBoneMatrixFn = 0;	  ///< scanned GetBoneMatrix function pointer (bone projection)

	// --- Resolved game objects (cached pointers, filled by ResolveObjects) ---
	inline UEngine* GEngine = nullptr;						 ///< the global UEngine
	inline UWorld* World = nullptr;							 ///< the current UWorld (resolved snapshot)
	inline APortalWarsPlayerController* PlayerController = nullptr; ///< local player controller (updated as it changes)
	inline UGameplayStatics* GameplayStatics = nullptr;		 ///< UGameplayStatics CDO
	inline UKismetStringLibrary* KismetStringLibrary = nullptr; ///< UKismetStringLibrary CDO
	inline UKismetTextLibrary* KismetTextLibrary = nullptr;	 ///< UKismetTextLibrary CDO
	inline UCanvas* Canvas = nullptr;						 ///< draw canvas (set during rendering)

	/// Cached PlayerController->IsInGame(), refreshed by PostRender on the game thread. The external
	/// overlay renders the menu on its own thread; it reads this flag instead of dereferencing the
	/// controller, which the game may have freed mid map-load (a null check can't catch a freed object).
	inline std::atomic<bool> IsInGame = false;

	/// One-time bootstrap: resolve the scanned internals from byte signatures. Returns false if
	/// any signature fails. (UFunctions are resolved lazily at each wrapper's first call.)
	bool Init();

	/// Resolve GEngine/World and the static-library CDOs. Call after the engine is up and again
	/// on each map load (the world changes). Does not touch PlayerController/Canvas (set per-frame).
	void ResolveObjects();

	/// Look up a UClass by its dumped path (T::ClassName), cached per type on first use.
	/// Every SDK type that can be looked up declares `static constexpr const char* ClassName`.
	template <class T>
	UClass* StaticClass()
	{
		static UClass* clss = reinterpret_cast<UClass*>(GObjects->FindObject(T::ClassName));
		return clss;
	}

	/// The class-default object for T (the game reuses the UClass as the object), cached per type.
	template <class T>
	T* GetDefaultObj()
	{
		static T* def = reinterpret_cast<T*>(StaticClass<T>());
		return def;
	}
} // namespace Engine
