#pragma once

/// @file
/// @brief The UE-canvas menu backend: draws in the PostRender hook through UCanvasGUI (which draws via
/// the active Render backend). Owns everything UCanvasGUI — the window/tab chrome (built over UCanvasGUI's Window +
/// tab-column primitives), every widget, the accent theming, per-frame input sampling, and its own
/// software cursor. Works everywhere the UE canvas is valid (main menu / loading included). Nothing
/// here leaks into the shared menu layer.

#include <cstdio>
#include <cstring>

#include "Backend.h"
#include "../canvas/UCanvasGUI.h"
#include "../../../shared/Utilities.h"
#include "../../settings/Settings.h"
#include "../../utils/Rgb.h"

namespace Menu
{
	class CanvasBackend : public Backend
	{
	  public:
		Phase phase() const override { return Phase::PostRender; }
		bool ExtraToggle() override { return false; }

		void BeginFrame() override
		{
			Input::Handle(); // sample mouse/keyboard once per frame

			// Accent: retint from the rainbow while the RGB feature is on; otherwise reset to the palette.
			UCanvasGUI::Colors::Accent = Settings.MENU.Rgb
										  ? [] { const Color c = Rgb::Current(); return FLinearColor{c.R, c.G, c.B, c.A}; }()
										  : Render::Palette::Primary.To<FLinearColor>();
		}
		void EndFrame() override {}

		bool BeginWindow(const char* title) override
		{
			if (!UCanvasGUI::Window(title, &pos, FVector2D{700.f, 500.f}, Settings.MENU.ShowMenu)) return false;
			tabIndex = 0;
			return true;
		}
		void EndWindow() override
		{
			UCanvasGUI::Render();			// drain deferred pop-ups (combo/color swatches) on top
			UCanvasGUI::DrawCursor(); // the Canvas menu's own cursor
		}
		bool BeginTab(const char* label) override
		{
			// Place this tab's button at an absolute slot in the left column (deterministic regardless of
			// content drawn for other tabs), then, if it is the active tab, move the cursor to the content
			// column so the section draws there.
			const float headerH = 25.f, tabH = 32.f, contentX = 130.f;
			UCanvasGUI::offsetX = 0.f;
			UCanvasGUI::PushNextElementY(UCanvasGUI::menuPos.Y + headerH + tabIndex * tabH, false);
			if (UCanvasGUI::ButtonTab(label, FVector2D{112.f, 30.f}, activeTab == tabIndex))
				activeTab = tabIndex;

			const bool isActive = (activeTab == tabIndex);
			tabIndex++;
			if (isActive)
			{
				UCanvasGUI::offsetX = contentX;
				UCanvasGUI::PushNextElementY(UCanvasGUI::menuPos.Y + headerH + 5.f, false);
			}
			return isActive;
		}
		void EndTab() override {}

		// --- Core widgets. ---
		bool Toggle(const char* label, bool* v) override { return UCanvasGUI::Checkbox(Vis(label), v); }
		bool Button(const char* label) override { return UCanvasGUI::Button(Vis(label), FVector2D{150.f, 25.f}); }
		bool SmallButton(const char* label) override { return UCanvasGUI::Button(Vis(label), FVector2D{90.f, 20.f}); }
		bool Checkbox(const char* label, bool* v) override { return UCanvasGUI::Checkbox(Vis(label), v); }
		bool RadioBool(const char* label, bool active) override { return UCanvasGUI::ButtonTab(Vis(label), FVector2D{110.f, 24.f}, active); }
		bool RadioInt(const char* label, int* v, int value) override
		{
			if (UCanvasGUI::ButtonTab(Vis(label), FVector2D{110.f, 24.f}, *v == value))
			{
				*v = value;
				return true;
			}
			return false;
		}
		bool SliderFloat(const char* label, float* v, float min, float max, const char* fmt) override
		{
			const float old = *v;
			UCanvasGUI::SliderFloat(Vis(label), v, min, max, fmt);
			return *v != old;
		}
		bool SliderInt(const char* label, int* v, int min, int max) override
		{
			const int old = *v;
			UCanvasGUI::SliderInt(Vis(label), v, min, max);
			return *v != old;
		}
		bool Combo(const char* label, int* v, const char* const* items, int count) override { return UCanvasGUI::Combobox(Vis(label), FVector2D{150.f, 25.f}, v, items, count); }
		bool ColorEdit(const char* label, ::Color* c) override { return UCanvasGUI::ColorPicker(Vis(label), reinterpret_cast<FLinearColor*>(c)); }
		bool HotKey(const char* label, int* key) override { return UCanvasGUI::Hotkey(Vis(label), FVector2D{90.f, 22.f}, key); }
		void SameLine() override { UCanvasGUI::SameLine(); }
		void SeparatorText(const char* label) override { UCanvasGUI::Text(Vis(label)); }
		void Tooltip(const char*) override {} // no hover tooltip on the canvas backend
		void TextV(const char* fmt, va_list args) override
		{
			char buf[512];
			vsnprintf(buf, sizeof(buf), fmt, args);
			UCanvasGUI::Text(buf);
		}
		void TextDisabledV(const char* fmt, va_list args) override { TextV(fmt, args); }
		void BulletTextV(const char* fmt, va_list args) override { TextV(fmt, args); }

