/// @file
/// @brief Out-of-line UFunction wrappers for UPortalWarsNotificationManager.

#include "../Engine.h"
#include "../UObjects.h"

using namespace Engine;

void UPortalWarsNotificationManager::OpenNoticeDialog(FErrorInfo& ErrorInfo)
{
	auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsNotificationManager.OpenNoticeDialog");

	struct
	{
		FErrorInfo ErrorInfo;
	} Params;
	Params.ErrorInfo = ErrorInfo;

	UObject::ProcessEvent(Function, &Params);
}
void UPortalWarsNotificationManager::OpenErrorDialog(FErrorInfo& ErrorInfo)
{
	auto Function = ObjObjects->FindObject("Function PortalWars.PortalWarsNotificationManager.OpenErrorDialog");

	struct
	{
		FErrorInfo ErrorInfo;
	} Params;
	Params.ErrorInfo = ErrorInfo;

	UObject::ProcessEvent(Function, &Params);
}
