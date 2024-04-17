#pragma once
#include "imgui.h"

namespace GUI {
	namespace Config {
		void Init() {
			ImGuiIO& io = ImGui::GetIO(); (void)io;

			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
			io.IniFilename = 0;
		}
	}
}