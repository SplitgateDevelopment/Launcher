#pragma once

/// @file
/// Concrete feature wiring: pulls in every feature header, registers one
/// instance of each in the runner, and connects them to the event bus
/// (SettingsChanged, event-driven features, and the LoadIntoMap one-shot).

#include "FeatureRunner.h"
#include "../utils/Globals.h"
#include "../scripting/Events.h"

#include "GodMode.h"
#include "PlayerModifications.h"
#include "NoRecoil.h"
#include "SpinBot.h"
#include "Esp.h"
#include "UserScripts.h"
#include "InfiniteJetpack.h"
#include "WeaponModifications.h"
#include "ThirdPerson.h"
#include "FreeCam.h"
#include "Radar.h"
#include "DebugNames.h"
#include "Aimbot.h"
#include "Triggerbot.h"
#include "AimFov.h"
#include "AnnounceToggles.h"
#include "Crosshair.h"
#include "BulletTraces.h"
#include "Glow.h"
#include "Phasing.h"
#include "BulletTp.h"
#include "DiscordPresence.h"
#include "../network/Network.h"

namespace Features
{
	/// Populate the registry with one instance of each feature, seed their
	/// Enabled state, and wire event-bus subscriptions. Call once at startup,
	/// after Globals and the event system are ready.
	void Init()
	{
		Features.push_back(std::make_unique<GodMode>());
		Features.push_back(std::make_unique<PlayerModifications>());
		Features.push_back(std::make_unique<NoRecoil>());
		Features.push_back(std::make_unique<SpinBot>());
		Features.push_back(std::make_unique<Esp>());
		Features.push_back(std::make_unique<UserScripts>());
		Features.push_back(std::make_unique<InfiniteJetpack>());
		Features.push_back(std::make_unique<WeaponModifications>());
		Features.push_back(std::make_unique<ThirdPerson>());
		Features.push_back(std::make_unique<FreeCam>());
		Features.push_back(std::make_unique<Radar>());
		Features.push_back(std::make_unique<DebugNames>());
		Features.push_back(std::make_unique<Aimbot>());
		Features.push_back(std::make_unique<Triggerbot>());
		Features.push_back(std::make_unique<AimFov>());
		Features.push_back(std::make_unique<AnnounceToggles>());
		Features.push_back(std::make_unique<Crosshair>());
		Features.push_back(std::make_unique<BulletTraces>());
		Features.push_back(std::make_unique<Glow>());
		Features.push_back(std::make_unique<Phasing>());
		Features.push_back(std::make_unique<BulletTp>());
		Features.push_back(std::make_unique<DiscordPresence>());

		// The network subsystem (redirect + HTTP logging) is not a per-frame feature: install
		// its hooks once here. They self-gate on Settings.NETWORK, and MinHook is already
		// initialized by this point (Hook::Init runs before Features::Init).
		Network::Init();

		// Seed Enabled from current settings, then keep it in sync reactively:
		// the menu dispatches SettingsChanged on every change, so features no
		// longer poll their setting every frame.
		for (auto& feature : Features)
			feature->UpdateEnabled();
		Events::Register(Events::Type::SettingsChanged, []
						 {
			for (auto& feature : Features) feature->UpdateEnabled();

			// Autosave: persist on every change when enabled.
			if (Settings.MISC.AutoSave) SettingsHelper::File().Save(); });

		// Subscribe event-driven features (Event != "render") to the event bus;
		// render features run from Features::Execute each frame instead.
		for (auto& feature : Features)
		{
			if (feature->Event != Events::Type::Render)
			{
				Events::Register(feature->Event, [ptr = feature.get()]
								 { RunFeature(*ptr); });
			}
		}

		// One-shot action, triggered from the "Load into map" button. Replaces
		// the old LoadIntoMap feature + Settings.MISC.LoadIntoMap flag.
		Events::Register(Events::Type::LoadIntoMap, []
						 {
			auto* controller = Globals::PlayerController;
			if (controller && !controller->IsInGame())
			{
				Logger::Log("INFO", "Loading into map");
				controller->SwitchLevel(L"Simulation_Alpha");
			} });
	};
}; // namespace Features
