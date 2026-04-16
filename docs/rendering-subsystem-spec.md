# XE Engine — Rendering Middleware Specification
**Revision:** 0.4 — Static Dispatch, Native Handle Access, Sampler Resource, Asset Pipeline  
**Project:** XenoIDE / XE Engine  
**Scope:** Core rendering middleware; public API surface, class design, portability strategy

---

## Revision History

| Rev | Summary |
|---|---|
| **0.1** | Initial skeleton: `Pipeline`, `CommandBuffer`, `RenderDevice`, `SystemCaps`, `BackendVTable` |
| **0.2** | Multi-backend per platform, `XeServices` (allocator/log/profiler), mobile (Android/iOS), software rasterizers, C++ subset rules |
| **0.3** | Formalized threading contract; `TierLevel` extended with `Microcode` and `TEV`; console backends: N64 (libdragon), GameCube/Wii (libogc GX), PS3 (PSL1GHT/libGCM); `PipelineDesc` union extended; `RenderSurface` + `SystemCaps` console fields; additional target notes (Dreamcast, Xbox OG) |
| **0.4** | `RenderDeviceT<Policy>` + `CommandBufferT<Policy>` static dispatch; `xe_platform_config.hpp` alias pattern; `BackendVTable` native handle + extension query slots; per-backend extension headers; `SamplerHandle` / `SamplerDesc` first-class resource; GL Legacy display list optimization for sampler and pipeline state; asset pipeline architecture: `xe-asset-compiler`, `.xemesh`, `.xetex`, `.xetexprop`; `submit_batch()` added to `RenderDevice` class definition; PS3 GCM FIFO size configuration |

---

## 1. Goals and Constraints

| Goal | Notes |
|---|---|
| **Multi-tier hardware** | Microcode (N64) → TEV (GC/Wii) → Legacy (GL1.x–2.x) → Modern (GL4/D3D11/Metal) → Extended (Vulkan/Compute) |
| **Multi-backend per platform** | User-selectable to work around driver bugs; enumerated by `BackendRegistry` |
| **API-agnostic public surface** | No GL/GX/GCM/D3D types leak beyond backend translation units |
| **Custom allocators throughout** | Critical for console targets with no OS heap; every subsystem uses `XeAllocator` |
| **Injectable diagnostics** | `XeServices`: logging, profiling, assertion, memory tracking |
| **Zero-thread renderer** | No internal threads; client owns all parallelism via `XeJob` (separate library) |
| **Static or dynamic dispatch** | `RenderDeviceT<Policy>` collapses to direct calls on single-backend targets; dynamic vtable on desktop |
| **Native handle escape hatch** | `get_native_handle` + `query_extension` for out-of-source extensibility and native optimizations |
| **Mobile: Android + iOS** | GLES2, GLES3, Vulkan (Android); Metal, GLES2/3 (iOS) |
| **Consoles** | N64, GameCube, Wii, PS3 via open homebrew SDKs (libdragon, libogc, PSL1GHT) |
| **Software fallback** | Built-in tile rasterizer (`SoftBuiltin`) + Mesa3D reference (`SoftMesa`) |
| **Maximal portability** | Win98–Win11, Linux, macOS, Android, iOS, N64, GC/Wii, PS3; 32- and 64-bit |
| **C++ subset, no virtual dispatch** | Templates, `constexpr`, fn-pointer dispatch; no exceptions, RTTI, `std::function` |
| **Glaze for GL backends** | GL Legacy and GL4 backends use the existing `Glaze` C++ wrapper internally |
| **Offline asset pipeline** | `xe-asset-compiler` produces platform-native `.xemesh` / `.xetex` binaries; runtime loader is a thin DMA-copy layer |

---

## 2. Permitted C++ Subset

### Allowed
- `template` functions and structs
- `constexpr` / `consteval`
- `static_assert`, `[[nodiscard]]`, `[[maybe_unused]]`, `[[likely]]` / `[[unlikely]]`
- Aggregate initialization, designated initializers (C++20; C++17 fallback)
- `inline` forwarding functions in headers
- `noexcept` on all public functions
- Move semantics for `CommandBufferT` and `Pipeline` value types
- `std::type_traits` for `static_assert` / SFINAE (no runtime use)

### Forbidden
- `virtual` functions — replaced by `BackendVTable` fn-pointer dispatch
- `dynamic_cast`, `typeid`, RTTI of any kind
- Exceptions (`throw`, `try`, `catch`)
- `std::function`
- `std::shared_ptr` / `std::unique_ptr` owning GPU resources
- STL containers in public headers
- Multiple inheritance
- Global constructors with side effects

---

## 3. Threading Contract

### Renderer Has Zero Internal Threads

The renderer creates no threads, owns no mutexes, and makes no use of atomics in the
fast path. All parallelism is the client's responsibility. `XeThread` / `XeJob` is a
**separate utility library** sitting above the renderer in the dependency graph. The
renderer does not link against it.

### Thread Safety Contract

| Operation | Guarantee |
|---|---|
| `CommandBufferT` recording (`draw*`, `bind_*`, `set_*`) | **Safe from any thread** — no shared state |
| `RenderDevice` resource creation (`create_buffer`, etc.) | **Render thread only** |
| `RenderDevice::submit()` / `submit_batch()` | **Render thread only** |
| `RenderDevice::begin_frame()` / `end_frame()` / `present()` | **Render thread only** |
| `BackendRegistry::probe()` | **One-shot, pre-device** |
| `SystemCaps::query_host()` | **Safe from any thread** — read-only after return |

### Multithreaded Recording Pattern

```
[Worker thread A]  CommandBuffer cmdbuf_a(arena_a);
                   cmdbuf_a.bind_pipeline(pl);
                   cmdbuf_a.draw_indexed(n);

[Worker thread B]  CommandBuffer cmdbuf_b(arena_b);
                   cmdbuf_b.bind_pipeline(pl2);
                   cmdbuf_b.draw_indexed(m);

[Render thread]    const CommandBuffer* batch[] = { &cmdbuf_a, &cmdbuf_b };
                   device->submit_batch(batch, 2);   // ordered by array index
                   device->end_frame();
                   device->present();
```

### Deferred Resource Creation Pattern

`create_*` is render-thread-only. Worker threads enqueue creation requests into a
lock-free ring buffer; the render thread flushes them at `begin_frame()` before any
`CommandBuffer` records against the new handles. This is a recommended client idiom,
not part of the renderer itself.

---

## 4. `TierLevel`

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

## 5. `BackendHint`

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

## 6. `RenderSurface`

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

## 7. `SystemCaps`

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

## 8. `XeServices` — Facility Injection Hub

```cpp
enum class XeLogLevel : uint8_t { Trace, Debug, Info, Warn, Error, Fatal };

struct XeLogSink {
    void (*write)(void* user, XeLogLevel level, const char* tag,
                  const char* msg, const char* file, int line) noexcept;
    void*      user_data;
    XeLogLevel min_level = XeLogLevel::Info;
};

struct XeProfilerHook {
    void (*begin_cpu_scope)(void* user, const char* name, uint32_t argb_color) noexcept;
    void (*end_cpu_scope)  (void* user)                                         noexcept;
    void (*mark)           (void* user, const char* name)                       noexcept;
    void (*gpu_timestamp)  (void* user, const char* name, uint64_t ns)          noexcept;
    void (*counter)        (void* user, const char* name, int64_t value)        noexcept;
    void* user_data;
};

struct XeAllocator {
    void* (*alloc)  (void* user, size_t size, size_t align)                          noexcept;
    void  (*free)   (void* user, void* ptr, size_t size, size_t align)               noexcept;
    void* (*realloc)(void* user, void* ptr, size_t old_sz, size_t new_sz, size_t align) noexcept;
    void* user_data;
    static XeAllocator make_default()                              noexcept;
    static XeAllocator make_tracking(XeAllocator base, XeAllocTracker*) noexcept;
};

struct XeAllocTracker {
    void (*on_alloc)(void* user, void* ptr, size_t size, size_t align,
                     const char* tag, const char* file, int line) noexcept;
    void (*on_free) (void* user, void* ptr, size_t size)          noexcept;
    void* user_data;
};

struct XeAssertHandler {
    bool (*on_assert)(void* user, const char* expr, const char* msg,
                      const char* file, int line) noexcept; // false → abort()
    void* user_data;
};

struct XeServices {
    XeAllocator     allocator;
    XeLogSink       log;
    XeProfilerHook  profiler;
    XeAllocTracker* alloc_tracker = nullptr;
    XeAssertHandler assert_handler;
    static XeServices make_default() noexcept;
};

// Internal-use macros (implementation files only)
// XE_LOG(svc, level, tag, fmt, ...)
// XE_TRACE_SCOPE(svc, name, color)
// XE_ASSERT(svc, expr, msg)
// XE_ALLOC(svc, size, align)
// XE_FREE(svc, ptr, size, align)
```

