# Public Resource Types — Sampler, Pipeline, Shader, BackendRegistry, RenderDeviceDesc

Collects the descriptor and handle types that make up the renderer's public API surface: what the application uses to create GPU resources, select backends, and configure device creation.

---

## `SamplerDesc` and `SamplerHandle`

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

### Backend Behaviour Summary

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

## `PipelineDesc` — Full Union

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

## `ShaderDesc`

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

## `BackendRegistry`

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

## `RenderDeviceDesc`

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

## See Also

- [render-device.md](render-device.md) — `RenderDeviceT<Policy>` exposes `create_*` / `destroy_*` for each handle type
- [backend-abi.md](backend-abi.md) — `BackendVTable` slots that consume these descriptors
- [system-caps.md](system-caps.md) — `GPUCaps` queried to gate sampler / pipeline / shader features
- [native-handles.md](native-handles.md) — extension headers that cast handles to native API objects
- [asset-pipeline.md](asset-pipeline.md) — compiled `.xemesh` and `.xetex` files embed pre-baked `PipelineDesc`, `SamplerDesc`, and `TextureDesc` values
