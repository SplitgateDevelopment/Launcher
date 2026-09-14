#pragma once

/// @file
/// @brief This game's render-backend adapter selection: pulls in the UE, ImGui and settings
/// conversions so every render call site has them in scope. Render.h includes this. A different game
/// would ship a different set of adapters here (or its own) and reuse Vec2.h / Color.h verbatim.

#include "adapters/Ue.h"
#include "adapters/ImGui.h"
#include "adapters/Settings.h"
