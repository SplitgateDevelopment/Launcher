#pragma once

/// @file
/// @brief The ImGui menu backend: draws in the Present hook. Owns everything ImGui — the window/tab
/// chrome, every widget (native ImGui / Custom.h), the accent tinting, the demo/style-editor windows,
/// the software cursor, and the gamepad open-toggle. Nothing here leaks into the shared menu layer.

#include <array>
#include <cstdio>

#include <imgui.h>

#include "Backend.h"
#include "../gui/Custom.h" // ImGui::ToggleButton / HotKey / Tooltip
#include "../../settings/Settings.h"
#include "../../utils/Rgb.h"

namespace Menu
{
	class ImGuiBackend : public Backend
	{
	  public:
		Phase phase() const override { return Phase::Present; }

		bool ExtraToggle() override { return ImGui::IsKeyPressed(ImGuiKey_GamepadStart); }

		void BeginFrame() override
		{
			// ImGui paints its own software cursor while the menu is open (the Canvas backend draws its
			// own, so this is scoped to the ImGui backend).
			ImGui::GetIO().MouseDrawCursor = Settings.MENU.ShowMenu;

			if (Settings.DEBUG.ShowDemoWindow) ImGui::ShowDemoWindow(&Settings.DEBUG.ShowDemoWindow);
			if (Settings.DEBUG.ShowStyleEditor) ImGui::ShowStyleEditor();

			ApplyAccent();
		}

		void EndFrame() override {}

		bool BeginWindow(const char* title) override
		{
			const ImGuiViewport* vp = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x + 550, vp->WorkPos.y + 20), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(550, 350), ImGuiCond_FirstUseEver);

			if (!ImGui::Begin(title, &Settings.MENU.ShowMenu, ImGuiWindowFlags_NoCollapse))
			{
				ImGui::End();
				return false;
			}
			if (!ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
			{
				ImGui::End();
				return false;
			}
			return true;
		}
		void EndWindow() override
		{
			ImGui::EndTabBar();
			ImGui::End();
		}
		bool BeginTab(const char* label) override { return ImGui::BeginTabItem(label); }
		void EndTab() override { ImGui::EndTabItem(); }

		// --- Core widgets. ---
		bool Toggle(const char* label, bool* v) override { return ImGui::ToggleButton(label, v); }
		bool Button(const char* label) override { return ImGui::Button(label); }
		bool SmallButton(const char* label) override { return ImGui::SmallButton(label); }
		bool Checkbox(const char* label, bool* v) override { return ImGui::Checkbox(label, v); }
		bool RadioBool(const char* label, bool active) override { return ImGui::RadioButton(label, active); }
		bool RadioInt(const char* label, int* v, int value) override { return ImGui::RadioButton(label, v, value); }
		bool SliderFloat(const char* label, float* v, float min, float max, const char* fmt) override { return ImGui::SliderFloat(label, v, min, max, fmt); }
		bool SliderInt(const char* label, int* v, int min, int max) override { return ImGui::SliderInt(label, v, min, max); }
		bool Combo(const char* label, int* v, const char* const* items, int count) override { return ImGui::Combo(label, v, items, count); }
		bool ColorEdit(const char* label, ::Color* c) override { return ImGui::ColorEdit4(label, &c->R); }
		bool HotKey(const char* label, int* key) override
		{
			const int old = *key;
			ImGui::HotKey(label, key);
			return *key != old;
		}
		void SameLine() override { ImGui::SameLine(); }
		void SeparatorText(const char* label) override { ImGui::SeparatorText(label); }
		void Tooltip(const char* text) override { ImGui::Tooltip(text); }
		void TextV(const char* fmt, va_list args) override
		{
			char buf[512];
			vsnprintf(buf, sizeof(buf), fmt, args);
			ImGui::TextUnformatted(buf);
		}
		void TextDisabledV(const char* fmt, va_list args) override
		{
			char buf[512];
			vsnprintf(buf, sizeof(buf), fmt, args);
			ImGui::TextDisabled("%s", buf);
		}
		void BulletTextV(const char* fmt, va_list args) override
		{
			char buf[512];
			vsnprintf(buf, sizeof(buf), fmt, args);
			ImGui::BulletText("%s", buf);
		}

		// --- Layout / scope. ---
		void SetNextItemWidth(float w) override { ImGui::SetNextItemWidth(w); }
		void BeginDisabled(bool disabled) override { ImGui::BeginDisabled(disabled); }
		void EndDisabled() override { ImGui::EndDisabled(); }
		void PushID(const char* id) override { ImGui::PushID(id); }
		void PopID() override { ImGui::PopID(); }

		// --- Rich widgets. ---
		bool InputText(const char* label, char* buf, size_t size) override { return ImGui::InputText(label, buf, size); }
		bool InputTextHint(const char* label, const char* hint, char* buf, size_t size) override { return ImGui::InputTextWithHint(label, hint, buf, size); }
		bool InputTextMultiline(const char* label, char* buf, size_t size, float height) override { return ImGui::InputTextMultiline(label, buf, size, ImVec2(0, height)); }
		bool BeginChild(const char* id, float w, float h) override { return ImGui::BeginChild(id, ImVec2(w, h), true, ImGuiWindowFlags_HorizontalScrollbar); }
		void EndChild() override { ImGui::EndChild(); }
		bool BeginCombo(const char* label, const char* preview) override { return ImGui::BeginCombo(label, preview); }
		void EndCombo() override { ImGui::EndCombo(); }
		bool Selectable(const char* label, bool selected) override { return ImGui::Selectable(label, selected); }
		void ClippedList(int count, const std::function<void(int)>& drawRow) override
		{
			ImGuiListClipper clipper;
			clipper.Begin(count);
			while (clipper.Step())
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
					drawRow(i);
		}
		bool CollapsingHeader(const char* label) override { return ImGui::CollapsingHeader(label); }
		bool TreeNode(const char* label) override { return ImGui::TreeNode(label); }
		void TreePop() override { ImGui::TreePop(); }

		void SetClipboardText(const char* text) override { ImGui::SetClipboardText(text); }
		const char* GetClipboardText() override { return ImGui::GetClipboardText(); }

	  private:
		/// Tint the interactive accent slots each frame: the cycling RGB color when enabled, else the
		/// theme's default reds. Defaults are snapshotted once (before the first override) so toggling RGB
		/// off restores the original shades instead of freezing on the last rainbow frame.
		static void ApplyAccent()
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
	};
} // namespace Menu
