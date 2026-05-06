
#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/render/RenderBackend.h>
#include <cassert>
#include <utility>

#include "glcore3-types.h"

namespace xe {
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

    tl::expected<VertexLayoutHandle, BackendError> createVertexLayoutGL(RenderDeviceBackendContext *ctx, const VertexLayoutDescriptor &desc) {
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
} // namespace xe
