#pragma once

#include "Fwd.h"
#include "Values.h"
#include "AActor.h"

// Class Engine.HUD
// Size: 0x310 (Inherited: 0x220)
struct AHUD : AActor
{
	struct APlayerController* PlayerOwner;				// 0x220(0x08)
	char bLostFocusPaused : 1;							// 0x228(0x01)
	char bShowHUD : 1;									// 0x228(0x01)
	char bShowDebugInfo : 1;							// 0x228(0x01)
	char pad_228_3 : 5;									// 0x228(0x01)
	char pad_229[0x3];									// 0x229(0x03)
	int32_t CurrentTargetIndex;							// 0x22c(0x04)
	char bShowHitBoxDebugInfo : 1;						// 0x230(0x01)
	char bShowOverlays : 1;								// 0x230(0x01)
	char bEnableDebugTextShadow : 1;					// 0x230(0x01)
	char pad_230_3 : 5;									// 0x230(0x01)
	char pad_231[0x7];									// 0x231(0x07)
	struct TArray<struct AActor*> PostRenderedActors;	// 0x238(0x10)
	char pad_248[0x8];									// 0x248(0x08)
	struct TArray<struct FName> DebugDisplay;			// 0x250(0x10)
	struct TArray<struct FName> ToggledDebugCategories; // 0x260(0x10)
	struct UCanvas* Canvas;								// 0x270(0x08)
	struct UCanvas* DebugCanvas;						// 0x278(0x08)
	struct TArray<struct FDebugTextInfo> DebugTextList; // 0x280(0x10)
	struct AActor* ShowDebugTargetDesiredClass;			// 0x290(0x08)
	struct AActor* ShowDebugTargetActor;				// 0x298(0x08)
	char pad_2A0[0x70];									// 0x2a0(0x70)

