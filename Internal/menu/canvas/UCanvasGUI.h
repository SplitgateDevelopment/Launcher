#pragma once

/// @file
/// @brief UCanvasGUI — the immediate-mode UE-canvas menu backend. Recovered from the project's original
/// GUI and adapted to the neutral Render API: every primitive draws through the active `Render`
/// backend (canvas or the ImGui recorder), so there is no direct K2_Draw* here and no per-scanline
/// fill loops. Positions stay FVector2D and colors FLinearColor (they convert implicitly to Render::Vec2 /
/// Render::Color). This is the umbrella header: it pulls in the shared core plus every widget
/// component from `components/`; include it to use the GUI. Everything is `inline`.

#include "components/Core.h"
#include "components/Window.h"
#include "components/Text.h"
#include "components/Tab.h"
#include "components/Button.h"
#include "components/Checkbox.h"
#include "components/Slider.h"
#include "components/Combobox.h"
#include "components/Hotkey.h"
#include "components/ColorPicker.h"
#include "components/TextField.h"
#include "components/Selectable.h"
#include "components/CollapsingHeader.h"
#include "components/Combo.h"
#include "components/Child.h"
