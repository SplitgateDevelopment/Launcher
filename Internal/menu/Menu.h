#pragma once

/// @file
/// @brief Top-level menu driver: owns the tab list and the per-frame frame loop, and drives whichever
/// backend is active through the abstract Menu::Backend interface. It names no concrete backend and
/// includes neither ImGui nor UCanvasGUI — all backend-specific chrome/widgets live in menu/backend/.

#include "../settings/Settings.h"
#include "../scripting/Events.h"
#include "../utils/Input.h"
#include "backend/Backend.h"
#include "backend/Backends.h"
#include "sections/Misc.h"
#include "sections/Exploits.h"
#include "sections/Visuals.h"
#include "sections/Aim.h"
#include "sections/Network.h"
#include "sections/Config.h"
#include "sections/Scripts.h"
#include "sections/Sdk.h"
#include "sections/Discord.h"
#include "sections/Debug.h"

namespace Menu
{
	/// One menu tab: its label and the section function that draws it (through Menu::UI).
	struct Tab
	{
		const char* label;
		void (*fn)();
	};

	/// The tab order, shared by every backend (each renders it through its own window/tab chrome).
	inline const Tab Tabs[] = {
		{"Misc", Sections::MiscTab},
		{"Exploits", Sections::ExploitsTab},
		{"Visuals", Sections::VisualsTab},
		{"Aim", Sections::AimTab},
		{"Network", Sections::NetworkTab},
		{"Config", Sections::ConfigTab},
		{"Scripts", Sections::ScriptsTab},
		{"SDK", Sections::SdkTab},
		{"Discord", Sections::DiscordTab},
		{"Debug", Sections::DebugTab},
	};

	/// Flip menu visibility on the show/hotkey (or @p extra — a backend's own signal, e.g. gamepad
	/// Start), dispatching MenuOpened/MenuClosed. The two backends are mutually exclusive per frame, so
	/// this never double-fires.
	inline void HandleToggle(bool extra)
	{
		if (Input::Pressed(Settings.MENU.ShowHotkey) || extra)
		{
			Settings.MENU.ShowMenu = !Settings.MENU.ShowMenu;
			Events::Dispatch(Settings.MENU.ShowMenu ? Events::Type::MenuOpened : Events::Type::MenuClosed);
		}
	}

	/// Drive one frame of the menu for a given hook @p phase. Selects the configured backend and, if it
	/// is driven by this phase, runs its frame: setup, toggle, then the window + tab loop. This is the
	/// only shared code that touches a backend, and it does so purely through the interface.
	inline void Frame(Phase phase)
	{
		Select(Settings.MENU.Backend);
		if (active->phase() != phase) return; // the backend picks its own hook

		active->BeginFrame();
		HandleToggle(active->ExtraToggle());

		if (Settings.MENU.ShowMenu && active->BeginWindow("Splitgate Internal"))
		{
			for (const Tab& tab : Tabs)
				if (active->BeginTab(tab.label))
				{
					tab.fn();
					active->EndTab();
				}
			active->EndWindow();
		}

		active->EndFrame();
	}
} // namespace Menu
