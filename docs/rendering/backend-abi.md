# `BackendVTable` and `BackendContext`

Defines the C ABI that every backend translation unit implements. The vtable is the stable boundary between the policy-parameterized `RenderDeviceT<Policy>` dispatch layer and the concrete backend (OpenGL, D3D11, Metal, libdragon rdpq, libogc GX, PSL1GHT libGCM, software rasterizers, …). The vtable's end-of-struct extensibility rule is what lets new slots land without breaking existing shipped backends.

---

## Factory Signatures

```c
/* C ABI — each backend translation unit exports one of these */
XE_API BackendVTable xe_backend_gl_legacy_create   (const RenderDeviceDesc*, BackendContext**);
XE_API BackendVTable xe_backend_gl4_create         (const RenderDeviceDesc*, BackendContext**);
XE_API BackendVTable xe_backend_gles2_create       (const RenderDeviceDesc*, BackendContext**);
XE_API BackendVTable xe_backend_gles3_create       (const RenderDeviceDesc*, BackendContext**);
XE_API BackendVTable xe_backend_d3d11_create       (const RenderDeviceDesc*, BackendContext**);
XE_API BackendVTable xe_backend_metal_create       (const RenderDeviceDesc*, BackendContext**);
XE_API BackendVTable xe_backend_n64_rdp_create     (const RenderDeviceDesc*, BackendContext**);
XE_API BackendVTable xe_backend_gcn_gx_create      (const RenderDeviceDesc*, BackendContext**);
XE_API BackendVTable xe_backend_ps3_gcm_create     (const RenderDeviceDesc*, BackendContext**);
XE_API BackendVTable xe_backend_soft_builtin_create(const RenderDeviceDesc*, BackendContext**);
XE_API BackendVTable xe_backend_soft_mesa_create   (const RenderDeviceDesc*, BackendContext**);
XE_API BackendVTable xe_backend_null_create        (const RenderDeviceDesc*, BackendContext**);
```

## `BackendVTable`

```c
enum class XeHandleType : uint8_t {
    Buffer, Texture, Sampler, Shader, Pipeline, RenderTarget
};

typedef struct BackendContext BackendContext; /* opaque */
typedef struct CommandBufferBase CommandBufferBase; /* opaque */

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

    /* ── Memory query ───────────────────────────────────────── */
    void (*query_memory) (BackendContext*, RenderMemoryStats*);

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

---

## GL Legacy — Display List Optimization for Sampler and Pipeline State

The GL Legacy backend compiles sampler and pipeline state into GL display lists at
creation time. This trades one-time compile cost for minimal per-bind overhead —
important on constrained Legacy hardware where individual GL state calls carry
significant per-call overhead.

### Sampler Display List

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

### Pipeline State Display List

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

### GL Legacy Display List Usage Summary

| Object | Compiled at | Contains | Called at |
|---|---|---|---|
| Sampler state list | `create_sampler()` | `glTexParameter*` calls | `bind_texture()` after `glBindTexture` |
| Pipeline state list | `create_pipeline()` | rasterizer + blend + depth + fixed-function state | `bind_pipeline()` |
| Static geometry list | Optional, content-driven | `glBegin/glEnd` geometry | Static world draw calls only |

---

## See Also

- [render-device.md](render-device.md) — `DispatchCore<Policy>` wraps this vtable
- [native-handles.md](native-handles.md) — `get_native_handle` and `query_extension` are consumed by the per-backend extension headers
- [resources.md](resources.md) — `SamplerDesc`, `PipelineDesc`, `ShaderDesc`, `RenderDeviceDesc` types used by the factory signatures
- [services.md](services.md) — unsupported-slot stubs log via `XeServices.log`
