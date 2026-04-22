
#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/math/Vector.h>
#include <xe/render/RenderBackend.h>
#include <cassert>
#include <new>
#include <utility>
#include <vector>
#include <array>

#include "glcore3-api.h"

namespace xe {
    namespace {
        inline RenderDeviceBackendContextGL *glctx(RenderDeviceBackendContext *ctx) {
            assert(ctx);
            return static_cast<RenderDeviceBackendContextGL *>(ctx);
        }

        // 16-bit index field in the handle bit layout; caps every pool at 65536 entries.
        constexpr uint32_t kHandleIndexLimit = 0x10000u;
        // 8-bit gen field in the handle bit layout; rolls over every 256 reuses per slot.
        constexpr uint32_t kHandleGenMask = 0xFFu;

        /**
         * @brief True if the pool has room for one more acquire (either unused capacity or a freed slot).
         *
         * Called by the public factories before acquireSlot; converts the hard 16-bit index cap into
         * a soft tl::expected error (BackendErrorCode::HandlePoolExhausted). Scan cost is O(n) only on
         * the overflow path, which is unreachable under normal usage.
         *
         * Slot-shape agnostic: works for any slot type exposing an `obj` field contextually convertible
         * to bool (Slot<T> via glaze::Unique, OptSlot<T> via std::optional).
         *
         * @tparam SlotT the concrete slot struct (Slot<T> or OptSlot<T>)
         * @param pool the pool to inspect; not mutated
         */
        template <class SlotT> bool poolHasCapacity(const std::vector<SlotT> &pool) {
            if (pool.size() < kHandleIndexLimit)
                return true;
            for (auto const &s : pool) {
                if (!s.obj)
                    return true;
            }
            return false;
        }

        /**
         * @brief Reuse an empty slot (or append a new one) and return (index, gen).
         *
         * Linear-scans pool looking for a slot whose holder is empty - those are free slots awaiting
         * reuse. On hit, moves obj into that slot and bumps its gen counter. On miss, appends a new
         * slot. Scan cost is O(n); resource creation is not on the frame hot path so this is
         * intentional.
         *
         * Callers must ensure poolHasCapacity(pool) is true before calling; the tail assert is a
         * defensive invariant, not a user-facing failure path.
         *
         * Slot-shape agnostic: accepts any slot type whose `obj` field is assignable from ObjT and
         * contextually convertible to bool (Slot<T> holding glaze::Unique<T>, or OptSlot<T> holding
         * std::optional<T>).
         *
         * @tparam SlotT the concrete slot struct (Slot<T> or OptSlot<T>)
         * @tparam ObjT type of the resource holder being installed (deduced)
         * @param pool the pool vector to acquire a slot in; grows if no free slot is available
         * @param obj resource holder to install at the acquired slot
         * @return {index, gen} pair ready to be passed to HandleT::make
         */
        template <class SlotT, class ObjT> std::pair<uint32_t, uint8_t> acquireSlot(std::vector<SlotT> &pool, ObjT &&obj) {
            for (uint32_t i = 0; i < pool.size(); ++i) {
                if (!pool[i].obj) {
                    pool[i].obj = std::forward<ObjT>(obj);
                    pool[i].gen = static_cast<uint8_t>((pool[i].gen + 1) & kHandleGenMask);
                    return {i, pool[i].gen};
                }
            }
            assert(pool.size() < kHandleIndexLimit && "acquireSlot: pool exhausted - caller skipped poolHasCapacity");
            pool.push_back({std::forward<ObjT>(obj), 0});
            return {static_cast<uint32_t>(pool.size() - 1), 0};
        }
    } // namespace

    tl::expected<RenderDeviceBackendContext *, BackendError> createContextGL() {
        auto *ctx = new (std::nothrow) RenderDeviceBackendContextGL();
        if (!ctx) {
            return makeBackendError(BackendErrorCode::AllocationFailed, "failed to allocate RenderDeviceBackendContextGL");
        }
        return ctx;
    }

    void destroyContextGL(RenderDeviceBackendContext *ctx) {
        delete glctx(ctx);
    }

    inline const void *mipLevelDataAt(const TextureDescriptor &desc, size_t mip, size_t face, size_t faceCount) {
        if (desc.mipLevels == nullptr) {
            return nullptr;
        }
        return desc.mipLevels[mip * faceCount + face].data;
    }