---

## 9. `BackendVTable` and `BackendContext`

### 9.1 Factory Signatures

```c
/* C ABI — each backend translation unit exports one of these */
XE_API BackendVTable xe_backend_gl_legacy_create  (const XeServices*, const RenderSurface*, BackendContext**);
XE_API BackendVTable xe_backend_gl4_create         (const XeServices*, const RenderSurface*, BackendContext**);
XE_API BackendVTable xe_backend_gles2_create       (const XeServices*, const RenderSurface*, BackendContext**);
XE_API BackendVTable xe_backend_gles3_create       (const XeServices*, const RenderSurface*, BackendContext**);
XE_API BackendVTable xe_backend_d3d11_create       (const XeServices*, const RenderSurface*, BackendContext**);
XE_API BackendVTable xe_backend_metal_create       (const XeServices*, const RenderSurface*, BackendContext**);
XE_API BackendVTable xe_backend_n64_rdp_create     (const XeServices*, const RenderSurface*, BackendContext**);
XE_API BackendVTable xe_backend_gcn_gx_create      (const XeServices*, const RenderSurface*, BackendContext**);
XE_API BackendVTable xe_backend_ps3_gcm_create     (const XeServices*, const RenderSurface*, BackendContext**, uint32_t gcm_fifo_kb);
XE_API BackendVTable xe_backend_soft_builtin_create(const XeServices*, const RenderSurface*, BackendContext**);
XE_API BackendVTable xe_backend_soft_mesa_create   (const XeServices*, const RenderSurface*, BackendContext**, const char* mesa_lib_path);
XE_API BackendVTable xe_backend_null_create        (const XeServices*, const RenderSurface*, BackendContext**);
```

### 9.2 `BackendVTable`

```c
typedef struct BackendContext BackendContext; /* opaque */

typedef struct BackendVTable {
    /* ── Lifecycle ─────────────────────────────────────────── */
    void     (*shutdown)     (BackendContext*);
    void     (*begin_frame)  (BackendContext*);
    void     (*end_frame)    (BackendContext*);
    void     (*present)      (BackendContext*);

    /* ── Buffer management ──────────────────────────────────── */
    uint32_t (*buffer_create)  (BackendContext*, const BufferDesc*);
    void     (*buffer_update)  (BackendContext*, uint32_t, const void*, uint32_t, uint32_t);
    void     (*buffer_destroy) (BackendContext*, uint32_t);

    /* ── Texture management ─────────────────────────────────── */
    uint32_t (*texture_create)        (BackendContext*, const TextureDesc*);
    void     (*texture_update)        (BackendContext*, uint32_t, const TextureUpdate*);
    void     (*texture_destroy)       (BackendContext*, uint32_t);
    uint32_t (*rendertarget_create)   (BackendContext*, const RenderTargetDesc*);
    void     (*rendertarget_destroy)  (BackendContext*, uint32_t);

    /* ── Sampler management ─────────────────────────────────── */
    uint32_t (*sampler_create)  (BackendContext*, const SamplerDesc*);
    void     (*sampler_destroy) (BackendContext*, uint32_t);

    /* ── Shader management (Modern / Extended only) ─────────── */
    uint32_t (*shader_create)   (BackendContext*, const ShaderDesc*);
    void     (*shader_destroy)  (BackendContext*, uint32_t);

    /* ── Pipeline management ────────────────────────────────── */
    uint32_t (*pipeline_create) (BackendContext*, const PipelineDesc*);
    void     (*pipeline_destroy)(BackendContext*, uint32_t);

    /* ── Command submission ─────────────────────────────────── */
    void (*submit_batch)(BackendContext*, const CommandBufferBase* const*, uint32_t count);

    /* ── Synchronisation ────────────────────────────────────── */
    void (*flush)  (BackendContext*);
    void (*finish) (BackendContext*);

    /* ── Native handle access (new in 0.4) ──────────────────── */
    // Returns the underlying API object for a given handle.
    // Result type is backend-specific; client casts via the extension header.
    // Returns nullptr if unsupported (Null, SoftBuiltin, etc.)
    void* (*get_native_handle)(BackendContext*, XeHandleType, uint32_t handle);

    // Extension struct negotiation.
    // `extension_id` is a backend-specific constant (e.g. GL4DeviceExt::kID).
    // Returns a pointer to a backend-owned struct, or nullptr if unsupported.
    void* (*query_extension)(BackendContext*, uint32_t extension_id);

    /* ── Debug ──────────────────────────────────────────────── */
    void        (*set_debug_label) (BackendContext*, XeHandleType, uint32_t, const char*);
    const char* (*get_error_string)(BackendContext*);

} BackendVTable;
```

**Unsupported slot contract:** Slots that a backend does not support are set to a
stub that logs an error via `XeServices.log` and returns `XE_INVALID_HANDLE` or
`nullptr`. This is validated once at device creation — the application gates feature
use on `SystemCaps`.

**ABI stability rule:** The `BackendVTable` struct may only be extended at the end.
Existing slots must never be reordered or removed.

### 9.3 GL Legacy — Display List Optimization for Sampler and Pipeline State

The GL Legacy backend compiles sampler and pipeline state into GL display lists at
creation time. This trades one-time compile cost for minimal per-bind overhead —
important on constrained Legacy hardware where individual GL state calls carry
significant per-call overhead.

#### Sampler Display List

```cpp
// Inside GL1BackendContext
struct GL1SamplerEntry {
    GLuint  display_list;    // compiled glTexParameter* sequence
    // State mirror for dirty checking
    GLenum  wrap_s, wrap_t, min_filter, mag_filter;
    GLfloat aniso;
};

uint32_t gl1_sampler_create(BackendContext* ctx, const SamplerDesc* desc) {
    auto* c = static_cast<GL1BackendContext*>(ctx);
    uint32_t idx = c->sampler_pool.alloc();
    GL1SamplerEntry& e = c->samplers[idx];

    e.wrap_s     = xe_wrap_to_gl(desc->wrap_u);
    e.wrap_t     = xe_wrap_to_gl(desc->wrap_v);
    e.min_filter = xe_filter_min_to_gl(desc->filter_min);
    e.mag_filter = xe_filter_mag_to_gl(desc->filter_mag);
    e.aniso      = (float)desc->aniso_level;

    e.display_list = glGenLists(1);
    glNewList(e.display_list, GL_COMPILE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     e.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     e.wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, e.min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, e.mag_filter);
        if (c->caps.has_anisotropic && desc->aniso_level > 1)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, e.aniso);
    glEndList();

    return xe_make_handle(XeHandleType::Sampler, idx);
}
```

Binding a texture + sampler then becomes:

```cpp
void gl1_bind_texture(BackendContext* ctx, uint32_t tex_h,
                      uint32_t smp_h, uint8_t unit) {
    auto* c = static_cast<GL1BackendContext*>(ctx);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, c->textures[tex_h & HANDLE_INDEX_MASK]);
    glCallList(c->samplers[smp_h & HANDLE_INDEX_MASK].display_list);
    // One glCallList replaces 4–5 glTexParameter* calls
}
```

#### Pipeline State Display List

The rasterizer, blend, and depth/stencil state — along with fixed-function
lighting/fog/alpha-test setup — are compiled into a pipeline display list at
`create_pipeline()` time:

```cpp
// Compiled once; called once per bind_pipeline()
glNewList(entry.state_list, GL_COMPILE);
    // Rasterizer
    (rast.cull_mode == CullMode::None)
        ? glDisable(GL_CULL_FACE)
        : (glEnable(GL_CULL_FACE), glCullFace(xe_cull_to_gl(rast.cull_mode)));
    glPolygonMode(GL_FRONT_AND_BACK, xe_fill_to_gl(rast.fill_mode));
    // Depth
    ds.depth_test
        ? (glEnable(GL_DEPTH_TEST), glDepthFunc(xe_compare_to_gl(ds.depth_func)))
        : glDisable(GL_DEPTH_TEST);
    glDepthMask(ds.depth_write ? GL_TRUE : GL_FALSE);
    // Blend
    blend.enabled
        ? (glEnable(GL_BLEND),
           glBlendFuncSeparate(xe_blend_to_gl(blend.src_color),
                               xe_blend_to_gl(blend.dst_color),
                               xe_blend_to_gl(blend.src_alpha),
                               xe_blend_to_gl(blend.dst_alpha)))
        : glDisable(GL_BLEND);
    // Fixed-function (lighting, fog, alpha test, tex env)
    // ... compiled from FixedFunctionState fields
glEndList();
```

