#pragma once

/// @file
/// @brief Settings adapter: bridges the persisted (game-independent) ::Color into a Render::Color by
/// specializing ColorTraits<::Color>. `From` only — nothing converts a draw color back into a
/// settings color. Keeping this out of Color.h and Settings.h preserves both the neutral core and
/// the settings module's ImGui/UE-independence (so the unit tests stay clean).

#include "../Color.h"
#include "../../settings/Settings.h"

namespace Render
{
	template <>
	struct ColorTraits<::Color>
	{
		static Color From(const ::Color& c) { return {c.R, c.G, c.B, c.A}; }
	};
} // namespace Render
