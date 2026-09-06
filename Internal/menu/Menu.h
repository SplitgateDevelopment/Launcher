#pragma once

/// @file
/// @brief Top-level in-game GUI: owns the main ImGui window and dispatches to each tab section.

#include "../settings/Settings.h"
#include "../scripting/Events.h"
#include "../utils/Input.h"
#include "../utils/Rgb.h"
#include "ui/UI.h"
#include "canvas/ZeroGUI.h"
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

#include <imgui.h>

#include <array>

/// @brief Top-level menu: owns the main window and routes each frame to the tab sections.
namespace Menu
{
	static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
	static int tab = 0;
	static ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;

	/// Canvas-backend window position (draggable) and active tab index. ImGui keeps its own state.
	inline FVector2D canvasPos = {400.f, 200.f};
	inline int canvasTab = 0;

	/// Flip menu visibility on the show/hotkey (or @p extra, e.g. the gamepad Start button), dispatching
	/// MenuOpened/MenuClosed. Factored out so both the ImGui (Present) and Canvas (PostRender) paths share
	/// one toggle; the two backends are mutually exclusive per frame, so this never double-fires.
	inline void HandleToggle(bool extra = false)
	{
		if (Input::Pressed(Settings.MENU.ShowHotkey) || extra)
		{
			Settings.MENU.ShowMenu = !Settings.MENU.ShowMenu;
			Events::Dispatch(Settings.MENU.ShowMenu ? Events::Type::MenuOpened : Events::Type::MenuClosed);
		}
	}

	/// @brief Renders one full frame of the ImGui menu (from the Present hook, ImGui backend only).
	/// Toggles menu visibility on the configured hotkey / gamepad Start, and when visible draws the main
	/// window with its tab bar plus the optional ImGui demo/style-editor windows.
	void Draw()
	{
		// The watermark is its own Watermark feature now (drawn through the Render backend), so it
		// follows the active renderer — including the streamproof external window — and isn't drawn here.
		HandleToggle(ImGui::IsKeyPressed(ImGuiKey_GamepadStart));
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
				for (size_t i = 0; i < saved.size(); ++i)
					saved[i] = colors[accentSlots[i]];
				return saved;
			}();

			if (Settings.MENU.Rgb)
			{
				const Color rgb = Rgb::Current();
				const ImVec4 accent(rgb.R, rgb.G, rgb.B, rgb.A);
				for (ImGuiCol slot : accentSlots)
					colors[slot] = accent;
			}
			else
			{
				for (size_t i = 0; i < defaults.size(); ++i)
					colors[accentSlots[i]] = defaults[i];
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

	/// @brief Renders one full frame of the Canvas (ZeroGUI) menu, drawn through Render::canvas from the
	/// PostRender hook (Canvas backend only). Works everywhere the UE canvas is valid — in a match and at
	/// the main menu / loading. Owns its own software cursor, so it never depends on ImGui.
	void Tick()
	{
		ZeroGUI::Input::Handle(); // sample mouse/keyboard once per frame

		HandleToggle();
		if (!Settings.MENU.ShowMenu) return;

		// RGB accent: retint the red accent group from the rainbow when enabled, else the theme red.
		{
			const FLinearColor accent = Settings.MENU.Rgb
											? []
			{ const Color c = Rgb::Current(); return FLinearColor{c.R, c.G, c.B, c.A}; }()
											: FLinearColor{1.f, 0.f, 0.f, 1.f};
			ZeroGUI::Colors::MainColor = accent;
			ZeroGUI::Colors::Window_Header = accent;
			ZeroGUI::Colors::Button_Idle = accent;
			ZeroGUI::Colors::Button_Hovered = accent;
			ZeroGUI::Colors::Button_Active = accent;
			ZeroGUI::Colors::Slider_Progress = accent;
		}

		const FVector2D winSize{700.f, 500.f};
		if (!ZeroGUI::Window("Splitgate Internal", &canvasPos, winSize, true)) return;

		// Left column: one tab button per section (stacks under the header).
		static const char* const tabs[] = {"Misc", "Exploits", "Visuals", "Aim", "Network", "Config", "Scripts", "SDK", "Discord", "Debug"};
		for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
			if (ZeroGUI::ButtonTab(tabs[i], FVector2D{112.f, 30.f}, canvasTab == i))
				canvasTab = i;

		// Content column: only the active tab's section (so inactive sections don't draw over it).
		ZeroGUI::NextColumn(130.f);
		switch (canvasTab)
		{
		case 0:
			Sections::MiscTab();
			break;
		case 1:
			Sections::ExploitsTab();
			break;
		case 2:
			Sections::VisualsTab();
			break;
		case 3:
			Sections::AimTab();
			break;
		case 4:
			Sections::NetworkTab();
			break;
		case 5:
			Sections::ConfigTab();
			break;
		case 6:
			Sections::ScriptsTab();
			break;
		case 7:
			Sections::SdkTab();
			break;
		case 8:
			Sections::DiscordTab();
			break;
		case 9:
			Sections::DebugTab();
			break;
		}

		ZeroGUI::Render();			// drain deferred pop-ups (combo dropdowns, color swatches) on top
		ZeroGUI::Draw_Cursor(true); // the Canvas menu's own cursor
	};
}; // namespace Menu