**Display list scope rules:** `glBindTexture`, `glVertexPointer`, and `glDrawElements`
capture values at `glNewList` time, not at `glCallList` time. Therefore texture binds
must happen *before* calling the sampler display list. Pipeline and sampler display
lists are orthogonal and correct to call in any order relative to each other.

**Display lists are compatibility-profile only.** This optimization applies exclusively
to the GL Legacy backend (GL 1.x–2.x / compatibility). The GL4 backend uses core
profile and has no access to display lists — the driver's internal state caching
provides equivalent or better optimization there.

#### GL Legacy Display List Usage Summary

| Object | Compiled at | Contains | Called at |
|---|---|---|---|
| Sampler state list | `create_sampler()` | `glTexParameter*` calls | `bind_texture()` after `glBindTexture` |
| Pipeline state list | `create_pipeline()` | rasterizer + blend + depth + fixed-function state | `bind_pipeline()` |
| Static geometry list | Optional, content-driven | `glBegin/glEnd` geometry | Static world draw calls only |

---

## 10. Static Backend Dispatch — `RenderDeviceT<Policy>`

### 10.1 Backend Policy Tags

```cpp
// xe_backend_policy.hpp — no backend-specific headers included here

struct BackendPolicy_Dynamic    {};  // Runtime fn-pointer dispatch (default, all desktop/mobile)
struct BackendPolicy_N64_RDP    {};  // Static: libdragon rdpq direct calls
struct BackendPolicy_GCN_GX     {};  // Static: libogc GX direct calls
struct BackendPolicy_PS3_GCM    {};  // Static: PSL1GHT libGCM direct calls
struct BackendPolicy_GL4        {};  // Static: GL4 Glaze (dev/CI single-backend builds)
struct BackendPolicy_SoftBuiltin{};  // Static: tile rasterizer
```

### 10.2 `DispatchCore<Policy>` — One Pointer Indirection

The C++ template shell that wraps the vtable. All methods are `inline` — with LTO on
static builds they collapse to direct calls with no overhead whatsoever.

```cpp
// xe_dispatch.hpp

template<typename Policy = BackendPolicy_Dynamic>
class DispatchCore {
public:
    DispatchCore(const BackendVTable& vt, BackendContext* ctx) noexcept
        : _vt(vt), _ctx(ctx) {}

    [[nodiscard]] inline BufferHandle
    create_buffer(const BufferDesc& d) noexcept { return _vt.buffer_create(_ctx, &d); }
    inline void destroy_buffer(BufferHandle h) noexcept { _vt.buffer_destroy(_ctx, h); }

    [[nodiscard]] inline SamplerHandle
    create_sampler(const SamplerDesc& d) noexcept { return _vt.sampler_create(_ctx, &d); }
    inline void destroy_sampler(SamplerHandle h) noexcept { _vt.sampler_destroy(_ctx, h); }

    // ... one inline forwarder per BackendVTable slot

    inline void* get_native_handle(XeHandleType t, uint32_t h) const noexcept {
        return _vt.get_native_handle(_ctx, t, h);
    }
    inline void* query_extension(uint32_t id) const noexcept {
        return _vt.query_extension(_ctx, id);
    }
    inline BackendContext* ctx() const noexcept { return _ctx; }

private:
    BackendVTable   _vt;
    BackendContext* _ctx;
};
```

### 10.3 `RenderDeviceT<Policy>`

```cpp
// Primary template — forward-declared; specialized per policy below
template<typename Policy = BackendPolicy_Dynamic>
class RenderDeviceT;

// ── Dynamic specialization (desktop / mobile shipping builds) ─────────────
template<>
class RenderDeviceT<BackendPolicy_Dynamic> {
public:
    static RenderDeviceT* create (const RenderDeviceDesc&) noexcept;
    static void           destroy(RenderDeviceT*)          noexcept;

    // ── Backend introspection ────────────────────────────────────────
    TierLevel         tier()           const noexcept;
    BackendHint       active_backend() const noexcept;
    const SystemCaps& caps()           const noexcept;
    const XeServices& services()       const noexcept;

    // ── Frame lifecycle ──────────────────────────────────────────────
    void begin_frame()                                        noexcept;
    void end_frame()                                          noexcept;
    void present()                                            noexcept;

    // ── Submission ───────────────────────────────────────────────────
    void submit      (CommandBuffer& cmdbuf)                  noexcept;
    void submit_batch(const CommandBuffer* const* bufs,
                      uint32_t count)                         noexcept;

    // ── Buffer resources ─────────────────────────────────────────────
    [[nodiscard]] BufferHandle create_buffer        (const BufferDesc&)         noexcept;
    void                       update_buffer        (BufferHandle, const void*,
                                                     uint32_t, uint32_t offset=0) noexcept;
    void                       destroy_buffer       (BufferHandle)              noexcept;

    // ── Texture resources ────────────────────────────────────────────
    [[nodiscard]] TextureHandle create_texture       (const TextureDesc&)       noexcept;
    void                        update_texture       (TextureHandle,
                                                      const TextureUpdate&)     noexcept;
    void                        destroy_texture      (TextureHandle)            noexcept;
    [[nodiscard]] TextureHandle create_render_target (const RenderTargetDesc&)  noexcept;

    // ── Sampler resources (new in 0.4) ───────────────────────────────
    // On GL Legacy, internally creates a compiled display list.
    // On GL4/D3D11/Metal, maps to a real sampler state object.
    // On console backends without a sampler concept, stores state applied at bind.
    [[nodiscard]] SamplerHandle create_sampler  (const SamplerDesc&)            noexcept;
    void                        destroy_sampler (SamplerHandle)                 noexcept;

    // ── Shader resources ─────────────────────────────────────────────
    [[nodiscard]] ShaderHandle create_shader  (const ShaderDesc&)               noexcept;
    void                       destroy_shader (ShaderHandle)                    noexcept;

    // ── Pipeline resources ───────────────────────────────────────────
    [[nodiscard]] PipelineHandle create_pipeline  (const PipelineDesc&)         noexcept;
    void                         destroy_pipeline (PipelineHandle)              noexcept;

    // ── Native handle access (new in 0.4) ────────────────────────────
    // O(1) array lookup — single pointer dereference into the backend table.
    // Result type is backend-specific; cast via the appropriate extension header.
    void* get_native_handle (XeHandleType type, uint32_t handle) const noexcept;

    // Extension struct negotiation — typed native access beyond single handles.
    void* query_extension   (uint32_t extension_id)              const noexcept;

    // Exposes the opaque backend context for use in extension header helpers.
    BackendContext* backend_ctx() const noexcept;

    // ── Memory stats ─────────────────────────────────────────────────
    void query_memory_usage(RenderMemoryStats& out) const noexcept;

    // ── Debug ────────────────────────────────────────────────────────
    void        set_debug_label(PipelineHandle, const char*) noexcept;
    void        set_debug_label(BufferHandle,   const char*) noexcept;
    void        set_debug_label(TextureHandle,  const char*) noexcept;
    void        set_debug_label(SamplerHandle,  const char*) noexcept;
    const char* last_error()                           const noexcept;

    RenderDeviceT(const RenderDeviceT&) = delete;
    RenderDeviceT& operator=(const RenderDeviceT&) = delete;

private:
    DispatchCore<BackendPolicy_Dynamic> _dispatch;
    SystemCaps  _caps;
    XeServices  _svc;
    BackendHint _backend;
};

// ── N64 static specialization ─────────────────────────────────────────────
// Defined in xe_backend_n64.hpp — only compiled on XE_PLATFORM_N64 builds.
// BackendContext is embedded BY VALUE — no heap allocation, no pointer chase.
template<>
class RenderDeviceT<BackendPolicy_N64_RDP> {
public:
    static RenderDeviceT* create (const RenderDeviceDesc&) noexcept;
    static void           destroy(RenderDeviceT*)          noexcept;

    // Public interface identical to the dynamic specialization.
    // Every call is a direct function call — zero pointer indirection.

    [[nodiscard]] BufferHandle create_buffer(const BufferDesc& d) noexcept {
        return n64_rdp_buffer_create(&_ctx, &d);   // direct call, inlineable
    }
    void submit_batch(const CommandBuffer* const* bufs, uint32_t n) noexcept {
        n64_rdp_submit_batch(&_ctx, bufs, n);       // direct call
    }
    // Native handle: direct array index, no function pointer at all
    void* get_native_handle(XeHandleType t, uint32_t h) const noexcept {
        uint32_t idx = h & HANDLE_INDEX_MASK;
        switch (t) {
            case XeHandleType::Buffer:  return _ctx.rdp_buffers[idx];
            case XeHandleType::Texture: return &_ctx.tex_objs[idx];
            default:                    return nullptr;
        }
    }
    // ... full interface follows same pattern

private:
    N64BackendContext _ctx;   // Embedded by value — no heap, no pointer chase
    SystemCaps        _caps;
    XeServices        _svc;
};

// GCN_GX and PS3_GCM specializations follow the same pattern.
// Defined in xe_backend_gcn.hpp and xe_backend_ps3.hpp respectively.
```

