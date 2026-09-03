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

	// Drives a single feature once: init on first use, refresh Enabled, skip if
	// idle-disabled, then Run() while enabled (once, if OneTime) or Destroy()
	// exactly once on the enabled -> disabled edge. Used both by the per-frame
	// render loop and by the event bus (for event-driven features). Features
	// whose Check() still returns Enabled keep their previous behavior (they just
	// skip on disable and never reach Destroy).
	inline void RunFeature(Feature& feature)
	{
		try
		{
			if (!feature.Initialized)
			{
				feature.Init();
			};

			feature.UpdateEnabled();

			// Idle disabled features have nothing to run and nothing applied to
			// revert, so skip their per-frame validity work entirely.
			if (!feature.Enabled && !feature.applied)
			{
				return;
			};

			if (!feature.Check())
			{
				return;
			};

			if (feature.Enabled)
			{
				if (!feature.OneTime || !feature.hasRun)
				{
					feature.Run();
					feature.hasRun = true;
				}
				feature.applied = true;
			}
			else
			{
				if (feature.applied)
				{
					feature.Destroy();
					feature.applied = false;
				}
				feature.hasRun = false;
			}
		}
		catch (char* e)
		{
			Logger::Log("ERROR", "Failed to execute feature: " + std::string(e));
		}
	}

	// Runs every render-driven feature. Called once per rendered frame from
	// PostRender. Event-driven features (Event != "render") are skipped here and
	// run from the event bus instead.
	inline void Execute()
	{
		for (const auto& feature : Features)
		{
			if (feature->Event == Events::Type::Render)
			{
				RunFeature(*feature);
			}
		}
	}
};
