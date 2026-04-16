# Design Decisions Log

Records the rationale behind key architectural decisions made in each spec revision. Intended to grow over time — each revision appends its decisions here.

---

## Revision 0.4

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

## See Also

- [render-device.md](render-device.md) — `RenderDeviceT<Policy>` design context
- [backend-abi.md](backend-abi.md) — `BackendVTable` extensibility and GL Legacy display list details
- [resources.md](resources.md) — `SamplerHandle`, `RenderDeviceDesc.gcm_fifo_kb`
- [asset-pipeline.md](asset-pipeline.md) — asset compiler, `.xetexprop`, `.xemesh` chunk layout
