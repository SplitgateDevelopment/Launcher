#pragma once

/// @file
/// The feature registry container and the per-feature tick/lifecycle loop
/// (RunFeature) plus the per-frame render pass (Execute). Depends only on
/// Feature.h so it stays unit-testable; Features.h layers concrete features on top.

#include <vector>
#include <memory>
#include <algorithm>

#include "Feature.h"

// The feature registry and per-frame execution loop, kept in a header that only
// depends on Feature.h (no game-specific feature includes) so it can be unit
// tested in isolation. Features.h includes this and adds the concrete feature
// registration on top.
/// Feature registry plus execution loop. Owns every registered Feature and
/// drives them from the render hook and the event bus.
namespace Features
{
	/// The registry: owns every registered feature for the lifetime of the DLL.
	inline std::vector<std::unique_ptr<Feature>> Features;

	// Drives a single feature once: init on first use, refresh Enabled, skip if
	// idle-disabled, then Run() while enabled (once, if OneTime) or Destroy()
	// exactly once on the enabled -> disabled edge. Used both by the per-frame
	// render loop and by the event bus (for event-driven features). The triggering
	// @p event and @p payload are forwarded to Run() (defaulting to the Render loop's
	// empty payload). Features whose Check() still returns Enabled keep their previous
	// behavior (they just skip on disable and never reach Destroy).
	inline void RunFeature(Feature& feature, Events::Type event = Events::Type::Render, const Events::Payload& payload = {})
	{
		try
		{
			if (!feature.Initialized)
			{
				feature.Init();
			};

			// Enabled is refreshed reactively on Events::Type::SettingsChanged
			// (see Features::Init), not polled here every frame.

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
					feature.Run(event, payload);
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
	// PostRender. Features without a Render trigger are skipped here and run from
	// the event bus instead (a feature may have both).
	inline void Execute()
	{
		for (const auto& feature : Features)
		{
			const auto& triggers = feature->Triggers;
			if (std::find(triggers.begin(), triggers.end(), Events::Type::Render) != triggers.end())
			{
				RunFeature(*feature);
			}
		}
	}
}; // namespace Features