    inline int halveDimension(int size) {
        return size > 1 ? size / 2 : 1;
    }

    static constexpr std::array<gl::TextureTarget, 6> cubeMapSides{
        gl::TextureTarget::eTextureCubeMapPositiveX,
        gl::TextureTarget::eTextureCubeMapNegativeX,
        gl::TextureTarget::eTextureCubeMapPositiveY,
        gl::TextureTarget::eTextureCubeMapNegativeY,
        gl::TextureTarget::eTextureCubeMapPositiveZ,
        gl::TextureTarget::eTextureCubeMapNegativeZ,
    };

    gl::BufferTarget toBufferTargetGL(BufferType type) {
        switch (type) {
        case BufferType::Vertex:
            return gl::BufferTarget::eArrayBuffer;

        case BufferType::Index:
            return gl::BufferTarget::eElementArrayBuffer;

        default:
            assert(false && "toTextureTargetGL: Invalid BufferType");
            return gl::BufferTarget::eArrayBuffer;
        }
    }

    gl::BufferUsage toBufferUsageGL(const BufferUsage usage) {
        switch (usage) {
        case BufferUsage::StreamDraw:
            return gl::BufferUsage::eStreamDraw;
        case BufferUsage::StreamRead:
            return gl::BufferUsage::eStreamRead;
        case BufferUsage::StreamCopy:
            return gl::BufferUsage::eStreamCopy;
        case BufferUsage::StaticDraw:
            return gl::BufferUsage::eStaticDraw;
        case BufferUsage::StaticRead:
            return gl::BufferUsage::eStaticRead;
        case BufferUsage::StaticCopy:
            return gl::BufferUsage::eStaticCopy;
        case BufferUsage::DynamicDraw:
            return gl::BufferUsage::eDynamicDraw;
        case BufferUsage::DynamicRead:
            return gl::BufferUsage::eDynamicRead;
        case BufferUsage::DynamicCopy:
            return gl::BufferUsage::eDynamicCopy;
        }

        assert(false && "toBufferUsageGL: Invalid BufferUsage");
        return gl::BufferUsage::eStreamDraw;
    }

    tl::expected<BufferHandle, BackendError> createBufferGL(RenderDeviceBackendContext *ctx, const BufferDescriptor &desc) {
        auto &buffers = glctx(ctx)->buffers;

        if (!poolHasCapacity(buffers)) {
            return makeBackendError(BackendErrorCode::HandlePoolExhausted, "buffer pool exhausted (16-bit index field)");
        }

        gl::BufferTarget const target = toBufferTargetGL(desc.type);
        gl::BufferUsage const usage = toBufferUsageGL(desc.usage);

        auto buffer = glaze::makeUnique<gl::BufferId>();
        gl::bindBuffer(target, buffer);
        gl::bufferData(target, desc.size, desc.data, usage);

        auto const [index, gen] = acquireSlot(buffers, std::move(buffer));
        return BufferHandle::make(gen, index, desc.type);
    }

    void destroyBufferGL(RenderDeviceBackendContext *ctx, BufferHandle handle) {
        auto &buffers = glctx(ctx)->buffers;
        uint32_t const index = handle.index();
        assert(index < buffers.size() && "destroyBufferGL: handle index out of range");
        auto &slot = buffers[index];
        assert(slot.obj && "destroyBufferGL: slot already free (double destroy)");
        assert(slot.gen == handle.gen() && "destroyBufferGL: stale handle (generation mismatch)");

        slot.obj.reset({});
    }

    void readBufferGL(RenderDeviceBackendContext *ctx, BufferHandle handle, const BufferReadDescriptor &desc) {
        assert(desc.data != nullptr && "BufferReadDescriptor: data must not be null");
        assert(desc.size > 0 && "BufferReadDescriptor: size must be greater than zero");

        auto &buffers = glctx(ctx)->buffers;
        uint32_t const index = handle.index();
        assert(index < buffers.size() && "readBufferGL: handle index out of range");
        auto &slot = buffers[index];
        assert(slot.obj && "readBufferGL: use of freed handle");
        assert(slot.gen == handle.gen() && "readBufferGL: stale handle (generation mismatch)");

        gl::BufferTarget const target = toBufferTargetGL(handle.subType());

        gl::bindBuffer(target, slot.obj);
        gl::getBufferSubData(target, static_cast<GLintptr>(desc.offset), static_cast<GLsizeiptr>(desc.size), desc.data);
    }

