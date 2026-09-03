#pragma once

/// @file
/// @brief Custom ImGui widgets and helpers (tooltips, key-name conversion, hotkey capture, toggle switches)
/// added into the ImGui namespace and reused across the menu sections.

#include <string>
#include <Windows.h>

#include "imgui.h"
#include <imgui_internal.h>

namespace ImGui
{
	/// @brief Shows a tooltip with @p text when the previous item is hovered; no-op for empty text.
	void Tooltip(const char* text)
	{
		if (strlen(text) && ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(text);
			ImGui::EndTooltip();
		}
	}

	/**
	 * @brief Converts a Win32 virtual-key code into a human-readable key name.
	 * @param virtualKey The VK_* code to translate.
	 * @return Names mouse buttons explicitly (MOUSE0/MOUSE1/MBUTTON/XBUTTON1/XBUTTON2) and otherwise
	 *         resolves the key name via the keyboard layout, flagging extended keys where required.
	 */
	std::string VirtualKeyCodeToString(UCHAR virtualKey)
	{
		UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

		if (virtualKey == VK_LBUTTON)
		{
			return "MOUSE0";
		}
		if (virtualKey == VK_RBUTTON)
		{
			return "MOUSE1";
		}
		if (virtualKey == VK_MBUTTON)
		{
			return "MBUTTON";
		}
		if (virtualKey == VK_XBUTTON1)
		{
			return "XBUTTON1";
		}
		if (virtualKey == VK_XBUTTON2)
		{
			return "XBUTTON2";
		}

		CHAR szName[128];
		int result = 0;
		switch (virtualKey)
		{
		case VK_LEFT:
		case VK_UP:
		case VK_RIGHT:
		case VK_DOWN:
		case VK_RCONTROL:
		case VK_RMENU:
		case VK_LWIN:
		case VK_RWIN:
		case VK_APPS:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_END:
		case VK_HOME:
		case VK_INSERT:
		case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
			scanCode |= KF_EXTENDED;
		default:
			result = GetKeyNameTextA(scanCode << 16, szName, 128);
		}

		return szName;
	}

	/**
	 * @brief Button that rebinds a hotkey: click to enter capture mode, then the next pressed key is stored.
	 * @param key In/out virtual-key code; updated to the newly captured key.
	 * @param size_arg Optional button size.
	 * @note Mouse buttons are ignored during capture so a click cannot bind itself.
	 */
	void HotKeyEx(int* key, const ImVec2& size_arg = ImVec2(0, 0))
	{
		static const std::vector<int> ignoredKeys =
			{
				VK_LBUTTON, VK_RBUTTON, VK_MBUTTON};

		static std::string keyName = VirtualKeyCodeToString(*key);
		static bool isPressed = false;

		if (ImGui::Button(keyName.c_str(), size_arg))
		{
			keyName = "...";
			isPressed = true;
		}
		if (isPressed)
		{
			bool ignore = false;
			for (auto ignoredKey : ignoredKeys)
			{
				if (GetAsyncKeyState(ignoredKey) & 0x8000)
				{
					ignore = true;
					break;
				}
			};

			if (!ignore)
			{
				for (int code = 0; code < 255; code++)
				{
					if (GetAsyncKeyState(code) & 0x8000)
					{
						*key = code;
						isPressed = false;
					}
				}
			}
		}
		else
		{
			keyName = VirtualKeyCodeToString(*key);
		}
	}

	/// @brief Labeled hotkey row: draws @p label on the left and a right-aligned HotKeyEx capture button.
	void HotKey(const char* label, int* key, float width = 50.0f, float pad = 2.0f)
	{
		ImGuiStyle* style = &ImGui::GetStyle();

		ImGui::BeginGroup();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(pad, pad));
		ImGui::Spacing();
		ImGui::PopStyleVar();

		ImGui::TextUnformatted(label);
		ImGui::EndGroup();

		ImGui::SameLine(ImGui::GetWindowSize().x - width - style->WindowPadding.x);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(pad, pad));
		HotKeyEx(key, ImVec2(width, ImGui::GetFontSize() + pad * 2));
		ImGui::PopStyleVar();
	}

	/**
	 * @brief Animated on/off toggle switch drawn as a sliding pill with a trailing label.
	 * @param label Widget label (also used for the ImGui id).
	 * @param v In/out boolean state; flipped when the switch is clicked.
	 * @return True on the frame the switch was toggled.
	 */
	bool ToggleButtonEx(const char* label, bool* v)
	{
		using namespace ImGui;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		// float height = ImGui::GetFrameHeight( );
		float height = ImGui::GetFontSize();
		const ImVec2 pos = window->DC.CursorPos;

		float width = height * 2.f;
		float radius = height * 0.50f;

		const ImRect total_bb(pos, ImVec2(pos.x + width + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), pos.y + label_size.y /*+ style.FramePadding.y * 2.0f*/));

		ItemSize(total_bb /*, style.FramePadding.y*/);
		if (!ItemAdd(total_bb, id))
			return false;

		float last_active_id_timer = g.LastActiveIdTimer;

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
		{
			*v = !(*v);
			MarkItemEdited(id);
			g.LastActiveIdTimer = 0.f;
		}

		if (g.LastActiveIdTimer == 0.f && g.LastActiveId == id && !pressed)
			g.LastActiveIdTimer = last_active_id_timer;

		float t = *v ? 1.0f : 0.0f;

		if (g.LastActiveId == id)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / 0.1f);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg = GetColorU32((held && hovered) ? *v ? ImGuiCol_SliderGrabActive : ImGuiCol_FrameBgActive : hovered ? *v ? ImGuiCol_SliderGrabActive : ImGuiCol_FrameBgHovered
																											 : *v		 ? ImGuiCol_SliderGrab
																														 : ImGuiCol_FrameBg);

		const ImRect frame_bb(pos, ImVec2(pos.x + width, pos.y + height));

		RenderFrame(frame_bb.Min, frame_bb.Max, col_bg, false, height * 0.5f);
		RenderNavHighlight(total_bb, id);

		ImVec2 label_pos = ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y /*+ style.FramePadding.y*/);
		RenderText(label_pos, label);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + radius + t * (width - radius * 2.0f), pos.y + radius), radius /*- style.FramePadding.x*/, ImGui::GetColorU32(*v ? ImGuiCol_CheckMark : ImGuiCol_TextDisabled), 36);

		return pressed;
	}

	/// @brief Row wrapper around ToggleButtonEx: left-aligned @p label with the switch pushed to the right edge.
	/// @return True on the frame the switch was toggled.
	bool ToggleButton(const char* label, bool* v)
	{
		ImGuiStyle* style = &ImGui::GetStyle();

		ImGui::TextUnformatted(label);
		ImGui::SameLine(ImGui::GetWindowSize().x - style->WindowPadding.x - ImGui::GetFontSize() * 2);
		std::string str1 = "##";
		std::string str2 = label;
		return ToggleButtonEx((str1 + str2).c_str(), v);
	}
} // namespace ImGui