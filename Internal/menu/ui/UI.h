#pragma once

/// @file
/// @brief Menu::UI — the backend-neutral widget facade the tab sections call. Each function
/// dispatches on Settings.MENU.Backend: the ImGui backend forwards to the native ImGui / Custom.h
/// widgets (drawn in the Present frame), the Canvas backend forwards to the ZeroGUI widgets (drawn
/// through Render::canvas in the PostRender frame). Only one backend is active per frame and each
/// hook drives only its own backend's sections, so every call always lands in a valid context.
///
/// Value widgets return `bool changed` (true on the change frame) so the sections keep the
/// `changed |= UI::Toggle(...); if (changed) Dispatch(SettingsChanged);` pattern across both backends.

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>

#include <imgui.h>

#include "../../settings/Settings.h"
#include "../../scripting/Events.h"
#include "../gui/Custom.h" // ImGui::ToggleButton / HotKey / Tooltip / ...
#include "../canvas/ZeroGUI.h"

namespace Menu
{
	namespace UI
	{
		/// True when the ImGui menu backend is active (the only place sections may call ImGui:: directly,
		/// inside an `if (UI::IsImGui())` guard — that branch only runs in the Present/ImGui path).
		inline bool IsImGui()
		{
			return Settings.MENU.Backend == MenuBackend::ImGui;
		}
		inline bool IsCanvas()
		{
			return Settings.MENU.Backend == MenuBackend::Canvas;
		}

		/// The visible part of an ImGui label: everything before a "##id" disambiguation suffix (ImGui
		/// hides it; the Canvas backend would otherwise draw it literally). Kept alive by the caller for
		/// the duration of the widget call.
		inline std::string Vis(const char* label)
		{
			const char* p = std::strstr(label, "##");
			return p ? std::string(label, static_cast<size_t>(p - label)) : std::string(label);
		}

		// --- Default Canvas widget sizes (px). ImGui sizes itself from its layout. ---
		inline constexpr float ButtonW = 150.f, ButtonH = 25.f;
		inline constexpr float SmallButtonW = 90.f, SmallButtonH = 20.f;
		inline constexpr float ComboW = 150.f, ComboH = 25.f;
		inline constexpr float HotKeyW = 90.f, HotKeyH = 22.f;

		/// Animated on/off toggle. @return true on the frame it flipped.
		inline bool Toggle(const char* label, bool* v)
		{
			if (IsImGui()) return ImGui::ToggleButton(label, v);
			return ZeroGUI::Checkbox(Vis(label).c_str(), v);
		}

		/// Toggle that dispatches SettingsChanged (payload name=label, value=0/1) when flipped.
		inline bool ToggleSetting(const char* label, bool* v)
		{
			if (!Toggle(label, v)) return false;
			Events::Dispatch(Events::Type::SettingsChanged, Events::Payload{.value = *v ? 1.f : 0.f, .name = label});
			return true;
		}

		inline bool Checkbox(const char* label, bool* v)
		{
			if (IsImGui()) return ImGui::Checkbox(label, v);
			return ZeroGUI::Checkbox(Vis(label).c_str(), v);
		}

		inline bool Button(const char* label)
		{
			if (IsImGui()) return ImGui::Button(label);
			return ZeroGUI::Button(Vis(label).c_str(), FVector2D{ButtonW, ButtonH});
		}

		inline bool SmallButton(const char* label)
		{
			if (IsImGui()) return ImGui::SmallButton(label);
			return ZeroGUI::Button(Vis(label).c_str(), FVector2D{SmallButtonW, SmallButtonH});
		}

		/// Clickable radio row. @return true on the frame it was clicked.
		inline bool RadioButton(const char* label, bool active)
		{
			if (IsImGui()) return ImGui::RadioButton(label, active);
			return ZeroGUI::Button(Vis(label).c_str(), FVector2D{ButtonW, ButtonH});
		}

		inline bool SliderFloat(const char* label, float* v, float min, float max, const char* fmt = "%.0f")
		{
			if (IsImGui()) return ImGui::SliderFloat(label, v, min, max, fmt);
			const float old = *v;
			ZeroGUI::SliderFloat(Vis(label).c_str(), v, min, max, fmt);
			return *v != old;
		}

		inline bool SliderInt(const char* label, int* v, int min, int max)
		{
			if (IsImGui()) return ImGui::SliderInt(label, v, min, max);
			const int old = *v;
			ZeroGUI::SliderInt(Vis(label).c_str(), v, min, max);
			return *v != old;
		}

		inline bool Combo(const char* label, int* v, const char* const* items, int count)
		{
			if (IsImGui()) return ImGui::Combo(label, v, items, count);
			return ZeroGUI::Combobox(Vis(label).c_str(), FVector2D{ComboW, ComboH}, v, items, count);
		}

		/// RGBA color editor over a settings ::Color (layout-compatible with FLinearColor).
		/// @return true if the color changed this frame.
		inline bool ColorEdit(const char* label, ::Color* c)
		{
			if (IsImGui()) return ImGui::ColorEdit4(label, &c->R);
			return ZeroGUI::ColorPicker(Vis(label).c_str(), reinterpret_cast<FLinearColor*>(c));
		}

		/// Rebindable hotkey row. @return true on the frame the key changed.
		inline bool HotKey(const char* label, int* key)
		{
			if (IsImGui())
			{
				const int old = *key;
				ImGui::HotKey(label, key);
				return *key != old;
			}
			return ZeroGUI::Hotkey(Vis(label).c_str(), FVector2D{HotKeyW, HotKeyH}, key);
		}

		inline void SameLine()
		{
			if (IsImGui())
				ImGui::SameLine();
			else
				ZeroGUI::SameLine();
		}

		/// Section header with a label.
		inline void SeparatorText(const char* label)
		{
			if (IsImGui())
				ImGui::SeparatorText(label);
			else
				ZeroGUI::Text(Vis(label).c_str());
		}

		/// Hover tooltip on the previous widget (ImGui only; a no-op on the Canvas backend).
		inline void Tooltip(const char* text)
		{
			if (IsImGui()) ImGui::Tooltip(text);
		}

		// --- Text (printf-style). Formats once, then routes to the active backend. ---
		inline void TextV(const char* fmt, va_list args)
		{
			char buf[512];
			vsnprintf(buf, sizeof(buf), fmt, args);
			if (IsImGui())
				ImGui::TextUnformatted(buf);
			else
				ZeroGUI::Text(buf);
		}
		inline void Text(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			TextV(fmt, args);
			va_end(args);
		}
		inline void TextDisabled(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			if (IsImGui())
			{
				char buf[512];
				vsnprintf(buf, sizeof(buf), fmt, args);
				ImGui::TextDisabled("%s", buf);
			}
			else
			{
				TextV(fmt, args);
			}
			va_end(args);
		}
		inline void BulletText(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			if (IsImGui())
			{
				char buf[512];
				vsnprintf(buf, sizeof(buf), fmt, args);
				ImGui::BulletText("%s", buf);
			}
			else
			{
				TextV(fmt, args);
			}
			va_end(args);
		}
	} // namespace UI
} // namespace Menu
