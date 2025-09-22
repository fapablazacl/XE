
#include "Util.h"
#include "Common.h"
#include "xe/DataType.h"
#include "xe/graphics/BufferDescriptor.h"
#include "xe/graphics/GraphicsDevice.h"
#include "xe/graphics/Program.h"
#include "xe/graphics/Shader.h"
#include "xe/graphics/Subset.h"
#include "xe/math/Vector.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace demo {
    XE::ProgramDescriptor makeSimpleProgramDesc(const std::string &vs, const std::string &fs) {
        XE::ProgramDescriptor desc;

        desc.sources = {
            {XE::ShaderType::Vertex, vs},
            {XE::ShaderType::Fragment, fs},
        };

        return desc;
    }

    Mesh makeIndexedCubeMesh(const float width, const float height, const float depth) {
        Mesh mesh;

        const XE::Vector3 n = {0.0F, 0.0F, 1.0};

        const std::vector<Vertex> vertices = {
            {{-0.5F * width, -0.5F * height, 0.5F * depth}, n, {1.0F, 0.0F, 0.0F, 1.0F}},
            {{0.5F * width, -0.5F * height, 0.5F * depth}, n, {0.0F, 1.0F, 0.0F, 1.0F}},
            {{-0.5F * width, 0.5F * height, 0.5F * depth}, n, {0.0F, 0.0F, 1.0F, 1.0F}},
            {{0.5F * width, 0.5F * height, 0.5F * depth}, n, {1.0F, 1.0F, 0.0F, 1.0F}},
            {{-0.5F * width, -0.5F * height, -0.5F * depth}, n, {0.0F, 1.0F, 1.0F, 1.0F}},
            {{0.5F * width, -0.5F * height, -0.5F * depth}, n, {1.0F, 0.0F, 1.0F, 1.0F}},
            {{-0.5F * width, 0.5F * height, -0.5F * depth}, n, {0.0F, 0.0F, 0.0F, 1.0F}},
            {{0.5F * width, 0.5F * height, -0.5F * depth}, n, {1.0F, 1.0F, 1.0F, 1.0F}}
        };

        mesh.primitive = XE::PrimitiveType::TriangleList;

        const std::vector<int> indices = {0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1};

        // convertir primitiva a lista de triangulos
        bool order = true;

        for (std::size_t i = 0; i < indices.size() - 2; i++) {
            Vertex p1;
            Vertex p2;
            Vertex p3;

            if (order) {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 1]];
                p3 = vertices[indices[i + 2]];
            } else {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 2]];
                p3 = vertices[indices[i + 1]];
            }

            const XE::Vector3 edge1 = p2.coord - p1.coord;
            const XE::Vector3 edge2 = p3.coord - p1.coord;
            const XE::Vector3 normal = XE::normalize(XE::cross(edge1, edge2));

            p1.normal = normal;
            p2.normal = normal;
            p3.normal = normal;

            mesh.vertices.push_back(p1);
            mesh.vertices.push_back(p2);
            mesh.vertices.push_back(p3);

            order = !order;
        }

        return mesh;
    }

    Mesh makeAxisMesh(const float width, const float height, const float depth) {
        const XE::Vector3 n = {0.0F, 1.0F, 0.0};

        Mesh mesh;

        mesh.vertices = {
            {{0.0F, 0.0F, 0.0F}, n, {0.5F, 0.5F, 0.5F, 1.0F}},
            {{width, 0.0F, 0.0F}, n, {1.0F, 0.5F, 0.5F, 1.0F}},
            {{0.0F, height, 0.0F}, n, {0.5F, 1.0F, 0.5F, 1.0F}},
            {{0.0F, 0.0F, depth}, n, {0.5F, 0.0F, 1.0F, 1.0F}}
        };
        mesh.primitive = XE::PrimitiveType::LineList;
        mesh.indices = {0, 1, 0, 2, 0, 3};

        return mesh;
    }

    Mesh makeColoredCubeMesh(const float width, const float height, const float depth) {
        const XE::Vector3 n = {0.0F, 0.0F, 1.0};

        Mesh mesh;

        const std::vector<Vertex> vertices = {
            {{-0.5F * width, -0.5F * height, 0.5F * depth}, n, {1.0F, 0.0F, 1.0F, 1.0F}},
            {{0.5F * width, -0.5F * height, 0.5F * depth}, n, {1.0F, 0.0F, 1.0F, 1.0F}},
            {{-0.5F * width, 0.5F * height, 0.5F * depth}, n, {0.0F, 1.0F, 1.0F, 1.0F}},
            {{0.5F * width, 0.5F * height, 0.5F * depth}, n, {0.0F, 1.0F, 1.0F, 1.0F}},
            {{-0.5F * width, -0.5F * height, -0.5F * depth}, n, {1.0F, 0.0F, 0.0F, 1.0F}},
            {{0.5F * width, -0.5F * height, -0.5F * depth}, n, {1.0F, 0.0F, 0.0F, 1.0F}},
            {{-0.5F * width, 0.5F * height, -0.5F * depth}, n, {1.0F, 1.0F, 0.0F, 1.0F}},
            {{0.5F * width, 0.5F * height, -0.5F * depth}, n, {0.0F, 1.0F, 0.0F, 1.0F}}
        };

        mesh.primitive = XE::PrimitiveType::TriangleList;

        const std::vector<int> indices = {0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1};

        // convertir primitiva a lista de triangulos
        bool order = true;

        for (std::size_t i = 0; i < indices.size() - 2; i++) {
            Vertex p1;
            Vertex p2;
            Vertex p3;

            if (order) {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 2]];
                p3 = vertices[indices[i + 1]];
            } else {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 1]];
                p3 = vertices[indices[i + 2]];
            }

            const XE::Vector3 edge1 = p2.coord - p1.coord;
            const XE::Vector3 edge2 = p3.coord - p1.coord;
            const XE::Vector3 normal = XE::normalize(XE::cross(edge1, edge2));

            p1.normal = normal;
            p2.normal = normal;
            p3.normal = normal;

            mesh.vertices.push_back(p1);
            mesh.vertices.push_back(p2);
            mesh.vertices.push_back(p3);

            order = !order;
        }

        return mesh;
    }

    Mesh makeCubeMesh(const float width, const float height, const float depth) {
        const XE::Vector3 n = {0.0F, 1.0F, 0.0};

        Mesh mesh;

        const std::vector<Vertex> vertices = {
            {{-0.5F * width, -0.5F * height, 0.5F * depth}, n, {1.0F, 1.0F, 1.0F, 1.0F}},
            {{0.5F * width, -0.5F * height, 0.5F * depth}, n, {1.0F, 1.0F, 1.0F, 1.0F}},
            {{-0.5F * width, 0.5F * height, 0.5F * depth}, n, {1.0F, 1.0F, 1.0F, 1.0F}},
            {{0.5F * width, 0.5F * height, 0.5F * depth}, n, {1.0F, 1.0F, 1.0F, 1.0F}},
            {{-0.5F * width, -0.5F * height, -0.5F * depth}, n, {1.0F, 1.0F, 1.0F, 1.0F}},
            {{0.5F * width, -0.5F * height, -0.5F * depth}, n, {1.0F, 1.0F, 1.0F, 1.0F}},
            {{-0.5F * width, 0.5F * height, -0.5F * depth}, n, {1.0F, 1.0F, 1.0F, 1.0F}},
            {{0.5F * width, 0.5F * height, -0.5F * depth}, n, {1.0F, 1.0F, 1.0F, 1.0F}}
        };

        mesh.primitive = XE::PrimitiveType::TriangleList;

        const std::vector<int> indices = {0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1};

        // convertir primitiva a lista de triangulos
        bool order = true;

        for (std::size_t i = 0; i < indices.size() - 2; i++) {
            Vertex p1;
            Vertex p2;
            Vertex p3;

            if (order) {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 2]];
                p3 = vertices[indices[i + 1]];
            } else {
                p1 = vertices[indices[i + 0]];
                p2 = vertices[indices[i + 1]];
                p3 = vertices[indices[i + 2]];
            }

            const XE::Vector3 edge1 = p2.coord - p1.coord;
            const XE::Vector3 edge2 = p3.coord - p1.coord;
            const XE::Vector3 normal = XE::normalize(XE::cross(edge1, edge2));

            p1.normal = normal;
            p2.normal = normal;
            p3.normal = normal;

            mesh.vertices.push_back(p1);
            mesh.vertices.push_back(p2);
            mesh.vertices.push_back(p3);

            order = !order;
        }

        return mesh;
    }

    std::string loadTextFile(const std::string &filePath) {
        assert(!filePath.empty());

        std::fstream fs;
        fs.open(filePath.c_str());

        assert(fs.is_open());

        std::string content;
        std::string line;

        while (std::getline(fs, line)) {
            content += line + "\n";
        }

        return content;
    }

    XE::Subset *createSubset(XE::GraphicsDevice *graphicsDevice, const Mesh &mesh) {
        const XE::BufferDescriptor bufferDesc{
            XE::BufferType::Vertex,
            XE::BufferUsage::Read,
            XE::BufferAccess::Static,
            static_cast<size_t>(sizeof(Vertex) * mesh.vertices.size()),
            reinterpret_cast<const void *>(mesh.vertices.data())
        };

        XE::Buffer *vertexBuffer = graphicsDevice->createBuffer(bufferDesc);

        XE::Buffer *indexBuffer = nullptr;

        if (!mesh.indices.empty()) {
            const XE::BufferDescriptor indexBufferDesc{
                XE::BufferType::Index,
                XE::BufferUsage::Read,
                XE::BufferAccess::Static,
                static_cast<size_t>(sizeof(uint32_t) * mesh.indices.size()),
                reinterpret_cast<const void *>(mesh.indices.data())
            };

            indexBuffer = graphicsDevice->createBuffer(indexBufferDesc);
        }

        const std::vector<XE::SubsetVertexAttrib> attribs{
            {0, XE::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, coord)},
            {1, XE::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, normal)},
            {2, XE::DataType::Float32, 4, false, sizeof(Vertex), 0, OFFSETOF(Vertex, color)}
        };

        XE::SubsetDescriptor const subsetDesc{{vertexBuffer}, attribs, indexBuffer};
        XE::Subset *subset = graphicsDevice->createSubset(subsetDesc);

        return subset;
    }

    XE::Subset *createCubeSubset2(XE::GraphicsDevice *graphicsDevice, const Mesh &mesh) {
        const XE::BufferDescriptor bufferDesc{
            XE::BufferType::Vertex,
            XE::BufferUsage::Read,
            XE::BufferAccess::Static,
            static_cast<size_t>(sizeof(Vertex) * mesh.vertices.size()),
            reinterpret_cast<const void *>(mesh.vertices.data())
        };

        XE::Buffer *vertexBuffer = graphicsDevice->createBuffer(bufferDesc);

        const std::vector<XE::SubsetVertexAttrib> attribs{
            {0, XE::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, coord)},
            {1, XE::DataType::Float32, 3, false, sizeof(Vertex), 0, OFFSETOF(Vertex, normal)},
            {2, XE::DataType::Float32, 4, false, sizeof(Vertex), 0, OFFSETOF(Vertex, color)}
        };

        XE::SubsetDescriptor const subsetDesc{{vertexBuffer}, attribs, nullptr};

        XE::Subset *subset = graphicsDevice->createSubset(subsetDesc);

        return subset;
    }

    Mesh makeGridMesh(const float tileSize, const int tilesInX, const int tilesInZ) {
        const float half = tileSize * 0.5F;

        const XE::Vector3 normal = {0.0F, 0.0F, 1.0};

        const XE::Vector4 white = {0.8F, 0.8F, 0.8F, 1.0F};
        const XE::Vector4 black = {0.2F, 0.2F, 0.2F, 1.0F};

        const XE::Vector3 p1 = {-half, 0.0F, -half};
        const XE::Vector3 p2 = {half, 0.0F, -half};
        const XE::Vector3 p3 = {-half, 0.0F, half};
        const XE::Vector3 p4 = {half, 0.0F, half};

        Mesh mesh;

        bool colorSide = false;

        for (int i = 0; i < tilesInX; i++) {
            for (int k = 0; k < tilesInZ; k++) {
                const XE::Vector3 centroid = {(i * tileSize) - (half * tilesInX), 0.0F, (k * tileSize) - (half * tilesInZ)};

                const auto color = colorSide ? white : black;

                mesh.vertices.push_back({p1 + centroid, normal, color});
                mesh.vertices.push_back({p2 + centroid, normal, color});
                mesh.vertices.push_back({p3 + centroid, normal, color});

                mesh.vertices.push_back({p2 + centroid, normal, color});
                mesh.vertices.push_back({p4 + centroid, normal, color});
                mesh.vertices.push_back({p3 + centroid, normal, color});

                const XE::Vector3 pp1 = mesh.vertices[mesh.vertices.size() - 3].coord;
                const XE::Vector3 pp2 = mesh.vertices[mesh.vertices.size() - 2].coord;
                const XE::Vector3 pp3 = mesh.vertices[mesh.vertices.size() - 1].coord;

                const XE::Vector3 edge1 = pp2 - pp1;
                const XE::Vector3 edge2 = pp3 - pp1;
                const XE::Vector3 nnormal = XE::normalize(XE::cross(edge1, edge2));

                for (int ii = 0; ii < 6; ii++) {
                    mesh.vertices[mesh.vertices.size() - ii - 1].normal = nnormal;
                }

                colorSide = !colorSide;
            }
            colorSide = !colorSide;
        }

        mesh.primitive = XE::PrimitiveType::TriangleList;

        return mesh;
    }
} // namespace demo
