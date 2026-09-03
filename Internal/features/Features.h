#pragma once

#include "FeatureRunner.h"
#include "../utils/Globals.h"
#include "../scripting/Events.h"

#include "GodMode.h"
#include "PlayerModifications.h"
#include "NoRecoil.h"
#include "SpinBot.h"
#include "DrawActors.h"
#include "UserScripts.h"
#include "InfiniteJetpack.h"
#include "WeaponModifications.h"
#include "ThirdPerson.h"

namespace Features
{
	void Init()
	{
		Features.push_back(std::make_unique<GodMode>());
		Features.push_back(std::make_unique<PlayerModifications>());
		Features.push_back(std::make_unique<NoRecoil>());
		Features.push_back(std::make_unique<SpinBot>());
		Features.push_back(std::make_unique<DrawActors>());
		Features.push_back(std::make_unique<UserScripts>());
		Features.push_back(std::make_unique<InfiniteJetpack>());
		Features.push_back(std::make_unique<WeaponModifications>());
		Features.push_back(std::make_unique<ThirdPerson>());

		// Seed Enabled from current settings, then keep it in sync reactively:
		// the menu dispatches SettingsChanged on every change, so features no
		// longer poll their setting every frame.
		for (auto& feature : Features) feature->UpdateEnabled();
		Events::Register(Events::Type::SettingsChanged, [] {
			for (auto& feature : Features) feature->UpdateEnabled();
		});

		// Subscribe event-driven features (Event != "render") to the event bus;
		// render features run from Features::Execute each frame instead.
		for (auto& feature : Features)
		{
			if (feature->Event != Events::Type::Render)
			{
				Events::Register(feature->Event, [ptr = feature.get()] { RunFeature(*ptr); });
			}
		}

		// One-shot action, triggered from the "Load into map" button. Replaces
		// the old LoadIntoMap feature + Settings.MISC.LoadIntoMap flag.
		Events::Register(Events::Type::LoadIntoMap, [] {
			auto* controller = Globals::PlayerController;
			if (controller && !controller->IsInGame())
			{
				Logger::Log("INFO", "Loading into map");
				controller->SwitchLevel(L"Simulation_Alpha");
			}
		});
	};
};
