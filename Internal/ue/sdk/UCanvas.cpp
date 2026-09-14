/// @file
/// @brief Out-of-line UFunction wrappers for UCanvas.

#include "../Engine.h"

using namespace Engine;

void UCanvas::K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, FLOAT Thickness, FLinearColor Color)
{
	struct
	{
		FVector2D ScreenPositionA;
		FVector2D ScreenPositionB;
		FLOAT Thickness;
		FLinearColor Color;
	} Parameters;

	Parameters.ScreenPositionA = ScreenPositionA;
	Parameters.ScreenPositionB = ScreenPositionB;
	Parameters.Thickness = Thickness;
	Parameters.Color = Color;

	static auto Function = GObjects->FindObject("Function Engine.Canvas.K2_DrawLine");
	ProcessEvent(Function, &Parameters);
}
void UCanvas::K2_DrawText(struct UFont* RenderFont, struct FString RenderText, struct FVector2D ScreenPosition, struct FVector2D Scale, struct FLinearColor RenderColor, float Kerning, struct FLinearColor ShadowColor, struct FVector2D ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, struct FLinearColor OutlineColor)
{
	struct
	{
		struct UFont* RenderFont;
		FString RenderText;
		FVector2D ScreenPosition;
		FVector2D Scale;
		FLinearColor RenderColor;
		float Kerning;
		FLinearColor ShadowColor;
		FVector2D ShadowOffset;
		bool bCentreX;
		bool bCentreY;
		bool bOutlined;
		FLinearColor OutlineColor;
	} Parameters;

	static auto FontObj = GObjects->FindObject("Font Roboto.Roboto");
	Parameters.RenderFont = RenderFont ? RenderFont : (UFont*)FontObj;
	Parameters.RenderText = RenderText;
	Parameters.ScreenPosition = ScreenPosition;
	Parameters.Scale = Scale;
	Parameters.RenderColor = RenderColor;
	Parameters.Kerning = Kerning;
	Parameters.ShadowColor = ShadowColor;
	Parameters.ShadowOffset = ShadowOffset;
	Parameters.bCentreX = bCentreX;
	Parameters.bCentreY = bCentreY;
	Parameters.bOutlined = bOutlined;

	static auto Function = GObjects->FindObject("Function Engine.Canvas.K2_DrawText");
	ProcessEvent(Function, &Parameters);
};

FVector2D UCanvas::K2_TextSize(struct UFont* RenderFont, struct FString RenderText, struct FVector2D Scale)
{
	struct
	{
		struct UFont* RenderFont;
		FString RenderText;
		FVector2D Scale;
		FVector2D ReturnValue;
	} Parameters;

	static auto FontObj = GObjects->FindObject("Font Roboto.Roboto");
	Parameters.RenderFont = RenderFont ? RenderFont : (UFont*)FontObj;
	Parameters.RenderText = RenderText;
	Parameters.Scale = Scale;

	static auto Function = GObjects->FindObject("Function Engine.Canvas.K2_TextSize");
	ProcessEvent(Function, &Parameters);
	return Parameters.ReturnValue;
}

FVector2D UCanvas::K2_StrLen(struct UFont* RenderFont, struct FString RenderText)
{
	struct
	{
		struct UFont* RenderFont;
		FString RenderText;
		FVector2D ReturnValue;
	} Parameters;

	static auto FontObj = GObjects->FindObject("Font Roboto.Roboto");
	Parameters.RenderFont = RenderFont ? RenderFont : (UFont*)FontObj;
	Parameters.RenderText = RenderText;

	static auto Function = GObjects->FindObject("Function Engine.Canvas.K2_StrLen");
	ProcessEvent(Function, &Parameters);
	return Parameters.ReturnValue;
}

void UCanvas::K2_DrawBox(FVector2D ScreenPosition, FVector2D ScreenSize, float Thickness, FLinearColor RenderColor)
{
	struct
	{
		FVector2D ScreenPosition;
		FVector2D ScreenSize;
		float Thickness;
		FLinearColor RenderColor;
	} Parameters;

	Parameters.ScreenPosition = ScreenPosition;
	Parameters.ScreenSize = ScreenSize;
	Parameters.Thickness = Thickness;
	Parameters.RenderColor = RenderColor;

	static auto Function = GObjects->FindObject("Function Engine.Canvas.K2_DrawBox");
	ProcessEvent(Function, &Parameters);
}
