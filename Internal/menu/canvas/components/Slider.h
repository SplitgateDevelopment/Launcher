#pragma once

/// @file
/// @brief Slider widgets: SliderInt / SliderFloat — a draggable track with a knob and a live value
/// readout. Dragging is captured by widget id in `activeElement` so it continues off the track.

#include "Core.h"

namespace UCanvasGUI
{
	inline void SliderInt(const char* name, int* value, int min, int max)
	{
		elementCount++;

		const FVector2D size{240, 50};
		const FVector2D track{200, 10};
		const FVector2D pos = NextPos({10, 15}, size.Y + 15.0f);

		const FVector2D trackPos{pos.X, pos.Y + track.Y + 15.0f};
		const bool isHovered = MouseInZone(trackPos, track);

		if (isHovered || activeElement == elementCount)
		{
			elementHovered = true;
			if (Input::IsMouseClicked(0, elementCount, true))
			{
				activeElement = elementCount;
				const FVector2D cursor = CursorPos();
				*value = (int)(((cursor.X - pos.X) * ((max - min) / track.X)) + min);
				if (*value < min) *value = min;
				if (*value > max) *value = max;
			}
		}

		// Track + progress fill + knob.
		DrawRect(trackPos, track.X, track.Y, Colors::Frame);
		DrawRect(FVector2D{trackPos.X, trackPos.Y + 5.0f}, 5.0f, 5.0f, Colors::Accent);

		const float step = track.X / (max - min);
		DrawRect(trackPos, step * (*value - min), track.Y, Colors::Accent);
		DrawCircle(FVector2D{pos.X + step * (*value - min), trackPos.Y + 3.3f}, 10.0f, Colors::Knob);
		DrawCircle(FVector2D{pos.X + step * (*value - min), trackPos.Y + 3.3f}, 5.0f, Colors::Accent);

		char buffer[32];
		sprintf_s(buffer, "%i", *value);
		TextCenter(buffer, FVector2D{pos.X + step * (*value - min), trackPos.Y + 25.0f}, Colors::Text, false);
		TextLeft(name, FVector2D{pos.X + 5, pos.Y + 10}, Colors::Text, false);

		EndElement(pos, size);
	}

	inline void SliderFloat(const char* name, float* value, float min, float max, const char* format = "%.0f")
	{
		elementCount++;

		const FVector2D size{210, 40};
		const FVector2D track{170, 7};
		const FVector2D pos = NextPos({10, 15}, size.Y + 15.0f);

		const FVector2D trackPos{pos.X, pos.Y + track.Y + 15.0f};
		// The hit zone is taller than the track so the thin bar is easy to grab.
		const bool isHovered = MouseInZone(FVector2D{pos.X, trackPos.Y - 20.0f}, FVector2D{track.X, track.Y + 30.0f});

		if (isHovered || activeElement == elementCount)
		{
			elementHovered = true;
			if (Input::IsMouseClicked(0, elementCount, true))
			{
				activeElement = elementCount;
				const FVector2D cursor = CursorPos();
				*value = ((cursor.X - pos.X) * ((max - min) / track.X)) + min;
				if (*value < min) *value = min;
				if (*value > max) *value = max;
			}
		}

		// Track + end caps.
		DrawRect(trackPos, track.X, track.Y, Colors::Frame);
		DrawCircle(FVector2D{pos.X, pos.Y + 24.3f}, 3.1f, Colors::Accent);
		DrawCircle(FVector2D{pos.X + track.X, pos.Y + 24.3f}, 3.1f, Colors::Frame);

		TextLeft(name, FVector2D{pos.X, pos.Y + 5}, Colors::Text, false);

		// Progress fill + knob.
		const float step = track.X / (max - min);
		DrawRect(trackPos, step * (*value - min), track.Y, Colors::Accent);
		DrawCircle(FVector2D{pos.X + step * (*value - min), trackPos.Y + 2.66f}, 8.0f, Colors::Knob);
		DrawCircle(FVector2D{pos.X + step * (*value - min), trackPos.Y + 2.66f}, 4.0f, Colors::Accent);

		char buffer[32];
		sprintf_s(buffer, format, *value);
		TextCenter(buffer, FVector2D{pos.X + step * (*value - min), trackPos.Y + 20.0f}, Colors::Text, false);

		EndElement(pos, size);
	}
} // namespace UCanvasGUI