    gl::TextureTarget toTextureTargetGL(const TextureType type) {
        switch (type) {
        case TextureType::Tex1D:
            return gl::TextureTarget::eTexture1d;
        case TextureType::Tex2D:
            return gl::TextureTarget::eTexture2d;
        case TextureType::Tex3D:
            return gl::TextureTarget::eTexture3d;
        case TextureType::TexCubeMap:
            return gl::TextureTarget::eTextureCubeMap;
        case TextureType::Tex2DArray:
            return gl::TextureTarget::eTexture2dArray;
        }
        assert(false && "toTextureTargetGL: Invalid TextureType");
        return gl::TextureTarget::eTexture2d;
    }

    gl::InternalFormat toInternalFormatGL(const PixelFormat pixelFormat) {
        switch (pixelFormat) {
        case PixelFormat::R8G8B8:
            return gl::InternalFormat::eRgb;
        case PixelFormat::R8G8B8A8:
            return gl::InternalFormat::eRgba;
        default:
            break;
        }
        assert(false && "toInternalFormatGL: Invalid PixelFormat");
        return gl::InternalFormat::eRgba;
    }

    gl::PixelFormat toPixelFormatGL(const PixelFormat pixelFormat) {
        switch (pixelFormat) {
        case PixelFormat::R8G8B8:
            return gl::PixelFormat::eRgb;
        case PixelFormat::R8G8B8A8:
            return gl::PixelFormat::eRgba;
        default:
            break;
        }
        assert(false && "toPixelFormatGL: Invalid PixelFormat");
        return gl::PixelFormat::eRgba;
    }

    gl::PixelType toPixelTypeGL(const DataType dataType) {
        switch (dataType) {
        case DataType::Int8:
            return gl::PixelType::eByte;
        case DataType::UInt8:
            return gl::PixelType::eUnsignedByte;
        case DataType::Int16:
            return gl::PixelType::eShort;
        case DataType::UInt16:
            return gl::PixelType::eUnsignedShort;
        case DataType::Int32:
            return gl::PixelType::eInt;
        case DataType::UInt32:
            return gl::PixelType::eUnsignedInt;
        case DataType::Float32:
            return gl::PixelType::eFloat;
        case DataType::Float16:
            return gl::PixelType::eHalfFloat;
        }
        assert(false && "toPixelTypeGL: Invalid DataType");
        return gl::PixelType::eUnsignedByte;
    }

    gl::DrawElementsType toDrawElementsTypeGL(const GeometryIndexType indexType) {
        switch (indexType) {
        case GeometryIndexType::uint16:
            return gl::DrawElementsType::eUnsignedShort;
        case GeometryIndexType::uint32:
            return gl::DrawElementsType::eUnsignedInt;
        }

        assert(false && "toDrawElementsTypeGL: Invalid GeometryIndexType");
        return gl::DrawElementsType::eUnsignedShort;
    }

    gl::AttributeType toAttributeTypeGL(const VertexAttribFormat attributeType) {
        switch (attributeType) {
        case VertexAttribFormat::float1: return gl::AttributeType::eFloat;
        case VertexAttribFormat::float2: return gl::AttributeType::eFloatVec2;
        case VertexAttribFormat::float3: return gl::AttributeType::eFloatVec3;
        case VertexAttribFormat::float4: return gl::AttributeType::eFloatVec4;
        case VertexAttribFormat::int1: return gl::AttributeType::eInt;
        case VertexAttribFormat::int2: return gl::AttributeType::eIntVec2;
        case VertexAttribFormat::int3: return gl::AttributeType::eIntVec3;
        case VertexAttribFormat::int4: return gl::AttributeType::eIntVec4;
        }

        assert(false && "toPixelTypeGL: Invalid DataType");
        return gl::AttributeType::eFloat;
    }

    gl::VertexAttribPointerType toVertexAttribPointerTypeGL(const VertexAttribFormat attributeType) {
        switch (attributeType) {
        case VertexAttribFormat::float1:
        case VertexAttribFormat::float2:
        case VertexAttribFormat::float3:
        case VertexAttribFormat::float4:
            return gl::VertexAttribPointerType::eFloat;

        case VertexAttribFormat::int1:
        case VertexAttribFormat::int2:
        case VertexAttribFormat::int3:
        case VertexAttribFormat::int4:
            return gl::VertexAttribPointerType::eInt;
        }

        assert(false && "toPixelTypeGL: Invalid DataType");
        return gl::VertexAttribPointerType::eInt;
    }

