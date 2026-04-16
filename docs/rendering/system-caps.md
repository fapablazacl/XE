# System Capabilities — `TierLevel`, `BackendHint`, `RenderSurface`, `SystemCaps`

Describes how the renderer classifies the hardware it is running on and how it gets told what surface to draw into. `SystemCaps` is populated in two phases (host-first, GPU-after-device-creation) so the client can make informed backend and asset decisions before any graphics API is initialized.

---

## `TierLevel`

```cpp
enum class TierLevel : uint8_t {
    Microcode = 0, // N64 RDP/RSP — combiner equation, no shaders, no HW T&L
    TEV       = 1, // GC/Wii GX  — HW T&L, up to 16 TEV stages, no GLSL
    Legacy    = 2, // GL 1.x–2.x — fixed-function matrix stacks, per-vertex lighting
    Modern    = 3, // GL 3.3+ / GLES 3 / D3D11 / Metal — programmable shaders
    Extended  = 4, // GL 4.x / Vulkan / Metal 3 — compute, geometry, tessellation
};
```

---

## `BackendHint`

```cpp
enum class BackendHint : uint8_t {
    Auto,
    // Desktop
    GLLegacy, GL4, D3D11, Vulkan,
    // Mobile
    GLES2, GLES3, Metal,
    // Consoles
    N64_RDP, GCN_GX, PS3_GCM,
    // Software
    SoftBuiltin, SoftMesa, Null,
    _Count
};
```

---

## `RenderSurface`

```cpp
enum class SurfaceType : uint8_t {
    Win32, X11, Wayland, Cocoa, UIKit, ANativeWindow,
    N64, GameCube, Wii, PS3, Offscreen,
};

struct RenderSurface {
    SurfaceType type;
    void*    native_window;      // HWND / X11 Window / wl_surface* / NSView* / UIView* / ANativeWindow*
    void*    native_display;     // HDC / X11 Display* / wl_display* / nullptr on Apple/Android
    void*    metal_layer;        // CAMetalLayer* — required for Metal backend
    void*    console_context;    // gcmContextData* (PS3); nullptr on N64/GC/Wii
    uint32_t console_buffer_idx;
    uint32_t width, height;
    PixelFormat color_format   = PixelFormat::RGBA8;
    PixelFormat depth_format   = PixelFormat::Depth24Stencil8;
    uint8_t     msaa_samples   = 1;
    bool        srgb           = false;
    bool        hdr            = false;
    uint8_t     swap_interval  = 1;
};
```

---

## `SystemCaps`

```cpp
struct CPUCaps {
    char     brand[64];
    uint32_t physical_cores, logical_cores, cache_line_bytes;
    uint64_t l1_cache_bytes, l2_cache_bytes, l3_cache_bytes;
    // x86
    bool has_sse2, has_sse4, has_avx, has_avx2;
    // ARM
    bool has_neon, has_sve, is_apple_silicon;
    // PowerPC (GC, Wii, PS3 PPU)
    bool has_altivec, is_powerpc;
    // Common
    bool is_bigendian; // N64, GC, Wii, PS3 PPU — critical for texture swizzle
    bool is_64bit, is_arm;
    // Auxiliary processors
    uint8_t  spu_count;              // PS3: usable SPEs (typically 6)
    uint32_t spu_local_store_bytes;  // PS3: 256 KB per SPU
    uint32_t rsp_dmem_bytes;         // N64: 4 KB
    uint32_t rsp_imem_bytes;         // N64: 4 KB
};

struct GPUCaps {
    char      renderer[128], vendor[64], api_version[32];
    TierLevel tier;
    // Memory
    uint64_t  vram_bytes;     // 0 on UMA / console shared memory
    uint64_t  aram_bytes;     // GC/Wii auxiliary RAM (~16 MB)
    uint64_t  tmem_bytes;     // N64 TMEM (4 KB) — first-class budget
    bool      is_uma;
    // Limits
    uint32_t max_texture_size, max_texture_units;
    uint32_t max_uniform_bindings, max_vertex_attribs, max_msaa_samples;
    uint8_t  max_tev_stages;  // GX: 16; 0 on other backends
    uint8_t  max_hw_lights;   // GX / GL Legacy: 8; 0 on Modern+
    // Feature flags
    bool has_geometry_shaders, has_tessellation, has_compute;
    bool has_instancing, has_vao, has_fbo, has_anisotropic;
    // Texture compression
    bool has_s3tc, has_etc2, has_astc, has_pvrtc, has_cmpr;
    bool has_tile_memory, has_float_textures, has_depth_texture, supports_npot;
    bool is_bigendian_gpu;    // RDP, GX, RSX (PS3) — affects texture upload swizzle
    bool uses_display_list;   // N64_RDP, GCN_GX, PS3_GCM
};

struct MemoryCaps {
    uint64_t system_ram_bytes, available_ram_bytes, page_size_bytes;
    bool     has_mmu;            // false on N64 (libdragon bare-metal)
    bool     has_large_pages, is_low_memory_device;
    uint64_t display_list_pool_bytes; // GX / rdpq DL budget
    uint64_t rsx_local_bytes;    // PS3 RSX GDDR3 (256 MB)
    uint64_t rsx_main_bytes;     // PS3 RSX XDR window (up to 256 MB)
};

struct PlatformCaps {
    char  os_name[64], os_version[32];
    bool  is_64bit_os, is_mobile, is_tablet, is_console, is_low_power_mode;
    bool  has_os;          // false on bare N64 (libdragon, no OS)
    bool  has_filesystem;
    float display_scale;
};

struct SystemCaps {
    CPUCaps cpu; GPUCaps gpu; MemoryCaps memory; PlatformCaps platform;

    static SystemCaps query_host(const XeServices& svc) noexcept; // Phase 1: pre-context
    void              query_gpu (const XeServices& svc) noexcept; // Phase 2: post-create

    bool supports_tier        (TierLevel t)       const noexcept;
    bool is_extension_available(const char* name) const noexcept;
};
```

---

## See Also

- [services.md](services.md) — `XeServices` is passed to both `query_host` and `query_gpu`
- [resources.md](resources.md) — `BackendRegistry` consults `SystemCaps` when choosing `best_available()`
- [platform-support.md](platform-support.md) — which `BackendHint` values are available on which platform
- [threading-contract.md](threading-contract.md) — `SystemCaps::query_host` is safe from any thread
