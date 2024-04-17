#pragma once

#include <string>
#include <Windows.h>

namespace GUI {
	std::string VirtualKeyCodeToString(UCHAR virtualKey)
	{
		UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

		if (virtualKey == VK_LBUTTON)
		{
			return "MOUSE0";
		}
		if (virtualKey == VK_RBUTTON)
		{
			return "MOUSE1";
		}
		if (virtualKey == VK_MBUTTON)
		{
			return "MBUTTON";
		}
		if (virtualKey == VK_XBUTTON1)
		{
			return "XBUTTON1";
		}
		if (virtualKey == VK_XBUTTON2)
		{
			return "XBUTTON2";
		}

		CHAR szName[128];
		int result = 0;
		switch (virtualKey)
		{
		case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
		case VK_RCONTROL: case VK_RMENU:
		case VK_LWIN: case VK_RWIN: case VK_APPS:
		case VK_PRIOR: case VK_NEXT:
		case VK_END: case VK_HOME:
		case VK_INSERT: case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
			scanCode |= KF_EXTENDED;
		default:
			result = GetKeyNameTextA(scanCode << 16, szName, 128);
		}

		return szName;
	}
}