    int getVertexAttribDim(const VertexAttribFormat attributeType) {
        switch (attributeType) {
        case VertexAttribFormat::float1:
        case VertexAttribFormat::int1:
            return 1;

        case VertexAttribFormat::float2:
        case VertexAttribFormat::int2:
            return 2;

        case VertexAttribFormat::float3:
        case VertexAttribFormat::int3:
            return 3;

        case VertexAttribFormat::float4:
        case VertexAttribFormat::int4:
            return 4;
        }

        assert(false && "toPixelTypeGL: Unhandled or Invalid DataType");
        return 1;
    }

    tl::expected<TextureHandle, BackendError> createTextureGL(RenderDeviceBackendContext *ctx, const TextureDescriptor &desc) {
        auto &textures = glctx(ctx)->textures;
        if (!poolHasCapacity(textures)) {
            return makeBackendError(BackendErrorCode::HandlePoolExhausted, "texture pool exhausted (16-bit index field)");
        }

        size_t const faceCount = (desc.type == TextureType::TexCubeMap) ? 6u : 1u;
        size_t const mipCount = desc.mipLevelCount == 0 ? 1u : (desc.mipLevelCount / faceCount);

        if (mipCount > 1 && desc.generateMipmaps) {
            return makeBackendError(BackendErrorCode::InvalidDescriptor, "TextureDescriptor: generateMipmaps is mutually exclusive with supplying >1 mip level");
        }
        if (desc.mipLevels != nullptr && desc.mipLevelCount != mipCount * faceCount) {
            return makeBackendError(BackendErrorCode::InvalidDescriptor, "TextureDescriptor: mipLevelCount must equal mipCount * faceCount");
        }

        auto texture = glaze::makeUnique<gl::Texture>();

        gl::TextureTarget const target = toTextureTargetGL(desc.type);
        gl::InternalFormat const internalFormat = toInternalFormatGL(desc.format);
        gl::PixelFormat const pixelFormat = toPixelFormatGL(desc.sourceFormat);
        gl::PixelType const pixelType = toPixelTypeGL(desc.sourceDataType);

        gl::bindTexture(target, texture);

        int w = desc.size.x;
        int h = desc.size.y;
        int d = desc.size.z;

        for (size_t mip = 0; mip < mipCount; ++mip) {
            GLint const level = static_cast<GLint>(mip);

            switch (desc.type) {
            case TextureType::Tex1D:
                gl::texImage1D(target, level, internalFormat, w, 0, pixelFormat, pixelType, mipLevelDataAt(desc, mip, 0, faceCount));
                break;

            case TextureType::Tex2D:
                gl::texImage2D(target, level, internalFormat, w, h, 0, pixelFormat, pixelType, mipLevelDataAt(desc, mip, 0, faceCount));
                break;

            case TextureType::Tex3D:
            case TextureType::Tex2DArray:
                gl::texImage3D(target, level, internalFormat, w, h, d, 0, pixelFormat, pixelType, mipLevelDataAt(desc, mip, 0, faceCount));
                break;

            case TextureType::TexCubeMap:
                for (size_t face = 0; face < 6u; ++face) {
                    gl::texImage2D(cubeMapSides[face], level, internalFormat, w, h, 0, pixelFormat, pixelType, mipLevelDataAt(desc, mip, face, faceCount));
                }
                break;

            default:
                return makeBackendError(BackendErrorCode::InternalError, "createTextureGL: unknown TextureType");
            }

            w = halveDimension(w);
            h = halveDimension(h);
            d = halveDimension(d);
        }

        if (desc.generateMipmaps) {
            gl::generateMipmap(target);
        } else {
            gl::texParameteri(target, gl::TextureParameterName::eTextureBaseLevel, 0);
            gl::texParameteri(target, gl::TextureParameterName::eTextureMaxLevel, static_cast<GLint>(mipCount - 1));
        }

        // TODO: Use a Sampler Resource instead for later
        bool const hasMips = desc.generateMipmaps || mipCount > 1;

        GLint const param = hasMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
        gl::texParameteri(target, gl::TextureParameterName::eTextureMinFilter, param);
        gl::texParameteri(target, gl::TextureParameterName::eTextureMagFilter, GL_LINEAR);
        gl::texParameteri(target, gl::TextureParameterName::eTextureWrapS, GL_REPEAT);
        gl::texParameteri(target, gl::TextureParameterName::eTextureWrapT, GL_REPEAT);
        gl::texParameteri(target, gl::TextureParameterName::eTextureWrapR, GL_REPEAT);

        auto const [index, gen] = acquireSlot(textures, std::move(texture));
        return TextureHandle::make(gen, index, desc.type);
    }

