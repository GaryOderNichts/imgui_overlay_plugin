#include "gfx_shader_mappedmem.h"
#include <gfd.h>
#include <gx2r/buffer.h>
#include <gx2/mem.h>
#include <gx2/shaders.h>
#include <gx2/utils.h>
#include <string.h>
#include <whb/gfx.h>
#include <whb/log.h>

#include <memory/mappedmemory.h>

GX2PixelShader *
WHBGfxLoadGFDPixelShaderMappedMem(uint32_t index,
                                  const void *file)
{
   uint32_t headerSize, programSize;
   GX2PixelShader *shader = NULL;
   void *program = NULL;

   if (index >= GFDGetPixelShaderCount(file)) {
      WHBLogPrintf("%s: index %u >= %u GFDGetPixelShaderCount(file)",
                   __FUNCTION__,
                   index,
                   GFDGetPixelShaderCount(file));
      goto error;
   }

   headerSize = GFDGetPixelShaderHeaderSize(index, file);
   if (!headerSize) {
      WHBLogPrintf("%s: headerSize == 0", __FUNCTION__);
      goto error;
   }

   programSize = GFDGetPixelShaderProgramSize(index, file);
   if (!programSize) {
      WHBLogPrintf("%s: programSize == 0", __FUNCTION__);
      goto error;
   }

   shader = (GX2PixelShader *)MEMAllocFromMappedMemoryEx(headerSize, 64);
   if (!shader) {
      WHBLogPrintf("%s: MEMAllocFromMappedMemoryEx(%u, 64) failed", __FUNCTION__,
                   headerSize);
      goto error;
   }

   program = MEMAllocFromMappedMemoryForGX2Ex(programSize, GX2_SHADER_PROGRAM_ALIGNMENT);
   if (!program) {
      WHBLogPrintf("%s: MEMAllocFromMappedMemoryForGX2Ex failed", __FUNCTION__);
      goto error;
   }

   if (!GFDGetPixelShader(shader, program, index, file)) {
      WHBLogPrintf("%s: GFDGetPixelShader failed", __FUNCTION__);
      GX2RUnlockBufferEx(&shader->gx2rBuffer,
                         GX2R_RESOURCE_DISABLE_CPU_INVALIDATE |
                         GX2R_RESOURCE_DISABLE_GPU_INVALIDATE);
      goto error;
   }

   shader->program = program;
   shader->size = programSize;

   GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, shader->program, shader->size);
   return shader;

error:
   if (shader) {
      if (shader->program) {
         MEMFreeToMappedMemory(shader->program);
      }

      MEMFreeToMappedMemory(shader);
   }

   return NULL;
}

BOOL
WHBGfxFreePixelShaderMappedMem(GX2PixelShader *shader)
{
   if (shader->program) {
      MEMFreeToMappedMemory(shader->program);
   }

   MEMFreeToMappedMemory(shader);
   return TRUE;
}

GX2VertexShader *
WHBGfxLoadGFDVertexShaderMappedMem(uint32_t index,
                                   const void *file)
{
   uint32_t headerSize, programSize;
   GX2VertexShader *shader = NULL;
   void *program = NULL;

   if (index >= GFDGetVertexShaderCount(file)) {
      WHBLogPrintf("%s: index %u >= %u GFDGetVertexShaderCount(file)",
                   __FUNCTION__,
                   index,
                   GFDGetVertexShaderCount(file));
      goto error;
   }

   headerSize = GFDGetVertexShaderHeaderSize(index, file);
   if (!headerSize) {
      WHBLogPrintf("%s: headerSize == 0", __FUNCTION__);
      goto error;
   }

   programSize = GFDGetVertexShaderProgramSize(index, file);
   if (!programSize) {
      WHBLogPrintf("%s: programSize == 0", __FUNCTION__);
      goto error;
   }

   shader = (GX2VertexShader *)MEMAllocFromMappedMemoryEx(headerSize, 64);
   if (!shader) {
      WHBLogPrintf("%s: MEMAllocFromMappedMemoryEx(%u, 64) failed", __FUNCTION__,
                   headerSize);
      goto error;
   }

   program = MEMAllocFromMappedMemoryForGX2Ex(programSize, GX2_SHADER_PROGRAM_ALIGNMENT);
   if (!program) {
      WHBLogPrintf("%s: MEMAllocFromMappedMemoryForGX2Ex failed", __FUNCTION__);
      goto error;
   }

   if (!GFDGetVertexShader(shader, program, index, file)) {
      WHBLogPrintf("%s: GFDGetVertexShader failed", __FUNCTION__);
      GX2RUnlockBufferEx(&shader->gx2rBuffer,
                         GX2R_RESOURCE_DISABLE_CPU_INVALIDATE |
                         GX2R_RESOURCE_DISABLE_GPU_INVALIDATE);
      goto error;
   }

   shader->program = program;
   shader->size = programSize;

   GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, shader->program, shader->size);
   return shader;

error:
   if (shader) {
      if (shader->program) {
         MEMFreeToMappedMemory(shader->program);
      }

      MEMFreeToMappedMemory(shader);
   }

   return NULL;
}

BOOL
WHBGfxFreeVertexShaderMappedMem(GX2VertexShader *shader)
{
   if (shader->program) {
      MEMFreeToMappedMemory(shader->program);
   }

   MEMFreeToMappedMemory(shader);
   return TRUE;
}

BOOL
WHBGfxLoadGFDShaderGroupMappedMem(WHBGfxShaderGroup *group,
                                  uint32_t index,
                                  const void *file)
{
   memset(group, 0, sizeof(WHBGfxShaderGroup));
   group->vertexShader = WHBGfxLoadGFDVertexShaderMappedMem(index, file);
   group->pixelShader = WHBGfxLoadGFDPixelShaderMappedMem(index, file);

   if (!group->vertexShader || !group->pixelShader) {
      // A shader group requires at least a vertex shader and a pixel shader.
      WHBGfxFreeShaderGroupMappedMem(group);
      return FALSE;
   }

   return TRUE;
}

BOOL
WHBGfxInitFetchShaderMappedMem(WHBGfxShaderGroup *group)
{
   uint32_t size = GX2CalcFetchShaderSizeEx(group->numAttributes,
                                            GX2_FETCH_SHADER_TESSELLATION_NONE,
                                            GX2_TESSELLATION_MODE_DISCRETE);
   group->fetchShaderProgram = MEMAllocFromMappedMemoryForGX2Ex(size, GX2_SHADER_PROGRAM_ALIGNMENT);

   GX2InitFetchShaderEx(&group->fetchShader,
                        group->fetchShaderProgram,
                        group->numAttributes,
                        group->attributes,
                        GX2_FETCH_SHADER_TESSELLATION_NONE,
                        GX2_TESSELLATION_MODE_DISCRETE);

   GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, group->fetchShaderProgram, size);
   return TRUE;
}

BOOL
WHBGfxFreeShaderGroupMappedMem(WHBGfxShaderGroup *group)
{
   if (group->fetchShaderProgram) {
      MEMFreeToMappedMemory(group->fetchShaderProgram);
      group->fetchShaderProgram = NULL;
   }

   if (group->pixelShader) {
      WHBGfxFreePixelShaderMappedMem(group->pixelShader);
      group->pixelShader = NULL;
   }

   if (group->vertexShader) {
      WHBGfxFreeVertexShaderMappedMem(group->vertexShader);
      group->vertexShader = NULL;
   }

   return TRUE;
}
