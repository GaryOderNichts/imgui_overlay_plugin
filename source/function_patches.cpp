#include <wups.h>
#include "main.h"
#include "overlay.h"

#include <gx2/context.h>
#include <gx2/enum.h>
#include <gx2/surface.h>
#include <gx2/registers.h>
#include <gx2/mem.h>

#include <cstring> // for memcpy

#include "imgui_backend/imgui_impl_wiiu.h"

namespace {

GX2SurfaceFormat tvSurfaceFormat = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
GX2SurfaceFormat drcSurfaceFormat = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
GX2ContextState* gOriginalContextState = nullptr;
GX2ColorBuffer lastTVColorBuffer;
GX2ColorBuffer lastDRCColorBuffer;

}

bool drawScreenshotSavedTexture(const GX2ColorBuffer *colorBuffer, GX2ScanTarget scan_target);

DECL_FUNCTION(void, GX2GetCurrentScanBuffer, GX2ScanTarget scanTarget, GX2ColorBuffer *cb)
{
    real_GX2GetCurrentScanBuffer(scanTarget, cb);
    if (scanTarget == GX2_SCAN_TARGET_TV) {
        memcpy(&lastTVColorBuffer, cb, sizeof(GX2ColorBuffer));
    } else {
        memcpy(&lastDRCColorBuffer, cb, sizeof(GX2ColorBuffer));
    }
}

DECL_FUNCTION(void, GX2SetContextState, GX2ContextState *curContext)
{
    real_GX2SetContextState(curContext);

    gOriginalContextState = curContext;
}

DECL_FUNCTION(void, GX2SetupContextStateEx, GX2ContextState *state, BOOL unk1)
{
    real_GX2SetupContextStateEx(state, unk1);
    gOriginalContextState = state;
}

DECL_FUNCTION(void, GX2SetTVBuffer, void *buffer, uint32_t buffer_size, int32_t tv_render_mode, GX2SurfaceFormat surface_format, GX2BufferingMode buffering_mode)
{
    tvSurfaceFormat = surface_format;

    return real_GX2SetTVBuffer(buffer, buffer_size, tv_render_mode, surface_format, buffering_mode);
}

DECL_FUNCTION(void, GX2SetDRCBuffer, void *buffer, uint32_t buffer_size, uint32_t drc_mode, GX2SurfaceFormat surface_format, GX2BufferingMode buffering_mode)
{
    drcSurfaceFormat = surface_format;

    return real_GX2SetDRCBuffer(buffer, buffer_size, drc_mode, surface_format, buffering_mode);
}

void drawIntoColorBuffer(const GX2ColorBuffer *colorBuffer, GX2ScanTarget scan_target)
{
    real_GX2SetContextState(gContextState);

    GX2SetColorBuffer((GX2ColorBuffer *) colorBuffer, GX2_RENDER_TARGET_0);

    Overlay_Draw(colorBuffer->surface.width, colorBuffer->surface.height);

    real_GX2SetContextState(gOriginalContextState);
}

DECL_FUNCTION(void, GX2CopyColorBufferToScanBuffer, const GX2ColorBuffer *colorBuffer, GX2ScanTarget scan_target) {
    gDrawReady = true;
    if (drawScreenshotSavedTexture(colorBuffer, scan_target)) {
        // if it returns true we don't need to call GX2CopyColorBufferToScanBuffer
        return;
    }

    real_GX2CopyColorBufferToScanBuffer(colorBuffer, scan_target);
}

bool drawScreenshotSavedTexture(const GX2ColorBuffer *colorBuffer, GX2ScanTarget scan_target)
{
    GX2ColorBuffer cb = *colorBuffer;
    drawIntoColorBuffer(&cb, scan_target);

    real_GX2CopyColorBufferToScanBuffer(&cb, scan_target);

    return true;
}

DECL_FUNCTION(void, GX2Init, uint32_t attributes)
{
    real_GX2Init(attributes);
    if (!gOverlayInitDone) {
        // has been allocated in INITIALIZE_PLUGIN
        if (!gContextState) {
            OSFatal("Failed to alloc gContextState");
        }
        real_GX2SetupContextStateEx(gContextState, GX2_TRUE);
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU, gContextState, sizeof(GX2ContextState));
        
        gOverlayInitDone = true;
    }
}

DECL_FUNCTION(void, GX2MarkScanBufferCopied, GX2ScanTarget scan_target)
{
    gDrawReady = true;
    if (scan_target == GX2_SCAN_TARGET_TV) {
        drawIntoColorBuffer(&lastTVColorBuffer, scan_target);
    } else {
        drawIntoColorBuffer(&lastDRCColorBuffer, scan_target);
    }

    real_GX2MarkScanBufferCopied(scan_target);
}

DECL_FUNCTION(void, GX2SwapScanBuffers, void) {
    real_GX2SwapScanBuffers();
}

DECL_FUNCTION(int32_t, VPADRead, VPADChan chan, VPADStatus* buffers, uint32_t count, VPADReadError* error)
{
    VPADReadError real_error;
    int32_t result = real_VPADRead(chan, buffers, count, &real_error);

    if (result > 0 && real_error == VPAD_READ_SUCCESS) {
        if (ImGui_ImplWiiU_ProcessVPADInput(&buffers[0])) {
            // If we're here ImGui has captured the touch and we clear it for all other applications
            for (uint32_t i = 0; i < count; i++) {
                buffers[i].tpNormal.touched = 0;
                buffers[i].tpFiltered1.touched = 0;
                buffers[i].tpFiltered2.touched = 0;
            }
        }
    }

    if (error) {
        *error = real_error;
    }
    return result;
}

WUPS_MUST_REPLACE(GX2GetCurrentScanBuffer,        WUPS_LOADER_LIBRARY_GX2, GX2GetCurrentScanBuffer);
WUPS_MUST_REPLACE(GX2SetContextState,             WUPS_LOADER_LIBRARY_GX2, GX2SetContextState);
WUPS_MUST_REPLACE(GX2SetupContextStateEx,         WUPS_LOADER_LIBRARY_GX2, GX2SetupContextStateEx);
WUPS_MUST_REPLACE(GX2SetTVBuffer,                 WUPS_LOADER_LIBRARY_GX2, GX2SetTVBuffer);
WUPS_MUST_REPLACE(GX2SetDRCBuffer,                WUPS_LOADER_LIBRARY_GX2, GX2SetDRCBuffer);
WUPS_MUST_REPLACE(GX2CopyColorBufferToScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyColorBufferToScanBuffer);
WUPS_MUST_REPLACE(GX2Init,                        WUPS_LOADER_LIBRARY_GX2, GX2Init);
WUPS_MUST_REPLACE(GX2MarkScanBufferCopied,        WUPS_LOADER_LIBRARY_GX2, GX2MarkScanBufferCopied);
WUPS_MUST_REPLACE(GX2SwapScanBuffers,             WUPS_LOADER_LIBRARY_GX2, GX2SwapScanBuffers);

WUPS_MUST_REPLACE(VPADRead, WUPS_LOADER_LIBRARY_VPAD, VPADRead);
