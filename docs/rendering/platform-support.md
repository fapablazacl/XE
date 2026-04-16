# Platform / Backend Availability and Console Target Notes

Documents which rendering backends are available on which platforms, including per-platform conditions and footnotes. Also collects forward-looking notes on console targets not yet fully supported (Dreamcast, Xbox OG).

---

## Backend Availability Matrix

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

## Additional Console Target Notes

### Sega Dreamcast
**SDK:** KallistiOS + `sh-elf-gcc`. **Tier:** `Legacy`. GLdc (MIT) provides GL 1.2 over PVR
— the existing `GLLegacy` Glaze-backed backend could run on Dreamcast via GLdc with
minimal modifications. Future `BackendHint::Dreamcast_PVR`.

### Original Xbox
**SDK:** nxdk (MIT) + LLVM/Clang. **Tier:** `Legacy` / `Modern` boundary (NV2A supports
VS 1.1 / PS 1.4). Future `BackendHint::Xbox_D3D8`. Good candidate for a
`LegacyShader` sub-tier revision.

---

## See Also

- [system-caps.md](system-caps.md) — `TierLevel` and `BackendHint` enums that this matrix maps to
- [resources.md](resources.md) — `BackendRegistry` queries which backends are available at runtime
- [backend-abi.md](backend-abi.md) — factory signatures for each backend listed in the matrix
- [asset-pipeline.md](asset-pipeline.md) — `xe-asset-compiler` target list corresponds to this matrix
