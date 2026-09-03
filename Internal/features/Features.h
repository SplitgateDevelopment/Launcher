#pragma once

#include "FeatureRunner.h"

#include "GodMode.h"
#include "PlayerModifications.h"
#include "LoadIntoMap.h"
#include "NoRecoil.h"
#include "SpinBot.h"
#include "DrawActors.h"
#include "UserScripts.h"
#include "InfiniteJetpack.h"
#include "WeaponModifications.h"

namespace Features
{
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
