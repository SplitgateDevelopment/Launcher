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
	inline void Execute()
	{
		try
		{
			for (const auto& Feature : Features)
			{
				if (!Feature->Initialized)
				{
					Feature->Init();
				};

				if (!Feature->Check())
				{
					continue;
				};

				Feature->Enabled ? Feature->Run() : Feature->Destroy();
			}
		}
		catch (char* e)
		{
			Logger::Log("ERROR", "Failed to execute feature: " + std::string(e));
		}
	}
};
