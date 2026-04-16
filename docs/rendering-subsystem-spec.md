# XE Engine — Rendering Middleware Specification
**Revision:** 0.5 — Modular Spec Split, Structurizr Architecture Model  
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
| **0.5** | Spec split into per-module markdown files under `rendering/`; cross-cutting sections (goals, C++ subset, render graph scope, class diagram, init flow, out of scope) remain here; Structurizr C4 architecture model added in `docs/architecture/` |

---

## Modules

Each module spec is self-contained with its own code listings, tables, and cross-references.

| Module | File | Contents |
|---|---|---|
| **Threading Contract** | [rendering/threading-contract.md](rendering/threading-contract.md) | Zero-thread rule, thread-safety table, multithreaded recording and deferred creation patterns |
| **System Capabilities** | [rendering/system-caps.md](rendering/system-caps.md) | `TierLevel`, `BackendHint`, `RenderSurface`, `SystemCaps` (`CPUCaps`, `GPUCaps`, `MemoryCaps`, `PlatformCaps`) |
| **Services** | [rendering/services.md](rendering/services.md) | `XeServices` — allocator, log sink, profiler hook, assert handler, alloc tracker |
| **Backend ABI** | [rendering/backend-abi.md](rendering/backend-abi.md) | `BackendVTable`, `BackendContext`, factory signatures, unsupported-slot contract, GL Legacy display list optimization |
| **Render Device** | [rendering/render-device.md](rendering/render-device.md) | `RenderDeviceT<Policy>`, `DispatchCore<Policy>`, `CommandBufferT<Policy>`, `xe_platform_config.hpp` alias convention |
| **Native Handles** | [rendering/native-handles.md](rendering/native-handles.md) | Per-backend extension headers (`xe/native/`), `get_native_handle`, `query_extension`, static dispatch codegen |
| **Resources** | [rendering/resources.md](rendering/resources.md) | `SamplerDesc`/`SamplerHandle`, `PipelineDesc` union, `ShaderDesc`, `BackendRegistry`, `RenderDeviceDesc` |
| **Asset Pipeline** | [rendering/asset-pipeline.md](rendering/asset-pipeline.md) | `xe-asset-compiler` CLI, `.xetex`, `.xemesh`, `.xetexprop`, glTF mapping, CI integration |
| **Platform Support** | [rendering/platform-support.md](rendering/platform-support.md) | Backend availability matrix, Dreamcast / Xbox OG target notes |
| **Design Log** | [rendering/design-log.md](rendering/design-log.md) | Per-revision design decision tables with rationale |

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

## 3. Render Graph — Why It Is Out of Scope

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

## 4. Class Relationship Diagram

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

Module references:
- `XeServices` → [rendering/services.md](rendering/services.md)
- `BackendRegistry` → [rendering/resources.md](rendering/resources.md)
- `SystemCaps` → [rendering/system-caps.md](rendering/system-caps.md)
- `RenderDeviceT<Policy>` → [rendering/render-device.md](rendering/render-device.md)
- `BackendVTable` → [rendering/backend-abi.md](rendering/backend-abi.md)
- Extension headers → [rendering/native-handles.md](rendering/native-handles.md)

---

## 5. Initialization Flow

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

Module references:
- Step 1 → [rendering/services.md](rendering/services.md)
- Step 2 → [rendering/system-caps.md](rendering/system-caps.md)
- Step 4 → [rendering/resources.md](rendering/resources.md) (`BackendRegistry`)
- Step 5 → [rendering/resources.md](rendering/resources.md) (`RenderDeviceDesc`)
- Step 6 → [rendering/asset-pipeline.md](rendering/asset-pipeline.md)
- Step 7 → [rendering/render-device.md](rendering/render-device.md) (`RenderDeviceT`, `CommandBufferT`)

---

## 6. Out of Scope for This Iteration

- Compute pipelines and compute dispatch (Extended tier)
- Multi-GPU / multi-adapter enumeration
- Async resource streaming / transfer queues
- Ray tracing
- Explicit Vulkan-style memory heap sub-allocation
- Render graph / frame graph (rationale in Section 3 above)
- Material and scene graph systems (live above this layer)
- SPIRV-Cross integration for automatic shader cross-compilation
- `SoftBuiltin` multi-threaded tile dispatch (`max_threads` slot reserved; `XeJob` integration deferred)
- SPU-accelerated tile dispatch on PS3 (handoff point defined in `CPUCaps.spu_count`; implementation deferred)
- Dreamcast `PVR` backend and Xbox OG `D3D8` backend (architecture noted in [rendering/platform-support.md](rendering/platform-support.md); deferred)
- `LegacyShader` sub-tier for VS 1.x / PS 1.x assembly shaders (Xbox OG NV2A)
- Pre-tiled `native_format` fast-path for GX CMPR uploads (deferred to 0.5)
- `.xepkg` multi-asset bundle format (structure noted; full spec deferred to 0.5)
- Static geometry display lists for world meshes (GL Legacy; architecture noted in [rendering/backend-abi.md](rendering/backend-abi.md); content-pipeline decision deferred)
