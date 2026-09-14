#pragma once

/// @file
/// @brief Config tab: settings persistence (save/reload/reset, autosave, delete-on-crash), the menu
/// hotkey/watermark, named profiles, and share codes.

#include <cstring>

#include "../../settings/Profiles.h"
#include "../../../shared/Utilities.h"
#include "../ui/UI.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Config tab.
		void ConfigTab()
		{
			UI::SeparatorText("Config");
			if (UI::Button("Save")) SettingsHelper::File().Save();
			UI::SameLine();
			if (UI::Button("Reload"))
			{
				SettingsHelper::File().Load();
				Events::Dispatch(Events::Type::SettingsChanged);
			}
			UI::SameLine();
			if (UI::Button("Reset defaults"))
			{
				SettingsHelper::File().Reset();
				Events::Dispatch(Events::Type::SettingsChanged);
			}
			UI::SameLine();
			if (UI::Button("Open folder"))
				Shared::Utilities::OpenFolder(SettingsHelper::File().Path().parent_path());
			UI::Tooltip("Open the folder holding the settings file.");

			if (UI::Toggle("Autosave on change", &Settings.MISC.AutoSave))
				Events::Dispatch(Events::Type::SettingsChanged);
			if (UI::Toggle("Delete config on crash", &Settings.DEBUG.DeleteSettingsOnCrash))
				Events::Dispatch(Events::Type::SettingsChanged);

			UI::SeparatorText("Menu");
			UI::HotKey("Open Menu", &Settings.MENU.ShowHotkey);
			UI::Toggle("Watermark", &Settings.MENU.ShowWatermark);
			UI::Toggle("RGB", &Settings.MENU.Rgb);
			UI::Tooltip("Cycle the watermark, menu accent, and radar self-icon through a rainbow. Off = the defaults (red / white).");

			// Restore every hotkey (menu, no-clip, aim, trigger) to its struct default, leaving the
			// rest of the config untouched.
			if (UI::Button("Reset keybinds"))
			{
				Settings.MENU.ShowHotkey = MenuSettings{}.ShowHotkey;
				Settings.EXPLOITS.NoClip = ExploitsSettings{}.NoClip;
				Settings.AIM.AimKey = AimSettings{}.AimKey;
				Settings.AIM.TriggerKey = AimSettings{}.TriggerKey;
				Events::Dispatch(Events::Type::SettingsChanged);
			}
			UI::Tooltip("Reset all hotkeys (menu, no-clip, aim, trigger) to their defaults.");

			UI::SeparatorText("Save current config");
			static char nameBuffer[64] = "";
			UI::SetNextItemWidth(200.f);
			UI::InputText("##name", nameBuffer, sizeof(nameBuffer));
			UI::SameLine();
			if (UI::Button("Save as") && nameBuffer[0])
			{
				Profiles::Save(nameBuffer);
				nameBuffer[0] = '\0';
			}

			UI::SeparatorText("Profiles");
			std::string toLoad, toDelete;
			for (const auto& name : Profiles::List())
			{
				UI::PushID(name.c_str());
				if (UI::SmallButton("Load")) toLoad = name;
				UI::SameLine();
				if (UI::SmallButton("Delete")) toDelete = name;
				UI::SameLine();
				UI::Text("%s", name.c_str());
				UI::PopID();
			}
			if (!toLoad.empty()) Profiles::Load(toLoad);
			if (!toDelete.empty()) Profiles::Delete(toDelete);

			UI::SeparatorText("Share code");
			UI::Tooltip("A portable code for the current config. Copy to share; paste + import to apply.");
			if (UI::Button("Copy current config")) UI::SetClipboardText(Profiles::Export().c_str());

			static char codeBuffer[8192] = "";
			UI::InputTextMultiline("##code", codeBuffer, sizeof(codeBuffer), 80);

			if (UI::Button("Paste"))
			{
				const char* clip = UI::GetClipboardText();
				if (clip) strncpy_s(codeBuffer, clip, _TRUNCATE);
			}
			UI::SameLine();
			if (UI::Button("Import") && codeBuffer[0])
			{
				if (Profiles::Import(codeBuffer)) codeBuffer[0] = '\0';
			}
		}
	} // namespace Sections
} // namespace Menu
