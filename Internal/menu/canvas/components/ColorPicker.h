#pragma once

/// @file
/// @brief ColorPicker widget: a swatch that opens a 12x9 palette grid (ColorPixel cells), drawn via PostRenderer.

#include "Core.h"

namespace UCanvasGUI
{
	inline int activePicker = -1;
	inline FLinearColor savedColor;
	/// One swatch of the open palette grid: draws @p color, outlines it while it's the current pick,
	/// and writes it into @p original when clicked.
	inline void ColorPixel(FVector2D pos, FVector2D size, FLinearColor* original, FLinearColor color)
	{
		PostRenderer::DrawRect(pos, size.X, size.Y, color);

		if (original->R == color.R && original->G == color.G && original->B == color.B)
		{
			constexpr FLinearColor outline{0.0f, 0.0f, 0.0f, 1.0f};
			PostRenderer::DrawLine(FVector2D{pos.X, pos.Y}, FVector2D{pos.X + size.X - 1, pos.Y}, 1, outline);
			PostRenderer::DrawLine(FVector2D{pos.X, pos.Y + size.Y - 1}, FVector2D{pos.X + size.X - 1, pos.Y + size.Y - 1}, 1, outline);
			PostRenderer::DrawLine(FVector2D{pos.X, pos.Y}, FVector2D{pos.X, pos.Y + size.Y - 1}, 1, outline);
			PostRenderer::DrawLine(FVector2D{pos.X + size.X - 1, pos.Y}, FVector2D{pos.X + size.X - 1, pos.Y + size.Y - 1}, 1, outline);
		}

		if (MouseInZone(pos, size) && Input::IsMouseClicked(0, elementCount, false))
			*original = color;
	}
	inline bool ColorPicker(const char* name, FLinearColor* color)
	{
		elementCount++;

		constexpr float box = 25.0f;
		const FVector2D pos = NextPos({10, 10}, box + 10.0f);
		const bool isHovered = MouseInZone(pos, FVector2D{box, box});

		if (activePicker == elementCount)
		{
			elementHovered = true;

			float sizePickerX = 250;
			float sizePickerY = 250;
			bool isHoveredPicker = MouseInZone(FVector2D{pos.X, pos.Y}, FVector2D{sizePickerX, sizePickerY - 60});

			// Background
			PostRenderer::DrawRect(FVector2D{pos.X, pos.Y}, sizePickerX, sizePickerY - 65, Colors::Background);

			FVector2D pixelSize = FVector2D{sizePickerX / 12, sizePickerY / 12};

			// 0
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{174 / 255.f, 235 / 255.f, 253 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{136 / 255.f, 225 / 255.f, 251 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{108 / 255.f, 213 / 255.f, 250 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{89 / 255.f, 175 / 255.f, 213 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{76 / 255.f, 151 / 255.f, 177 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{60 / 255.f, 118 / 255.f, 140 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{43 / 255.f, 85 / 255.f, 100 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{32 / 255.f, 62 / 255.f, 74 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 0, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{255 / 255.f, 255 / 255.f, 255 / 255.f, 1.0f});
			}
			// 1
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{175 / 255.f, 205 / 255.f, 252 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{132 / 255.f, 179 / 255.f, 252 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{90 / 255.f, 152 / 255.f, 250 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{55 / 255.f, 120 / 255.f, 250 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{49 / 255.f, 105 / 255.f, 209 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{38 / 255.f, 83 / 255.f, 165 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{28 / 255.f, 61 / 255.f, 120 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{20 / 255.f, 43 / 255.f, 86 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 1, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{247 / 255.f, 247 / 255.f, 247 / 255.f, 1.0f});
			}
			// 2
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{153 / 255.f, 139 / 255.f, 250 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{101 / 255.f, 79 / 255.f, 249 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{64 / 255.f, 50 / 255.f, 230 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{54 / 255.f, 38 / 255.f, 175 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{39 / 255.f, 31 / 255.f, 144 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{32 / 255.f, 25 / 255.f, 116 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{21 / 255.f, 18 / 255.f, 82 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{16 / 255.f, 13 / 255.f, 61 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 2, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{228 / 255.f, 228 / 255.f, 228 / 255.f, 1.0f});
			}
			// 3
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{194 / 255.f, 144 / 255.f, 251 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{165 / 255.f, 87 / 255.f, 249 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{142 / 255.f, 57 / 255.f, 239 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{116 / 255.f, 45 / 255.f, 184 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{92 / 255.f, 37 / 255.f, 154 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{73 / 255.f, 29 / 255.f, 121 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{53 / 255.f, 21 / 255.f, 88 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{37 / 255.f, 15 / 255.f, 63 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 3, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{203 / 255.f, 203 / 255.f, 203 / 255.f, 1.0f});
			}
			// 4
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{224 / 255.f, 162 / 255.f, 197 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{210 / 255.f, 112 / 255.f, 166 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{199 / 255.f, 62 / 255.f, 135 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{159 / 255.f, 49 / 255.f, 105 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{132 / 255.f, 41 / 255.f, 89 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{104 / 255.f, 32 / 255.f, 71 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{75 / 255.f, 24 / 255.f, 51 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{54 / 255.f, 14 / 255.f, 36 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 4, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{175 / 255.f, 175 / 255.f, 175 / 255.f, 1.0f});
			}
			// 5
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{235 / 255.f, 175 / 255.f, 176 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{227 / 255.f, 133 / 255.f, 135 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{219 / 255.f, 87 / 255.f, 88 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{215 / 255.f, 50 / 255.f, 36 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{187 / 255.f, 25 / 255.f, 7 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{149 / 255.f, 20 / 255.f, 6 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{107 / 255.f, 14 / 255.f, 4 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{77 / 255.f, 9 / 255.f, 3 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 5, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{144 / 255.f, 144 / 255.f, 144 / 255.f, 1.0f});
			}
			// 6
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{241 / 255.f, 187 / 255.f, 171 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{234 / 255.f, 151 / 255.f, 126 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{229 / 255.f, 115 / 255.f, 76 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{227 / 255.f, 82 / 255.f, 24 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{190 / 255.f, 61 / 255.f, 15 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{150 / 255.f, 48 / 255.f, 12 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{107 / 255.f, 34 / 255.f, 8 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{79 / 255.f, 25 / 255.f, 6 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 6, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{113 / 255.f, 113 / 255.f, 113 / 255.f, 1.0f});
			}
			// 7
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{245 / 255.f, 207 / 255.f, 169 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{240 / 255.f, 183 / 255.f, 122 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{236 / 255.f, 159 / 255.f, 74 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{234 / 255.f, 146 / 255.f, 37 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{193 / 255.f, 111 / 255.f, 28 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{152 / 255.f, 89 / 255.f, 22 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{110 / 255.f, 64 / 255.f, 16 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{80 / 255.f, 47 / 255.f, 12 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 7, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{82 / 255.f, 82 / 255.f, 82 / 255.f, 1.0f});
			}
			// 8
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{247 / 255.f, 218 / 255.f, 170 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{244 / 255.f, 200 / 255.f, 124 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{241 / 255.f, 182 / 255.f, 77 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{239 / 255.f, 174 / 255.f, 44 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{196 / 255.f, 137 / 255.f, 34 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{154 / 255.f, 108 / 255.f, 27 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{111 / 255.f, 77 / 255.f, 19 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{80 / 255.f, 56 / 255.f, 14 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 8, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{54 / 255.f, 54 / 255.f, 54 / 255.f, 1.0f});
			}
			// 9
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{254 / 255.f, 243 / 255.f, 187 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{253 / 255.f, 237 / 255.f, 153 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{253 / 255.f, 231 / 255.f, 117 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{254 / 255.f, 232 / 255.f, 85 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{242 / 255.f, 212 / 255.f, 53 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{192 / 255.f, 169 / 255.f, 42 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{138 / 255.f, 120 / 255.f, 30 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{101 / 255.f, 87 / 255.f, 22 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 9, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{29 / 255.f, 29 / 255.f, 29 / 255.f, 1.0f});
			}
			// 10
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{247 / 255.f, 243 / 255.f, 185 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{243 / 255.f, 239 / 255.f, 148 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{239 / 255.f, 232 / 255.f, 111 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{235 / 255.f, 229 / 255.f, 76 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{208 / 255.f, 200 / 255.f, 55 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{164 / 255.f, 157 / 255.f, 43 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{118 / 255.f, 114 / 255.f, 31 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{86 / 255.f, 82 / 255.f, 21 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 10, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{9 / 255.f, 9 / 255.f, 9 / 255.f, 1.0f});
			}
			// 11
			{
				ColorPixel(FVector2D{pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 0}, pixelSize, color, FLinearColor{218 / 255.f, 232 / 255.f, 182 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 1}, pixelSize, color, FLinearColor{198 / 255.f, 221 / 255.f, 143 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 2}, pixelSize, color, FLinearColor{181 / 255.f, 210 / 255.f, 103 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 3}, pixelSize, color, FLinearColor{154 / 255.f, 186 / 255.f, 76 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 4}, pixelSize, color, FLinearColor{130 / 255.f, 155 / 255.f, 64 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 5}, pixelSize, color, FLinearColor{102 / 255.f, 121 / 255.f, 50 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 6}, pixelSize, color, FLinearColor{74 / 255.f, 88 / 255.f, 36 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 7}, pixelSize, color, FLinearColor{54 / 255.f, 64 / 255.f, 26 / 255.f, 1.0f});
				ColorPixel(FVector2D{pos.X + pixelSize.X * 11, pos.Y + pixelSize.Y * 8}, pixelSize, color, FLinearColor{0 / 255.f, 0 / 255.f, 0 / 255.f, 1.0f});
			}

			// Consume the click; only one outside the grid closes the picker.
			if (Input::IsMouseClicked(0, elementCount, false) && !isHoveredPicker)
				activePicker = -1;
		}
		else
		{
			// Closed swatch: accent border, the current color inset, and the label.
			DrawRect(pos, box, box, Colors::Accent);
			if (isHovered)
				elementHovered = true;
			DrawRect(FVector2D{pos.X + 4, pos.Y + 4}, box - 8, box - 8, *color);
			TextLeft(name, FVector2D{pos.X + box + 5.0f, pos.Y + box / 2}, Colors::Text, false);

			if (isHovered && Input::IsMouseClicked(0, elementCount, false))
			{
				savedColor = *color;
				activePicker = elementCount;
			}
		}

		EndElement(pos);

		// Report a change when the live color differs from the value saved when the picker opened.
		return activePicker != elementCount && (color->R != savedColor.R || color->G != savedColor.G || color->B != savedColor.B);
	}
} // namespace UCanvasGUI
