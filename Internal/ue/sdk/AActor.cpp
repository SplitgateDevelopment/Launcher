/// @file
/// @brief Out-of-line UFunction wrappers for AActor.

#include "../Engine.h"

using namespace Engine;

bool AActor::K2_TeleportTo(struct FVector DestLocation, struct FRotator DestRotation)
{
	struct
	{
		FVector DestLocation;
		FRotator DestRotation;
		bool ReturnValue;
	} Parameters;
	Parameters.DestLocation = DestLocation;
	Parameters.DestRotation = DestRotation;

	static auto Function = ObjObjects->FindObject("Function Engine.Actor.K2_TeleportTo");
	ProcessEvent(Function, &Parameters);
	return Parameters.ReturnValue;
};
FVector AActor::K2_GetActorLocation()
{
	struct
	{
		FVector ReturnValue;
	} Parameters;

	static auto Function = ObjObjects->FindObject("Function Engine.Actor.K2_GetActorLocation");
	ProcessEvent(Function, &Parameters);
	return Parameters.ReturnValue;
};
FRotator AActor::K2_GetActorRotation()
{
	struct
	{
		FRotator ReturnValue;
	} Parameters;

	static auto Function = ObjObjects->FindObject("Function Engine.Actor.K2_GetActorRotation");
	ProcessEvent(Function, &Parameters);
	return Parameters.ReturnValue;
};
void AActor::SetActorEnableCollision(bool bNewActorEnableCollision)
{
	struct
	{
		bool bNewActorEnableCollision;
	} Parameters;
	Parameters.bNewActorEnableCollision = bNewActorEnableCollision;

	static auto Function = ObjObjects->FindObject("Function Engine.Actor.SetActorEnableCollision");
	ProcessEvent(Function, &Parameters);
};
bool AActor::GetActorEnableCollision()
{
	struct
	{
		bool ReturnValue;
	} Parameters;

	static auto Function = ObjObjects->FindObject("Function Engine.Actor.GetActorEnableCollision");
	ProcessEvent(Function, &Parameters);
	return Parameters.ReturnValue;
};
bool AActor::K2_SetActorLocation(struct FVector NewLocation, bool bSweep, struct FHitResult& SweepHitResult, bool bTeleport)
{
	struct
	{
		FVector NewLocation;
		bool bSweep;
		FHitResult SweepHitResult;
		bool bTeleport;
		bool ReturnValue;
	} Parameters;

	Parameters.NewLocation = NewLocation;
	Parameters.bSweep = bSweep;
	Parameters.SweepHitResult = SweepHitResult;
	Parameters.bTeleport = bTeleport;

	static auto Function = ObjObjects->FindObject("Function Engine.Actor.K2_SetActorLocation");
	ProcessEvent(Function, &Parameters);
	return Parameters.ReturnValue;
}
void AActor::GetActorBounds(bool bOnlyCollidingComponents, struct FVector& Origin, struct FVector& BoxExtent, bool bIncludeFromChildActors)
{
	auto Function = ObjObjects->FindObject("Function Engine.Actor.GetActorBounds");

	struct
	{
		bool bOnlyCollidingComponents;
		struct FVector Origin;
		struct FVector BoxExtent;
		bool bIncludeFromChildActors;
	} Params;
	Params.bOnlyCollidingComponents = bOnlyCollidingComponents;
	Params.Origin = Origin;
	Params.BoxExtent = BoxExtent;
	Params.bIncludeFromChildActors = bIncludeFromChildActors;

	UObject::ProcessEvent(Function, &Params);
};
bool AActor::WasRecentlyRendered(float Tolerance)
{
	static auto Function = ObjObjects->FindObject("Function Engine.Actor.WasRecentlyRendered");
	if (!Function) return true; // if the check can't run, don't hide the actor

	struct
	{
		float Tolerance;
		bool ReturnValue;
	} Parameters;
	Parameters.Tolerance = Tolerance;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
}
FVector AActor::GetVelocity()
{
	static auto Function = ObjObjects->FindObject("Function Engine.Actor.GetVelocity");
	if (!Function) return FVector{0.f, 0.f, 0.f};

	struct
	{
		FVector ReturnValue;
	} Parameters;

	ProcessEvent(Function, &Parameters);

	return Parameters.ReturnValue;
}
