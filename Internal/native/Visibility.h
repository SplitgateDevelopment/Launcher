#pragma once

/// @file
/// @brief Line-of-sight visibility for a character, shared by the ESP visibility recolor and the aim
/// features. Traces from the viewer's eye to the target's chest (ignoring the target) via
/// LineTraceVisible — a real occlusion test. This deliberately does NOT use WasRecentlyRendered or
/// the LastRenderTimeOnScreen/LastSubmitTime timestamps: those only tell you the actor is in the view
/// frustum (they stay "true" through walls), so they can't drive a visible/occluded ESP.

#include "../ue/Engine.h"
#include "ActorLocation.h"

namespace Visibility
{
	/// Whether @p target is in line of sight from @p eye (nothing solid between the eye and the
	/// target's chest). @p eye is the viewer's eye/camera world position — the caller passes it so the
	/// per-frame POV is computed once. Returns false for a null target.
	///
	/// @note One line trace per call (a ProcessEvent), so only invoke it behind a "visibility check"
	/// gate. Uses trace channel 0 (Visibility) via LineTraceVisible — the one value to verify in-game
	/// if occlusion looks inverted.
	inline bool IsVisible(APortalWarsCharacter* target, const FVector& eye)
	{
		if (!target) return false;

		FVector point = target->Mesh ? target->Mesh->GetBoneMatrix(BoneFNames::spine_03)
									 : ActorLocation(reinterpret_cast<AActor*>(target));

		return LineTraceVisible(reinterpret_cast<UObject*>(Engine::PlayerController), eye, point,
								reinterpret_cast<AActor*>(target));
	}

	/// The local viewer's eye position (local pawn location at ~eye height), or {0,0,0} if not in a
	/// game. Convenience so callers that don't already have it can pass a consistent origin.
	inline FVector LocalEye()
	{
		if (!Engine::PlayerController) return FVector{0.f, 0.f, 0.f};
		auto* pawn = reinterpret_cast<AActor*>(Engine::PlayerController->AcknowledgedPawn);
		if (!pawn) return FVector{0.f, 0.f, 0.f};
		FVector eye = ActorLocation(pawn);
		eye.Z += 80.f; // rough eye height
		return eye;
	}
} // namespace Visibility
