#include "main.h"
#include "overlay.h"

#include <wups.h>
#include <memory/mappedmemory.h>

#include <gx2/context.h>
#include <gx2/enum.h>

WUPS_PLUGIN_NAME("imgui_overlay_plugin");
WUPS_PLUGIN_DESCRIPTION("imgui_overlay_plugin");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("GaryOderNichts");
WUPS_PLUGIN_LICENSE("MIT");

GX2ContextState* gContextState = nullptr;
bool gOverlayInitDone = false;
bool gDrawReady = false;

INITIALIZE_PLUGIN()
{
    gOverlayInitDone = false;

    gContextState = (GX2ContextState *) MEMAllocFromMappedMemoryForGX2Ex(
            sizeof(GX2ContextState),
            GX2_CONTEXT_STATE_ALIGNMENT);
    if (gContextState == nullptr) {
        OSFatal("Failed to allocate gContextState");
    }

    Overlay_Initialize();
}

ON_APPLICATION_START() {
    gDrawReady = false;
}

ON_APPLICATION_ENDS() {

}
