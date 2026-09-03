#pragma once

/// @file
/// @brief Watermark overlay: draws the product name and live FPS in the top-left corner.

#include "../../settings/Settings.h"

namespace Menu
{
	namespace Sections
	{
		/// @brief Draws the "Splitgate Internal" watermark and current FPS onto the foreground draw list.
		/// No-op when Settings.MENU.ShowWatermark is disabled; rendered every frame independently of menu visibility.
		void Watermark()
		{
			if (!Settings.MENU.ShowWatermark) return;

			ImGuiStyle* style = &ImGui::GetStyle();
			ImU32 color = ImGui::ColorConvertFloat4ToU32(style->Colors[ImGuiCol_TitleBgActive]);

			ImGuiIO& io = ImGui::GetIO();
			(void)io;
			std::string fps = std::format("FPS: {:.1f}", io.Framerate);

			ImGui::GetForegroundDrawList()->AddText(ImVec2(0.f, 0.f), color, "Splitgate Internal");
			ImGui::GetForegroundDrawList()->AddText(ImVec2(0.f, 15.f), color, fps.c_str());
			return;
		};
	} // namespace Sections
} // namespace Menu