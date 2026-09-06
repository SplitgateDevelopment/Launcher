#pragma once

/// @file
/// @brief Menu::UI — the backend-neutral widget facade the tab sections call. Every function is a
/// one-line forward to the active backend (Menu::active->*); this layer names no concrete backend and
/// includes neither ImGui nor UCanvasGUI. Switching or adding a backend never touches this file or the
/// sections. Value widgets return `bool changed` (true on the change frame).

#include <cstdarg>

#include "../backend/Backend.h"
#include "../backend/Backends.h"
#include "../../scripting/Events.h"

namespace Menu
{
	namespace UI
	{
		/// Element count of a C array (replaces ImGui's IM_ARRAYSIZE without depending on ImGui).
		template <class T, size_t N>
		constexpr int Count(const T (&)[N])
		{
			return static_cast<int>(N);
		}

		// --- Core widgets. ---
		inline bool Toggle(const char* label, bool* v) { return active->Toggle(label, v); }

		/// Toggle that dispatches SettingsChanged (payload name=label, value=0/1) when flipped.
		inline bool ToggleSetting(const char* label, bool* v)
		{
			if (!active->Toggle(label, v)) return false;
			Events::Dispatch(Events::Type::SettingsChanged, Events::Payload{.value = *v ? 1.f : 0.f, .name = label});
			return true;
		}

		inline bool Checkbox(const char* label, bool* v) { return active->Checkbox(label, v); }
		inline bool Button(const char* label) { return active->Button(label); }
		inline bool SmallButton(const char* label) { return active->SmallButton(label); }
		inline bool RadioButton(const char* label, int* v, int value) { return active->RadioInt(label, v, value); }
		inline bool SliderFloat(const char* label, float* v, float min, float max, const char* fmt = "%.0f") { return active->SliderFloat(label, v, min, max, fmt); }
		inline bool SliderInt(const char* label, int* v, int min, int max) { return active->SliderInt(label, v, min, max); }
		inline bool Combo(const char* label, int* v, const char* const* items, int count) { return active->Combo(label, v, items, count); }
		inline bool ColorEdit(const char* label, ::Color* c) { return active->ColorEdit(label, c); }
		inline bool HotKey(const char* label, int* key) { return active->HotKey(label, key); }
		inline void SameLine() { active->SameLine(); }
		inline void SeparatorText(const char* label) { active->SeparatorText(label); }
		inline void Tooltip(const char* text) { active->Tooltip(text); }

		// --- Text (printf-style; the va_list is packed here and lowered to the backend's TextV). ---
		inline void Text(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			active->TextV(fmt, args);
			va_end(args);
		}
		inline void TextDisabled(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			active->TextDisabledV(fmt, args);
			va_end(args);
		}
		inline void BulletText(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			active->BulletTextV(fmt, args);
			va_end(args);
		}

		// --- Layout / scope. ---
		inline void SetNextItemWidth(float w) { active->SetNextItemWidth(w); }
		inline void BeginDisabled(bool disabled) { active->BeginDisabled(disabled); }
		inline void EndDisabled() { active->EndDisabled(); }
		inline void PushID(const char* id) { active->PushID(id); }
		inline void PopID() { active->PopID(); }

		// --- Rich widgets. ---
		inline bool InputText(const char* label, char* buf, size_t size) { return active->InputText(label, buf, size); }
		inline bool InputTextHint(const char* label, const char* hint, char* buf, size_t size) { return active->InputTextHint(label, hint, buf, size); }
		inline bool InputTextMultiline(const char* label, char* buf, size_t size, float height) { return active->InputTextMultiline(label, buf, size, height); }
		inline bool BeginChild(const char* id, float w, float h) { return active->BeginChild(id, w, h); }
		inline void EndChild() { active->EndChild(); }
		inline bool BeginCombo(const char* label, const char* preview) { return active->BeginCombo(label, preview); }
		inline void EndCombo() { active->EndCombo(); }
		inline bool Selectable(const char* label, bool selected = false) { return active->Selectable(label, selected); }
		inline void ClippedList(int count, const std::function<void(int)>& drawRow) { active->ClippedList(count, drawRow); }
		inline bool CollapsingHeader(const char* label) { return active->CollapsingHeader(label); }
		inline bool TreeNode(const char* label) { return active->TreeNode(label); }
		inline void TreePop() { active->TreePop(); }

		// --- Clipboard. ---
		inline void SetClipboardText(const char* text) { active->SetClipboardText(text); }
		inline const char* GetClipboardText() { return active->GetClipboardText(); }
	} // namespace UI
} // namespace Menu
