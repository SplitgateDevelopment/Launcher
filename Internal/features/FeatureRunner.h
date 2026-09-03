#pragma once

#include <vector>
#include <memory>

#include "Feature.h"

// The feature registry and per-frame execution loop, kept in a header that only
// depends on Feature.h (no game-specific feature includes) so it can be unit
// tested in isolation. Features.h includes this and adds the concrete feature
// registration on top.
namespace Features
{
	inline std::vector<std::unique_ptr<Feature>> Features;

	// Runs a single execution pass over every registered feature. Called once
	// per rendered frame from PostRender.
	//
	// A feature is Run() while enabled (once, if OneTime) and Destroy()'d exactly
	// once when it goes enabled -> disabled, so a disabled feature costs nothing
	// beyond UpdateEnabled()/Check() instead of Destroy() every frame. Features
	// whose Check() still returns Enabled keep their previous behavior (they just
	// skip on disable and never reach Destroy).
	inline void Execute()
	{
		try
		{
			for (const auto& feature : Features)
			{
				if (!feature->Initialized)
				{
					feature->Init();
				};

				feature->UpdateEnabled();

				if (!feature->Check())
				{
					continue;
				};

				if (feature->Enabled)
				{
					if (!feature->OneTime || !feature->hasRun)
					{
						feature->Run();
						feature->hasRun = true;
					}
					feature->applied = true;
				}
				else
				{
					if (feature->applied)
					{
						feature->Destroy();
						feature->applied = false;
					}
					feature->hasRun = false;
				}
			}
		}
		catch (char* e)
		{
			Logger::Log("ERROR", "Failed to execute feature: " + std::string(e));
		}
	}
};
