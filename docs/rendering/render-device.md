# Static Backend Dispatch — `RenderDeviceT<Policy>` and `CommandBufferT<Policy>`

The renderer's core dispatch mechanism. A single template parameter selects whether function calls go through a vtable (dynamic, desktop/mobile) or are compiled as direct calls (static, console targets). A build-system-generated `xe_platform_config.hpp` alias makes all client code backend-agnostic.

---

## Backend Policy Tags

```cpp
// xe_backend_policy.hpp — no backend-specific headers included here

struct BackendPolicy_Dynamic    {};  // Runtime fn-pointer dispatch (default, all desktop/mobile)
struct BackendPolicy_N64_RDP    {};  // Static: libdragon rdpq direct calls
struct BackendPolicy_GCN_GX     {};  // Static: libogc GX direct calls
struct BackendPolicy_PS3_GCM    {};  // Static: PSL1GHT libGCM direct calls
struct BackendPolicy_GL4        {};  // Static: GL4 Glaze (dev/CI single-backend builds)
struct BackendPolicy_SoftBuiltin{};  // Static: tile rasterizer
```

## `DispatchCore<Policy>` — One Pointer Indirection

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

## `RenderDeviceT<Policy>`

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

    // ── Synchronisation ──────────────────────────────────────────────
    void flush()                                              noexcept;
    void finish()                                             noexcept;

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
    void                        destroy_render_target(TextureHandle)            noexcept;

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

## `CommandBufferT<Policy>`

The same policy parameterization applies to `CommandBuffer`. On N64 and GC/Wii the
static specialization encodes directly into the native display list format — the
generic decode pass in `submit()` is eliminated entirely.

```cpp
template<typename Policy = BackendPolicy_Dynamic>
class CommandBufferT;

// Dynamic: generic tagged byte-stream, decoded during submit()
template<>
class CommandBufferT<BackendPolicy_Dynamic> : public CommandBufferBase {
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
        const auto& cfg = _ctx->pipeline_table[h & HANDLE_INDEX_MASK];
        rdpq_set_combiner_raw(cfg.combiner_word);
    }
    void bind_vertex_buffer(BufferHandle h, uint8_t, uint32_t, uint32_t offset = 0) noexcept {
        _verts = (const uint8_t*)_ctx->rdp_buffers[h & HANDLE_INDEX_MASK] + offset;
    }
    void draw_indexed(uint32_t count, uint32_t first, int32_t) noexcept {
        rdpq_triangle_strip((const void*)((uintptr_t)_verts + first * sizeof(uint16_t)), count);
    }
    // submit() = rspq_flush() / DMA kickoff — no decode pass
    // On a 93MHz R4300 this is the difference between shipping and not shipping.

private:
    uint64_t* _dl;        // rdpq display list buffer
    uint32_t  _dl_used, _dl_capacity;
    const void* _verts = nullptr;
    const N64BackendContext* _ctx;  // pipeline table access
};
```

## `xe_platform_config.hpp` — The Alias Convention

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

## See Also

- [backend-abi.md](backend-abi.md) — `BackendVTable` that `DispatchCore` wraps
- [native-handles.md](native-handles.md) — `get_native_handle` / `query_extension` exposed on `RenderDeviceT`
- [resources.md](resources.md) — all `*Desc` / `*Handle` types used by `RenderDeviceT`'s public API
- [threading-contract.md](threading-contract.md) — which `RenderDeviceT` methods are render-thread-only and which `CommandBufferT` methods are safe from any thread
- [services.md](services.md) — `XeServices` stored by value inside `RenderDeviceT`
