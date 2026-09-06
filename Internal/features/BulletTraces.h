#pragma once

/// @file
/// The BulletTraces feature: records the path of each live projectile and draws a fading trail for
/// a few seconds, so you can see where shots went. Trails linger after the projectile despawns and
/// are only ever drawn from recorded positions (never by dereferencing a dead actor).

#include "Feature.h"
#include "../ue/Engine.h"
#include "../cache/ActorCache.h"
#include "../native/WorldToScreen.h"
#include "../utils/Rgb.h"
#include "../render/Render.h"

#include <chrono>
#include <unordered_map>
#include <vector>

class BulletTraces : public Feature
{
  private:
	struct Point
	{
		FVector pos;
		float time; ///< seconds (steady clock) when recorded
	};

	std::unordered_map<void*, std::vector<Point>> trails; ///< keyed by actor pointer

	static float Now()
	{
		using namespace std::chrono;
		return duration<float>(steady_clock::now().time_since_epoch()).count();
	}

  public:
	BulletTraces()
	{
		Name = "BulletTraces";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.VISUALS.BulletTraces;
	};

	bool Check()
	{
		if (!Initialized) return false;
		if (!Engine::IsInGame) return false;
		if (!Engine::World || !Engine::Canvas) return false;
		return true;
	};

	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	/// Forget all recorded trails on the enabled -> disabled edge, so re-enabling starts clean.
	void Destroy()
	{
		trails.clear();
	};

	void Run()
	{
		const float now = Now();
		const float duration = Settings.VISUALS.BulletTraceDuration;

		// 1. Append the current position of every live projectile (from the shared actor pass).
		// Projectiles move via a ProjectileMovementComponent, so the offset location can be stale — use
		// the reliable UFunction (only a handful are live, so the per-projectile ProcessEvent is cheap).
		for (AActor* actor : ActorCache::Projectiles())
			trails[actor].push_back({actor->K2_GetActorLocation(), now});

		// 2. Prune expired points, draw the rest (fading by age), and drop empty trails.
		const Render::Color base = Settings.MENU.Rgb ? Render::Color(Rgb::Current()) : Render::Color(Settings.VISUALS.BulletTraceColor);

		for (auto it = trails.begin(); it != trails.end();)
		{
			auto& points = it->second;
			while (!points.empty() && now - points.front().time > duration)
				points.erase(points.begin());

			if (points.size() < 2)
			{
				if (points.empty())
				{
					it = trails.erase(it);
					continue;
				}
				++it;
				continue;
			}

			FVector2D prevScreen{};
			bool havePrev = false;
			for (const auto& point : points)
			{
				FVector2D screen{};
				if (!Projection::WorldToScreen(point.pos, screen))
				{
					havePrev = false;
					continue;
				}
				if (havePrev)
				{
					const float age = now - point.time;
					const float alpha = base.a * (1.f - age / duration); // fade with age
					Render::Line(prevScreen, screen, 1.f, Render::Color{base.r, base.g, base.b, alpha});
				}
				prevScreen = screen;
				havePrev = true;
			}
			++it;
		}
	};
};