    void destroyTextureGL(RenderDeviceBackendContext *ctx, TextureHandle handle) {
        auto &textures = glctx(ctx)->textures;
        uint32_t const index = handle.index();
        assert(index < textures.size() && "destroyTextureGL: handle index out of range");
        auto &slot = textures[index];
        assert(slot.obj && "destroyTextureGL: slot already free (double destroy)");
        assert(slot.gen == handle.gen() && "destroyTextureGL: stale handle (generation mismatch)");
        slot.obj.reset({});
    }

    void updateTextureGL(RenderDeviceBackendContext *ctx, TextureHandle handle, const TextureUpdateDescriptor &desc) {
        auto &textures = glctx(ctx)->textures;
        uint32_t const index = handle.index();
        assert(index < textures.size() && "updateTextureGL: handle index out of range");
        auto &slot = textures[index];
        assert(slot.obj && "updateTextureGL: use of freed handle");
        assert(slot.gen == handle.gen() && "updateTextureGL: stale handle (generation mismatch)");

        TextureType const type = handle.subType();
        gl::TextureTarget const target = toTextureTargetGL(type);
        gl::PixelFormat const pixelFormat = toPixelFormatGL(desc.sourceFormat);
        gl::PixelType const pixelType = toPixelTypeGL(desc.sourceDataType);

        GLint const level = desc.mipLevel;
        GLint const x = desc.offset.x;
        GLint const y = desc.offset.y;
        GLint const z = desc.offset.z;
        GLsizei const w = desc.size.x;
        GLsizei const h = desc.size.y;
        GLsizei const d = desc.size.z;

        gl::bindTexture(target, slot.obj);

        switch (type) {
        case TextureType::Tex1D:
            gl::texSubImage1D(target, level, x, w, pixelFormat, pixelType, desc.sourceData);
            break;

        case TextureType::Tex2D:
            gl::texSubImage2D(target, level, x, y, w, h, pixelFormat, pixelType, desc.sourceData);
            break;

        case TextureType::Tex3D:
        case TextureType::Tex2DArray:
            gl::texSubImage3D(target, level, x, y, z, w, h, d, pixelFormat, pixelType, desc.sourceData);
            break;

        case TextureType::TexCubeMap:
            assert(desc.faceIndex >= 0 && desc.faceIndex < 6 && "TextureUpdateDescriptor: faceIndex out of range");
            gl::texSubImage2D(cubeMapSides[desc.faceIndex], level, x, y, w, h, pixelFormat, pixelType, desc.sourceData);
            break;

        default:
            assert(false && "TextureType is unknown");
        }
    }

    void  readTextureGL(RenderDeviceBackendContext *ctx, TextureHandle handle, const TextureReadDescriptor &desc) {
        assert(desc.data != nullptr && "TextureReadDescriptor: data must not be null");
        assert(desc.offset.x == 0 && desc.offset.y == 0 && desc.offset.z == 0 && "TextureReadDescriptor: GL 3.3 backend requires offset == {0,0,0}");

        auto &textures = glctx(ctx)->textures;
        uint32_t const index = handle.index();
        assert(index < textures.size() && "readTextureGL: handle index out of range");
        auto &slot = textures[index];
        assert(slot.obj && "readTextureGL: use of freed handle");
        assert(slot.gen == handle.gen() && "readTextureGL: stale handle (generation mismatch)");

        TextureType const type = handle.subType();
        gl::TextureTarget const bindTarget = toTextureTargetGL(type);
        gl::PixelFormat const pixelFormat = toPixelFormatGL(desc.destFormat);
        gl::PixelType const pixelType = toPixelTypeGL(desc.destDataType);
        gl::TextureTarget const readTarget = (type == TextureType::TexCubeMap) ? cubeMapSides[desc.faceIndex] : bindTarget;

        if (type == TextureType::TexCubeMap) {
            assert(desc.faceIndex >= 0 && desc.faceIndex < 6 && "TextureReadDescriptor: faceIndex out of range");
        }

        gl::bindTexture(bindTarget, slot.obj);

#ifndef NDEBUG
        GLint actualWidth = 0;
        GLint actualHeight = 0;
        gl::getTexLevelParameteriv(readTarget, desc.mipLevel, gl::GetTextureParameter::eTextureWidth, &actualWidth);
        gl::getTexLevelParameteriv(readTarget, desc.mipLevel, gl::GetTextureParameter::eTextureHeight, &actualHeight);
        assert(actualWidth == desc.size.x && "TextureReadDescriptor: GL 3.3 backend requires size.x to match the mip's full width");
        assert((type == TextureType::Tex1D || actualHeight == desc.size.y) && "TextureReadDescriptor: GL 3.3 backend requires size.y to match the mip's full height");
#endif

        gl::getTexImage(readTarget, desc.mipLevel, pixelFormat, pixelType, desc.data);
    }

