#pragma once

/// @file
/// @brief Child rich widget: BeginChild / EndChild — a bordered region marker (no hard clip).

#include "Core.h"

namespace UCanvasGUI
{
	/// Bordered region marker: draws a frame of height @p h; content flows inside (no hard clip).
	inline float childStartY = 0.0f;
	inline bool BeginChild(const char* /*id*/, float w, float h)
	{
		const FVector2D pos{menuPos.X + 10.0f + offsetX, menuPos.Y + 10.0f + offsetY};
		const float width = w > 0.0f ? w : 320.0f;
		DrawRect(pos, width, h, Colors::Background);
		childStartY = offsetY;
		return true;
	}
	inline void EndChild(float h)
	{
		const float end = childStartY + h + 10.0f;
		if (offsetY < end) offsetY = end;
	}
} // namespace UCanvasGUI
