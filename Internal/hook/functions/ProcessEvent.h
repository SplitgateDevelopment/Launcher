#pragma once

#include "../../ue/Engine.h"
#include "../../settings/Settings.h"
#include "../../utils/ExceptionHandler.h"

namespace ProcessEvent {
	void** VTable;
	void (*Original)(UObject*, UFunction*, void*) = nullptr;
	int Index = 68;

	void HookedProcessEvent(UObject* Class, UFunction* Function, void* Params) {
		static UObject* ReceiveShutdown = ObjObjects->FindObject("Function Engine.GameInstance.ReceiveShutdown");

		if (Settings.DEBUG.LogProcessEvent) {

			std::string className = Class->GetName().c_str();
			std::string functionName = Function->GetFullName();

			if (className != "DebugCanvasObject") {
				Logger::Log("INFO", std::format("Caller [{}] Function [{}]", className, functionName));
			}
		}

		if (Function == ReceiveShutdown) {
			Logger::Log("INFO", "Received shutdown");
			ExceptionHandler::Disable();
			Logger::DestroyConsole();
			//Hook::UnHook();
		};

		/* 
		TODO: Intercept and modify widget IsInputActionEnabled function
		static UObject* IsInputActionEnabled = ObjObjects->FindObject("Function PortalWars.PortalWarsUserWidget.IsInputActionEnabled");
		//Pointer comparison is faster
		if (Function == IsInputActionEnabled) {
			Logger::Log("INFO", "IsInputActionEnabled intercepted");

			struct IsInputActionEnabledStruct {
				struct FGameplayTag& ActionTag;
				bool& InIsEnabled;
			} *IsInputActionEnabledParams;
			IsInputActionEnabledParams = reinterpret_cast<IsInputActionEnabledStruct*>(Params);

			std::cout << IsInputActionEnabledParams << std::endl;
			
			IsInputActionEnabledParams->InIsEnabled = true;

			Logger::Log("INFO", "IsInputActionEnabled modified");
			return;
		}*/

		return Original(Class, Function, Params);
	};
}