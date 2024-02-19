#pragma once

#include "../../ue/Engine.h"
#include "../Hook.h"

void ProcessEvent(UObject* Class, UObject* Function, void* params) {
	return Hook::OriginalProcessEvent(Class, Function, params);
};