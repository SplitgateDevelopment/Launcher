#pragma once

/// @file
/// @brief The menu backend registry: the concrete backend instances, the table indexed by the
/// MenuBackend setting, the active pointer, and Select — mirroring render/Render.h. Adding a backend
/// is a new instance + one table entry here (and one MenuBackend enum value); no shared-code change.

#include "Backend.h"
#include "ImGuiBackend.h"
#include "CanvasBackend.h"
#include "../../settings/Settings.h"

namespace Menu
{
	inline ImGuiBackend imguiBackend;
	inline CanvasBackend canvasBackend;

	/// Indexed by MenuBackend (order must match the enum). Adding a backend = a new entry, no branching.
	inline Backend* const backends[] = {
		&imguiBackend,	// MenuBackend::ImGui
		&canvasBackend, // MenuBackend::Canvas
	};

	/// The backend the shared menu layer draws through this frame.
	inline Backend* active = backends[0];

	/// Point @ref active at the backend the setting selects (called once per frame per hook).
	inline void Select(MenuBackend backend)
	{
		const size_t index = static_cast<size_t>(backend);
		active = (index < (sizeof(backends) / sizeof(*backends))) ? backends[index] : backends[0];
	}
} // namespace Menu
