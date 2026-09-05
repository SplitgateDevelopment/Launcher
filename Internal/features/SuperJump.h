#pragma once

/// @file
/// The SuperJump feature: launches the local character upward when the configured hotkey is pressed.

#include "Feature.h"
#include "../utils/Globals.h"
#include "../utils/Input.h"

class SuperJump : public Feature
{
  private:
	bool wasDown = false; ///< edge tracking so each key press launches once

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

	bool Check()
	{
		return Initialized && Globals::PlayerController && Globals::PlayerController->IsInGame();
	};

	void Init()
	{
		Initialized = true;
		Log("Initialized");
	};

	void Destroy()
	{
		wasDown = false;
	};

	void Run()
	{
		const bool down = Input::Down(Settings.EXPLOITS.SuperJumpKey);
		if (down && !wasDown)
			if (auto* character = reinterpret_cast<APortalWarsCharacter*>(Globals::PlayerController->Character))
				character->LaunchCharacter(FVector{0.f, 0.f, Settings.EXPLOITS.SuperJumpForce}, false, true);
		wasDown = down;
	};
};
