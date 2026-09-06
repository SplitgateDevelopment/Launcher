#pragma once

/// @file
/// @brief Menu::Backend — the abstract menu GUI backend. The shared menu layer (Menu::UI facade,
/// the Menu::Frame driver, the tab sections) draws through this interface and never names a concrete
/// backend, exactly like the render layer's Renderer. Concrete backends (ImGuiBackend, CanvasBackend)
/// live in this folder and are the ONLY place ImGui / UCanvasGUI are named. Adding a third backend is a
/// new implementation + one registry entry — no shared-code change.
///
/// This header is a pure interface: it depends only on the settings color type and the standard
/// library — no ImGui, no UCanvasGUI, no UE.

#include <cstdarg>
#include <cstddef>
#include <functional>
#include <string>

#include "../../settings/Settings.h" // ::Color

namespace Menu
{
	/// Which per-frame hook drives a backend. ImGui draws in the Present hook; the UE canvas draws in
	/// PostRender. The shared driver asks the active backend for its phase — that is the only place
	/// backend selection happens, and it is a single comparison, not a per-widget branch.
	enum class Phase
	{
		Present,
		PostRender,
	};

	/// The menu GUI backend interface: window/tab chrome, every widget, and per-frame lifecycle.
	class Backend
	{
	  public:
		virtual ~Backend() = default;

		// --- Lifecycle (each backend owns its input sampling, theming/accent, cursor, debug windows). ---
		virtual Phase phase() const = 0;
		virtual bool ExtraToggle() = 0; ///< extra "open menu" signal (e.g. ImGui gamepad Start); false if none
		virtual void BeginFrame() = 0;	///< once per frame before the window (sample input, apply accent, ...)
		virtual void EndFrame() = 0;	///< once per frame after the window

		// --- Window + tab chrome. ---
		virtual bool BeginWindow(const char* title) = 0; ///< false => don't draw the menu this frame
		virtual void EndWindow() = 0;
		virtual bool BeginTab(const char* label) = 0; ///< true => this tab is active; draw its section
		virtual void EndTab() = 0;

		// --- Core widgets (return `changed` where a value can change this frame). ---
		virtual bool Toggle(const char* label, bool* v) = 0;
		virtual bool Button(const char* label) = 0;
		virtual bool SmallButton(const char* label) = 0;
		virtual bool Checkbox(const char* label, bool* v) = 0;
		virtual bool RadioBool(const char* label, bool active) = 0;
		virtual bool RadioInt(const char* label, int* v, int value) = 0;
		virtual bool SliderFloat(const char* label, float* v, float min, float max, const char* fmt) = 0;
		virtual bool SliderInt(const char* label, int* v, int min, int max) = 0;
		virtual bool Combo(const char* label, int* v, const char* const* items, int count) = 0;
		virtual bool ColorEdit(const char* label, ::Color* c) = 0;
		virtual bool HotKey(const char* label, int* key) = 0;
		virtual void SameLine() = 0;
		virtual void SeparatorText(const char* label) = 0;
		virtual void Tooltip(const char* text) = 0;
		virtual void TextV(const char* fmt, va_list args) = 0;
		virtual void TextDisabledV(const char* fmt, va_list args) = 0;
		virtual void BulletTextV(const char* fmt, va_list args) = 0;

		// --- Layout / scope helpers. ---
		virtual void SetNextItemWidth(float w) = 0;
		virtual void BeginDisabled(bool disabled) = 0;
		virtual void EndDisabled() = 0;
		virtual void PushID(const char* id) = 0;
		virtual void PopID() = 0;

		// --- Rich widgets (the advanced panels use only these; both backends implement them). ---
		virtual bool InputText(const char* label, char* buf, size_t size) = 0;
		virtual bool InputTextHint(const char* label, const char* hint, char* buf, size_t size) = 0;
		virtual bool InputTextMultiline(const char* label, char* buf, size_t size, float height) = 0;
		virtual bool BeginChild(const char* id, float w, float h) = 0;
		virtual void EndChild() = 0;
		virtual bool BeginCombo(const char* label, const char* preview) = 0;
		virtual void EndCombo() = 0;
		virtual bool Selectable(const char* label, bool selected) = 0;
		/// Draw a (potentially long) list of @p count rows; @p drawRow(i) draws row i. The backend
		/// decides how to clip/scroll (ImGui uses ImGuiListClipper; the canvas culls to a visible window).
		virtual void ClippedList(int count, const std::function<void(int)>& drawRow) = 0;
		virtual bool CollapsingHeader(const char* label) = 0;
		virtual bool TreeNode(const char* label) = 0;
		virtual void TreePop() = 0;

		// --- Clipboard. ---
		virtual void SetClipboardText(const char* text) = 0;
		virtual const char* GetClipboardText() = 0;
	};
} // namespace Menu
