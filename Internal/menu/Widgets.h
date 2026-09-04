#pragma once

/// @file
/// @brief Menu helpers that tie ImGui controls to the event bus.

#include "gui/Custom.h" // ImGui::ToggleButton
#include "../scripting/Events.h"

namespace Menu
{
	/// A toggle that, when flipped, dispatches SettingsChanged tagged with its label (so handlers
	/// and scripts know which setting/feature changed — payload.name = label, payload.value = 0/1).
	/// @return true if the value changed this frame.
	inline bool ToggleSetting(const char* label, bool* value)
	{
		if (!ImGui::ToggleButton(label, value)) return false;
		Events::Dispatch(Events::Type::SettingsChanged, Events::Payload{.value = *value ? 1.f : 0.f, .name = label});
		return true;
	}
} // namespace Menu
