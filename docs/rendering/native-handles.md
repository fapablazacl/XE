# Native Handle Access — Extension Headers

Per-backend extension headers provide type-safe access to underlying API objects. They live in a separate include path (`xe/native/`) and are **never included by the middleware core itself**. The client explicitly includes the header when it knows which backend is active.

---

## `xe/native/xe_native_gl4.hpp`

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

## `xe/native/xe_native_gl_legacy.hpp`

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

## `xe/native/xe_native_gcn_gx.hpp`

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

## `xe/native/xe_native_ps3_gcm.hpp`

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

## Native Handle Access Under Static Dispatch

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

## See Also

- [render-device.md](render-device.md) — `RenderDeviceT<Policy>` exposes `get_native_handle`, `query_extension`, `backend_ctx`
- [backend-abi.md](backend-abi.md) — `BackendVTable` slots that these extension headers consume
- [resources.md](resources.md) — `SamplerHandle`, `BufferHandle`, `TextureHandle`, `PipelineHandle` types passed to extension functions
