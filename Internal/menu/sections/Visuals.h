#pragma once

/// @file
/// @brief Visuals tab: player ESP element toggles, radar, and per-element colors.

#include <vector>

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../../cache/FontCache.h"
#include "../ui/UI.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Renders the Visuals tab.
		/// Master ESP toggle plus per-element switches (name, box, 3D box, bones, snaplines, health, distance),
		/// a radar toggle, and color pickers for name/box/bones/snaplines. Dispatches SettingsChanged when any
		/// toggle changes (the color pickers write directly into Settings).
		void VisualsTab()
		{
			bool changed = false;
			auto& v = Settings.VISUALS;

			UI::SeparatorText("Renderer");
			const char* renderers[] = {"UE Canvas", "ImGui (faster)", "None", "External (streamproof)"};
			int renderer = static_cast<int>(Settings.MENU.Renderer);
			if (UI::Combo("Draw with", &renderer, renderers, UI::Count(renderers)))
			{
				Settings.MENU.Renderer = static_cast<RendererMode>(renderer);
				changed = true;
			}
			UI::Tooltip("ImGui draws the overlay without a ProcessEvent per line/text - much faster for a busy ESP.\n"
						"External draws the ESP, watermark, and everything the renderer produces into a separate\n"
						"window hidden from screen capture (OBS, Game Bar); the menu stays on the game window.");

			// Font picker — only the UE-canvas renderer draws text through a UFont (ImGui uses its own
			// atlas), so this applies to canvas-mode text (ESP + the canvas menu). Index 0 = Roboto default.
			if (Settings.MENU.Renderer == RendererMode::Canvas)
			{
				const auto& faces = FontCache::Get();
				std::vector<const char*> items{"Default (Roboto)"};
				int fontIdx = 0;
				for (int i = 0; i < static_cast<int>(faces.size()); i++)
				{
					items.push_back(faces[i].name.c_str());
					if (faces[i].name == Settings.VISUALS.CanvasFont) fontIdx = i + 1;
				}

				UI::SetNextItemWidth(220.f);
				if (UI::Combo("Canvas font", &fontIdx, items.data(), static_cast<int>(items.size())))
				{
					Settings.VISUALS.CanvasFont = fontIdx == 0 ? "" : faces[fontIdx - 1].name;
					changed = true;
				}
				UI::SameLine();
				if (UI::SmallButton("Refresh##fonts")) FontCache::Rebuild();
				UI::Tooltip("Text font for the UE-canvas renderer (ESP + the canvas menu). Default is the engine Roboto.\nRefresh rescans GObjects for newly-loaded fonts.");
			}

			const char* backends[] = {"ImGui", "UE Canvas"};
			int backend = static_cast<int>(Settings.MENU.Backend);
			if (UI::Combo("Menu backend", &backend, backends, UI::Count(backends)))
			{
				Settings.MENU.Backend = static_cast<MenuBackend>(backend);
				changed = true;
			}
			UI::Tooltip("Which GUI engine draws THIS menu (independent of the ESP renderer above).\n"
						"ImGui = the Present overlay; UE Canvas = drawn on the game canvas (works at the main menu too).");

			UI::SeparatorText("Player ESP");
			UI::ToggleSetting("Enable", &v.Esp);

			UI::SeparatorText("Elements");
			UI::ToggleSetting("Name", &v.Name);
			UI::ToggleSetting("Box", &v.Box);
			UI::ToggleSetting("3D Box", &v.Box3D);
			UI::ToggleSetting("Bones", &v.Bones);
			UI::ToggleSetting("Snaplines", &v.Snaplines);
			UI::ToggleSetting("Health", &v.Health);
			UI::ToggleSetting("Distance", &v.Distance);
			UI::ToggleSetting("K/D", &v.KD);
			UI::Tooltip("Draw each player's kills/deaths, and [killstreak] for the current life.");
			UI::ToggleSetting("Rank", &v.Rank);
			UI::Tooltip("Draw each player's rank/level (from their player state).");

			UI::SeparatorText("Range");
			changed |= UI::SliderFloat("Max distance (m)", &v.MaxDistance, 0.f, 300.f, v.MaxDistance <= 0.f ? "unlimited" : "%.0f");
			UI::Tooltip("Only draw enemies within this many metres. 0 = unlimited.");

			UI::SeparatorText("Visibility");
			UI::ToggleSetting("Visibility check", &v.EspVisibleCheck);
			UI::Tooltip("Recolor visible (recently-rendered) enemies in the Visible color below;\noccluded enemies keep the normal box/bone/snapline colors.");
			UI::ToggleSetting("Hide bots", &v.HideBots);
			UI::Tooltip("Don't draw AI bots in the ESP at all.");
			UI::ToggleSetting("Bot tag", &v.BotTag);
			UI::Tooltip("Prefix an AI bot's name with a colored \"[BOT]\" tag.");
			if (v.BotTag)
				UI::ColorEdit("Bot tag color", &v.BotTagColor);

			UI::SeparatorText("Teams");
			UI::ToggleSetting("Show teammates", &v.ShowFriendly);
			UI::Tooltip("Also draw teammates (ESP + radar), in the friendly color below.");

			UI::SeparatorText("Radar");
			UI::ToggleSetting("Enable Radar", &v.Radar);
			UI::ToggleSetting("Radar teammates", &v.RadarShowFriendly);

			UI::SeparatorText("Debug");
			UI::ToggleSetting("Draw all object names", &v.DrawAllNames);
			UI::Tooltip("Draws the UObject name of every actor in the world (not just players).");

			UI::SeparatorText("Text");
			changed |= UI::SliderFloat("Font size", &v.FontScale, 0.5f, 3.f, "%.2f");

			UI::SeparatorText("Crosshair");
			UI::ToggleSetting("Crosshair", &v.Crosshair);
			if (v.Crosshair)
			{
				changed |= UI::SliderFloat("Size", &v.CrosshairSize, 1.f, 30.f, "%.0f");
				changed |= UI::SliderFloat("Gap", &v.CrosshairGap, 0.f, 20.f, "%.0f");
				changed |= UI::SliderFloat("Thickness", &v.CrosshairThickness, 1.f, 6.f, "%.0f");
				UI::ColorEdit("Crosshair color", &v.CrosshairColor);
				UI::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}

			UI::SeparatorText("Bullet traces");
			UI::ToggleSetting("Bullet traces", &v.BulletTraces);
			UI::Tooltip("Draw a fading trail behind each projectile (PortalWars.Projectile and subclasses).");
			if (v.BulletTraces)
			{
				changed |= UI::SliderFloat("Trail duration", &v.BulletTraceDuration, 0.5f, 6.f, "%.1fs");
				UI::ColorEdit("Trail color", &v.BulletTraceColor);
				UI::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}

			UI::SeparatorText("Glow / chams");
			UI::ToggleSetting("Glow enemies", &v.GlowEnemy);
			UI::Tooltip("Force a custom-depth outline on enemies, visible through walls.\nRides on the game's team-outline post-process (verify color mapping in-game).");
			if (v.GlowEnemy)
			{
				UI::ColorEdit("Enemy glow", &v.GlowEnemyColor);
				UI::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}
			UI::ToggleSetting("Glow teammates", &v.GlowFriendly);
			if (v.GlowFriendly)
			{
				UI::ColorEdit("Teammate glow", &v.GlowFriendlyColor);
				UI::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}
			UI::ToggleSetting("Glow self", &v.GlowSelf);
			UI::Tooltip("Outline your own pawn - only visible in third person.");
			if (v.GlowSelf)
			{
				UI::ColorEdit("Self glow", &v.GlowSelfColor);
				UI::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}

			UI::SeparatorText("Colors");
			UI::ColorEdit("Name", &v.NameColor);
			UI::ColorEdit("Box", &v.BoxColor);
			UI::ColorEdit("Bones", &v.BonesColor);
			UI::ColorEdit("Snaplines", &v.SnaplineColor);
			UI::ColorEdit("Friendly", &v.FriendColor);
			UI::ColorEdit("Visible", &v.VisibleColor);
			UI::Tooltip("Color for visible enemies when the visibility check is on.");

			if (changed) Events::Dispatch(Events::Type::SettingsChanged);
		}
	} // namespace Sections
} // namespace Menu
