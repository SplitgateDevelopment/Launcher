#pragma once

/// @file
/// @brief Top-level in-game GUI: owns the main ImGui window and dispatches to each tab section.

#include "../settings/Settings.h"
#include "../scripting/Events.h"
#include "sections/Misc.h"
#include "sections/Exploits.h"
#include "sections/Visuals.h"
#include "sections/Network.h"
#include "sections/Settings.h"
#include "sections/Profiles.h"
#include "sections/Debug.h"
#include "sections/Watermark.h"

#include <imgui.h>

/// @brief Top-level menu: owns the main window and routes each frame to the tab sections.
namespace Menu
{
	static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
	static int tab = 0;
	static ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;

	/// @brief Renders one full frame of the GUI.
	/// Always draws the watermark; toggles menu visibility on the configured hotkey / gamepad Start
	/// (dispatching MenuOpened/MenuClosed), and when the menu is visible draws the main window with its
	/// tab bar (Misc, Exploits, Visuals, Settings, Debug) plus the optional ImGui demo/style-editor windows.
	void Draw()
	{
		Menu::Sections::Watermark();

		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		if ((GetAsyncKeyState(Settings.MENU.ShowHotkey) & 1) || ImGui::IsKeyPressed(ImGuiKey_GamepadStart))
		{
			Settings.MENU.ShowMenu = !Settings.MENU.ShowMenu;
			Events::Dispatch(Settings.MENU.ShowMenu ? Events::Type::MenuOpened : Events::Type::MenuClosed);
		}
		if (!Settings.MENU.ShowMenu) return;

		if (Settings.DEBUG.ShowDemoWindow)
		{
			ImGui::ShowDemoWindow(&Settings.DEBUG.ShowDemoWindow);
		}
		if (Settings.DEBUG.ShowStyleEditor)
		{
			ImGui::ShowStyleEditor();
		}

		if (!ImGui::Begin("Splitgate Internal", &Settings.MENU.ShowMenu, windowFlags))
		{
			ImGui::End();
			return;
		};

		if (!ImGui::BeginTabBar("MainTabBar", tabFlags))
		{
			ImGui::End();
			return;
		};

		if (ImGui::BeginTabItem("Misc"))
		{
			Menu::Sections::MiscTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Exploits"))
		{
			Menu::Sections::ExploitsTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Visuals"))
		{
			Menu::Sections::VisualsTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Network"))
		{
			Menu::Sections::NetworkTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Settings"))
		{
			Menu::Sections::SettingsTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Profiles"))
		{
			Menu::Sections::ProfilesTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Debug"))
		{
			Menu::Sections::DebugTab();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
		ImGui::End();
	};
}; // namespace Menu