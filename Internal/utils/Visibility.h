#pragma once

/// @file
/// @brief Trace-free visibility test for a character, shared by the ESP visibility recolor and the
/// aim features. Compares the mesh's LastRenderTimeOnScreen against LastSubmitTime (two field reads,
/// no ProcessEvent) — if it rendered within a tick of being submitted, it's on screen / visible.
/// Falls back to the game's WasRecentlyRendered UFunction when the Debug NativeVisibility toggle is
/// off (or the mesh is missing), so it's always safe.

#include "../ue/Engine.h"
#include "../settings/Settings.h"

namespace Visibility
{
	/// Whether @p character is currently visible (rendered on screen within @p tolerance seconds).
	/// Native path: `LastRenderTimeOnScreen + tolerance >= LastSubmitTime` on the character's mesh —
	/// while off-screen, LastSubmitTime keeps advancing but LastRenderTimeOnScreen stalls, so the sum
	/// falls behind. Returns false for a null character; falls back to WasRecentlyRendered when the
	/// native toggle is off or there's no mesh.
	///
	/// @note "rendered" means drawn anywhere on screen, so it can read as visible through thin
	/// geometry or at frame edges — the aimbot's per-bone LineTrace mode is the stricter option.
	inline bool IsVisible(APortalWarsCharacter* character, float tolerance)
	{
		if (!character) return false;

		if (Settings.DEBUG.NativeVisibility)
			if (auto* mesh = character->Mesh)
				return mesh->LastRenderTimeOnScreen + tolerance >= mesh->LastSubmitTime;

		return reinterpret_cast<AActor*>(character)->WasRecentlyRendered(tolerance);
	}
} // namespace Visibility