### 10.4 `CommandBufferT<Policy>`

The same policy parameterization applies to `CommandBuffer`. On N64 and GC/Wii the
static specialization encodes directly into the native display list format — the
generic decode pass in `submit()` is eliminated entirely.

```cpp
template<typename Policy = BackendPolicy_Dynamic>
class CommandBufferT;

// Dynamic: generic tagged byte-stream, decoded during submit()
template<>
class CommandBufferT<BackendPolicy_Dynamic> {
public:
    explicit CommandBufferT(XeAllocator alloc = XeAllocator::make_default(),
                            uint32_t initial_capacity = 64 * 1024) noexcept;
    ~CommandBufferT() noexcept;
    void reset() noexcept;

    // ── Render pass ──────────────────────────────────────────────────
    void begin_pass(const PassDesc&) noexcept;
    void end_pass()                  noexcept;

    // ── State binding ────────────────────────────────────────────────
    void bind_pipeline      (PipelineHandle)                           noexcept;
    void bind_vertex_buffer (BufferHandle, uint8_t slot, uint32_t stride,
                             uint32_t offset = 0)                      noexcept;
    void bind_index_buffer  (BufferHandle, IndexType, uint32_t offset=0) noexcept;
    void bind_texture       (TextureHandle, SamplerHandle, uint8_t unit) noexcept;
    void bind_uniform_buffer(BufferHandle, uint8_t binding)            noexcept;

    // ── Viewport / scissor ───────────────────────────────────────────
    void set_viewport(float x, float y, float w, float h,
                      float min_d = 0.f, float max_d = 1.f)            noexcept;
    void set_scissor (int32_t x, int32_t y, uint32_t w, uint32_t h)   noexcept;

    // ── Fixed-function state helpers (Legacy / Microcode / TEV) ─────
    void set_matrix_modelview  (const float m[16]) noexcept;
    void set_matrix_projection (const float m[16]) noexcept;
    void set_light             (uint8_t idx, const LightDesc&) noexcept;
    void set_material          (const MaterialDesc&)           noexcept;
    void set_fog               (const FogDesc&)                noexcept;

    // ── Uniforms (Modern / Extended) ─────────────────────────────────
    void push_constants(const void* data, uint32_t bytes,
                        uint32_t offset = 0)                           noexcept;

    // ── Draw ─────────────────────────────────────────────────────────
    void draw                    (uint32_t vertex_count,
                                  uint32_t first_vertex = 0)           noexcept;
    void draw_indexed            (uint32_t index_count,
                                  uint32_t first_index = 0,
                                  int32_t  vertex_offset = 0)          noexcept;
    void draw_instanced          (uint32_t vertex_count,
                                  uint32_t instance_count,
                                  uint32_t first_vertex = 0,
                                  uint32_t first_instance = 0)         noexcept;
    void draw_indexed_instanced  (uint32_t index_count,
                                  uint32_t instance_count,
                                  uint32_t first_index = 0,
                                  int32_t  vertex_offset = 0,
                                  uint32_t first_instance = 0)         noexcept;

    // ── Inline buffer update (streaming geometry) ────────────────────
    void update_buffer_inline(BufferHandle, const void*, uint32_t bytes,
                              uint32_t offset = 0)                     noexcept;

    // ── GPU profiling scopes ─────────────────────────────────────────
    void begin_gpu_scope (const char* name)                            noexcept;
    void end_gpu_scope   ()                                            noexcept;

    // ── Debug markers ────────────────────────────────────────────────
    void push_debug_group  (const char* label)                         noexcept;
    void pop_debug_group   ()                                          noexcept;
    void insert_debug_marker(const char* label)                        noexcept;

    // ── Inspection ───────────────────────────────────────────────────
    uint32_t    recorded_command_count() const noexcept;
    uint32_t    used_bytes()             const noexcept;
    uint32_t    capacity_bytes()         const noexcept;
    bool        is_empty()               const noexcept;

    CommandBufferT(CommandBufferT&&) noexcept;
    CommandBufferT& operator=(CommandBufferT&&) noexcept;
    CommandBufferT(const CommandBufferT&) = delete;
    CommandBufferT& operator=(const CommandBufferT&) = delete;

private:
    uint8_t*    _data;
    uint32_t    _used, _capacity, _cmd_count;
    XeAllocator _alloc;
};

// N64 static specialization — encodes directly into rdpq display list format.
// submit() is a DMA kickoff, not a decode loop.
template<>
class CommandBufferT<BackendPolicy_N64_RDP> {
public:
    explicit CommandBufferT(XeAllocator alloc, uint32_t capacity) noexcept;

    // Public interface identical to dynamic specialization.
    // Internally: each method writes rdpq_* commands into _dl buffer.

    void bind_pipeline(PipelineHandle h) noexcept {
        // Reads baked combiner config from pipeline table;
        // emits SET_COMBINE_MODE directly into the display list.
        const auto& cfg = _pipeline_table[h & HANDLE_INDEX_MASK];
        rdpq_set_combiner_raw(cfg.combiner_word);
    }
    void draw_indexed(uint32_t count, uint32_t first, int32_t) noexcept {
        rdpq_triangle_strip(_verts + first, count);
    }
    // submit() = rspq_flush() / DMA kickoff — no decode pass
    // On a 93MHz R4300 this is the difference between shipping and not shipping.

private:
    uint64_t* _dl;        // rdpq display list buffer
    uint32_t  _dl_used, _dl_capacity;
    const N64BackendContext* _ctx;  // pipeline table access
};
```

### 10.5 `xe_platform_config.hpp` — The Alias Convention

Templates do not leak upward. A single `using` alias, generated by CMake or set by
a `#define`, makes all client code use `RenderDevice` without knowing the policy:

```cpp
// xe_platform_config.hpp — generated by build system or set manually

#if   defined(XE_PLATFORM_N64)
    using RenderDevice  = RenderDeviceT<BackendPolicy_N64_RDP>;
    using CommandBuffer = CommandBufferT<BackendPolicy_N64_RDP>;
#elif defined(XE_PLATFORM_GAMECUBE) || defined(XE_PLATFORM_WII)
    using RenderDevice  = RenderDeviceT<BackendPolicy_GCN_GX>;
    using CommandBuffer = CommandBufferT<BackendPolicy_GCN_GX>;
#elif defined(XE_PLATFORM_PS3)
    using RenderDevice  = RenderDeviceT<BackendPolicy_PS3_GCM>;
    using CommandBuffer = CommandBufferT<BackendPolicy_PS3_GCM>;
#else
    // Desktop / mobile — dynamic dispatch, all backends available
    using RenderDevice  = RenderDeviceT<BackendPolicy_Dynamic>;
    using CommandBuffer = CommandBufferT<BackendPolicy_Dynamic>;
#endif
```

All downstream code is written against `RenderDevice` and `CommandBuffer` with no
awareness of the policy. On N64, every call is a direct function call. On desktop,
every call is one pointer indirection through the vtable.

---

## 11. Native Handle Access — Extension Headers

Per-backend extension headers provide type-safe access to underlying API objects.
They live in a separate include path (`xe/native/`) and are **never included by the
middleware core itself**. The client explicitly includes the header when it knows
which backend is active.

### 11.1 `xe/native/xe_native_gl4.hpp`

```cpp
// Only include when active_backend() == BackendHint::GL4.
#include <GL/gl.h>
#include "xe_render_device.hpp"

namespace xe::native::gl4 {

struct GL4DeviceExt {
    static constexpr uint32_t kID = 0x474C3400; // "GL4\0"

    GLuint (*get_buffer_name)  (BackendContext*, BufferHandle)   noexcept;
    GLuint (*get_texture_name) (BackendContext*, TextureHandle)  noexcept;
    GLuint (*get_program_id)   (BackendContext*, PipelineHandle) noexcept;
    GLuint (*get_sampler_name) (BackendContext*, SamplerHandle)  noexcept;

    // Escape to raw GL — make the device's context current on the calling thread
    void   (*make_current)     (BackendContext*)                 noexcept;
    void   (*release_current)  (BackendContext*)                 noexcept;

    // Insert a GL fence before and after native GL calls for safe interop
    GLsync (*insert_fence)     (BackendContext*)                 noexcept;
};

// Typed access helpers — return 0 / nullptr if backend is not GL4
inline const GL4DeviceExt* get_ext(RenderDevice* dev) noexcept {
    return static_cast<const GL4DeviceExt*>(dev->query_extension(GL4DeviceExt::kID));
}
inline GLuint buffer_name (RenderDevice* dev, BufferHandle  h) noexcept {
    auto* e = get_ext(dev); return e ? e->get_buffer_name(dev->backend_ctx(), h) : 0;
}
inline GLuint texture_name(RenderDevice* dev, TextureHandle h) noexcept {
    auto* e = get_ext(dev); return e ? e->get_texture_name(dev->backend_ctx(), h) : 0;
}

} // namespace xe::native::gl4
```

