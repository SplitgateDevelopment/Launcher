#pragma once

/// @file
/// @brief The menu's ImGui theme: spacing/rounding metrics and the full color palette.

#include "imgui.h"

#include "../../render/Colors.h"
#include "../../render/adapters/ImGui.h" // provides ColorTraits<ImVec4> for Palette::X.To<ImVec4>()

namespace GUI
{
	/// @brief Visual theme for the menu.
	namespace Styles
	{
		/// @brief Applies the menu's style metrics (padding, rounding, spacing) and the cream/red color palette
		/// to the active ImGui style. Called once during overlay initialization.
		void Init()
		{
			ImGuiStyle* style = &ImGui::GetStyle();

			style->WindowPadding = ImVec2(15, 15);
			style->WindowRounding = 5.0f;
			style->FramePadding = ImVec2(5, 5);
			style->FrameRounding = 4.0f;
			style->ItemSpacing = ImVec2(12, 8);
			style->ItemInnerSpacing = ImVec2(8, 6);
			style->IndentSpacing = 25.0f;
			style->ScrollbarSize = 15.0f;
			style->ScrollbarRounding = 9.0f;
			style->GrabMinSize = 5.0f;
			style->GrabRounding = 3.0f;

			ImVec4* colors = style->Colors;

			using namespace Render;

			colors[ImGuiCol_Text] = Palette::Text.To<ImVec4>();
			colors[ImGuiCol_TextDisabled] = Palette::Text.Alpha(0.77f).To<ImVec4>();
			colors[ImGuiCol_WindowBg] = Palette::Cream.Alpha(0.82f).To<ImVec4>();
			colors[ImGuiCol_ChildBg] = Palette::Frame.Alpha(0.58f).To<ImVec4>();
			colors[ImGuiCol_PopupBg] = Palette::Cream.Alpha(0.92f).To<ImVec4>();
			colors[ImGuiCol_Border] = Palette::Border.Alpha(0.65f).To<ImVec4>();
			colors[ImGuiCol_BorderShadow] = Palette::Cream.Alpha(0.00f).To<ImVec4>();
			colors[ImGuiCol_FrameBg] = Palette::Frame.To<ImVec4>();
			colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 0.40f, 0.40f, 0.78f); // one-off light red
			colors[ImGuiCol_FrameBgActive] = Palette::Primary.To<ImVec4>();
			colors[ImGuiCol_TitleBg] = Palette::Frame.To<ImVec4>();
			colors[ImGuiCol_TitleBgActive] = Palette::Primary.To<ImVec4>();
			colors[ImGuiCol_TitleBgCollapsed] = Palette::Frame.Alpha(0.75f).To<ImVec4>();
			colors[ImGuiCol_MenuBarBg] = Palette::Frame.Alpha(0.47f).To<ImVec4>();
			colors[ImGuiCol_ScrollbarBg] = Palette::Frame.To<ImVec4>();
			colors[ImGuiCol_ScrollbarGrab] = Palette::Black.Alpha(0.21f).To<ImVec4>();
			colors[ImGuiCol_ScrollbarGrabHovered] = Palette::Primary.Alpha(0.78f).To<ImVec4>();
			colors[ImGuiCol_ScrollbarGrabActive] = Palette::Primary.To<ImVec4>();
			colors[ImGuiCol_CheckMark] = Palette::Primary.Alpha(0.80f).To<ImVec4>();
			colors[ImGuiCol_SliderGrab] = Palette::Black.Alpha(0.14f).To<ImVec4>();
			colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.98f, 0.98f, 1.00f); // one-off near-white
			colors[ImGuiCol_Button] = Palette::Black.Alpha(0.12f).To<ImVec4>();
			colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.22f, 0.22f, 0.86f); // one-off bright red
			colors[ImGuiCol_ButtonActive] = Palette::Primary.To<ImVec4>();
			colors[ImGuiCol_Header] = Palette::Primary.Alpha(0.76f).To<ImVec4>();
			colors[ImGuiCol_HeaderHovered] = Palette::Primary.Alpha(0.86f).To<ImVec4>();
			colors[ImGuiCol_HeaderActive] = Palette::Primary.To<ImVec4>();
			colors[ImGuiCol_Separator] = Palette::Slate.Alpha(0.50f).To<ImVec4>();
			colors[ImGuiCol_SeparatorHovered] = Palette::Blue.Alpha(0.78f).To<ImVec4>();
			colors[ImGuiCol_SeparatorActive] = Palette::Blue.To<ImVec4>();
			colors[ImGuiCol_ResizeGrip] = Palette::Black.Alpha(0.04f).To<ImVec4>();
			colors[ImGuiCol_ResizeGripHovered] = Palette::Primary.Alpha(0.78f).To<ImVec4>();
			colors[ImGuiCol_ResizeGripActive] = Palette::Primary.To<ImVec4>();
			colors[ImGuiCol_Tab] = Palette::Black.Alpha(0.11f).To<ImVec4>();
			colors[ImGuiCol_TabHovered] = Palette::Primary.Alpha(0.78f).To<ImVec4>();
			colors[ImGuiCol_TabActive] = Palette::Primary.To<ImVec4>();
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);		  // one-off dark blue
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f); // one-off blue
			colors[ImGuiCol_PlotLines] = Palette::Text.Alpha(0.63f).To<ImVec4>();
			colors[ImGuiCol_PlotLinesHovered] = Palette::Primary.To<ImVec4>();
			colors[ImGuiCol_PlotHistogram] = Palette::Text.Alpha(0.63f).To<ImVec4>();
			colors[ImGuiCol_PlotHistogramHovered] = Palette::Primary.To<ImVec4>();
			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);	 // one-off table gray
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f); // one-off table gray
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);	 // one-off table gray
			colors[ImGuiCol_TableRowBg] = Palette::Black.Alpha(0.00f).To<ImVec4>();
			colors[ImGuiCol_TableRowBgAlt] = Palette::White.Alpha(0.06f).To<ImVec4>();
			colors[ImGuiCol_TextSelectedBg] = Palette::Primary.Alpha(0.43f).To<ImVec4>();
			colors[ImGuiCol_DragDropTarget] = Palette::Primary.Alpha(0.90f).To<ImVec4>();
			colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f); // one-off nav blue
			colors[ImGuiCol_NavWindowingHighlight] = Palette::White.Alpha(0.70f).To<ImVec4>();
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f); // one-off gray
			colors[ImGuiCol_ModalWindowDimBg] = Palette::Frame.Alpha(0.73f).To<ImVec4>();
		}
	} // namespace Styles
}; // namespace GUI