#pragma once

#include <Windows.h>
#include <iostream>
#include <Psapi.h>
#include <vector>
#include <string>
#include <algorithm>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include ".asm/no_inlinesyscalls.h"
#include ".asm/spoof.h"

#include "menu/menu.h"

#include "ZeroGUI.h"
#include "ZeroInput.h"