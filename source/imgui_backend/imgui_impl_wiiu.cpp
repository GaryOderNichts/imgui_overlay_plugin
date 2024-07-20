// dear imgui: Platform Backend for the Wii U
//             Adjusted for usage with WUPS/WUMS.
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_wiiu.h"

// Wii U Data
struct ImGui_ImplWiiU_Data
{
    bool WasTouched;

    ImGui_ImplWiiU_Data()   { memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendPlatformUserData
static ImGui_ImplWiiU_Data* ImGui_ImplWiiU_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplWiiU_Data*)ImGui::GetIO().BackendPlatformUserData : NULL;
}

bool     ImGui_ImplWiiU_Init()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

    // Setup backend data
    ImGui_ImplWiiU_Data* bd = IM_NEW(ImGui_ImplWiiU_Data)();
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_wiiu";
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

    return true;
}

void     ImGui_ImplWiiU_Shutdown()
{
    ImGui_ImplWiiU_Data* bd = ImGui_ImplWiiU_GetBackendData();
    IM_ASSERT(bd != NULL && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    io.BackendPlatformName = NULL;
    io.BackendPlatformUserData = NULL;
    IM_DELETE(bd);
}

bool     ImGui_ImplWiiU_ProcessVPADInput(VPADStatus* input)
{
    ImGui_ImplWiiU_Data* bd = ImGui_ImplWiiU_GetBackendData();
    IM_ASSERT(bd != NULL && "Did you call ImGui_ImplWiiU_Init()?");
    ImGuiIO& io = ImGui::GetIO();

    VPADTouchData touch;
    VPADGetTPCalibratedPoint(VPAD_CHAN_0, &touch, &input->tpNormal);

    if (touch.touched)
    {
        float scale_x = (io.DisplaySize.x / io.DisplayFramebufferScale.x) / 1280.0f;
        float scale_y = (io.DisplaySize.y / io.DisplayFramebufferScale.y) / 720.0f;
        io.AddMousePosEvent(touch.x * scale_x, touch.y * scale_y);
    }

    if (touch.touched != bd->WasTouched)
    {
        io.AddMouseButtonEvent(ImGuiMouseButton_Left, touch.touched);
        bd->WasTouched = touch.touched;
    }

    return io.WantCaptureMouse;
}

bool     ImGui_ImplWiiU_ProcessWPADInput(WPADStatusProController* input)
{
    return true; // TODO
}
