#pragma once

#include <string>
#include <Windows.h>

#include "imgui.h"
#include <imgui_internal.h>

namespace ImGui
{
	void Tooltip(const char* text)
	{
		if (strlen(text) && ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(text);
			ImGui::EndTooltip();
		}
	}

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
		case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
		case VK_RCONTROL: case VK_RMENU:
		case VK_LWIN: case VK_RWIN: case VK_APPS:
		case VK_PRIOR: case VK_NEXT:
		case VK_END: case VK_HOME:
		case VK_INSERT: case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
			scanCode |= KF_EXTENDED;
		default:
			result = GetKeyNameTextA(scanCode << 16, szName, 128);
		}

		return szName;
	}

	void HotKeyEx(int* key, const ImVec2& size_arg = ImVec2(0, 0))
	{
		static const std::vector<int> ignoredKeys =
		{
			VK_LBUTTON, VK_RBUTTON, VK_MBUTTON
		};

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
			for (auto ignoredKey : ignoredKeys) {
				if (GetAsyncKeyState(ignoredKey) & 0x8000) {
					ignore = true;
					break;
				}
			};

			if (!ignore)
			{
				for (int code = 0; code < 255; code++)
				{
					if (GetAsyncKeyState(code) & 0x8000) {
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

	void HotKey(const char* label, int* key, int width = 50, int pad = 2)
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

		//float height = ImGui::GetFrameHeight( );
		float height = ImGui::GetFontSize();
		const ImVec2 pos = window->DC.CursorPos;

		float width = height * 2.f;
		float radius = height * 0.50f;

		const ImRect total_bb(pos, ImVec2(pos.x + width + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), pos.y + label_size.y /*+ style.FramePadding.y * 2.0f*/));

		ItemSize(total_bb/*, style.FramePadding.y*/);
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

		ImU32 col_bg = GetColorU32((held && hovered) ? *v ? ImGuiCol_SliderGrabActive : ImGuiCol_FrameBgActive : hovered ? *v ? ImGuiCol_SliderGrabActive : ImGuiCol_FrameBgHovered : *v ? ImGuiCol_SliderGrab : ImGuiCol_FrameBg);

		const ImRect frame_bb(pos, ImVec2(pos.x + width, pos.y + height));

		RenderFrame(frame_bb.Min, frame_bb.Max, col_bg, false, height * 0.5f);
		RenderNavHighlight(total_bb, id);

		ImVec2 label_pos = ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y /*+ style.FramePadding.y*/);
		RenderText(label_pos, label);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + radius + t * (width - radius * 2.0f), pos.y + radius), radius /*- style.FramePadding.x*/, ImGui::GetColorU32(*v ? ImGuiCol_CheckMark : ImGuiCol_TextDisabled), 36);

		return pressed;
	}

	bool ToggleButton(const char* label, bool* v)
	{
		ImGuiStyle* style = &ImGui::GetStyle();

		ImGui::TextUnformatted(label);
		ImGui::SameLine(ImGui::GetWindowSize().x - style->WindowPadding.x - ImGui::GetFontSize() * 2);
		std::string str1 = "##";
		std::string str2 = label;
		return ToggleButtonEx((str1 + str2).c_str(), v);
	}
}