    /**
     * @brief Compile a single shader stage. Always returns a valid glaze::Unique; compile status
     * lives on the shader itself and must be inspected by the caller (getShaderiv + getShaderInfoLog).
     */
    static glaze::Unique<gl::Shader> compileShader(gl::ShaderType type, const char *src) {
        auto shader = glaze::makeUnique<gl::Shader>(type);
        gl::shaderSource(shader, 1, &src, nullptr);
        gl::compileShader(shader);
        return shader;
    }

    /**
     * @brief Attach/link the given shaders into a fresh program. Always returns a valid glaze::Unique;
     * link status lives on the program itself and must be inspected by the caller
     * (getProgramiv + getProgramInfoLog).
     */
    static glaze::Unique<gl::Program> linkProgram(const std::vector<glaze::Unique<gl::Shader>> &shaders) {
        auto program = glaze::makeUnique<gl::Program>();

        for (auto const &s : shaders) {
            gl::attachShader(program, s);
        }

        gl::linkProgram(program);

        for (auto const &s : shaders) {
            gl::detachShader(program, s);
        }

        return program;
    }

    tl::expected<ShaderHandle, BackendError> createShaderProgramGL(RenderDeviceBackendContext *ctx, const ShaderProgramDescriptor &desc) {
        auto &shaderPrograms = glctx(ctx)->shaderPrograms;

        auto vs = compileShader(gl::ShaderType::eVertexShader, desc.glslVertexShader.c_str());
        if (!gl::getShaderiv(vs, gl::ShaderParameterName::eCompileStatus)) {
            return makeBackendError(BackendErrorCode::ShaderCompileFailed, gl::getShaderInfoLog(vs));
        }

        auto fs = compileShader(gl::ShaderType::eFragmentShader, desc.glslFragmentShader.c_str());
        if (!gl::getShaderiv(fs, gl::ShaderParameterName::eCompileStatus)) {
            return makeBackendError(BackendErrorCode::ShaderCompileFailed, gl::getShaderInfoLog(fs));
        }

        std::vector<glaze::Unique<gl::Shader>> shaders;
        shaders.push_back(std::move(vs));
        shaders.push_back(std::move(fs));

        auto program = linkProgram(shaders);
        if (!gl::getProgramiv(program, gl::ProgramProperty::eLinkStatus)) {
            return makeBackendError(BackendErrorCode::ShaderLinkFailed, gl::getProgramInfoLog(program));
        }

        if (!poolHasCapacity(shaderPrograms)) {
            return makeBackendError(BackendErrorCode::HandlePoolExhausted, "shader program pool exhausted (16-bit index field)");
        }

        auto const [index, gen] = acquireSlot(shaderPrograms, std::move(program));
        return ShaderHandle::make(gen, index);
    }

    void destroyShaderProgramGL(RenderDeviceBackendContext *ctx, ShaderHandle handle) {
        auto &shaderPrograms = glctx(ctx)->shaderPrograms;
        uint32_t const index = handle.index();
        assert(index < shaderPrograms.size() && "destroyShaderProgramGL: handle index out of range");
        auto &slot = shaderPrograms[index];
        assert(slot.obj && "destroyShaderProgramGL: slot already free (double destroy)");
        assert(slot.gen == handle.gen() && "destroyShaderProgramGL: stale handle (generation mismatch)");
        slot.obj.reset({});
    }

