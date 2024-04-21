#pragma once

#include "Feature.h"

#include "features/GodMode.h"
#include "features/PlayerModifications.h"
#include "features/LoadIntoMap.h"
#include "features/NoRecoil.h"
#include "features/SpinBot.h"
#include "features/DrawActors.h"
#include "features/UserScripts.h"
#include "features/InfiniteJetpack.h"
#include "features/WeaponModifications.h"

namespace Features
{
	inline std::vector<std::unique_ptr<Feature>> Features;

	void Init()
	{
		Features.push_back(std::make_unique<GodMode>());
		Features.push_back(std::make_unique<PlayerModifications>());
		Features.push_back(std::make_unique<LoadIntoMap>());
		Features.push_back(std::make_unique<NoRecoil>());
		Features.push_back(std::make_unique<SpinBot>());
		Features.push_back(std::make_unique<DrawActors>());
		Features.push_back(std::make_unique<UserScripts>());
		Features.push_back(std::make_unique<InfiniteJetpack>());
		Features.push_back(std::make_unique<WeaponModifications>());
	};
};