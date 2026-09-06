#pragma once

/// @file
/// @brief Visuals tab: player ESP element toggles, radar, and per-element colors.

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
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
			changed |= UI::Toggle("Enable", &v.Esp);

			UI::SeparatorText("Elements");
			changed |= UI::Toggle("Name", &v.Name);
			changed |= UI::Toggle("Box", &v.Box);
			changed |= UI::Toggle("3D Box", &v.Box3D);
			changed |= UI::Toggle("Bones", &v.Bones);
			changed |= UI::Toggle("Snaplines", &v.Snaplines);
			changed |= UI::Toggle("Health", &v.Health);
			changed |= UI::Toggle("Distance", &v.Distance);
			changed |= UI::Toggle("K/D", &v.KD);
			UI::Tooltip("Draw each player's kills/deaths, and [killstreak] for the current life.");
			changed |= UI::Toggle("Rank", &v.Rank);
			UI::Tooltip("Draw each player's rank/level (from their player state).");

			UI::SeparatorText("Range");
			changed |= UI::SliderFloat("Max distance (m)", &v.MaxDistance, 0.f, 300.f, v.MaxDistance <= 0.f ? "unlimited" : "%.0f");
			UI::Tooltip("Only draw enemies within this many metres. 0 = unlimited.");

			UI::SeparatorText("Visibility");
			changed |= UI::Toggle("Visibility check", &v.EspVisibleCheck);
			UI::Tooltip("Recolor visible (recently-rendered) enemies in the Visible color below;\noccluded enemies keep the normal box/bone/snapline colors.");
			changed |= UI::Toggle("Hide bots", &v.HideBots);
			UI::Tooltip("Don't draw AI bots in the ESP at all.");
			changed |= UI::Toggle("Bot tag", &v.BotTag);
			UI::Tooltip("Prefix an AI bot's name with a colored \"[BOT]\" tag.");
			if (v.BotTag)
				UI::ColorEdit("Bot tag color", &v.BotTagColor);

			UI::SeparatorText("Teams");
			changed |= UI::Toggle("Show teammates", &v.ShowFriendly);
			UI::Tooltip("Also draw teammates (ESP + radar), in the friendly color below.");

			UI::SeparatorText("Radar");
			changed |= UI::Toggle("Enable Radar", &v.Radar);
			changed |= UI::Toggle("Radar teammates", &v.RadarShowFriendly);

			UI::SeparatorText("Debug");
			changed |= UI::Toggle("Draw all object names", &v.DrawAllNames);
			UI::Tooltip("Draws the UObject name of every actor in the world (not just players).");

			UI::SeparatorText("Text");
			changed |= UI::SliderFloat("Font size", &v.FontScale, 0.5f, 3.f, "%.2f");

			UI::SeparatorText("Crosshair");
			changed |= UI::Toggle("Crosshair", &v.Crosshair);
			if (v.Crosshair)
			{
				changed |= UI::SliderFloat("Size", &v.CrosshairSize, 1.f, 30.f, "%.0f");
				changed |= UI::SliderFloat("Gap", &v.CrosshairGap, 0.f, 20.f, "%.0f");
				changed |= UI::SliderFloat("Thickness", &v.CrosshairThickness, 1.f, 6.f, "%.0f");
				UI::ColorEdit("Crosshair color", &v.CrosshairColor);
				UI::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}

			UI::SeparatorText("Bullet traces");
			changed |= UI::Toggle("Bullet traces", &v.BulletTraces);
			UI::Tooltip("Draw a fading trail behind each projectile (PortalWars.Projectile and subclasses).");
			if (v.BulletTraces)
			{
				changed |= UI::SliderFloat("Trail duration", &v.BulletTraceDuration, 0.5f, 6.f, "%.1fs");
				UI::ColorEdit("Trail color", &v.BulletTraceColor);
				UI::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}

			UI::SeparatorText("Glow / chams");
			changed |= UI::Toggle("Glow enemies", &v.GlowEnemy);
			UI::Tooltip("Force a custom-depth outline on enemies, visible through walls.\nRides on the game's team-outline post-process (verify color mapping in-game).");
			if (v.GlowEnemy)
			{
				UI::ColorEdit("Enemy glow", &v.GlowEnemyColor);
				UI::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}
			changed |= UI::Toggle("Glow teammates", &v.GlowFriendly);
			if (v.GlowFriendly)
			{
				UI::ColorEdit("Teammate glow", &v.GlowFriendlyColor);
				UI::Tooltip("Overridden by the RGB rainbow when RGB is on.");
			}
			changed |= UI::Toggle("Glow self", &v.GlowSelf);
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
