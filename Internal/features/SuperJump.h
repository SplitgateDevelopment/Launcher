#pragma once

/// @file
/// The SuperJump feature: launches the local character upward when the configured hotkey is pressed.
/// Driven by the Events::HotKeyPressed edge event (fired by Input::DispatchHotKeys), so it doesn't
/// poll every frame.

#include "Feature.h"
#include "../ue/Engine.h"
#include "../scripting/Events.h"

class SuperJump : public Feature
{
  public:
	SuperJump()
	{
		Name = "SuperJump";
		UpdateEnabled();
		Log("Created");
	};

	void UpdateEnabled()
	{
		Enabled = Settings.EXPLOITS.SuperJump;
	};

	// Acts from the HotKeyPressed handler (registered in Init), not the per-frame render loop.
	bool Check()
	{
		return false;
	};

	void Init()
	{
		Initialized = true;
		Events::Register(Events::Type::HotKeyPressed, [](const Events::Payload& p)
						 {
			if (!Settings.EXPLOITS.SuperJump) return;
			if (static_cast<int>(p.value) != Settings.EXPLOITS.SuperJumpKey) return;
			if (!Engine::IsInGame) return;
			if (auto* c = reinterpret_cast<APortalWarsCharacter*>(Engine::PlayerController->Character))
				c->LaunchCharacter(FVector{0.f, 0.f, Settings.EXPLOITS.SuperJumpForce}, false, true); });
		Log("Initialized");
	};

	void Destroy() {
	};

	void Run() {
	};
};
