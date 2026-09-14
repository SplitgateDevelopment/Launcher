#pragma once

/// @file
/// @brief UCanvasGUI::Colors — the canvas menu's theme colors (mutable FLinearColor). Each member is
/// seeded at startup from a Render::Palette entry (bridged to FLinearColor by the UE adapter). They
/// stay mutable so the canvas backend can retint the accent per frame for the RGB feature and reset
/// it to the palette default when that feature is off.

#include "../../ue/sdk/FLinearColor.h"
#include "../../render/Colors.h"
#include "../../render/adapters/Ue.h"

namespace UCanvasGUI
{
	/// The canvas menu theme (0-1 float RGBA), defaulting to the shared Render::Palette. Only `Accent`
	/// changes at runtime (RGB retint); the rest are effectively constants sourced from the palette.
	namespace Colors
	{
		inline FLinearColor Accent = Render::Palette::Primary.To<FLinearColor>();	   ///< titles, active tabs, buttons, slider fill (retinted by the RGB feature)
		inline FLinearColor Text = Render::Palette::Text.To<FLinearColor>();			   ///< body and label text
		inline FLinearColor Background = Render::Palette::Background.To<FLinearColor>(); ///< window, popup, and child backgrounds
		inline FLinearColor Frame = Render::Palette::Border.To<FLinearColor>();		   ///< neutral element backgrounds (tab strip, sliders, combos, headers, fields)
		inline FLinearColor Selection = Render::Palette::Blue.To<FLinearColor>();	   ///< highlighted / selected row
		inline FLinearColor Enabled = Render::Palette::Success.To<FLinearColor>();	   ///< checked-toggle mark
		inline FLinearColor Knob = Render::Palette::Muted.To<FLinearColor>();		   ///< slider knob
	} // namespace Colors
} // namespace UCanvasGUI