### 11.2 `xe/native/xe_native_gl_legacy.hpp`

```cpp
#include <GL/gl.h>
#include "xe_render_device.hpp"

namespace xe::native::gl_legacy {

struct GL1DeviceExt {
    static constexpr uint32_t kID = 0x474C3100; // "GL1\0"

    GLuint (*get_texture_name)    (BackendContext*, TextureHandle)   noexcept;
    // Display list access — for advanced native GL1 optimizations
    GLuint (*get_sampler_list)    (BackendContext*, SamplerHandle)   noexcept;
    GLuint (*get_pipeline_list)   (BackendContext*, PipelineHandle)  noexcept;
    void   (*make_current)        (BackendContext*)                  noexcept;
    void   (*release_current)     (BackendContext*)                  noexcept;
};

inline const GL1DeviceExt* get_ext(RenderDevice* dev) noexcept {
    return static_cast<const GL1DeviceExt*>(dev->query_extension(GL1DeviceExt::kID));
}

} // namespace xe::native::gl_legacy
```

### 11.3 `xe/native/xe_native_gcn_gx.hpp`

```cpp
#include <gccore.h>   // libogc
#include "xe_render_device.hpp"

namespace xe::native::gcn_gx {

struct GXDeviceExt {
    static constexpr uint32_t kID = 0x47585F47; // "GX_G"

    GXTexObj* (*get_tex_obj)  (BackendContext*, TextureHandle)  noexcept;
    void*     (*get_dl_buf)   (BackendContext*, uint32_t* out_size) noexcept; // raw GX DL buffer
};

inline const GXDeviceExt* get_ext(RenderDevice* dev) noexcept {
    return static_cast<const GXDeviceExt*>(dev->query_extension(GXDeviceExt::kID));
}

} // namespace xe::native::gcn_gx
```

### 11.4 `xe/native/xe_native_ps3_gcm.hpp`

```cpp
#include <rsx/rsx.h>       // PSL1GHT
#include <sysutil/video.h>
#include "xe_render_device.hpp"

namespace xe::native::ps3_gcm {

struct PS3GCMDeviceExt {
    static constexpr uint32_t kID = 0x50533347; // "PS3G"

    rsxBuffer*      (*get_rsx_buffer)  (BackendContext*, TextureHandle)  noexcept;
    gcmContextData* (*get_gcm_context) (BackendContext*)                  noexcept;
    uint32_t        (*get_rsx_offset)  (BackendContext*, BufferHandle)   noexcept;
};

inline const PS3GCMDeviceExt* get_ext(RenderDevice* dev) noexcept {
    return static_cast<const PS3GCMDeviceExt*>(dev->query_extension(PS3GCMDeviceExt::kID));
}

} // namespace xe::native::ps3_gcm
```

### 11.5 Native Handle Access Under Static Dispatch

With a static backend policy, `get_native_handle()` eliminates even the single array
dereference overhead — the compiler sees the implementation directly and reduces the
call to an address computation:

```cpp
// On an N64 static build the compiler sees this:
auto* raw = dev->get_native_handle(XeHandleType::Texture, tex);
auto* obj = static_cast<rdpq_tile_descriptor_t*>(raw);

// As equivalent to:
auto* obj = &n64_ctx.tex_objs[tex & HANDLE_INDEX_MASK];
// A single address computation. No function call.
```

---

## 12. `SamplerDesc` and `SamplerHandle`

```cpp
using SamplerHandle = uint32_t;

enum class WrapMode : uint8_t {
    Repeat, ClampToEdge, MirroredRepeat,
    ClampToBorder,  // Desktop GL / D3D11 only; ignored on console / GLES2
};

enum class FilterMode : uint8_t {
    Nearest,
    Linear,
    LinearMip,    // Trilinear: linear + linear mip
    NearestMip,   // Bilinear:  linear + nearest mip
    Anisotropic,  // Requires has_anisotropic; falls back to LinearMip if unsupported
};

struct SamplerDesc {
    WrapMode   wrap_u        = WrapMode::Repeat;
    WrapMode   wrap_v        = WrapMode::Repeat;
    WrapMode   wrap_w        = WrapMode::Repeat; // Tex3D only
    FilterMode filter_min    = FilterMode::LinearMip;
    FilterMode filter_mag    = FilterMode::Linear;
    uint8_t    aniso_level   = 1;                // 1 = no anisotropy; max from GPUCaps
    float      lod_bias      = 0.0f;
    float      lod_min       = 0.0f;
    float      lod_max       = 1000.0f;
    bool       srgb          = false;
    bool       compare_enable = false;           // For shadow / depth samplers
    CompareFunc compare_func  = CompareFunc::Less;
    float      border_color[4] = {0,0,0,1};     // Used only with ClampToBorder
    const char* debug_name    = nullptr;
};
```

**Backend behaviour summary:**

| Backend | Implementation |
|---|---|
| GL Legacy | Compiled GL display list of `glTexParameter*` calls; called once per bind |
| GL4 | `glGenSamplers` / `glSamplerParameter*` — real sampler object |
| GLES2 | Per-texture `glTexParameter*` calls inline (no sampler objects in GLES2) |
| GLES3 | `glGenSamplers` — real sampler object (GLES3 has `GL_OES_sampler_objects`) |
| D3D11 | `ID3D11SamplerState` |
| Metal | `MTLSamplerState` |
| GCN_GX | GX has no sampler object; state stored in `GL1SamplerEntry`; applied at bind via `GX_InitTexObjFilterMode` |
| N64_RDP | TMEM tile parameters embedded in the draw command; state stored and applied at `bind_texture()` |
| PS3_GCM | `rsxTextureControl` + `rsxTextureFilter`; state stored, applied at bind |
| SoftBuiltin | Sampler state applied in tile inner loop |
| Null | No-op |

`bind_texture()` in `CommandBufferT` now takes both a `TextureHandle` and a
`SamplerHandle`. They are independent resources:

```cpp
cmdbuf.bind_texture(albedo_tex, trilinear_sampler, 0);
cmdbuf.bind_texture(normal_tex, no_mip_sampler,    1);
```

---

## 13. `PipelineDesc` — Full Union (Unchanged from 0.3, Reproduced for Completeness)

```cpp
struct PipelineDesc {
    TierLevel tier;

    union {
        MicrocodePipelineState microcode; // TierLevel::Microcode
        TEVPipelineState       tev;       // TierLevel::TEV
        FixedFunctionState     fixed;     // TierLevel::Legacy
        ProgrammableState      prog;      // TierLevel::Modern / Extended
    } stage;

    VertexLayout      vertex_layout;
    RasterizerState   rasterizer;
    BlendState        blend;
    DepthStencilState depth_stencil;
    PrimitiveType     primitive             = PrimitiveType::Triangles;

    uint8_t     color_attachment_count      = 1;
    PixelFormat color_formats[8];
    PixelFormat depth_format                = PixelFormat::Depth24Stencil8;
    uint8_t     msaa_samples                = 1;
};
```

Tier-specific state structs (`MicrocodePipelineState`, `TEVPipelineState`,
`FixedFunctionState`, `ProgrammableState`) are defined in full in Revision 0.3,
Section 7. They are unchanged.

---

## 14. `ShaderDesc`

```cpp
struct ShaderDesc {
    ShaderStage stage;
    // Source forms — backend picks what it can use; ignores the rest
    const char* glsl_source;          // GL 3.3+ / GLES 3.0+
    const char* glsl_es_100_source;   // GLES 2.0 / GL Legacy
    const char* hlsl_source;          // D3D11
    const char* metal_source;         // MSL
    const char* cg_source;            // PS3 Cg (PSL1GHT / PSGL)
    // Precompiled forms (take precedence over source)
    const void* spirv_bytecode;   uint32_t spirv_size;
    const void* dxbc_bytecode;    uint32_t dxbc_size;
    const void* metallib_bytecode;uint32_t metallib_size;
    const void* cg_microcode;     uint32_t cg_microcode_size;
    const char* entry_point  = "main";
    const char* debug_name;
};
```

