// dear imgui: Platform Backend for the Wii U
//             Adjusted for usage with WUPS/WUMS.
#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

// GamePad Input
#include <vpad/input.h>
// Controller Input
#include <padscore/wpad.h>

IMGUI_IMPL_API bool     ImGui_ImplWiiU_Init();
IMGUI_IMPL_API void     ImGui_ImplWiiU_Shutdown();
IMGUI_IMPL_API bool     ImGui_ImplWiiU_ProcessVPADInput(VPADStatus* input);
IMGUI_IMPL_API bool     ImGui_ImplWiiU_ProcessWPADInput(WPADStatusProController* input);
