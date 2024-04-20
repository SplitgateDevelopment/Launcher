#pragma once

#include "../settings/Settings.h"
#include "sections/Misc.h"
#include "sections/Exploits.h"
#include "sections/Scripts.h"
#include "sections/Settings.h"
#include "sections/Debug.h"
#include "sections/Watermark.h"

#include <imgui.h>

namespace Menu {
    static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
	static int tab = 0;
    static ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;

	void Draw()
	{
        Menu::Sections::Watermark();
        
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        if ((GetAsyncKeyState(Settings.MENU.ShowHotkey) & 1) || ImGui::IsKeyPressed(ImGuiKey_GamepadStart)) Settings.MENU.ShowMenu = !Settings.MENU.ShowMenu;
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

        if (ImGui::BeginTabBar("MainTabBar", tabFlags))
        {
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
            if (ImGui::BeginTabItem("User Scripts"))
            {
                Menu::Sections::ScriptsTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Settings"))
            {
                Menu::Sections::SettingsTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Debug"))
            {
                Menu::Sections::DebugTab();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        };

		ImGui::End();
	};
};