		// --- Layout / scope (no-ops on the canvas: UCanvasGUI auto-sizes and ids are per-call). ---
		void SetNextItemWidth(float) override {}
		void BeginDisabled(bool) override {}
		void EndDisabled() override {}
		void PushID(const char*) override {}
		void PopID() override {}

		// --- Rich widgets. ---
		bool InputText(const char* label, char* buf, size_t size) override { return UCanvasGUI::TextField(Vis(label), buf, size, 220.f, 22.f, nullptr); }
		bool InputTextHint(const char* label, const char* hint, char* buf, size_t size) override { return UCanvasGUI::TextField(Vis(label), buf, size, 220.f, 22.f, hint); }
		bool InputTextMultiline(const char* label, char* buf, size_t size, float height) override { return UCanvasGUI::TextField(Vis(label), buf, size, 320.f, height, nullptr); }
		bool BeginChild(const char* id, float w, float h) override
		{
			lastChildH = h;
			return UCanvasGUI::BeginChild(id, w, h);
		}
		void EndChild() override { UCanvasGUI::EndChild(lastChildH); }
		bool BeginCombo(const char* label, const char* preview) override { return UCanvasGUI::BeginCombo(Vis(label), preview, 220.f); }
		void EndCombo() override { UCanvasGUI::EndCombo(); }
		bool Selectable(const char* label, bool selected) override { return UCanvasGUI::Selectable(Vis(label), selected, 300.f); }
		void ClippedList(int count, const std::function<void(int)>& drawRow) override
		{
			// No mouse-wheel on the canvas: draw a bounded window of rows and prompt to filter for more.
			constexpr int maxVisible = 14;
			const int shown = count < maxVisible ? count : maxVisible;
			for (int i = 0; i < shown; i++)
				drawRow(i);
			if (count > shown)
			{
				char note[64];
				snprintf(note, sizeof(note), "... %d more (filter to narrow)", count - shown);
				UCanvasGUI::Text(note);
			}
		}
		bool CollapsingHeader(const char* label) override { return UCanvasGUI::CollapsingHeader(Vis(label)); }
		bool TreeNode(const char* label) override { return UCanvasGUI::CollapsingHeader(Vis(label)); }
		void TreePop() override {}

		void SetClipboardText(const char* text) override { Shared::Utilities::CopyToClipboard(text ? text : ""); }
		const char* GetClipboardText() override
		{
			static std::string buffer;
			buffer = Shared::Utilities::PasteFromClipboard();
			return buffer.c_str();
		}

	  private:
		FVector2D pos{400.f, 200.f}; ///< draggable window position
		int tabIndex = 0;			 ///< reset each frame in BeginWindow
		int activeTab = 0;			 ///< persists across frames
		float lastChildH = 0.f;		 ///< height of the open BeginChild, for EndChild

		/// The visible part of a label: everything before an ImGui "##id" suffix (which the canvas would
		/// otherwise draw literally). Returns a thread-local buffer valid for the widget call.
		static const char* Vis(const char* label)
		{
			static std::string buf;
			const char* p = std::strstr(label, "##");
			buf.assign(label, p ? static_cast<size_t>(p - label) : std::strlen(label));
			return buf.c_str();
		}
	};
} // namespace Menu
