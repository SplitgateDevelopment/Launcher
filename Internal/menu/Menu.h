#pragma once

/// @file
/// @brief Top-level in-game GUI: owns the main ImGui window and dispatches to each tab section.

#include "../settings/Settings.h"
#include "../scripting/Events.h"
#include "../utils/Input.h"
#include "../utils/Rgb.h"
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
#include "sections/Watermark.h"

#include <imgui.h>

#include <array>

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

		if (Input::Pressed(Settings.MENU.ShowHotkey) || ImGui::IsKeyPressed(ImGuiKey_GamepadStart))
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

		// Tint the interactive accent slots each frame: the cycling RGB color when enabled, else the
		// theme's default reds. The defaults are snapshotted once (before the first override) so
		// toggling RGB back off restores the original per-slot shades instead of freezing on the last
		// rainbow frame.
		{
			static constexpr ImGuiCol accentSlots[] = {
				ImGuiCol_CheckMark, ImGuiCol_SliderGrab, ImGuiCol_SliderGrabActive,
				ImGuiCol_Header, ImGuiCol_HeaderHovered, ImGuiCol_HeaderActive,
				ImGuiCol_Tab, ImGuiCol_TabHovered, ImGuiCol_TabActive, ImGuiCol_TitleBgActive};

			ImVec4* colors = ImGui::GetStyle().Colors;

			static const std::array<ImVec4, IM_ARRAYSIZE(accentSlots)> defaults = [&]
			{
				std::array<ImVec4, IM_ARRAYSIZE(accentSlots)> saved{};
				for (size_t i = 0; i < saved.size(); ++i) saved[i] = colors[accentSlots[i]];
				return saved;
			}();

			if (Settings.MENU.Rgb)
			{
				const Color rgb = Rgb::Current();
				const ImVec4 accent(rgb.R, rgb.G, rgb.B, rgb.A);
				for (ImGuiCol slot : accentSlots) colors[slot] = accent;
			}
			else
			{
				for (size_t i = 0; i < defaults.size(); ++i) colors[accentSlots[i]] = defaults[i];
			}
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

		if (ImGui::BeginTabItem("Aim"))
		{
			Menu::Sections::AimTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Network"))
		{
			Menu::Sections::NetworkTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Config"))
		{
			Menu::Sections::ConfigTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Scripts"))
		{
			Menu::Sections::ScriptsTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("SDK"))
		{
			Menu::Sections::SdkTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Discord"))
		{
			Menu::Sections::DiscordTab();
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