#pragma once

/// @file
/// @brief CollapsingHeader rich widget: a persistent collapsible section header (also backs TreeNode).

#include "Core.h"

namespace UCanvasGUI
{
	inline bool headerOpen[256]; ///< per-id open state for CollapsingHeader / TreeNode
	/// Collapsible section header; returns whether it is open (persistent per id).
	inline bool CollapsingHeader(const char* name)
	{
		elementCount++;
		const int id = elementCount;

		const FVector2D size{260.0f, 22.0f};
		const FVector2D pos = NextPos(FVector2D{10, 10}, size.Y + 8.0f);
		const bool isHovered = MouseInZone(pos, size);

		DrawRect(pos, size.X, size.Y, Colors::Frame);
		if (isHovered) elementHovered = true;

		std::string label = std::string(headerOpen[id] ? "- " : "+ ") + name;
		TextLeft(label.c_str(), FVector2D{pos.X + 4.0f, pos.Y + size.Y / 2}, Colors::Text, false);

		if (isHovered && Input::IsMouseClicked(0, elementCount, false))
			headerOpen[id] = !headerOpen[id];

		EndElement(pos, size);
		return headerOpen[id];
	}
} // namespace UCanvasGUI