---

## 15. `BackendRegistry`

```cpp
struct BackendInfo {
    BackendHint hint;
    const char* name;             // "OpenGL 4.x (Glaze)"
    const char* short_name;       // "gl4"
    TierLevel   tier;
    bool        available;
    bool        hardware_accel;   // false for Soft* and Null
    const char* unavailable_reason; // nullptr if available
};

class BackendRegistry {
public:
    static void               probe          (const XeServices&)      noexcept;
    static uint32_t           count          ()                        noexcept;
    static const BackendInfo& get            (uint32_t index)         noexcept;
    static const BackendInfo* find           (BackendHint)            noexcept;
    static BackendHint        best_available ()                        noexcept;
    static bool               is_available   (BackendHint)            noexcept;
    static const BackendHint* platform_candidates()                   noexcept;
};
```

---

## 16. `RenderDeviceDesc`

```cpp
struct RenderDeviceDesc {
    RenderSurface surface;
    BackendHint   backend_hint   = BackendHint::Auto;
    XeServices    services;
    uint32_t      max_buffers    = 4096;
    uint32_t      max_textures   = 2048;
    uint32_t      max_samplers   = 512;    // new in 0.4
    uint32_t      max_pipelines  = 256;
    uint32_t      max_shaders    = 512;
    bool          enable_debug_layer = false;
    SoftRastDesc  soft_rast;               // SoftBuiltin tuning
    uint32_t      gcm_fifo_kb    = 512;    // PS3 GCM command buffer ring (default 512 KB)
};
```

---

## 17. Asset Pipeline

### 17.1 Architecture Overview

The asset pipeline is a **build-time tool**, not a runtime component. Its job is to
translate any supported source format into a platform-native binary that the runtime
loads with zero conversion overhead. The compiler, the asset formats, and the
runtime loader are three distinct concerns with clean boundaries.

```
┌───────────────────────────────────────────────────────────────┐
│  Source Assets (checked into source control)                  │
│  .glb / .gltf / .fbx / .obj / .dae / .blend / .3ds           │
│  .png / .jpg / .tga / .hdr / .exr / .dds / .ktx2             │
│  .png.xetexprop  (sidecar: usage, wrap, filter, compression)  │
└────────────────────────────┬──────────────────────────────────┘
                             │  build-time step (CI / offline)
                             ▼
┌───────────────────────────────────────────────────────────────┐
│  xe-asset-compiler  (standalone CLI tool, not linked in game) │
│                                                               │
│  Stage 1 — Import                                             │
│    Any source format → Compiler IR                            │
│    (cgltf / OpenFBX / tinyobjloader / stb_image / tinyexr)   │
│    No source-format types survive past this stage.            │
│                                                               │
│  Stage 2 — Process                                            │
│    Mesh optimization      (meshoptimizer)                     │
│    Texture compression    (Basis Universal, libsquish)        │
│    Platform tiling        (N64 TMEM, GX block, RSX swizzle)   │
│    LOD generation         (meshoptimizer simplify)            │
│    Mip chain generation   (Kaiser filter)                     │
│    Animation baking       (resample, quantize)                │
│    Material baking        (PBR → tier-appropriate PipelineDesc│
│    Vertex quantization    (float32 → int16 normalized)        │
│    Bounds computation                                         │
│                                                               │
│  Stage 3 — Export                                             │
│    Processed IR → platform-native binary                      │
└────────────────────────────┬──────────────────────────────────┘
                             │
                             ▼
┌───────────────────────────────────────────────────────────────┐
│  Runtime Binaries (stored in package registry / ROM)          │
│  .xemesh  — mesh + materials + embedded texture refs          │
│  .xetex   — texture (tiled, compressed, mip-chained)          │
│  .xeanim  — baked animation clip                              │
│  .xepkg   — multi-asset bundle                                │
└────────────────────────────┬──────────────────────────────────┘
                             │  runtime load
                             ▼
┌───────────────────────────────────────────────────────────────┐
│  Runtime Loader (thin; one loop of create_* calls)            │
│  mmap() / fread() → RenderDevice::create_buffer()             │
│                    RenderDevice::create_texture()             │
│                    RenderDevice::create_sampler()             │
│                    RenderDevice::create_pipeline()            │
└───────────────────────────────────────────────────────────────┘
```

The **Intermediate Representation (IR)** is the compiler-private neutral type system.
Every importer converts to IR; every exporter reads from IR. Adding a new source
format costs only a new importer; adding a new target platform costs only a new
exporter. The LLVM frontend/middleend/backend analogy is exact.

### 17.2 `xe-asset-compiler` CLI

```
xe-asset-compiler [options] <input> --target <platform> --output <path>

Targets:
  n64          Nintendo 64 (libdragon)
  gamecube     GameCube (libogc GX)
  wii          Wii (libogc GX)
  ps3          PlayStation 3 (PSL1GHT)
  desktop-gl4  Desktop OpenGL 4.x
  desktop-d3d11 Desktop Direct3D 11
  ios-metal    iOS / macOS Metal
  android      Android (ASTC + GLES3)

Options:
  --lod-levels  1–4          LOD count (default: 1)
  --strip                    Convert to triangle strips (N64 / GC benefit)
  --quantize-pos none|i16|i10 Vertex position quantization
  --anim-rate   <fps>        Animation resample rate
  --no-compress              Skip texture compression (debugging)
  --verbose                  Log all processing decisions
```

### 17.3 `.xetex` — Texture Binary Format

The compiled texture binary is a directly-mappable flat structure. The pixel data
region points to bytes already in the exact memory layout the GPU expects — no
conversion at load time.

```
┌──────────────────────────────────────────────────────────────┐
│  XeTexHeader                                                 │
│  magic[4]         = "XETX"                                   │
│  version          = 4                                        │
│  platform_id      (XE_PLATFORM_* constant)                   │
│  texture_type     (TextureType enum)                         │
│  pixel_format     (PixelFormat enum — platform-native value) │
│  width, height, depth, array_count, mip_count                │
│  wrap_u, wrap_v   (WrapMode enum)                            │
│  filter_min, filter_mag  (FilterMode enum)                   │
│  aniso_level, lod_bias                                       │
│  srgb, premul_alpha                                          │
│  data_offset      (byte offset to pixel data region)         │
│  data_size                                                   │
├──────────────────────────────────────────────────────────────┤
│  MipDescriptor[mip_count]                                    │
│  { offset, size, width, height }  per mip level              │
├──────────────────────────────────────────────────────────────┤
│  Pixel data                                                  │
│  Platform-tiled, compressed, mip-chained.                    │
│  Exact layout the GPU / TMEM / ARAM expects.                 │
│  No conversion required at runtime.                          │
└──────────────────────────────────────────────────────────────┘
```

Runtime load:

```cpp
XeTexHeader* hdr  = (XeTexHeader*) mapped_file; // zero-copy mmap

TextureDesc desc;
desc.type         = (TextureType) hdr->texture_type;
desc.format       = (PixelFormat) hdr->pixel_format; // already platform-native
desc.width        = hdr->width;
desc.height       = hdr->height;
desc.mip_count    = hdr->mip_count;
desc.initial_data = mapped_file + hdr->data_offset;  // points into file

SamplerDesc samp;
samp.wrap_u     = (WrapMode)   hdr->wrap_u;
samp.wrap_v     = (WrapMode)   hdr->wrap_v;
samp.filter_min = (FilterMode) hdr->filter_min;
samp.filter_mag = (FilterMode) hdr->filter_mag;
samp.aniso_level = hdr->aniso_level;
samp.srgb        = hdr->srgb;

TextureHandle tex = device->create_texture(desc);
SamplerHandle smp = device->create_sampler(samp);
// Done. create_texture() is a DMA transfer of already-correct data.
// On N64: dma_transfer() to TMEM. On desktop: glTexImage2D with pre-tiled data.
```

### 17.4 `.xetexprop` — Texture Properties Sidecar

A TOML sidecar file checked in alongside the source image. It provides the
semantic context the compiler needs to make correct processing decisions.

```toml
# hero_albedo.png.xetexprop

usage        = "albedo"   # albedo | normal | roughness_metallic |
                          # emissive | ui | cubemap_face | heightmap
srgb         = true       # false for normal / roughness / metallic maps
wrap_u       = "repeat"   # repeat | clamp | mirror
wrap_v       = "repeat"
filter_min   = "linear_mip"
filter_mag   = "linear"
aniso_level  = 4
mip_policy   = "full"     # full | none
mip_filter   = "kaiser"   # box | triangle | kaiser
max_size     = 512        # downsample if source exceeds this
premul_alpha = false
pot_enforce  = true       # force power-of-two (required for N64 / GL Legacy)

# Per-platform compression overrides
[compress.desktop]    format = "bc7"
[compress.mobile]     format = "astc_6x6"
[compress.n64]        format = "rgba16"   # No block compression on N64; TMEM is 4KB
[compress.gamecube]   format = "cmpr"     # GX native DXT1 variant
[compress.ps3]        format = "dxt1"
```

