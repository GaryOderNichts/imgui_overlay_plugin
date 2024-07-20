#pragma once
#include <whb/gfx.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL
WHBGfxLoadGFDShaderGroupMappedMem(WHBGfxShaderGroup *group,
                                  uint32_t index,
                                  const void *file);

BOOL
WHBGfxInitFetchShaderMappedMem(WHBGfxShaderGroup *group);

BOOL
WHBGfxFreeShaderGroupMappedMem(WHBGfxShaderGroup *group);

#ifdef __cplusplus
}
#endif
