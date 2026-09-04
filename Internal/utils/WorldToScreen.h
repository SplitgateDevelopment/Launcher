#pragma once

/// @file
/// @brief Native (math) WorldToScreen from the cached camera POV — no ProcessEvent, unlike the
/// game's ProjectWorldLocationToScreen. Every bone/box/name projection went through a UFunction
/// before; this is the big projection-cost win for the ESP/aim. Both the projection and the bone
/// path fall back to the game's UFunctions when their Debug toggles are off
/// (Settings.DEBUG.NativeWorldToScreen / NativeBones).

#include <cmath>

#include "Globals.h"
#include "../settings/Settings.h"

namespace Projection
{
	/// Project a world point to screen space using the camera's cached POV. False (leaving @p out
	/// unchanged) when the point is behind the camera or the POV/canvas isn't ready yet. When
	/// Settings.DEBUG.NativeWorldToScreen is off, defers to the game's ProjectWorldLocationToScreen.
	inline bool WorldToScreen(const FVector& world, FVector2D& out)
	{
		if (!Globals::PlayerController || !Globals::Canvas) return false;

		// Fallback path: the game's projection UFunction (a ProcessEvent per point).
		if (!Settings.DEBUG.NativeWorldToScreen)
			return Globals::PlayerController->ProjectWorldLocationToScreen(world, out, false);

		auto* manager = Globals::PlayerController->PlayerCameraManager;
		if (!manager) return false;

		// Pick the freshest *populated* camera cache. This engine version keeps both the deprecated
		// public CameraCache and the CameraCachePrivate that UE 4.24+ actually fills each frame; the
		// public mirror can be stale or zeroed here, and projecting through a zero POV is what made the
		// overlay drift ("floating boxes"). Prefer the newer timestamp, skipping a cache whose FOV
		// isn't set yet, and bail if neither is ready.
		const FCameraCacheEntry& priv = manager->CameraCachePrivate;
		const FCameraCacheEntry& pub = manager->CameraCache;
		const FMinimalViewInfo* pov = nullptr;
		if (priv.POV.FOV >= 1.f && (pub.POV.FOV < 1.f || priv.Timestamp >= pub.Timestamp))
			pov = &priv.POV;
		else if (pub.POV.FOV >= 1.f)
			pov = &pub.POV;
		if (!pov) return false;

		constexpr float toRad = 3.14159265f / 180.f;
		const float pitch = pov->Rotation.Pitch * toRad;
		const float yaw = pov->Rotation.Yaw * toRad;
		const float roll = pov->Rotation.Roll * toRad;
		const float cp = cosf(pitch), sp = sinf(pitch);
		const float cy = cosf(yaw), sy = sinf(yaw);
		const float cr = cosf(roll), sr = sinf(roll);

		// UE FRotationMatrix basis vectors.
		const FVector forward{cp * cy, cp * sy, sp};
		const FVector right{sr * sp * cy - cr * sy, sr * sp * sy + cr * cy, -sr * cp};
		const FVector up{-(cr * sp * cy + sr * sy), cy * sr - cr * sp * sy, cr * cp};

		const FVector delta{world.X - pov->Location.X, world.Y - pov->Location.Y, world.Z - pov->Location.Z};

		const float depth = delta.X * forward.X + delta.Y * forward.Y + delta.Z * forward.Z;
		if (depth < 1.f) return false; // behind the camera

		const float x = delta.X * right.X + delta.Y * right.Y + delta.Z * right.Z;
		const float y = delta.X * up.X + delta.Y * up.Y + delta.Z * up.Z;

		const float halfW = Globals::Canvas->ClipX * 0.5f;
		const float halfH = Globals::Canvas->ClipY * 0.5f;
		const float scale = halfW / tanf(pov->FOV * 0.5f * toRad);

		out.X = halfW + x * scale / depth;
		out.Y = halfH - y * scale / depth;
		return true;
	}

	/// Project a mesh bone (BoneFNames index) to screen space; {0,0} when behind the camera (the
	/// ESP's off-screen convention). Native path: GetBoneMatrix (by signature) + the math
	/// WorldToScreen above. When Settings.DEBUG.NativeBones is off, uses the game's combined bone
	/// projection (USkeletalMeshComponent::GetBone = GetBoneMatrix + ProjectWorldLocationToScreen).
	inline FVector2D Bone(USkeletalMeshComponent* mesh, int index)
	{
		if (!Settings.DEBUG.NativeBones)
		{
			if (!Globals::PlayerController) return {0, 0};
			return mesh->GetBone(index, Globals::PlayerController);
		}

		FVector2D out{};
		WorldToScreen(mesh->GetBoneMatrix(index), out);
		return out;
	}
} // namespace Projection