If no sidecar exists, the compiler infers usage from filename conventions
(`_n` → normal map, `_rm` → roughness/metallic, `_e` → emissive) and logs a
warning.

### 17.5 `.xemesh` — Mesh Package Format

```
┌──────────────────────────────────────────────────────────────┐
│  XeMeshHeader                                                │
│  magic[4] = "XEMS", version, platform_id                    │
│  mesh_count, material_count, texture_count                   │
│  skin_count, anim_count                                      │
│  chunk_table_offset                                          │
├──────────────────────────────────────────────────────────────┤
│  Chunk table  { offset, size, type, name_hash } per chunk    │
├──────────────────────────────────────────────────────────────┤
│  Chunk: MESH  — VertexLayout, BufferDesc (vertex + index)    │
│  Chunk: MATL  — Baked PipelineDesc per material variant      │
│                 (tier-correct for target platform)           │
│  Chunk: TEXR  — TextureDesc + SamplerDesc per texture        │
│                 initial_data points into TEXD chunk          │
│  Chunk: TEXD  — Raw pixel data (tiled, compressed)           │
│  Chunk: SKIN  — Joint hierarchy + inverse bind matrices      │
│  Chunk: ANIM  — Baked, quantized animation tracks            │
│  Chunk: LODS  — LOD mesh descriptors + distance thresholds   │
│  Chunk: META  — Names, AABBs, LOD ranges, node hierarchy     │
└──────────────────────────────────────────────────────────────┘
```

Runtime loader:

```cpp
XeMeshPackage* pkg = xe_mesh_map(path); // mmap or fread

for (uint32_t i = 0; i < pkg->mesh_count; i++) {
    out.vb[i] = device->create_buffer(pkg->vertex_descs[i]);
    out.ib[i] = device->create_buffer(pkg->index_descs[i]);
}
for (uint32_t i = 0; i < pkg->material_count; i++)
    out.pipelines[i] = device->create_pipeline(pkg->pipeline_descs[i]);
for (uint32_t i = 0; i < pkg->texture_count; i++) {
    out.textures[i] = device->create_texture(pkg->texture_descs[i]);
    out.samplers[i] = device->create_sampler(pkg->sampler_descs[i]);
}
// No format decisions. No conversions. No hidden allocations.
```

### 17.6 glTF / Source Format Mapping to Runtime Concepts

| glTF concept | Resolved at | Runtime type |
|---|---|---|
| `bufferView` (ARRAY_BUFFER) | Compiler: stride, format, layout | `BufferHandle` (vertex) |
| `bufferView` (ELEMENT_ARRAY_BUFFER) | Compiler: U16 vs U32 | `BufferHandle` (index) |
| `accessor` | Compiler: `VertexAttrib` per semantic | Embedded in `VertexLayout` in `PipelineDesc` |
| `image` + `sampler` | Compiler: decode, compress, tile | `TextureHandle` + `SamplerHandle` |
| `material` (PBR) | Compiler: tier-appropriate `PipelineDesc` | `PipelineHandle` |
| `mesh.primitive` | Compile time + runtime | Draw call in `CommandBuffer` |
| Node transform (TRS) | Runtime | `push_constants` or uniform buffer |
| Skin + joints | Compiler: baked matrices + quantization | `BufferHandle` (joint matrix palette) |
| Animation channels | Compiler: resampled + compressed | Loaded from `.xeanim` |
| Sparse accessors | Compiler: materialized to dense | Dense `BufferHandle` at runtime |

Sparse accessors are the one glTF concept that would require special handling in a
runtime loader — they are fully materialized by the compiler and never appear in a
`.xemesh` file.

### 17.7 CI Integration

```
Content author commits source asset (.glb, .png, .png.xetexprop)
           │
           ▼  Forgejo Actions trigger
xe-asset-compiler --target n64        → model_n64.xemesh + textures_n64.xetex
xe-asset-compiler --target gamecube   → model_gcn.xemesh + textures_gcn.xetex
xe-asset-compiler --target desktop-gl4→ model_pc.xemesh  + textures_pc.xetex
           │
           ▼
Compiled assets stored in Forgejo package registry
alongside code build artifacts.
Runtime never touches .glb or .png.
```

---

## 18. Platform / Backend Availability Matrix

| Platform | GLLeg | GL4 | GLES2 | GLES3 | D3D11 | Metal | Vulkan | N64_RDP | GCN_GX | PS3_GCM | SoftBuiltin | SoftMesa | Null |
|---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| **Win 98/ME** | ✓ | – | – | – | – | – | – | – | – | – | ✓ | – | ✓ |
| **Win XP/Vista** | ✓ | △ | – | – | – | – | – | – | – | – | ✓ | △ | ✓ |
| **Win 7+** | ✓ | ✓ | – | – | ✓ | – | – | – | – | – | ✓ | ✓ | ✓ |
| **Win 10/11** | ✓ | ✓ | – | – | ✓ | – | ✓ | – | – | – | ✓ | ✓ | ✓ |
| **Linux** | ✓ | ✓ | – | – | – | – | ✓ | – | – | – | ✓ | ✓ | ✓ |
| **macOS ≤14** | ✓¹ | ✓¹ | – | – | – | ✓ | – | – | – | – | ✓ | ✓ | ✓ |
| **macOS 15+** | – | – | ✓² | ✓² | – | ✓ | – | – | – | – | ✓ | ✓ | ✓ |
| **Android 4.x** | – | – | ✓ | – | – | – | – | – | – | – | ✓ | – | ✓ |
| **Android 7+** | – | – | ✓ | ✓ | – | – | ✓ | – | – | – | ✓ | △ | ✓ |
| **iOS 8–17** | – | – | ✓³ | ✓³ | – | ✓ | – | – | – | – | ✓ | – | ✓ |
| **Nintendo 64** | – | – | – | – | – | – | – | ✓ | – | – | ✓ | – | ✓ |
| **GameCube** | – | – | – | – | – | – | – | – | ✓ | – | ✓ | – | ✓ |
| **Wii** | – | – | – | – | – | – | – | – | ✓ | – | ✓ | – | ✓ |
| **PS3** | – | – | – | – | – | – | – | – | – | ✓ | ✓ | – | ✓ |

¹ GL deprecated on macOS 10.14; functional through macOS 13.
² Via ANGLE (GL ES over Metal).
³ GLES deprecated iOS 12; Metal is the primary path.
△ Conditional on external library availability.

---

## 19. Additional Console Target Notes (Unchanged from 0.3)

### Sega Dreamcast
**SDK:** KallistiOS + `sh-elf-gcc`. **Tier:** `Legacy`. GLdc (MIT) provides GL 1.2 over PVR
— the existing `GLLegacy` Glaze-backed backend could run on Dreamcast via GLdc with
minimal modifications. Future `BackendHint::Dreamcast_PVR`.

### Original Xbox
**SDK:** nxdk (MIT) + LLVM/Clang. **Tier:** `Legacy` / `Modern` boundary (NV2A supports
VS 1.1 / PS 1.4). Future `BackendHint::Xbox_D3D8`. Good candidate for a
`LegacyShader` sub-tier revision.

---

## 20. Render Graph — Why It Is Out of Scope

The render graph belongs in the engine layer **above** this middleware, not inside it.
This middleware layer's contract is: *given resources and commands, get pixels onto a
surface.* A render graph's contract is: *given a description of the entire frame,
determine the optimal execution and resource plan.*

Concretely:

- Console backends (N64, GC/Wii) have no concept of pass dependencies, transient
  resource aliasing, or barrier insertion. Carrying render graph machinery into these
  backends would violate the portability goal.
- A render graph's output is a set of ordered `CommandBuffer`s submitted to a
  `RenderDevice`. That is precisely the API this layer already exposes — the render
  graph is a **client** of this layer.
- `PassDesc` (color/depth targets, clear values) is already expressive enough for
  a render graph to drive without requiring any changes to this spec.

---

## 21. Class Relationship Diagram

