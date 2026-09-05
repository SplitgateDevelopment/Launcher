#pragma once

/// @file
/// The BulletTraces feature: records the path of each live projectile and draws a fading trail for
/// a few seconds, so you can see where shots went. Trails linger after the projectile despawns and
/// are only ever drawn from recorded positions (never by dereferencing a dead actor).

#include "Feature.h"
#include "../utils/Globals.h"
#include "../native/WorldToScreen.h"
#include "../native/ActorLocation.h"
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

	UObject* projectileClass = nullptr;
	std::unordered_map<void*, std::vector<Point>> trails; ///< keyed by actor pointer

	static FLinearColor ToColor(const Color& c) { return FLinearColor{c.R, c.G, c.B, c.A}; }

	static float Now()
	{
		using namespace std::chrono;
		return duration<float>(steady_clock::now().time_since_epoch()).count();
	}

	/// A projectile actor is still safe to read while it has a root component and isn't being torn down.
	static bool Alive(AActor* actor)
	{
		return actor && actor->RootComponent && !actor->bActorIsBeingDestroyed;
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
		if (!projectileClass) return false;
		if (!Globals::PlayerController || !Globals::PlayerController->IsInGame()) return false;
		if (!Globals::World || !Globals::Canvas) return false;
		return true;
	};

	void Init()
	{
		projectileClass = ObjObjects->FindObject("Class PortalWars.Projectile");
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

		// 1. Append the current position of every live projectile.
		auto& levels = Globals::World->Levels;
		for (int l = 0, levelCount = levels.Num(); l < levelCount; l++)
		{
			if (!levels.IsValidIndex(l)) continue;
			ULevel* level = levels[l];
			if (!level) continue;

			auto& actors = level->Actors;
			for (int a = 0, actorCount = actors.Num(); a < actorCount; a++)
			{
				if (!actors.IsValidIndex(a)) continue;
				AActor* actor = actors[a];
				if (!Alive(actor) || !actor->IsA(projectileClass)) continue;

				trails[actor].push_back({ActorLocation(actor), now});
			}
		}

		// 2. Prune expired points, draw the rest (fading by age), and drop empty trails.
		const FLinearColor base = ToColor(Settings.MENU.Rgb ? Rgb::Current() : Settings.VISUALS.BulletTraceColor);

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
					const float alpha = base.A * (1.f - age / duration); // fade with age
					Render::Line(prevScreen, screen, 1.f, FLinearColor{base.R, base.G, base.B, alpha});
				}
				prevScreen = screen;
				havePrev = true;
			}
			++it;
		}
	};
};
