
#include <xe/mesh/MeshUpload.h>

#include <cstddef>

namespace xe {
    namespace {
        struct AttribSpec {
            std::uint32_t flag;
            std::size_t elementByteSize;
            VertexAttribFormat format;
            int pointerOffset; // offset into MeshStorage<float> field triplet: 0 pos, 1 normal, 2 texcoord
        };

        constexpr std::array<AttribSpec, 3> kAttribSpecs = {{
            {MeshAttribPosition, sizeof(xe::vec3), VertexAttribFormat::float3, 0},
            {MeshAttribNormal, sizeof(xe::vec3), VertexAttribFormat::float3, 1},
            {MeshAttribTexCoord0, sizeof(xe::vec2), VertexAttribFormat::float2, 2},
        }};

        const void *attributePointer(const MeshStorage<float> &storage, int which) {
            switch (which) {
            case 0:
                return storage.positions;
            case 1:
                return storage.normals;
            case 2:
                return storage.texCoords;
            default:
                return nullptr;
            }
        }

        VertexAttribSemantic semanticFor(int which) {
            switch (which) {
            case 0:
                return VertexAttribSemantic::Position;
            case 1:
                return VertexAttribSemantic::Normal;
            case 2:
                return VertexAttribSemantic::TexCoord0;
            default:
                return VertexAttribSemantic::DontUse;
            }
        }

        GeometryIndexType toGeometryIndexType(IndexType type) {
            return type == IndexType::UInt16 ? GeometryIndexType::uint16 : GeometryIndexType::uint32;
        }
    } // namespace

    tl::expected<MeshGeometry, BackendError> uploadMesh(
        const RenderDeviceBackendVTable &vtable, RenderDeviceBackendContext *ctx, const MeshCounts &counts, const MeshStorage<float> &storage, const MeshUploadOptions &opts
    ) {
        MeshGeometry mesh{};

        VertexLayoutDescriptor layoutDesc;
        layoutDesc.resolveMode = VertexLayoutResolveMode::Explicit;
        layoutDesc.indexType = toGeometryIndexType(counts.indexType);

        std::vector<GeometryBufferAttrib> bufferAttribs;
        bufferAttribs.reserve(kAttribSpecs.size());

        for (std::size_t k = 0; k < kAttribSpecs.size(); ++k) {
            const AttribSpec &spec = kAttribSpecs[k];
            if ((opts.attribMask & spec.flag) == 0u) {
                continue;
            }

            const void *data = attributePointer(storage, spec.pointerOffset);
            if (data == nullptr) {
                destroyMeshGeometry(vtable, ctx, mesh);
                return makeBackendError(BackendErrorCode::InvalidDescriptor, "uploadMesh: attribute bit set in attribMask but matching MeshStorage pointer is null");
            }

            BufferDescriptor bufDesc{};
            bufDesc.type = BufferType::Vertex;
            bufDesc.usage = opts.usage;
            bufDesc.size = counts.vertexCount * spec.elementByteSize;
            bufDesc.data = data;

            auto bufResult = vtable.createBuffer(ctx, bufDesc);
            if (!bufResult) {
                destroyMeshGeometry(vtable, ctx, mesh);
                return tl::unexpected<BackendError>{bufResult.error()};
            }
            mesh.attributeBuffers.push_back(*bufResult);

            VertexAttrib attrib{};
            attrib.semantic = semanticFor(spec.pointerOffset);
            attrib.location = opts.locations[k];
            attrib.format = spec.format;
            attrib.normalized = false;
            layoutDesc.attribs.push_back(attrib);

            GeometryBufferAttrib ba{};
            ba.bufferHandle = *bufResult;
            ba.attribIndex = static_cast<uint32_t>(layoutDesc.attribs.size() - 1);
            bufferAttribs.push_back(ba);
        }

        if (counts.indexCount > 0) {
            if (storage.indices == nullptr) {
                destroyMeshGeometry(vtable, ctx, mesh);
                return makeBackendError(BackendErrorCode::InvalidDescriptor, "uploadMesh: counts.indexCount > 0 but storage.indices is null");
            }

            BufferDescriptor idxDesc{};
            idxDesc.type = BufferType::Index;
            idxDesc.usage = opts.usage;
            idxDesc.size = counts.indexCount * indexByteSize(counts.indexType);
            idxDesc.data = storage.indices;

            auto idxResult = vtable.createBuffer(ctx, idxDesc);
            if (!idxResult) {
                destroyMeshGeometry(vtable, ctx, mesh);
                return tl::unexpected<BackendError>{idxResult.error()};
            }
            mesh.indexBuffer = *idxResult;
        }

        auto layoutResult = vtable.createVertexLayout(ctx, layoutDesc);
        if (!layoutResult) {
            destroyMeshGeometry(vtable, ctx, mesh);
            return tl::unexpected<BackendError>{layoutResult.error()};
        }
        mesh.layout = *layoutResult;

        GeometryDescriptor geomDesc{};
        geomDesc.layoutHandle = mesh.layout;
        geomDesc.bufferAttribs = std::move(bufferAttribs);
        geomDesc.indexBufferHandle = mesh.indexBuffer;

        auto geomResult = vtable.createGeometry(ctx, geomDesc);
        if (!geomResult) {
            destroyMeshGeometry(vtable, ctx, mesh);
            return tl::unexpected<BackendError>{geomResult.error()};
        }
        mesh.geometry = *geomResult;

        return mesh;
    }

    void destroyMeshGeometry(const RenderDeviceBackendVTable &vtable, RenderDeviceBackendContext *ctx, MeshGeometry &mesh) {
        if (mesh.geometry.isValid() && vtable.destroyGeometry != nullptr) {
            vtable.destroyGeometry(ctx, mesh.geometry);
        }
        if (mesh.layout.isValid() && vtable.destroyVertexLayout != nullptr) {
            vtable.destroyVertexLayout(ctx, mesh.layout);
        }
        if (vtable.destroyBuffer != nullptr) {
            for (BufferHandle b : mesh.attributeBuffers) {
                if (b.isValid()) {
                    vtable.destroyBuffer(ctx, b);
                }
            }
            if (mesh.indexBuffer.isValid()) {
                vtable.destroyBuffer(ctx, mesh.indexBuffer);
            }
        }
        mesh = MeshGeometry{};
    }
} // namespace xe
