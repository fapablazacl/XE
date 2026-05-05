
// TODO: Refactor the handle assert validation into generic functions that works across different Handle types

#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/math/Vector.h>
#include <xe/render/RenderBackend.h>
#include <cassert>
#include <new>
#include <string>
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

        case BufferType::Uniform:
            return gl::BufferTarget::eUniformBuffer;

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
        gl::bindBuffer(target, {});

        auto const [index, gen] = acquireSlot(buffers, BufferGL{std::move(buffer)});
        return BufferHandle::make(gen, index, desc.type);
    }

    void destroyBufferGL(RenderDeviceBackendContext *ctx, BufferHandle handle) {
        auto &buffers = glctx(ctx)->buffers;
        uint32_t const index = handle.index();
        assert(index < buffers.size() && "destroyBufferGL: handle index out of range");
        auto &slot = buffers[index];
        assert(slot.obj && "destroyBufferGL: slot already free (double destroy)");
        assert(slot.gen == handle.gen() && "destroyBufferGL: stale handle (generation mismatch)");

        slot.obj.reset();
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

        gl::bindBuffer(target, slot.obj->buffer);
        gl::getBufferSubData(target, static_cast<GLintptr>(desc.offset), static_cast<GLsizeiptr>(desc.size), desc.data);
        gl::bindBuffer(target, {});
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

    gl::PixelType toPixelTypeGL(const PixelDataType dataType) {
        const TypeEncoding te = static_cast<TypeEncoding>(dataType);
        const TypeKind kind = getTypeKind(te);
        const TypeSize size = getElementSize(te);
        if (kind == TypeKind::Float) {
            return size == TypeSize::Byte2 ? gl::PixelType::eHalfFloat : gl::PixelType::eFloat;
        }
        if (kind == TypeKind::UInt) {
            if (size == TypeSize::Byte1) return gl::PixelType::eUnsignedByte;
            if (size == TypeSize::Byte2) return gl::PixelType::eUnsignedShort;
            return gl::PixelType::eUnsignedInt;
        }
        // Int
        if (size == TypeSize::Byte1) return gl::PixelType::eByte;
        if (size == TypeSize::Byte2) return gl::PixelType::eShort;
        return gl::PixelType::eInt;
    }

    gl::DrawElementsType toDrawElementsTypeGL(const GeometryIndexType indexType) {
        const TypeSize size = getElementSize(static_cast<TypeEncoding>(indexType));
        return size == TypeSize::Byte2 ? gl::DrawElementsType::eUnsignedShort : gl::DrawElementsType::eUnsignedInt;
    }

    gl::AttributeType toAttributeTypeGL(const VertexAttribFormat attributeType) {
        const TypeEncoding te = static_cast<TypeEncoding>(attributeType);
        const uint8_t cols = getTypeCols(te);
        if (getTypeKind(te) == TypeKind::Float) {
            constexpr gl::AttributeType floatTypes[] = {gl::AttributeType::eFloat, gl::AttributeType::eFloatVec2, gl::AttributeType::eFloatVec3, gl::AttributeType::eFloatVec4};
            return floatTypes[cols - 1];
        }
        constexpr gl::AttributeType intTypes[] = {gl::AttributeType::eInt, gl::AttributeType::eIntVec2, gl::AttributeType::eIntVec3, gl::AttributeType::eIntVec4};
        return intTypes[cols - 1];
    }

    gl::VertexAttribPointerType toVertexAttribPointerTypeGL(const VertexAttribFormat attributeType) {
        const TypeKind kind = getTypeKind(static_cast<TypeEncoding>(attributeType));
        return kind == TypeKind::Float ? gl::VertexAttribPointerType::eFloat : gl::VertexAttribPointerType::eInt;
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
        gl::texParameteri(target, gl::TextureParameterName::eTextureWrapS, GL_CLAMP_TO_EDGE);

        if (h > 0) {
            gl::texParameteri(target, gl::TextureParameterName::eTextureWrapT, GL_CLAMP_TO_EDGE);
        }

        if (d > 0) {
            gl::texParameteri(target, gl::TextureParameterName::eTextureWrapR, GL_CLAMP_TO_EDGE);
        }

        gl::bindTexture(target, {});

        auto const [index, gen] = acquireSlot(textures, TextureGL{std::move(texture)});
        return TextureHandle::make(gen, index, desc.type);
    }

    void destroyTextureGL(RenderDeviceBackendContext *ctx, TextureHandle handle) {
        auto &textures = glctx(ctx)->textures;
        uint32_t const index = handle.index();
        assert(index < textures.size() && "destroyTextureGL: handle index out of range");
        auto &slot = textures[index];
        assert(slot.obj && "destroyTextureGL: slot already free (double destroy)");
        assert(slot.gen == handle.gen() && "destroyTextureGL: stale handle (generation mismatch)");
        slot.obj.reset();
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

        gl::bindTexture(target, slot.obj->texture);

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

        gl::bindTexture(target, {});
    }

    void readTextureGL(RenderDeviceBackendContext *ctx, TextureHandle handle, const TextureReadDescriptor &desc) {
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

        gl::bindTexture(bindTarget, slot.obj->texture);

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

        auto const [index, gen] = acquireSlot(shaderPrograms, ProgramGL{std::move(program)});
        return ShaderHandle::make(gen, index);
    }

    void destroyShaderProgramGL(RenderDeviceBackendContext *ctx, ShaderHandle handle) {
        auto &shaderPrograms = glctx(ctx)->shaderPrograms;
        uint32_t const index = handle.index();
        assert(index < shaderPrograms.size() && "destroyShaderProgramGL: handle index out of range");
        auto &slot = shaderPrograms[index];
        assert(slot.obj && "destroyShaderProgramGL: slot already free (double destroy)");
        assert(slot.gen == handle.gen() && "destroyShaderProgramGL: stale handle (generation mismatch)");
        slot.obj.reset();
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
                static_cast<int>(getTypeCols(static_cast<TypeEncoding>(attr.format))),
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
        auto &geometries = glctx(ctx)->geometries;
        auto &buffers = glctx(ctx)->buffers;
        auto &layouts = glctx(ctx)->layouts;

        if (!poolHasCapacity(geometries)) {
            return makeBackendError(BackendErrorCode::HandlePoolExhausted, "geometry pool exhausted (16-bit index field)");
        }

        const VertexLayoutGL *layout = tryObjectExtract<VertexLayoutGL>(layouts, desc.layoutHandle);
        if (layout == nullptr) {
            return makeBackendError(BackendErrorCode::InvalidDescriptor, "createGeometryGL: layoutHandle is invalid or references a freed layout");
        }

        // check buffer attribs
        for (const GeometryBufferAttrib &bufferAttrib : desc.bufferAttribs) {
            if (bufferAttrib.attribIndex >= layout->attributes.size()) {
                return makeBackendError(BackendErrorCode::InvalidDescriptor, "createGeometryGL: bufferAttrib.attribIndex out of range for the supplied layout");
            }
            BufferHandle const handle = bufferAttrib.bufferHandle;
            uint32_t const index = handle.index();
            if (index >= buffers.size() || !buffers[index].obj || buffers[index].gen != handle.gen()) {
                return makeBackendError(BackendErrorCode::InvalidDescriptor, "createGeometryGL: bufferAttrib.bufferHandle is invalid or references a freed buffer");
            }
        }

        uint32_t const indexBufIdx = desc.indexBufferHandle.index();

        // check index buffer
        if (indexBufIdx >= buffers.size() || !buffers[indexBufIdx].obj || buffers[indexBufIdx].gen != desc.indexBufferHandle.gen()) {
            return makeBackendError(BackendErrorCode::InvalidDescriptor, "createGeometryGL: indexBufferHandle is invalid or references a freed buffer");
        }

        GeometryGL geometry;
        geometry.vao = glaze::makeUnique<gl::VertexArray>();
        geometry.indexDataType = layout->indexDataType;

        gl::bindVertexArray(geometry.vao);

        for (const GeometryBufferAttrib &bufferAttrib : desc.bufferAttribs) {
            VertexAttribGL const &attrib = layout->attributes[bufferAttrib.attribIndex];
            gl::BufferId const bufferId = buffers[bufferAttrib.bufferHandle.index()].obj->buffer.get();

            gl::bindBuffer(gl::BufferTarget::eArrayBuffer, bufferId);
            gl::enableVertexAttribArray(attrib.loc);
            gl::vertexAttribPointer(attrib.loc, attrib.dim, attrib.dataType, attrib.normalized, 0, nullptr);
        }

        // The element array buffer binding is part of VAO state, so this sticks to the geometry.
        gl::bindBuffer(gl::BufferTarget::eElementArrayBuffer, buffers[indexBufIdx].obj->buffer.get());

        gl::bindVertexArray({});

        auto const [index, gen] = acquireSlot(geometries, std::move(geometry));
        return GeometryHandle::make(gen, index);
    }

    void destroyGeometryGL(RenderDeviceBackendContext *ctx, GeometryHandle handle) {
        auto &geometries = glctx(ctx)->geometries;
        uint32_t const index = handle.index();
        assert(index < geometries.size() && "destroyGeometryGL: handle index out of range");
        auto &slot = geometries[index];
        assert(slot.obj && "destroyGeometryGL: slot already free (double destroy)");
        assert(slot.gen == handle.gen() && "destroyGeometryGL: stale handle (generation mismatch)");
        slot.obj.reset();
    }

    /**
     * @brief Resolve a live gl::Program from a shader pool slot, or nullptr on validation failure.
     *
     * Performs the full index-range + slot-alive + generation-match check. Returns a borrowed
     * pointer into the pool; the pointer is valid until the next mutation of the shader-program pool.
     */
    static const ProgramGL *tryProgramExtract(const std::vector<OptSlot<ProgramGL>> &pool, ShaderHandle handle) {
        uint32_t const index = handle.index();
        if (index >= pool.size()) {
            return nullptr;
        }
        OptSlot<ProgramGL> const &slot = pool[index];
        if (!slot.obj) {
            return nullptr;
        }
        if (slot.gen != handle.gen()) {
            return nullptr;
        }
        return &*slot.obj;
    }

    tl::expected<PipelineHandle, BackendError> createPipelineGL(RenderDeviceBackendContext *ctx, const PipelineDescriptor &desc) {
        auto &pipelines = glctx(ctx)->pipelines;
        auto &shaderPrograms = glctx(ctx)->shaderPrograms;

        if (!poolHasCapacity(pipelines)) {
            return makeBackendError(BackendErrorCode::HandlePoolExhausted, "pipeline pool exhausted (16-bit index field)");
        }

        const ProgramGL *programPtr = tryProgramExtract(shaderPrograms, desc.shaderProgramHandle);
        if (programPtr == nullptr) {
            return makeBackendError(BackendErrorCode::InvalidDescriptor, "createPipelineGL: shaderProgramHandle is invalid or references a freed shader program");
        }

        PipelineGL pipeline;
        pipeline.clearColor = desc.clearColor;
        pipeline.shaderProgram = programPtr->program.get();
        pipeline.shaderHandle = desc.shaderProgramHandle;
        pipeline.uniformBlockBindings.reserve(desc.uniformBlocks.size());

        for (const PipelineUniformBlock &block : desc.uniformBlocks) {
            GLuint const blockIndex = gl::getUniformBlockIndex(pipeline.shaderProgram, block.blockName.c_str());
            if (blockIndex == GL_INVALID_INDEX) {
                return makeBackendError(BackendErrorCode::InvalidDescriptor,
                                        std::string{"createPipelineGL: uniform block '"} + block.blockName + "' is not active in the shader program");
            }

            gl::uniformBlockBinding(pipeline.shaderProgram, blockIndex, block.bindingPoint);
            pipeline.uniformBlockBindings.push_back({block.blockName, block.bindingPoint, blockIndex});
        }

        auto const [index, gen] = acquireSlot(pipelines, std::move(pipeline));
        return PipelineHandle::make(gen, index);
    }

    void destroyPipelineGL(RenderDeviceBackendContext *ctx, PipelineHandle handle) {
        auto &pipelines = glctx(ctx)->pipelines;
        uint32_t const index = handle.index();
        assert(index < pipelines.size() && "destroyPipelineGL: handle index out of range");
        auto &slot = pipelines[index];
        assert(slot.obj && "destroyPipelineGL: slot already free (double destroy)");
        assert(slot.gen == handle.gen() && "destroyPipelineGL: stale handle (generation mismatch)");
        slot.obj.reset();
    }

    tl::expected<UniformLocation, BackendError> resolveUniformLocationGL(RenderDeviceBackendContext *ctx, ShaderHandle handle, const char *name) {
        assert(name != nullptr && "resolveUniformLocationGL: name must not be null");

        auto &shaderPrograms = glctx(ctx)->shaderPrograms;
        const ProgramGL *programPtr = tryProgramExtract(shaderPrograms, handle);
        if (programPtr == nullptr) {
            return makeBackendError(BackendErrorCode::InvalidDescriptor, "resolveUniformLocationGL: shader handle is invalid or references a freed program");
        }

        gl::UniformLocation const loc = gl::getUniformLocation(programPtr->program.get(), name);
        if (!loc.valid()) {
            return makeBackendError(BackendErrorCode::InvalidDescriptor, std::string{"resolveUniformLocationGL: uniform '"} + name + "' is not active in the shader program");
        }

        UniformLocation out;
        out.raw = loc.loc;
        out.programKey = handle.raw;
        return out;
    }

    /**
     * @brief Dispatch a scalar/vector uniform upload to the correct glUniform{1,2,3,4}{f,i,ui}v call.
     *
     * The backend assumes the payload in `sub.data` is already in the memory layout the matching
     * glUniform*v entrypoint expects: count * getTypeCols(te) tightly-packed elements of
     * the C type implied by getTypeKind(te) (GLfloat / GLint / GLuint).
     */
    static void applyUniformGL(const UniformValueSubmission &sub) {
        gl::UniformLocation const loc{sub.location.raw};
        GLsizei const count = static_cast<GLsizei>(sub.count);
        const TypeEncoding te = static_cast<TypeEncoding>(sub.type);
        const uint8_t cols = getTypeCols(te);

        switch (getTypeKind(te)) {
        case TypeKind::Float: {
            auto const *data = static_cast<const GLfloat *>(sub.data);
            if (cols == 1) { gl::uniform1fv(loc, count, data); return; }
            if (cols == 2) { gl::uniform2fv(loc, count, data); return; }
            if (cols == 3) { gl::uniform3fv(loc, count, data); return; }
            if (cols == 4) { gl::uniform4fv(loc, count, data); return; }
            break;
        }
        case TypeKind::Int: {
            auto const *data = static_cast<const GLint *>(sub.data);
            if (cols == 1) { gl::uniform1iv(loc, count, data); return; }
            if (cols == 2) { gl::uniform2iv(loc, count, data); return; }
            if (cols == 3) { gl::uniform3iv(loc, count, data); return; }
            if (cols == 4) { gl::uniform4iv(loc, count, data); return; }
            break;
        }
        case TypeKind::UInt: {
            auto const *data = static_cast<const GLuint *>(sub.data);
            if (cols == 1) { gl::uniform1uiv(loc, count, data); return; }
            if (cols == 2) { gl::uniform2uiv(loc, count, data); return; }
            if (cols == 3) { gl::uniform3uiv(loc, count, data); return; }
            if (cols == 4) { gl::uniform4uiv(loc, count, data); return; }
            break;
        }
        }

        assert(false && "applyUniformValueGL: unhandled (kind, cols) pair");
    }

    /**
     * @brief Dispatch a matrix uniform upload to the correct glUniformMatrix*fv call.
     *
     * Data is always GLfloat; double-precision matrix uniforms are not part of this API surface.
     */
    static void applyUniformMatrixGL(const UniformMatrixSubmission &sub) {
        gl::UniformLocation const loc{sub.location.raw};
        GLsizei const count = static_cast<GLsizei>(sub.count);
        GLboolean const transpose = sub.transpose ? GL_TRUE : GL_FALSE;
        auto const *data = static_cast<const GLfloat *>(sub.data);
        const TypeEncoding te = static_cast<TypeEncoding>(sub.shape);
        const uint8_t cols = getTypeCols(te);
        const uint8_t rows = getTypeRows(te);

        if (cols == 2) {
            if (rows == 2) { gl::uniformMatrix2fv  (loc, count, transpose, data); return; }
            if (rows == 3) { gl::uniformMatrix2x3fv(loc, count, transpose, data); return; }
            if (rows == 4) { gl::uniformMatrix2x4fv(loc, count, transpose, data); return; }
        }
        if (cols == 3) {
            if (rows == 2) { gl::uniformMatrix3x2fv(loc, count, transpose, data); return; }
            if (rows == 3) { gl::uniformMatrix3fv  (loc, count, transpose, data); return; }
            if (rows == 4) { gl::uniformMatrix3x4fv(loc, count, transpose, data); return; }
        }
        if (cols == 4) {
            if (rows == 2) { gl::uniformMatrix4x2fv(loc, count, transpose, data); return; }
            if (rows == 3) { gl::uniformMatrix4x3fv(loc, count, transpose, data); return; }
            if (rows == 4) { gl::uniformMatrix4fv  (loc, count, transpose, data); return; }
        }

        assert(false && "applyUniformMatrixGL: unhandled (cols, rows) combination");
    }

    void applyUniformsGL(RenderDeviceBackendContext *ctx,
                         ShaderHandle handle,
                         const UniformValueSubmission *values, size_t valueCount,
                         const UniformMatrixSubmission *matrices, size_t matrixCount) {
        auto &shaderPrograms = glctx(ctx)->shaderPrograms;
        const ProgramGL *programPtr = tryProgramExtract(shaderPrograms, handle);
        assert(programPtr != nullptr && "applyUniformsGL: shader handle is invalid or references a freed program");

        gl::useProgram(programPtr->program.get());

        for (size_t i = 0; i < valueCount; ++i) {
            assert(values[i].location.isValid() && "applyUniformsGL: UniformValueSubmission carries an invalid location");
            assert(values[i].location.programKey == handle.raw && "applyUniformsGL: UniformValueSubmission location was resolved against a different program");
            assert(values[i].data != nullptr && "applyUniformsGL: UniformValueSubmission::data must not be null");
            applyUniformGL(values[i]);
        }

        for (size_t i = 0; i < matrixCount; ++i) {
            assert(matrices[i].location.isValid() && "applyUniformsGL: UniformMatrixSubmission carries an invalid location");
            assert(matrices[i].location.programKey == handle.raw && "applyUniformsGL: UniformMatrixSubmission location was resolved against a different program");
            assert(matrices[i].data != nullptr && "applyUniformsGL: UniformMatrixSubmission::data must not be null");
            applyUniformMatrixGL(matrices[i]);
        }
    }

    void bindUniformBufferGL(RenderDeviceBackendContext *ctx, uint32_t bindingPoint, BufferHandle handle, size_t offset, size_t size) {
        assert(handle.subType() == BufferType::Uniform && "bindUniformBufferGL: handle must reference a BufferType::Uniform buffer");

        auto &buffers = glctx(ctx)->buffers;
        uint32_t const index = handle.index();
        assert(index < buffers.size() && "bindUniformBufferGL: handle index out of range");
        auto &slot = buffers[index];
        assert(slot.obj && "bindUniformBufferGL: use of freed handle");
        assert(slot.gen == handle.gen() && "bindUniformBufferGL: stale handle (generation mismatch)");

        if (size == 0) {
            gl::bindBufferBase(gl::BufferTarget::eUniformBuffer, bindingPoint, slot.obj->buffer);
        } else {
            gl::bindBufferRange(gl::BufferTarget::eUniformBuffer, bindingPoint, slot.obj->buffer, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size));
        }
    }

    void clear(vec4 color, float depth, uint8_t stencil, gl::Flags<gl::ClearBufferMask> flags) {
        gl::clearColor(color.x, color.y, color.z, color.w);
        gl::clearDepth(depth);
        gl::clearStencil(stencil);

        gl::clear(flags);
    }

    void bindPipeline(RenderDeviceBackendContextGL *glctx, const PipelineGL &pipeline) {
        // TODO: Apply list of enable / render state, cull mode, etc
        gl::useProgram(pipeline.shaderProgram);
    }

    gl::TextureUnit getTextureUnit(const uint32_t unit) {
        assert(unit < 32);
        return static_cast<gl::TextureUnit>(static_cast<int>(gl::TextureUnit::eTexture0) + unit);
    }

    void bindTexture(RenderDeviceBackendContextGL *glctx, const uint32_t unit, gl::TextureTarget target, const gl::Texture &texture) {
        gl::activeTexture(getTextureUnit(unit));
        gl::bindTexture(target, texture);
    }

    void setUniform(const UniformValueSubmission *uniform, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            applyUniformGL(uniform[i]);
        }
    }

    void setUniformMatrix(const UniformMatrixSubmission *uniformMatrix, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            applyUniformMatrixGL(uniformMatrix[i]);
        }
    }

    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandClear &c) {
        // TODO: Add flags mapping to GL
        // clear(c.color, c.depth, c.depth, c.flags);
    }

    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandBindPipeline &c) {

    }


    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandBindTexture &c) {

    }


    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandSetUniform &c) {

    }


    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandSetUniformMatrix &c) {

    }


    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandDraw &c) {

    }

    void submitCommandGL(RenderDeviceBackendContext *ctx, const CommandBuffer::Command *command, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            switch (command[i].opcode) {
            case CommandOp::Clear: dispatch(glctx(ctx), command[i].cmd.clear); break;
            case CommandOp::BindPipeline: dispatch(glctx(ctx), command[i].cmd.bindPipeline); break;
            case CommandOp::BindTexture: dispatch(glctx(ctx), command[i].cmd.bindTexture); break;
            case CommandOp::SetUniform: dispatch(glctx(ctx), command[i].cmd.setUniform); break;
            case CommandOp::SetUniformMatrix: dispatch(glctx(ctx), command[i].cmd.setUniformMatrix); break;
            case CommandOp::Draw: dispatch(glctx(ctx), command[i].cmd.draw); break;
            }
        }
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
        vtable->createGeometry = &createGeometryGL;
        vtable->destroyGeometry = &destroyGeometryGL;
        vtable->createPipeline = &createPipelineGL;
        vtable->destroyPipeline = &destroyPipelineGL;
        vtable->resolveUniformLocation = &resolveUniformLocationGL;
        vtable->applyUniforms = &applyUniformsGL;
        vtable->bindUniformBuffer = &bindUniformBufferGL;
        vtable->submitCommand = &submitCommandGL;

    }
} // namespace xe
