
# XE

## Building with Docker
- Clone a separate repository for building with Docker.

## Quick thoughts
- Barebones for a future multiplatform game engine.
- Contains code for libraries, tools, and simple game and demos

## Vulkan 
- Needs VulkanSDK to be downloaded manually, because the Hunter package manager provided an old version that didn't worked with the current implementation

## GLTF loader 

Implement 3d model gltf loading for testing the renderer methods

- Don't allocate any memory for the loader (like the current implementation)
- Return a 3d model structure that allows for fast rendering
- Consider that 3d model can contains sub objects that can form a hierarchy
