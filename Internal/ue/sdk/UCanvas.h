#pragma once

#include "Fwd.h"
#include "Values.h"
#include "UObject.h"

// Class Engine.Canvas
// Size: 0x2d0 (Inherited: 0x28)
struct UCanvas : UObject
{
	float OrgX;							  // 0x28(0x04)
	float OrgY;							  // 0x2c(0x04)
	float ClipX;						  // 0x30(0x04)
	float ClipY;						  // 0x34(0x04)
	struct FColor DrawColor;			  // 0x38(0x04)
	char bCenterX : 1;					  // 0x3c(0x01)
	char bCenterY : 1;					  // 0x3c(0x01)
	char bNoSmooth : 1;					  // 0x3c(0x01)
	char pad_3C_3 : 5;					  // 0x3c(0x01)
	char pad_3D[0x3];					  // 0x3d(0x03)
	int32_t SizeX;						  // 0x40(0x04)
	int32_t SizeY;						  // 0x44(0x04)
	char pad_48[0x8];					  // 0x48(0x08)
	char ColorModulate[0x10];			  // 0x50(0x10)
	struct UTexture2D* DefaultTexture;	  // 0x60(0x08)
	struct UTexture2D* GradientTexture0;  // 0x68(0x08)
	struct UReporterGraph* ReporterGraph; // 0x70(0x08)
	char pad_78[0x258];					  // 0x78(0x258)

	struct FVector2D K2_TextSize(struct UFont* RenderFont, struct FString RenderText, struct FVector2D Scale);																																																																																																												 // Function Engine.Canvas.K2_TextSize // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c14b0
	struct FVector2D K2_StrLen(struct UFont* RenderFont, struct FString RenderText);																																																																																																																		 // Function Engine.Canvas.K2_StrLen // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c13c0
	struct FVector K2_Project(struct FVector WorldLocation);																																																																																																																								 // Function Engine.Canvas.K2_Project // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c1310
	void K2_DrawTriangle(struct UTexture* RenderTexture, struct TArray<struct FCanvasUVTri> Triangles);																																																																																																														 // Function Engine.Canvas.K2_DrawTriangle // (Final|Native|Public|BlueprintCallable) // @ game+0x36c11e0
	void K2_DrawTexture(struct UTexture* RenderTexture, struct FVector2D ScreenPosition, struct FVector2D ScreenSize, struct FVector2D CoordinatePosition, struct FVector2D CoordinateSize, struct FLinearColor RenderColor, enum class EBlendMode BlendMode, float Rotation, struct FVector2D PivotPoint);																																																													 // Function Engine.Canvas.K2_DrawTexture // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c0f30
	void K2_DrawText(struct UFont* RenderFont, struct FString RenderText, struct FVector2D ScreenPosition, struct FVector2D Scale, struct FLinearColor RenderColor, float Kerning, struct FLinearColor ShadowColor, struct FVector2D ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, struct FLinearColor OutlineColor);																																																							 // Function Engine.Canvas.K2_DrawText // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c0b70
	void K2_DrawPolygon(struct UTexture* RenderTexture, struct FVector2D ScreenPosition, struct FVector2D Radius, int32_t NumberOfSides, struct FLinearColor RenderColor);																																																																																													 // Function Engine.Canvas.K2_DrawPolygon // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c09c0
	void K2_DrawMaterialTriangle(struct UMaterialInterface* RenderMaterial, struct TArray<struct FCanvasUVTri> Triangles);																																																																																																									 // Function Engine.Canvas.K2_DrawMaterialTriangle // (Final|Native|Public|BlueprintCallable) // @ game+0x36c0890
	void K2_DrawMaterial(struct UMaterialInterface* RenderMaterial, struct FVector2D ScreenPosition, struct FVector2D ScreenSize, struct FVector2D CoordinatePosition, struct FVector2D CoordinateSize, float Rotation, struct FVector2D PivotPoint);																																																																										 // Function Engine.Canvas.K2_DrawMaterial // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c0650
	void K2_DrawLine(struct FVector2D ScreenPositionA, struct FVector2D ScreenPositionB, float Thickness, struct FLinearColor RenderColor);																																																																																																					 // Function Engine.Canvas.K2_DrawLine // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c04f0
	void K2_DrawBox(struct FVector2D ScreenPosition, struct FVector2D ScreenSize, float Thickness, struct FLinearColor RenderColor);																																																																																																						 // Function Engine.Canvas.K2_DrawBox // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36c0390
	void K2_DrawBorder(struct UTexture* BorderTexture, struct UTexture* BackgroundTexture, struct UTexture* LeftBorderTexture, struct UTexture* RightBorderTexture, struct UTexture* TopBorderTexture, struct UTexture* BottomBorderTexture, struct FVector2D ScreenPosition, struct FVector2D ScreenSize, struct FVector2D CoordinatePosition, struct FVector2D CoordinateSize, struct FLinearColor RenderColor, struct FVector2D BorderScale, struct FVector2D BackgroundScale, float Rotation, struct FVector2D PivotPoint, struct FVector2D CornerSize); // Function Engine.Canvas.K2_DrawBorder // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x36bff00
	void K2_Deproject(struct FVector2D ScreenPosition, struct FVector& WorldOrigin, struct FVector& WorldDirection);																																																																																																										 // Function Engine.Canvas.K2_Deproject // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable) // @ game+0x36bfde0
};
