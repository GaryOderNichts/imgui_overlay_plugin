#pragma once

#include <cstdint>

void Overlay_Initialize();

void Overlay_Finalize();

void Overlay_Update();

void Overlay_Draw(uint32_t width, uint32_t height);