	void ShowHUD();																																																																													// Function Engine.HUD.ShowHUD // (Exec|Native|Public) // @ game+0x164ce00
	void ShowDebugToggleSubCategory(struct FName Category);																																																																			// Function Engine.HUD.ShowDebugToggleSubCategory // (Final|Exec|Native|Public) // @ game+0x3703390
	void ShowDebugForReticleTargetToggle(struct AActor* DesiredClass);																																																																// Function Engine.HUD.ShowDebugForReticleTargetToggle // (Final|Exec|Native|Public) // @ game+0x3703300
	void ShowDebug(struct FName DebugType);																																																																							// Function Engine.HUD.ShowDebug // (Exec|Native|Public) // @ game+0x3703270
	void RemoveDebugText(struct AActor* SrcActor, bool bLeaveDurationText);																																																															// Function Engine.HUD.RemoveDebugText // (Final|Net|NetReliableNative|Event|Public|NetClient) // @ game+0x3703050
	void RemoveAllDebugStrings();																																																																									// Function Engine.HUD.RemoveAllDebugStrings // (Final|Net|NetReliableNative|Event|Public|NetClient) // @ game+0x3703030
	void ReceiveHitBoxRelease(struct FName BoxName);																																																																				// Function Engine.HUD.ReceiveHitBoxRelease // (BlueprintCosmetic|Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveHitBoxEndCursorOver(struct FName BoxName);																																																																			// Function Engine.HUD.ReceiveHitBoxEndCursorOver // (BlueprintCosmetic|Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveHitBoxClick(struct FName BoxName);																																																																					// Function Engine.HUD.ReceiveHitBoxClick // (BlueprintCosmetic|Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveHitBoxBeginCursorOver(struct FName BoxName);																																																																		// Function Engine.HUD.ReceiveHitBoxBeginCursorOver // (BlueprintCosmetic|Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	void ReceiveDrawHUD(int32_t SizeX, int32_t SizeY);																																																																				// Function Engine.HUD.ReceiveDrawHUD // (BlueprintCosmetic|Event|Public|BlueprintEvent) // @ game+0x1a5c6b0
	struct FVector Project(struct FVector Location);																																																																				// Function Engine.HUD.Project // (Final|Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x3702b80
	void PreviousDebugTarget();																																																																										// Function Engine.HUD.PreviousDebugTarget // (Exec|Native|Public) // @ game+0x1654360
	void NextDebugTarget();																																																																											// Function Engine.HUD.NextDebugTarget // (Exec|Native|Public) // @ game+0x1654340
	void GetTextSize(struct FString Text, float& OutWidth, float& OutHeight, struct UFont* Font, float Scale);																																																						// Function Engine.HUD.GetTextSize // (Final|Native|Public|HasOutParms|BlueprintCallable|BlueprintPure|Const) // @ game+0x3702060
	struct APlayerController* GetOwningPlayerController();																																																																			// Function Engine.HUD.GetOwningPlayerController // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x1224b90
	struct APawn* GetOwningPawn();																																																																									// Function Engine.HUD.GetOwningPawn // (Final|Native|Public|BlueprintCallable|BlueprintPure|Const) // @ game+0x3702030
	void GetActorsInSelectionRectangle(struct AActor* ClassFilter, struct FVector2D& FirstPoint, struct FVector2D& SecondPoint, struct TArray<struct AActor*>& OutActors, bool bIncludeNonCollidingComponents, bool bActorMustBeFullyEnclosed);																						// Function Engine.HUD.GetActorsInSelectionRectangle // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure) // @ game+0x3701e10
	void DrawTextureSimple(struct UTexture* Texture, float ScreenX, float ScreenY, float Scale, bool bScalePosition);																																																				// Function Engine.HUD.DrawTextureSimple // (Final|Native|Public|BlueprintCallable) // @ game+0x3701c60
	void DrawTexture(struct UTexture* Texture, float ScreenX, float ScreenY, float ScreenW, float ScreenH, float TextureU, float TextureV, float TextureUWidth, float TextureVHeight, struct FLinearColor TintColor, enum class EBlendMode BlendMode, float Scale, bool bScalePosition, float Rotation, struct FVector2D RotPivot); // Function Engine.HUD.DrawTexture // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x3701800
	void DrawText(struct FString Text, struct FLinearColor TextColor, float ScreenX, float ScreenY, struct UFont* Font, float Scale, bool bScalePosition);																																											// Function Engine.HUD.DrawText // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x37015c0
	void DrawRect(struct FLinearColor RectColor, float ScreenX, float ScreenY, float ScreenW, float ScreenH);																																																						// Function Engine.HUD.DrawRect // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x3701400
	void DrawMaterialTriangle(struct UMaterialInterface* Material, struct FVector2D V0_Pos, struct FVector2D V1_Pos, struct FVector2D V2_Pos, struct FVector2D V0_UV, struct FVector2D V1_UV, struct FVector2D V2_UV, struct FLinearColor V0_Color, struct FLinearColor V1_Color, struct FLinearColor V2_Color);					// Function Engine.HUD.DrawMaterialTriangle // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x3701110
	void DrawMaterialSimple(struct UMaterialInterface* Material, float ScreenX, float ScreenY, float ScreenW, float ScreenH, float Scale, bool bScalePosition);																																										// Function Engine.HUD.DrawMaterialSimple // (Final|Native|Public|BlueprintCallable) // @ game+0x3700ef0
	void DrawMaterial(struct UMaterialInterface* Material, float ScreenX, float ScreenY, float ScreenW, float ScreenH, float MaterialU, float MaterialV, float MaterialUWidth, float MaterialVHeight, float Scale, bool bScalePosition, float Rotation, struct FVector2D RotPivot);													// Function Engine.HUD.DrawMaterial // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x3700b20
	void DrawLine(float StartScreenX, float StartScreenY, float EndScreenX, float EndScreenY, struct FLinearColor LineColor, float LineThickness);																																													// Function Engine.HUD.DrawLine // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x3700920
	void Deproject(float ScreenX, float ScreenY, struct FVector& WorldPosition, struct FVector& WorldDirection);																																																					// Function Engine.HUD.Deproject // (Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const) // @ game+0x37007a0
	void AddHitBox(struct FVector2D Position, struct FVector2D Size, struct FName InName, bool bConsumesInput, int32_t Priority);																																																	// Function Engine.HUD.AddHitBox // (Final|Native|Public|HasDefaults|BlueprintCallable) // @ game+0x3700480
	void AddDebugText(struct FString DebugText, struct AActor* SrcActor, float Duration, struct FVector Offset, struct FVector DesiredOffset, struct FColor TextColor, bool bSkipOverwriteCheck, bool bAbsoluteLocation, bool bKeepAttachedToActor, struct UFont* InFont, float FontScale, bool bDrawShadow);						// Function Engine.HUD.AddDebugText // (Final|Net|NetReliableNative|Event|Public|HasDefaults|NetClient) // @ game+0x37000c0
};
