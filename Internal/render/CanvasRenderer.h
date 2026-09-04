#pragma once

/// @file
/// @brief Renderer backend that draws through the UE canvas (K2_DrawLine / K2_DrawText). Each call
/// is a ProcessEvent — the default, but the reason ImGuiRenderer exists.

#include "Renderer.h"
#include "../utils/Globals.h"

class CanvasRenderer : public Renderer
{
  public:
	void Line(const FVector2D& a, const FVector2D& b, float thickness, const FLinearColor& color) override
	{
		if (Globals::Canvas) Globals::Canvas->K2_DrawLine(a, b, thickness, color);
	}

	void Text(const FVector2D& pos, const std::string& text, float scale, const FLinearColor& color) override
	{
		if (Globals::Canvas)
			Globals::Canvas->K2_DrawText(0, FString(text), pos, {scale, scale}, color, 1.f, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f}, true, false, true, {0.f, 0.f, 0.f, 1.f});
	}
};