```
 ┌─────────────────────────────────────────────────────────────────────────┐
 │                         Application Layer                               │
 │              (+ XeJob / XeThread — separate library)                   │
 └──┬──────────────┬──────────────┬───────────────┬───────────┬────────────┘
    │              │              │               │           │
    ▼              ▼              ▼               ▼           ▼
┌──────────┐ ┌──────────┐ ┌────────────┐ ┌───────────┐ ┌──────────┐
│XeServices│ │Backend   │ │SystemCaps  │ │CommandBuf │ │CommandBuf│
│Allocator │ │Registry  │ │CPUCaps     │ │ (thread A)│ │(thread B)│
│LogSink   │ │probe()   │ │GPUCaps     │ │ XeAlloc   │ │XeAlloc   │
│Profiler  │ │best_avail│ │MemoryCaps  │ └─────┬─────┘ └────┬─────┘
│Assert    │ └────┬─────┘ │PlatformCaps│       │             │
└────┬─────┘      │       └────────────┘       └──────┬──────┘
     │            │                                   │ submit_batch (render thread)
     │            ▼                                   ▼
     │  ┌──────────────────────────────────────────────────────────┐
     └─▶│        RenderDeviceT<Policy>                             │
        │                                                          │
        │  Policy=Dynamic:   DispatchCore → BackendVTable          │
        │  Policy=N64_RDP:   Direct calls, BackendContext by value │
        │  Policy=GCN_GX:    Direct calls, BackendContext by value │
        │  Policy=PS3_GCM:   Direct calls, BackendContext by value │
        │                                                          │
        │  get_native_handle()  → O(1) array index                 │
        │  query_extension()    → typed native struct              │
        │  backend_ctx()        → for extension header helpers     │
        └─────────────────┬────────────────────────────────────────┘
                          │  via BackendVTable (dynamic)
                          │  or direct call (static specialization)
     ┌────────────────────┼────────────────────────────────────────┐
     ▼                    ▼                                        ▼
┌──────────┐      ┌──────────────┐                       ┌──────────────┐
│ N64_RDP  │      │   GCN_GX     │                       │   PS3_GCM    │
│ libdragon│      │  libogc GX   │                       │  PSL1GHT     │
│ TL:Micro │      │  TL:TEV      │                       │  TL:Leg/Mod  │
│ rdpq DL  │      │  TEV stages  │                       │  Cg shaders  │
└──────────┘      └──────────────┘                       └──────────────┘
     ┌──────────┬──────────────┬──────────────┬──────────────────┐
     ▼          ▼              ▼              ▼                  ▼
  GLLegacy    GL4           D3D11/Metal   SoftBuiltin         SoftMesa
  (Glaze)   (Glaze)         (native)     (tile rast)         (Mesa3D)
  TL:Legacy TL:Modern       TL:Modern    TL:Legacy           TL:Extended
  DL-opt.   sampler obj     native smp   sw sampler          sw sampler

Extension headers (opt-in, xe/native/):
  xe_native_gl4.hpp        → GL4DeviceExt    (GLuint names, make_current, fence)
  xe_native_gl_legacy.hpp  → GL1DeviceExt    (GLuint names, display list names)
  xe_native_gcn_gx.hpp     → GXDeviceExt     (GXTexObj*, raw DL buffer)
  xe_native_ps3_gcm.hpp    → PS3GCMDeviceExt (rsxBuffer*, gcmContextData*)

Resource handles (uint32_t, backend-private flat arrays):
  BufferHandle  TextureHandle  SamplerHandle  PipelineHandle  ShaderHandle
```

---

## 22. Initialization Flow

```
1. XeServices svc = XeServices::make_default();
   // Override svc.log.write, svc.profiler.*, svc.allocator as needed

2. SystemCaps caps = SystemCaps::query_host(svc);
   // CPU, memory, platform filled. GPU not yet known.

3. Create OS/console window → RenderSurface

4. BackendRegistry::probe(svc);
   BackendHint chosen = BackendRegistry::best_available();
   // Or explicit user override to work around driver bugs:
   // chosen = BackendHint::GL4;

5. RenderDeviceDesc desc{ .surface=surf, .backend_hint=chosen,
                          .services=svc, .gcm_fifo_kb=512 };
   RenderDevice* dev = RenderDevice::create(desc);
   // caps.gpu populated after this call

6. Load compiled assets:
   XeMeshPackage* pkg = xe_mesh_map("hero_n64.xemesh");
   // Thin loop — no format decisions:
   for (auto& vd : pkg->vertex_descs)   vb[i] = dev->create_buffer(vd);
   for (auto& td : pkg->texture_descs)  tex[i] = dev->create_texture(td);
   for (auto& sd : pkg->sampler_descs)  smp[i] = dev->create_sampler(sd);
   for (auto& pd : pkg->pipeline_descs) pl[i]  = dev->create_pipeline(pd);

7. Per-frame loop:
   dev->begin_frame();
   {
       CommandBuffer cmdbuf(svc.allocator);
       cmdbuf.begin_pass(pass_desc);
       cmdbuf.begin_gpu_scope("MainPass");
         cmdbuf.bind_pipeline(pl[0]);
         cmdbuf.bind_vertex_buffer(vb[0], 0, stride);
         cmdbuf.bind_index_buffer(ib[0], IndexType::U16);
         cmdbuf.bind_texture(tex[0], smp[0], 0);
         cmdbuf.push_constants(&xform, sizeof(xform));
         cmdbuf.draw_indexed(index_count);
       cmdbuf.end_gpu_scope();
       cmdbuf.end_pass();
       dev->submit(cmdbuf);
   }
   dev->end_frame();
   dev->present();

8. Shutdown:
   dev->destroy_pipeline(pl[0]);
   dev->destroy_sampler(smp[0]);
   dev->destroy_texture(tex[0]);
   dev->destroy_buffer(vb[0]);
   RenderDevice::destroy(dev);
```

---

## 23. Design Decisions — New in 0.4

| Decision | Rationale |
|---|---|
| **`RenderDeviceT<Policy>` + `CommandBufferT<Policy>`** | Single template parameter; `using` alias in `xe_platform_config.hpp` prevents leakage upward; LTO collapses static builds to direct calls |
| **`BackendContext` embedded by value in static specializations** | Eliminates heap allocation and pointer chase on N64/GC/PS3 where every byte and cycle counts |
| **`get_native_handle` + `query_extension` in `BackendVTable`** | Provides a typed, backend-specific escape hatch without breaking the API-neutral surface; extension headers are opt-in |
| **`SamplerHandle` as a distinct resource** | Matches GL4/D3D11/Metal's object model; allows sampler reuse across textures; on backends without sampler objects the state is stored and applied inline at bind — no API surface change for those backends |
| **GL Legacy display list for sampler + pipeline state** | One `glCallList` replaces 4–5 `glTexParameter*` calls and the entire rasterizer/blend/depth setup; compiled once at object creation; correct because both samplers and pipelines are immutable |
| **Asset compiler as a standalone CLI** | Heavy dependencies (meshoptimizer, Basis, libktx) never link into the runtime; compiler output size equals runtime memory cost with no hidden conversion buffers — essential for N64 memory budgeting |
| **`.xetexprop` sidecar** | Source images carry no semantic metadata; the sidecar provides usage, wrap/filter, and per-platform compression overrides in a human-readable, version-controllable form |
| **`.xemesh` TEXD chunk separates descriptors from pixel data** | `initial_data` in `TextureDesc` points directly into the TEXD chunk; `create_texture()` is a DMA transfer with no intermediate copy |
| **Sparse accessor materialization at compile time** | Eliminates the one glTF concept that would require special runtime handling; runtime loader never sees the concept |
| **`gcm_fifo_kb` in `RenderDeviceDesc`** | PS3 GCM ring buffer size was deferred in 0.3; now configurable with a 512 KB default |

---

## 24. Out of Scope for This Iteration

- Compute pipelines and compute dispatch (Extended tier)
- Multi-GPU / multi-adapter enumeration
- Async resource streaming / transfer queues
- Ray tracing
- Explicit Vulkan-style memory heap sub-allocation
- Render graph / frame graph (rationale in Section 20)
- Material and scene graph systems (live above this layer)
- SPIRV-Cross integration for automatic shader cross-compilation
- `SoftBuiltin` multi-threaded tile dispatch (`max_threads` slot reserved; `XeJob` integration deferred)
- SPU-accelerated tile dispatch on PS3 (handoff point defined in `CPUCaps.spu_count`; implementation deferred)
- Dreamcast `PVR` backend and Xbox OG `D3D8` backend (architecture noted in Section 19; deferred)
- `LegacyShader` sub-tier for VS 1.x / PS 1.x assembly shaders (Xbox OG NV2A)
- Pre-tiled `native_format` fast-path for GX CMPR uploads (deferred to 0.5)
- `.xepkg` multi-asset bundle format (structure noted; full spec deferred to 0.5)
- Static geometry display lists for world meshes (GL Legacy; architecture noted in Section 9.3; content-pipeline decision deferred)
