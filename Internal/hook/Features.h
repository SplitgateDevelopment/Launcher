#pragma once
#include "ue/Engine.h"
#include "../settings/Settings.h"
#include "../utils/Logger.h"
#include "../discord/rpc.h"


#include "Feature.h"
#include "features/GodMode.h"
#include "features/PlayerModifications.h"
#include "features/LoadIntoMap.h"
#include "features/NoRecoil.h"

namespace Features
{
	inline std::vector<std::unique_ptr<Feature>> Features;

	void Init()
	{
		Features.push_back(std::make_unique<GodMode>());
		Features.push_back(std::make_unique<PlayerModifications>());
		Features.push_back(std::make_unique<LoadIntoMap>());
		Features.push_back(std::make_unique<NoRecoil>());
	};
};