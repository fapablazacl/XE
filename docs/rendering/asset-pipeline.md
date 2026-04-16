# Asset Pipeline

The asset pipeline is a **build-time tool**, not a runtime component. Its job is to translate any supported source format into a platform-native binary that the runtime loads with zero conversion overhead. The compiler, the asset formats, and the runtime loader are three distinct concerns with clean boundaries.

---

## Architecture Overview

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

---

## `xe-asset-compiler` CLI

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

---

## `.xetex` — Texture Binary Format

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

---

## `.xetexprop` — Texture Properties Sidecar

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

---

## `.xemesh` — Mesh Package Format

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

---

## glTF / Source Format Mapping to Runtime Concepts

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

---

## CI Integration

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

## See Also

- [resources.md](resources.md) — `SamplerDesc`, `PipelineDesc`, `TextureDesc`, `BufferDesc` types produced by the compiler and consumed by the runtime loader
- [render-device.md](render-device.md) — `create_buffer`, `create_texture`, `create_sampler`, `create_pipeline` calls the loader makes
- [system-caps.md](system-caps.md) — `TierLevel` drives the compiler's material-baking decisions
- [platform-support.md](platform-support.md) — target platforms the compiler exports to