    tl::expected<VertexLayoutHandle, BackendError>
    createVertexLayoutGL(RenderDeviceBackendContext *ctx, const VertexLayoutDescriptor &desc) {
        auto &layouts = glctx(ctx)->layouts;

        if (!poolHasCapacity(layouts)) {
            return makeBackendError(BackendErrorCode::HandlePoolExhausted, "vertex layout pool exhausted (16-bit index field)");
        }

        VertexLayoutGL layout;
        layout.attributes.reserve(desc.attribs.size());
        layout.indexDataType = toDrawElementsTypeGL(desc.indexType);

        for (const VertexAttrib &attr : desc.attribs) {
            layout.attributes.emplace_back(
                gl::AttribLocation{attr.location},
                getVertexAttribDim(attr.format),
                toVertexAttribPointerTypeGL(attr.format),
                attr.normalized ? GL_TRUE : GL_FALSE
            );
        }

        auto const [index, gen] = acquireSlot(layouts, std::move(layout));
        return VertexLayoutHandle::make(gen, index);
    }

    void destroyVertexLayoutGL(RenderDeviceBackendContext *ctx, VertexLayoutHandle handle) {
        auto &layouts = glctx(ctx)->layouts;
        uint32_t const index = handle.index();
        assert(index < layouts.size() && "destroyVertexLayoutGL: handle index out of range");
        auto &slot = layouts[index];
        assert(slot.obj && "destroyVertexLayoutGL: slot already free (double destroy)");
        assert(slot.gen == handle.gen() && "destroyVertexLayoutGL: stale handle (generation mismatch)");
        slot.obj.reset();
    }


    tl::expected<GeometryHandle, BackendError> createGeometryGL(RenderDeviceBackendContext *ctx, const GeometryDescriptor &desc) {
        uint32_t const layoutIndex = desc.layoutHandle.index();

        // TODO: Refactor this into a search utility function. tryObjectExtract currently is failing
        // auto layoutOpt = tryObjectExtract<VertexLayoutGL, VertexLayoutHandle>(glctx(ctx)->layouts, desc.layoutHandle);
        auto& buffers = glctx(ctx)->buffers;
        std::optional<VertexLayoutGL> layoutOpt = glctx(ctx)->layouts[layoutIndex].obj;

        // TODO: Add error checking
        if (!layoutOpt) {

        }

        const VertexLayoutGL &layout = layoutOpt.value();

        auto vao = glaze::makeUnique<gl::VertexArray>();

        gl::bindVertexArray(vao);
        for (const GeometryBufferAttrib &bufferAttrib : desc.bufferAttribs) {
            const uint32_t attribIndex = bufferAttrib.attribIndex;
            VertexAttribGL const &attrib = layoutOpt->attributes[attribIndex];

            BufferHandle bufferHandle = bufferAttrib.bufferHandle;
            gl::BufferId const bufferId = buffers[bufferHandle.index()].obj.get();

            gl::bindBuffer(gl::BufferTarget::eArrayBuffer, bufferId);
            gl::enableVertexArrayAttrib(vao, attrib.loc.loc);
            gl::vertexAttribPointer(attrib.loc, attrib.dim, attrib.dataType, attrib.normalized, 0, nullptr);
        }
        gl::bindVertexArray({});
    }

    void destroyGeometryGL(RenderDeviceBackendContext *ctx, GeometryHandle handle) {

    }

    void initializeBackendTableGL(RenderDeviceBackendVTable *vtable) {
        vtable->createContext = &createContextGL;
        vtable->destroyContext = &destroyContextGL;
        vtable->createBuffer = &createBufferGL;
        vtable->destroyBuffer = &destroyBufferGL;
        vtable->readBuffer = &readBufferGL;
        vtable->createShaderProgram = &createShaderProgramGL;
        vtable->destroyShaderProgram = &destroyShaderProgramGL;
        vtable->createTexture = &createTextureGL;
        vtable->destroyTexture = &destroyTextureGL;
        vtable->updateTexture = &updateTextureGL;
        vtable->readTexture = &readTextureGL;
        vtable->createVertexLayout = &createVertexLayoutGL;
        vtable->destroyVertexLayout = &destroyVertexLayoutGL;
    }
} // namespace xe
