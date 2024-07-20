#include "overlay.h"

#include <imgui.h>
#include "imgui_backend/imgui_impl_gx2.h"
#include "imgui_backend/imgui_impl_wiiu.h"

#include <gx2/state.h>

void Overlay_Initialize()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Scale everything by 2 for the Wii U
    // ImGui::GetStyle().ScaleAllSizes(2.0f);
    // io.FontGlobalScale = 2.0f;

    io.DisplaySize.x = (float)854; // set the current display width
    io.DisplaySize.y = (float)480; // set the current display height here

    // Setup platform and renderer backends
    ImGui_ImplWiiU_Init();
    ImGui_ImplGX2_Init();
}

void Overlay_Finalize()
{

}

void Overlay_Update()
{

}

void Overlay_Draw(uint32_t width, uint32_t height)
{
    ImGuiIO& io = ImGui::GetIO();

    // TODO don't do this every frame
    io.DisplaySize.x = (float)width; // set the current display width
    io.DisplaySize.y = (float)height; // set the current display height here

    // Scale based on height
    // float scale = height / 480.0f;
    // ImGui::GetStyle().ScaleAllSizes(scale);
    // io.FontGlobalScale = scale;

    // Start the Dear ImGui frame
    ImGui_ImplGX2_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();

    ImGui_ImplGX2_RenderDrawData(ImGui::GetDrawData());

    GX2Flush();
}
